#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <chrono>
#include <omp.h>
#include <iomanip>

using namespace std;

//Graph
std::string graph_path;
long long nv;
long long ne;
long long *offsets;
int *edges;

std::string out_path;
int query_num;
int *src;
int *dst;


class myrandom {
public:
// http://xoroshiro.di.unimi.it/#shootout
// We use xoroshiro128+, the fastest generator available
    uint64_t rng_seed0, rng_seed1;

    myrandom(uint64_t seed);

    void reinit(uint64_t seed);

    static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    uint64_t lrand();

    double drand();

    int irand(int max) { return lrand() % max; }

    int irand(int min, int max) { return lrand() % (max - min) + min; }
};

static myrandom mainrandom(time(nullptr));


myrandom::myrandom(uint64_t seed) {
    for (int i = 0; i < 2; i++) {
    long long z = seed += UINT64_C(0x9E3779B97F4A7C15);
    z = (z ^ z >> 30) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ z >> 27) * UINT64_C(0x94D049BB133111EB);
    if (i == 0)
        rng_seed0 = z ^ (z >> 31);
    else
        rng_seed1 = z ^ (z >> 31);
    }
}

void myrandom::reinit(uint64_t seed) {
    for (int i = 0; i < 2; i++) {
        long long z = seed += UINT64_C(0x9E3779B97F4A7C15);
        z = (z ^ z >> 30) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ z >> 27) * UINT64_C(0x94D049BB133111EB);
        if (i == 0)
            rng_seed0 = z ^ (z >> 31);
        else
            rng_seed1 = z ^ (z >> 31);
    }
}

uint64_t myrandom::lrand() {
    const uint64_t s0 = rng_seed0;
    uint64_t s1 = rng_seed1;
    const uint64_t result = s0 + s1;
    s1 ^= s0;
    rng_seed0 = rotl(s0, 55) ^ s1 ^ (s1 << 14);
    rng_seed1 = rotl(s1, 36);
    return result;
}

double myrandom::drand() {
    const union un {
        uint64_t i;
        double d;
    } a = {UINT64_C(0x3FF) << 52 | lrand() >> 12};
    return a.d - 1.0;
}

int search_src(long long val) {
    int l = 0, r = (int)nv;
    while(l <= r) {
        int mid = (l+r) / 2;
        if(offsets[mid] < val) l = mid + 1;
        else r = mid - 1;
    }
    return r;
}

void gen_query(int num_of_query) {
    src = static_cast<int *>(malloc((num_of_query) * sizeof(int)));
    dst = static_cast<int *>(malloc((num_of_query) * sizeof(int)));
    int idx = 0;
    int exist_edge_num = int(num_of_query * 0.7);
    cout <<num_of_query<<"/"<< exist_edge_num << endl;
    //generate 70% existed edges
    for(int i = 0; i < exist_edge_num; i++) {
        long long edge_offset = mainrandom.lrand() % ne;
        int y = edges[edge_offset];
        int x = search_src(edge_offset);
        if(x < 0) {
            cout << "error: " << edge_offset << endl;
        }
        //cout <<x<<" " << y <<endl;
        src[idx] = x;
        dst[idx] = y;
        idx++;
    }
    
    cout << "remain" << endl;

    //generate 30% random pairs
    for(int i = exist_edge_num; i < num_of_query; i++) {
        int x = mainrandom.irand((int)nv);
        int y = mainrandom.irand((int)nv);
        //cout <<x<<" " << y <<endl;
        src[idx] = x;
        dst[idx] = y;
        idx++;
    }
    cout <<"all index " <<idx<<endl;
    ofstream outFile(out_path, ios::out | ios::binary);
    if (outFile.is_open()) {
        outFile.seekp(0, ios::beg);

        outFile.write(reinterpret_cast<char *>(&num_of_query), sizeof(int));
        outFile.write(reinterpret_cast<char *>(src), num_of_query * sizeof(int));
        outFile.write(reinterpret_cast<char *>(dst), num_of_query * sizeof(int));
        
        outFile.close();
    }
    return ;
}

void loadGraph() {
    ifstream inputFile(graph_path, ios::in | ios::binary);
    if (inputFile.is_open()) {
        inputFile.seekg(0, ios::beg);
        inputFile.read(reinterpret_cast<char *>(&nv), sizeof(long long));
        inputFile.read(reinterpret_cast<char *>(&ne), sizeof(long long));

        offsets = static_cast<long long *>(malloc((nv + 1) * sizeof(long long)));
        edges = static_cast<int *>(malloc(ne * sizeof(int32_t)));
        
        inputFile.read(reinterpret_cast<char *>(offsets), nv * sizeof(long long));
        offsets[nv] = static_cast<long long>(ne);
        inputFile.read(reinterpret_cast<char *>(edges), sizeof(int32_t) * ne);
        
        inputFile.close();
    }
    return ;
}

int argPos(char *str, int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        if (!strcmp(str, argv[i])) {
            if (i == argc - 1) {
                printf("Argument missing for %s\n", str);
                exit(1);
            }
            return i;
        }

    }
    return -1; 
}

void args(int argc, char **argv) {
    int a = 0;
    if ((a = argPos(const_cast<char *>("-input"), argc, argv)) > 0) {
        graph_path = std::string(argv[a + 1]);
    } 
    if ((a = argPos(const_cast<char *>("-output"), argc, argv)) > 0) {
        out_path = std::string(argv[a + 1]);
    } 
    if ((a = argPos(const_cast<char *>("-qnum"), argc, argv)) > 0) {
        query_num = atoi(argv[a + 1]);
    } 
}

int main(int argc, char **argv) {
    args(argc, argv);
    loadGraph();

    gen_query(query_num);
    return 0;
}
