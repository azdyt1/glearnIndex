#include "kgraph.h"
#include <string>
#include <chrono>
// #include <omp.h>
// #include <iomanip>

std::string graph_path;
std::string test_data_path;

int num_of_query;
int * src;
int * dst;

void logTime(std::string proc_name, float cost) {
    cout << proc_name 
         << " took "
         << cost
         << " s to run" 
         << endl;
}

void test(LSGraph* graph) {
    auto begin = chrono::steady_clock::now();
    // TODO: add parallel later!!!
    for(int i = 0; i < num_of_query; i++) {
        graph->find_edge(src[i], dst[i]);
    }    
    auto end = chrono::steady_clock::now();
    float cost = chrono::duration_cast<chrono::duration<float>>(end - begin).count();
    cout << num_of_query << " edges." << endl;
    logTime("find_edge", cost);
    return ;
}

void loadTestData() {
    ifstream inputFile(test_data_path, ios::in | ios::binary);
    if (inputFile.is_open()) {
        inputFile.seekg(0, ios::beg);

        inputFile.read(reinterpret_cast<char *>(&num_of_query), sizeof(int));
        src = static_cast<int *>(malloc((num_of_query) * sizeof(int)));
        dst = static_cast<int *>(malloc((num_of_query) * sizeof(int)));

        inputFile.read(reinterpret_cast<char *>(src), num_of_query * sizeof(int));
        inputFile.read(reinterpret_cast<char *>(dst), num_of_query * sizeof(int));

        inputFile.close();
    }
    return ;
}

void args(int argc, char **argv) {
    int a = 0;
    if ((a = argPos(const_cast<char *>("-input"), argc, argv)) > 0) {
        graph_path = std::string(argv[a + 1]);
    } 
    if ((a = argPos(const_cast<char *>("-test"), argc, argv)) > 0) {
        test_data_path = std::string(argv[a + 1]);
    } 
}

int main(int argc, char **argv) {
    args(argc, argv);
    LSGraph graph;
    std::cout << graph_path << std::endl;
    graph.loadCRSGraph(argc, argv);

    loadTestData();

    test(&graph);
    return 0;
}
