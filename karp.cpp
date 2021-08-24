#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <iterator>

using namespace std;

struct Graph {
private:
    int lines = 0, columns = 0, max_flow = 0;
    vector<int> source_edges, sink_edges;
    vector<vector<int>> edges;
    vector<char> minimum_cut;

public:
    Graph &calculateFlow();
    bool bfs();
    friend istream &operator>>(istream &is, Graph &self);
    friend ostream &operator<<(ostream &os, Graph &self);
};

istream &operator>>(istream &is, Graph &self) {
    is >> self.lines >> self.columns;

    self.source_edges.reserve(self.lines * self.columns);
    self.sink_edges.reserve(self.lines * self.columns);
    copy_n(istream_iterator<int>(is), self.lines * self.columns, back_inserter(self.source_edges));
    copy_n(istream_iterator<int>(is), self.lines * self.columns, back_inserter(self.sink_edges));

    self.edges.assign(self.lines * self.columns, vector<int>(4, -1));
    for (int i = 0; i < self.lines; i++)
        for (int j = 0, value; j < self.columns - 1; j++) {
            is >> value;
            self.edges[i * self.columns + j][3] = value;
            self.edges[i * self.columns + j + 1][2] = value;
        }
    for (int i = 0; i < self.lines - 1; i++)
        for (int j = 0, value; j < self.columns; j++) {
            is >> value;
            self.edges[i * self.columns + j][1] = value;
            self.edges[(i + 1) * self.columns + j][0] = value;
        }

    return is;
}

Graph &Graph::calculateFlow() {
    for (int i = 0; i < lines * columns; i++) {
        int sent = min(source_edges[i], sink_edges[i]);
        max_flow += sent;
        source_edges[i] -= sent;
        sink_edges[i] -= sent;
    }

    while (bfs());

    return *this;
}

bool Graph::bfs() {
    vector<int> predecessor(lines * columns + 1, -2);
    queue<int> bfs_queue;
    for (int i = 0; i < lines * columns; i++)
        if (source_edges[i] > 0) {
            predecessor[i] = -1;
            bfs_queue.push(i);
        }
    while (!bfs_queue.empty()) {
        int v = bfs_queue.front();
        bfs_queue.pop();
        if (sink_edges[v] > 0) {
            predecessor[lines * columns] = v;
            break;
        }
        if (edges[v][0] != -1 && edges[v][0] > 0 && predecessor[v - columns] == -2) {
            predecessor[v - columns] = v;
            bfs_queue.push(v - columns);
        }
        if (edges[v][1] != -1 && edges[v][1] > 0 && predecessor[v + columns] == -2) {
            predecessor[v + columns] = v;
            bfs_queue.push(v + columns);
        }
        if (edges[v][2] != -1 && edges[v][2] > 0 && predecessor[v - 1] == -2) {
            predecessor[v - 1] = v;
            bfs_queue.push(v - 1);
        }
        if (edges[v][3] != -1 && edges[v][3] > 0 && predecessor[v + 1] == -2) {
            predecessor[v + 1] = v;
            bfs_queue.push(v + 1);
        }
    }

    if (predecessor[lines * columns] != -2) {
        stack<int> bfs_stack;
        int prev = predecessor[lines * columns];
        int sent = sink_edges[prev];
        while (predecessor[prev] != -1) {
            if (prev + columns == predecessor[prev] && edges[predecessor[prev]][0] < sent)
                sent = edges[predecessor[prev]][0];
            else if (prev - columns == predecessor[prev] && edges[predecessor[prev]][1] < sent)
                sent = edges[predecessor[prev]][1];
            else if (prev + 1 == predecessor[prev] && edges[predecessor[prev]][2] < sent)
                sent = edges[predecessor[prev]][2];
            else if (prev - 1 == predecessor[prev] && edges[predecessor[prev]][3] < sent)
                sent = edges[predecessor[prev]][3];
            bfs_stack.push(prev);
            prev = predecessor[prev];
        }
        if (source_edges[prev] < sent)
            sent = source_edges[prev];
        bfs_stack.push(prev);

        max_flow += sent;
        int curr = bfs_stack.top();
        bfs_stack.pop();
        source_edges[curr] -= sent;
        while (!bfs_stack.empty()) {
            if (curr - columns == bfs_stack.top()) {
                edges[curr][0] -= sent;
                edges[curr - columns][1] += sent;
            } else if (curr + columns == bfs_stack.top()) {
                edges[curr][1] -= sent;
                edges[curr + columns][0] += sent;
            } else if (curr - 1 == bfs_stack.top()) {
                edges[curr][2] -= sent;
                edges[curr - 1][3] += sent;
            } else if (curr + 1 == bfs_stack.top()) {
                edges[curr][3] -= sent;
                edges[curr + 1][2] += sent;
            }
            curr = bfs_stack.top();
            bfs_stack.pop();
        }
        sink_edges[curr] -= sent;

        return true;
    }

    minimum_cut.assign(lines * columns, 'C');
    for (int i = 0; i < lines * columns; i++)
        if (predecessor[i] == -2)
            minimum_cut[i] = 'P';

    return false;
}

ostream &operator<<(ostream &os, Graph &self) {
    os << self.max_flow << "\n\n";
    for (auto iter = self.minimum_cut.begin(), end = self.minimum_cut.end(); iter != end; iter += self.columns) {
        copy_n(iter, self.columns, ostream_iterator<char>(os, " "));
        os << '\n';
    }
    return os;
}

int main() {
    ios::sync_with_stdio(false);
    Graph graph;
    cin >> graph;
    cout << graph.calculateFlow();
    return 0;
}