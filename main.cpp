// Copyright 2017 olderor (Yevchenko Bohdan)

#include <iostream>
#include <vector>

// Finds max flow with min cost in the given graph.
struct min_cost_max_flow_finder {
public:
    // Representation of the edge in the graph.
    struct edge {
        // Index of the start vertex.
        int from;
        // Index of the end vertex.
        int to;
        // Amount, which is required to use the edge.
        int cost;
        // Current value of the flow in the edge.
        int flow;
        // Maximum value of the flow in the edge.
        int capacity;
        // Index of the edge to go back from end vertex to start vertex.
        int back;
    };
    // Initializes with given number of vertices in the graph.
    explicit min_cost_max_flow_finder(const int vertices_count);
    // Adds new edge to the graph.
    void add_edge(
        const int from,
        const int to,
        const int capacity,
        const int cost);
    // Finds max flow of min cost in the current graph.
    // Return the value of the min cost.
    const int find_min_cost_max_flow();
    // Gets current state of the edges.
    // Returns current state of each edge in the graph.
    // Use to get flow values of the edges you need.
    std::vector<edge*> get_edges_state() const;
    // Gets number of vertices in the graph.
    const int get_vertices_count() const;

private:
    // Number of vertices in the graph.
    int verices_count;
    // Stores current cost value.
    int cost;
    // Represents infinity.
    const int inf = 10000000;
    // All edges in the graph.
    std::vector<edge*> edges;
    // State of graph where each vertex contains edges that start from this vertex.
    std::vector< std::vector<edge*> > graph;
    // Minimum sum of the costs to retrieve a vertex. 
    std::vector<int> distances;
    // Index of a parent vertex from which you came to the current vertex. 
    std::vector<int> parents;
    // Try to find maximum flow value with minimum cost value.
    // Return delta - difference between previous and new cost value.
    const int try_find_flow();
};

// Solves given problem.
struct matrix_min_cost_finder {
public:
    // Initializes matrix with given data.
    explicit matrix_min_cost_finder(std::vector< std::vector<int> > &matrix);
    // Chooses elements in different rows and columns with minimum sum.
    // It sets minimum sum to the given integer and sets column indexes for each row,
    // such that the element in the matrix in given row and column was choosen.
    // Also, it will be permutation of the columns indexes.
    void find_min_sum(int &min_sum, std::vector<int> &columns);
private:
    // Finds min cost max flow in the graph.
    min_cost_max_flow_finder *finder;
    // Number of rows, number of columns in the matrix. 
    int matrix_size;
    // Stores values in the matrix.
    std::vector< std::vector<int> > matrix;

    // Initializes finder with a graph.
    void initialize_finder();
};

// Gets data from input stream.
void read_data(
    std::istream &_Istr,
    std::vector< std::vector<int> > &matrix_data);

// Puts data to output stream.
void print_data(
    std::ostream &_Ostr,
    int &sum,
    std::vector<int> &columns);

// Gets answer to the given problem using given data.
void solve(
    std::vector< std::vector<int> > &matrix_data,
    int &sum,
    std::vector<int> &columns);

// Entry point.
int main();

min_cost_max_flow_finder::min_cost_max_flow_finder(const int vertices_count) {
    this->verices_count = vertices_count;
    graph.resize(verices_count);
}

void min_cost_max_flow_finder::add_edge(
    const int from,
    const int to,
    const int capacity,
    const int cost) {
    edge *first = new edge();
    first->from = from;
    first->to = to;
    first->capacity = capacity;
    first->cost = cost;
    first->back = edges.size() + 1;

    edge *second = new edge();
    second->from = to;
    second->to = from;
    second->cost = -cost;
    second->back = edges.size();

    edges.push_back(first);
    edges.push_back(second);
    graph[from].push_back(first);
}

const int min_cost_max_flow_finder::try_find_flow() {
    for (int i = 1; i < verices_count; ++i) {
        distances[i] = inf;
        parents[i] = -1;
    }
    parents[0] = -1;
    distances[0] = 0;
    bool found = true;
    while (found) {
        found = false;
        for (int index = 0; index < edges.size(); ++index) {
            if (edges[index]->flow >= edges[index]->capacity) {
                continue;
            }
            if (distances[edges[index]->from] == inf) {
                continue;
            }
            if (distances[edges[index]->to] >
                distances[edges[index]->from] + edges[index]->cost) {
                distances[edges[index]->to] =
                    distances[edges[index]->from] + edges[index]->cost;
                parents[edges[index]->to] = index;
                found = true;
            }
        }
    }

    if (distances[verices_count - 1] == inf) {
        return 0;
    }
    int cur = verices_count - 1;
    while (cur != 0) {
        edges[parents[cur]]->flow += 1;
        edges[edges[parents[cur]]->back]->flow -= 1;
        cur = edges[parents[cur]]->from;
    }
    return distances[verices_count - 1];
}

const int min_cost_max_flow_finder::find_min_cost_max_flow() {
    distances.resize(verices_count);
    parents.resize(verices_count);
    int cost_delta = 0;
    cost = try_find_flow();
    do {
        cost_delta = try_find_flow();
        cost += cost_delta;
    } while (cost_delta != 0);
    return cost;
}

const int min_cost_max_flow_finder::get_vertices_count() const {
    return verices_count;
}

std::vector<min_cost_max_flow_finder::edge*>
min_cost_max_flow_finder::get_edges_state() const {
    return edges;
}

matrix_min_cost_finder::matrix_min_cost_finder(
    std::vector< std::vector<int> > &matrix) {
    this->matrix_size = matrix.size();
    this->matrix = matrix;
}

void matrix_min_cost_finder::initialize_finder() {
    const int vertices_count = 2 * matrix_size + 2;
    finder = new min_cost_max_flow_finder(vertices_count);
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            finder->add_edge(i + 1, matrix_size + 1 + j, 1, matrix[i][j]);
        }
        finder->add_edge(0, i + 1, 1, 0);
        finder->add_edge(matrix_size + 1 + i, vertices_count - 1, 1, 0);
    }
}

void matrix_min_cost_finder::find_min_sum(
    int &min_sum,
    std::vector<int> &columns) {
    initialize_finder();
    min_sum = finder->find_min_cost_max_flow();
    std::vector<min_cost_max_flow_finder::edge*> edges =
        finder->get_edges_state();
    columns.clear();
    for (int i = 0; i < edges.size(); ++i) {
        if (edges[i]->flow > 0 && edges[i]->from != 0 &&
            edges[i]->to != finder->get_vertices_count() - 1) {
            columns.push_back(edges[i]->to - matrix_size);
        }
    }
}

void read_data(
    std::istream &_Istr,
    std::vector< std::vector<int> > &matrix_data) {
    int matrix_size;
    _Istr >> matrix_size;
    matrix_data.resize(matrix_size);
    for (int i = 0; i < matrix_size; ++i) {
        matrix_data[i].resize(matrix_size);
        for (int j = 0; j < matrix_size; ++j) {
            _Istr >> matrix_data[i][j];
        }
    }
}

void print_data(
    std::ostream &_Ostr,
    int &sum,
    std::vector<int> &columns) {
    _Ostr << sum << '\n';
    for (int i = 0; i < columns.size(); ++i) {
        _Ostr << columns[i] << ' ';
    }
    _Ostr << '\n';
}

void solve(
    std::vector< std::vector<int> > &matrix_data,
    int &sum,
    std::vector<int> &columns) {
    matrix_min_cost_finder finder = matrix_min_cost_finder(matrix_data);
    finder.find_min_sum(sum, columns);
}

int main() {
    std::vector< std::vector<int> > matrix_data;

    read_data(std::cin, matrix_data);

    int sum;
    std::vector<int> columns;
    solve(matrix_data, sum, columns);

    print_data(std::cout, sum, columns);

    return 0;
}
