#ifndef KGRAPH_H
#define KGRAPH_H

#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "utils.h"

using namespace std;
using EdgeIndexType = long long;

/** 
 * large scale graph
 * The graph storage model should be updated, in order to store large scale labeled weighted graphs.
 * Here we assume node has types, edge has types and weights.
 **/

class LSGraph {
private:
    long long *offsets;
    int *edges;
    int *degrees;
    int *node_types;
    float *weights;
    
    bool hetro;
    long long nv;
    long long ne;
    int type_num;
    long long *edges_r;
    
public:
    bool weighted; 
    LSGraph(){};
    ~LSGraph() {
        free(this->offsets);
        free(this->edges);
        free(this->degrees);
        free(this->weights);
        free(this->edges_r);
    }
    bool loadCRSGraph(string network_file);
    bool loadCRSGraph(int argc, char **argv);

    void printGraphInfo();
    
    void init_reverse();

    long long find_edge(int src, int dst);

    int has_edge(int from, int to);

    long long getNumberOfVertex();
    long long getNumberOfEdge();
    bool isHetro();

    int* getDegree();

    long long* getOffsets();

    int* getEdges();

    int *getTypes();

    int getTypeNum();

    float* getWeights();
    long long* getEdges_r();


};

#endif