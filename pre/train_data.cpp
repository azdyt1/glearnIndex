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

int threshold;

void gen_train_data() {
    for(int i = 0; i < nv; i++) {
        if(offsets[i+1] - offsets[i] > threshold) {
            cout <<"Deg: "<< i <<" " << offsets[i+1] - offsets[i] << endl;
            for(int j = offsets[i]; j < offsets[i+1]; j++) {
                cout << edges[j] << endl;
            }
        }
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
    if ((a = argPos(const_cast<char *>("-th"), argc, argv)) > 0) {
        threshold = atoi(argv[a + 1]);
    }
}

int main(int argc, char **argv) {
    args(argc, argv);
    loadGraph();

    gen_train_data();
    return 0;
}
