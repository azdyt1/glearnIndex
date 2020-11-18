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

//std::string out_path;

void count_degree() {
    int max_degree = 0;
    for(int i = 0; i < nv; i++) {
        // cout  << offsets[i+1] - offsets[i] << endl;
        if(offsets[i+1] - offsets[i] > max_degree) {
            max_degree = offsets[i+1] - offsets[i];
        }
    }

    int *cnt = static_cast<int *>(malloc((max_degree + 1) * sizeof(int)));
    for(int i = 0; i <= max_degree; i++)
        cnt[i] = 0;

    for(int i = 0; i < nv; i++) {
        cnt[offsets[i+1] - offsets[i]]++;
    }

    cout <<"#max_degree: " << max_degree << endl;
    for(int i = max_degree; i >= 0; i--) {
        if(cnt[i] > 0)
            cout << i  <<" " <<cnt[i] <<endl;
    }

    // ofstream outFile(out_path, ios::out | ios::binary);
    // if (outFile.is_open()) {
    //     outFile.seekg(0, ios::beg);

    //     outFile.write(reinterpret_cast<char *>(src), sizeof(int));
    //     outFile.write(reinterpret_cast<char *>(dst), sizeof(int));
        
    //     outFile.close();
    // }
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
    // cout <<nv << " " << ne <<endl;
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
    // if ((a = argPos(const_cast<char *>("-output"), argc, argv)) > 0) {
    //     out_path = std::string(argv[a + 1]);
    // } 
}

int main(int argc, char **argv) {
    args(argc, argv);
    loadGraph();

    count_degree();
    return 0;
}
