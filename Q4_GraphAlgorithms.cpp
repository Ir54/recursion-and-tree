// Q4_GraphAlgorithms.cpp
#include <bits/stdc++.h>
using namespace std;
const long long INF = (1LL<<60);

struct Graph {
    int n; // number of nodes
    vector<char> names;
    vector<vector<long long>> adj; // adjacency matrix: INF if no edge
    unordered_map<char,int> indexOf;

    Graph(const vector<char>& nodes) {
        names = nodes;
        n = nodes.size();
        adj.assign(n, vector<long long>(n, INF));
        for (int i=0;i<n;++i) {
            indexOf[nodes[i]] = i;
            adj[i][i] = 0;
        }
    }

    void addDirectedEdge(char u, char v, long long w) {
        int iu = indexOf[u], iv = indexOf[v];
        adj[iu][iv] = w;
    }

    void printAdjMatrix() {
        cout << "Adjacency Matrix (INF = no edge):\n   ";
        for (char c : names) cout << c << "    ";
        cout << "\n";
        for (int i = 0; i < n; ++i) {
            cout << names[i] << " ";
            for (int j = 0; j < n; ++j) {
                if (adj[i][j] >= INF/2) cout << " INF";
                else {
                    cout << setw(4) << adj[i][j];
                }
                cout << " ";
            }
            cout << "\n";
        }
    }

    // DFS with classification
    void DFS_with_classification(char start) {
        vector<int> disc(n, -1), finish(n, -1);
        vector<int> visited(n, 0);
        int time = 0;
        vector<pair<char,char>> treeEdges, backEdges, forwardEdges, crossEdges;
        vector<char> order;

        function<void(int, vector<int>&)> dfs = [&](int u, vector<int>& stack) {
            disc[u] = ++time;
            visited[u] = 1;
            stack.push_back(names[u]);
            cout << "Enter " << names[u] << ", discovery time " << disc[u] << "\n";
            // show recursion stack
            cout << "Recursion stack: ";
            for (char c : stack) cout << c << " ";
            cout << "\n";
            // neighbors in alphabetical order:
            vector<pair<char,int>> nbrs;
            for (int v=0; v<n; ++v) {
                if (adj[u][v] < INF/2) nbrs.push_back({names[v], v});
            }
            sort(nbrs.begin(), nbrs.end());
            for (auto &p : nbrs) {
                int v = p.second;
                if (disc[v] == -1) {
                    treeEdges.push_back({names[u], names[v]});
                    dfs(v, stack);
                } else {
                    // classify based on disc/finish
                    if (finish[v] == -1 && disc[v] < disc[u]) {
                        backEdges.push_back({names[u], names[v]});
                    } else if (finish[v] == -1 && disc[v] > disc[u]) {
                        // neighbor discovered later but still active -> forward? Actually forward edges have finish[v]==-1? We'll use standard rules below
                        // This case unlikely due to ordering; classification determined after finish times.
                        // We'll defer classification to after DFS using disc/finish.
                    } else {
                        // finish[v] != -1
                        // will classify later
                    }
                }
            }
            finish[u] = ++time;
            order.push_back(names[u]);
            cout << "Exit " << names[u] << ", finish time " << finish[u] << "\n";
            // show visited array status
            cout << "Visited status after processing " << names[u] << ": ";
            for (int i=0;i<n;++i) cout << (disc[i]!=-1 ? 1:0) << " ";
            cout << "\n";
            stack.pop_back();
        };

        vector<int> st;
        int sidx = indexOf[start];
        dfs(sidx, st);

        // After finishing all discovery/finish, classify all edges
        for (int u=0; u<n; ++u) {
            for (int v=0; v<n; ++v) {
                if (adj[u][v] < INF/2) {
                    if (disc[u] < disc[v] && finish[v] < finish[u]) {
                        // v is descendant of u in DFS tree -> TREE or FORWARD
                        // If (u,v) used in DFS tree edges -> tree; else forward
                        // Check if (u,v) is a tree edge by seeing disc[v] == disc[u]+1 maybe not robust. We can detect by checking parent relationship not stored.
                        // Simplify: if disc[u] < disc[v] and finish[v] < finish[u]:
                        // if (disc[v] == disc[u]+1) -> likely tree edge, but better to check by recorded treeEdges.
                    }
                }
            }
        }

        // For robust classification, we'll use parent map to detect tree edges
        // Re-run DFS to compute parent array and times (simpler approach)
        disc.assign(n,-1); finish.assign(n,-1);
        vector<int> parent(n, -1);
        time = 0;
        function<void(int)> dfs2 = [&](int u) {
            disc[u] = ++time;
            // neighbors alphabetical
            vector<pair<char,int>> nbrs;
            for (int v=0; v<n; ++v) if (adj[u][v] < INF/2) nbrs.push_back({names[v], v});
            sort(nbrs.begin(), nbrs.end());
            for (auto &p : nbrs) {
                int v = p.second;
                if (disc[v] == -1) {
                    parent[v] = u;
                    dfs2(v);
                }
            }
            finish[u] = ++time;
        };
        dfs2(indexOf[start]);

        // now classify
        vector<pair<char,char>> treeE, backE, forwardE, crossE;
        for (int u=0; u<n; ++u) {
            for (int v=0; v<n; ++v) {
                if (adj[u][v] >= INF/2) continue;
                if (parent[v] == u) treeE.push_back({names[u], names[v]});
                else if (disc[v] < disc[u] && finish[v] > finish[u]) backE.push_back({names[u], names[v]});
                else if (disc[u] < disc[v] && finish[u] > finish[v]) forwardE.push_back({names[u], names[v]});
                else crossE.push_back({names[u], names[v]});
            }
        }

        cout << "\nDFS traversal order (post-order): ";
        for (char c : order) cout << c << " ";
        cout << "\n\nEdge classification:\n";
        auto printEdges = [](const vector<pair<char,char>> &vec, const string &label){
            cout << label << ":\n";
            for (auto &e : vec) cout << e.first << "->" << e.second << "  ";
            cout << "\n";
        };
        printEdges(treeE, "Tree Edges");
        printEdges(backE, "Back Edges");
        printEdges(forwardE, "Forward Edges");
        printEdges(crossE, "Cross Edges");
    }

    // Dijkstra (detailed trace) from source char 'A' to dest 'G'
    void dijkstra_trace(char source, char dest) {
        int s = indexOf[source], t = indexOf[dest];
        vector<long long> dist(n, INF);
        vector<int> prev(n, -1);
        vector<int> visited(n, 0);
        dist[s] = 0;
        cout << "Initial distance table:\n";
        for (int i=0;i<n;++i) cout << names[i] << ":" << (dist[i] >= INF/2 ? -1 : dist[i]) << "  ";
        cout << "\n";

        for (int iter=0; iter<n; ++iter) {
            // pick unvisited with smallest dist
            int u = -1; long long best = INF;
            for (int i=0;i<n;++i) if (!visited[i] && dist[i] < best) { best = dist[i]; u = i; }
            if (u == -1 || dist[u] >= INF/2) break;
            visited[u] = 1;
            cout << "\nIteration " << iter+1 << " processing node " << names[u] << " (dist=" << dist[u] << ")\n";
            // relax neighbors
            for (int v=0; v<n; ++v) {
                if (adj[u][v] >= INF/2) continue;
                long long nd = dist[u] + adj[u][v];
                if (nd < dist[v]) {
                    dist[v] = nd;
                    prev[v] = u;
                    cout << "Relax: " << names[u] << "->" << names[v] << " new dist[" << names[v] << "]=" << nd << "\n";
                }
            }
            cout << "Distance table now: ";
            for (int i=0;i<n;++i) {
                if (dist[i] >= INF/2) cout << names[i] << ":INF ";
                else cout << names[i] << ":" << dist[i] << " ";
            }
            cout << "\n";
            if (u == t) break;
        }
        if (dist[t] >= INF/2) {
            cout << "Destination " << dest << " unreachable from " << source << "\n";
            return;
        }
        // reconstruct path
        vector<char> path;
        for (int cur = t; cur != -1; cur = prev[cur]) path.push_back(names[cur]);
        reverse(path.begin(), path.end());
        cout << "Shortest path from " << source << " to " << dest << ": ";
        for (char c : path) cout << c << (c==path.back() ? "" : "->");
        cout << " with total cost " << dist[t] << "\n";
    }
};
