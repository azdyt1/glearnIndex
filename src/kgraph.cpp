#include "kgraph.h"
#include <omp.h>
#include <iostream>
#include <set>

bool LSGraph::loadCRSGraph(string network_file) {
    ifstream inputFile(network_file, ios::in | ios::binary);
    if (inputFile.is_open()) {
        inputFile.seekg(0, ios::beg);
        inputFile.read(reinterpret_cast<char *>(&nv), sizeof(long long));
        inputFile.read(reinterpret_cast<char *>(&ne), sizeof(long long));
        offsets = static_cast<long long *>(malloc((nv + 1) * sizeof(long long)));
        edges = static_cast<int *>(malloc(ne * sizeof(int32_t)));
        weights = static_cast<float *>(malloc(ne * sizeof(int32_t)));
        node_types = static_cast<int *>(malloc(nv * sizeof(int32_t)));
        edges_r = static_cast<long long *>(malloc(ne * sizeof(long long)));
        
        inputFile.read(reinterpret_cast<char *>(offsets), nv * sizeof(long long));
        
        offsets[nv] = static_cast<long long>(ne);
        
        inputFile.read(reinterpret_cast<char *>(edges), sizeof(int32_t) * ne);

        
        degrees = static_cast<int *>(malloc(nv * sizeof(int32_t)));
        for (int i = 0; i < nv; i++)
            degrees[i] = offsets[i + 1] - offsets[i];

        if (this->weighted) {
            cout << "Weighted Graph" << endl;
            myrandom random(time(nullptr));
            for (long long i = 0; i < ne; i++) {
                inputFile.read(reinterpret_cast<char *>(&weights[i]), sizeof(float));
            } 
        } else for (long long i = 0; i < ne; i++) weights[i] = float(1);
        this->type_num = 1;
        std::set<int> typeSet;
        if (this->hetro) {
            cout << "Heterogeneous Graph" << endl;
            for (long long i = 0; i < nv; i++) {
                inputFile.read(reinterpret_cast<char *>(&node_types[i]), sizeof(int));
                typeSet.insert(node_types[i]);
                if (node_types[i] == 0) cout << node_types[i];
            }
            this->type_num = typeSet.size();
            cout << "Num of types: " << type_num << endl;
        }
        
        
        cout << nv <<" "<< ne<<endl;
        inputFile.close();
        
//        init_reverse(); // This should be optimized for the large graphs, using binary search for reverse edge identification.

        return true;
  } else {
    return false;
  }
}

bool LSGraph::loadCRSGraph(int argc, char **argv) {
    string network_file;
    bool findInCmd = false;
    weighted = false;
    hetro = false;
    for (int i = 0; i < argc; ++i) {
        if (!strcmp("-weighted", argv[i])) {
            weighted = true;
        }
        if (!strcmp("-hetro", argv[i])) {
            hetro = true;
        }
        if (!strcmp("-input", argv[i])) {
            if (i == argc - 1) {
                printf("Argument missing for -input\n");
                exit(1);
            } 
            findInCmd = true;
            network_file = argv[i + 1];
        }
        
    }
    if (!findInCmd) {
        printf("network file required, please check input...\n");
        exit(1);
    }


    return this->loadCRSGraph(network_file);

}

void LSGraph::init_reverse() {
#pragma omp parallel for
    for (int src = 0; src < nv; src++) {
        for (long long lastedgeidx = offsets[src]; lastedgeidx < offsets[src + 1]; lastedgeidx++) {
        int dst = edges[lastedgeidx];
        // accelerates
        if (degrees[src] < degrees[dst] || (degrees[src] == degrees[dst] && src < dst))
            continue;
        long long pos = find_edge(dst, src); // find edge from dst to src
        
        edges_r[lastedgeidx] = pos;
        edges_r[pos] = lastedgeidx;
        }
    }
  // check
#pragma omp parallel for schedule(dynamic)
    for (int src = 0; src < nv; src++) {
        for (long long lastedgeidx = offsets[src]; lastedgeidx < offsets[src + 1]; lastedgeidx++) {
            int dst = edges[lastedgeidx];
            long long rvs = edges_r[lastedgeidx];
            if (rvs >= offsets[dst + 1] || rvs < offsets[dst] || edges[rvs] != src) {
#pragma omp critical
{
                cout << "ERROR for " << src << "->" << dst << " : "
                     << edges[rvs] << " wrong or " << rvs << " not between "
                     << offsets[dst] << " and " << offsets[dst + 1] << endl;
}
                long long pos = find_edge(dst, src);
                edges_r[lastedgeidx] = pos;
            }
        }
    }
    std::cout << "Finish creating csr" << std::endl;

}

long long LSGraph::find_edge(int src, int dst) {
    long long l = offsets[src], r = offsets[src + 1], mid;
    while (l < r) {
        mid = (l + r) / 2;
        if (edges[mid] == dst)
            return mid;
        if (edges[mid] > dst)
            r = mid;
        else
            l = mid + 1;
    }
    return -1;
}

int LSGraph::has_edge(int from, int to) {
    return binary_search(&edges[offsets[from]], &edges[offsets[from + 1]], to);
}

void LSGraph::printGraphInfo() {
    printf("number of nodes: %lld, number of edges: %lld\n", this->nv, this->ne);
}

int* LSGraph::getDegree() {
    return this->degrees;
}

long long* LSGraph::getOffsets() {
    return this->offsets;
}

int* LSGraph::getEdges() {
    return this->edges;
}

long long LSGraph::getNumberOfVertex() {
    return this->nv;
}

long long LSGraph::getNumberOfEdge() {
    return this->ne;
}

long long* LSGraph::getEdges_r() {
    return this->edges_r;
}

float* LSGraph::getWeights() {
    return this->weights;
}

int LSGraph::getTypeNum() {
    return this->type_num;
}

int *LSGraph::getTypes() {
    return this->node_types;
}

bool LSGraph::isHetro() {
    return this->hetro;
}
