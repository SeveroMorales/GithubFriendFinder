#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
using namespace std;
struct Node {
    int id;
    string name;
    int ml_target;//it could be a boolean
    Node(int id, string name, int ml_target) : id(id), name(name), ml_target(ml_target) {}
};

class Graph {
private:
    // key is ID, value is a vector of node pointers
    map<int, vector<pair<Node, int>>> graph;

public:
    // this hold all the vertices, their index in the vector is also their id
    vector<Node> targets;
    Graph();
    void InsertEdge(int from, Node n, int weight);
    bool IsEdge(int from, int to);//Uses two ints to avoid operator overloading
    int GetWeight(int from, int to);
    vector<Node> GetNodes(int id);//Returns a vector of all the nodes, easier for access
    // vector<Node *> GetNodePointers(int id); //Returns a vector of node pointers, more efficient
    void printGraph();
    void Load(vector<Node> &set, string path);
    bool bfs(int src, int target, int *pre, int *dist);
    void bfsShortestPath(int src, int target);
    bool Dijkstra(int src, int target, int *pre, int *dist);
    void DijkstraShortestPath(int src, int target);
};

void Graph::InsertEdge(int from, Node n, int weight) {
    pair<Node, int> newPair = {n, weight};
    graph[from].push_back(newPair);//Push back the newest node pointer
    //if new "to" does not exist, create it as a key
    if (graph.count(n.id) == 0) {
        graph[(n.id)] = {};
    }
}
bool Graph::IsEdge(int from, int to) {
    for (auto &i : graph[from])
        if (i.first.id == to) return true;
    return false;
}
int Graph::GetWeight(int from, int to) {
    for (auto &i : graph[from]) {
        if (i.first.id == to) {
            return i.second;
        }
    }
    return 0;
}
vector<Node> Graph::GetNodes(int id) {
    // returns a vector of nodes of the graphs
    vector<Node> nodes;
    for (auto &i : graph[id]) {
        nodes.push_back(i.first);
    }
    return nodes;
}
/*vector<Node *> Graph::GetNodePointers(int id) {
    return graph[id];
}*/
Graph::Graph() = default;
void Graph::printGraph() {// NOTE: just for testing the printing, too much data to print the whole thing
    // prints the graph in a format sorted by ascending vertex matrix of weights
    for (auto it : graph) {
        cout << this->targets[it.first].name;
        for (auto vert : graph[it.first]) {
            cout << "->" << vert.first.name;
        }
        cout << endl;
    }
}

void LoadTargets(string thePath, vector<Node> &info) {// NOTE: this is severo's LoadCSV method
    // This would load the file an return
    // I can make a class of "Instructions"
    // The class will have a char (either s or l)
    // it will have a string with the key value or the address
    // it will have a int representing the value
    ifstream file(thePath);
    if (file.is_open()) {
        string lineFromFile;
        getline(file, lineFromFile);

        while (getline(file, lineFromFile)) {
            istringstream stream(lineFromFile);

            int id;
            string name;
            int ml_target;

            string tempId;
            string tempMl_target;

            getline(stream, tempId, ',');
            getline(stream, name, ',');
            getline(stream, tempMl_target, ',');
            try {
                id = stoi(tempId);
                ml_target = stoi(tempMl_target);

            } catch (const exception &e) {
                cout << e.what();
            }

            Node obj(id, name, ml_target);
            info.push_back(obj);
        }
    }
    file.close();
}
void Graph::Load(vector<Node> &nodes, string path) {
    // Loads the edges file and inserts the edges
    ifstream file("github_edges_1.csv");
    if (file.is_open()) {
        string lineFromFile;
        getline(file, lineFromFile);

        while (getline(file, lineFromFile)) {
            istringstream stream(lineFromFile);

            int from;
            int to;
            int weight;
            string token;

            getline(stream, token, ',');
            from = stoi(token);
            getline(stream, token, ',');
            to = stoi(token);
            getline(stream, token, ',');
            weight = stoi(token);
            this->InsertEdge(from, nodes[to], weight);
        }
    }
}
bool Graph::bfs(int src, int target, int pre[], int dist[]) {
    // This is basically a standard BFS with a few KEY modifications
    // inspired by the GFG solution and my own stepik implementation
    bool visited[this->targets.size()];
    queue<int> q;

    for (int i = 0; i < this->targets.size(); ++i) {
        visited[i] = false;
        dist[i] = INT_MAX;
        pre[i] = -1;
    }
    // push src node into visited
    visited[src] = true;
    dist[src] = 0;
    q.push(src);
    int distCount = 0;
    while (!q.empty()) {
        int f = q.front();
        q.pop();

        for (int i = 0; i < this->graph[f].size(); ++i) {
            if (!visited[graph[f][i].first.id]) {
                // NOTE: the main difference in this BFS are
                // the distance and predecessor arrays
                // these allow us to recreate the path later on
                // and also print the distance
                                // but it still wont affect the path at all just the distance
                visited[graph[f][i].first.id] = true;
                dist[graph[f][i].first.id] = dist[f] + graph[f][i].second;
                pre[graph[f][i].first.id] = f;
                q.push(graph[f][i].first.id);

                if (graph[f][i].first.id == target) {
                    return true;
                }
            }
        }
    }
    return false;
}
void Graph::bfsShortestPath(int src, int target) {
    int pre[this->targets.size()];
    int dist[this->targets.size()];

    if (!bfs(src, target, pre, dist)) {
        cout << "Source and target are not connected" << endl;
        return;
    }

    // store shortest path
    vector<int> path;
    int curr = target;
    path.push_back(target);
    // we go through the predecessor of each number until we reach 0
    while (pre[curr] != -1) {
        path.push_back(pre[curr]);
        curr = pre[curr];
    }

    // path to target
    cout << "Path is: " << endl;
    cout << this->targets[path[path.size() - 1]].name;
    for (int i = path.size() - 2; i >= 0; i--) {
        cout << "->" << this->targets[path[i]].name;
    }
    cout << "\nBFS Shortest distance is: " << dist[target] << endl;

}
bool Graph::Dijkstra(int src, int target, int pre[], int dist[]) {
    pair<int, int> p;

    for (int i = 0; i < this->targets.size(); ++i) {
        dist[i] = INT_MAX;
        pre[i] = -1;
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push(make_pair(0, src));
    dist[src] = 0;
    while (!pq.empty()) {
        int u = pq.top().second;
        int f = pq.top().first;
        pq.pop();

        for (auto x : graph[u]) {
            int v = x.first.id;
            int weight = x.second;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
                pre[v] = u;
            }
            if (x.first.id == target) {
                return true;
            }
        }
    }
    return false;
}
void Graph::DijkstraShortestPath(int src, int target) {
    int pre[this->targets.size()];
    int dist[this->targets.size()];

    if (!Dijkstra(src, target, pre, dist)) {
        cout << "Source and target are not connected" << endl;
        return;
    }

    // store shortest path
    vector<int> path;
    int curr = target;
    path.push_back(target);
    // we go through the predecessor of each number until we reach 0
    while (pre[curr] != -1) {
        path.push_back(pre[curr]);
        curr = pre[curr];
    }


    int count = 0;
    // path to target
    cout << "\nDijkstra Path is: " << endl;
    cout << this->targets[path[path.size() - 1]].name;
    for (int i = path.size() - 2; i >= 0; i--) {
        cout << "->" << this->targets[path[i]].name;
        count++;
    }
    cout << "\nDijkstra Shortest distance is: " << dist[target] - count << endl;
}
int main() {
    cout << "Welcome to Github Friend Finder!"
            "\nUse our algorithm to find out how close you are to other Github users!" << endl;
    cout << "========================================================================" << endl;
    Graph g;
    vector<Node> targets;
    LoadTargets("github_target_1.csv", g.targets);

    g.Load(g.targets, "github_edges_1.csv");
    // g.printGraph();
    int src = 0;
    int target = 0;
    while (true) {
        cout << "Enter your userID: (Enter -1 to exit program)" << endl;
        cin >> src;
        if (src == -1) break;
        cout << "Enter the other user's ID: " << endl;
        cin >> target;
        g.bfsShortestPath(src, target);
        g.DijkstraShortestPath(src, target);
        cout << "--------------------------------------------------------" << endl;
    }
    cout<< "Goodbye!"<<endl;

    return 0;
}