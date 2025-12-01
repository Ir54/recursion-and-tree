#include <bits/stdc++.h>
using namespace std;
#include "Q4_GraphAlgorithms.cpp"

int main() {
    vector<char> nodes = {'A','B','C','D','E','F','G'};
    Graph g(nodes);
    // build a sample directed weighted graph
    g.addDirectedEdge('A','B',2);
    g.addDirectedEdge('A','C',1);
    g.addDirectedEdge('B','D',1);
    g.addDirectedEdge('B','C',2);
    g.addDirectedEdge('C','D',4);
    g.addDirectedEdge('C','E',8);
    g.addDirectedEdge('D','E',3);
    g.addDirectedEdge('D','F',5);
    g.addDirectedEdge('E','F',2);
    g.addDirectedEdge('F','G',1);

    cout << "Adjacency matrix:\n";
    g.printAdjMatrix();

    cout << "\n--- DFS with classification starting at A ---\n";
    g.DFS_with_classification('A');

    cout << "\n--- Dijkstra trace from A to G ---\n";
    g.dijkstra_trace('A','G');
    return 0;
}
