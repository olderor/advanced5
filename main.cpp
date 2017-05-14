// Copyright 2017 olderor (Yevchenko Bohdan)

#include <iostream>
#include <vector>

// Represents infinity.
const int INTEGER_INFINITY = 10000000;

// Solves given problem.
struct matrix_network {
 public:
    // Initializes matrix with given data.
    explicit matrix_network(std::vector< std::vector<int> > &matrix);

    // Chooses elements in different rows and columns with minimum sum.
    // It sets minimum sum to the given integer
    // and sets column indexes for each row,
    // such that the element in the matrix in given row and column was choosen.
    // Also, it will be permutation of the columns indexes.
    void find_min_sum(int &min_sum, std::vector<int> &columns);

 private:
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

        // Initializes edge with an empty flow.
        edge(int from, int to, int cost, int capacity, int back) :
            from(from), to(to), cost(cost),
            flow(0), capacity(capacity), back(back) {}
    };

    // Number of rows, number of columns in the matrix.
    int matrix_size;

    // Stores values in the matrix.
    std::vector< std::vector<int> > matrix;

    // Number of vertices in the graph.
    int vertices_count;

    // Stores current cost value.
    int cost;

    // All edges in the graph.
    std::vector<edge*> edges;

    // State of graph where each vertex contains edges
    // that start from this vertex.
    std::vector< std::vector<edge*> > graph;

    // Minimum sum of the costs to retrieve a vertex.
    std::vector<int> distances;

    // Index of a parent vertex from which you came to the current vertex.
    std::vector<int> parents;

    // Initializes network by adding edges to graph.
    void initialize_matrix_network();

    // Adds new edge to the graph.
    void add_edge(
        const int from,
        const int to,
        const int capacity,
        const int cost);

    // Try to find maximum flow value with minimum cost value.
    // Return delta - difference between previous and new cost value.
    const int try_find_flow();

    // Finds max flow of min cost in the current graph.
    // Return the value of the min cost.
    const int find_min_cost_max_flow();
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

void matrix_network::add_edge(
    const int from,
    const int to,
    const int capacity,
    const int cost) {

    edge *first = new edge(from, to, cost, capacity, edges.size() + 1);
    edge *second = new edge(to, from, -cost, 0, edges.size());

    edges.push_back(first);
    edges.push_back(second);
    graph[from].push_back(first);
}

const int matrix_network::try_find_flow() {
    for (int i = 1; i < vertices_count; ++i) {
        distances[i] = INTEGER_INFINITY;
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
            if (distances[edges[index]->from] == INTEGER_INFINITY) {
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

    if (distances[vertices_count - 1] == INTEGER_INFINITY) {
        return 0;
    }
    int cur = vertices_count - 1;
    while (cur != 0) {
        edges[parents[cur]]->flow += 1;
        edges[edges[parents[cur]]->back]->flow -= 1;
        cur = edges[parents[cur]]->from;
    }
    return distances[vertices_count - 1];
}

const int matrix_network::find_min_cost_max_flow() {
    distances.resize(vertices_count);
    parents.resize(vertices_count);
    int cost_delta = 0;
    cost = try_find_flow();
    do {
        cost_delta = try_find_flow();
        cost += cost_delta;
    } while (cost_delta != 0);
    return cost;
}

matrix_network::matrix_network(
    std::vector< std::vector<int> > &matrix) {

    this->matrix_size = matrix.size();
    this->matrix = matrix;
}

void matrix_network::initialize_matrix_network() {
    vertices_count = 2 * matrix_size + 2;
    graph.resize(vertices_count);
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            add_edge(i + 1, matrix_size + 1 + j, 1, matrix[i][j]);
        }
        add_edge(0, i + 1, 1, 0);
        add_edge(matrix_size + 1 + i, vertices_count - 1, 1, 0);
    }
}

void matrix_network::find_min_sum(
    int &min_sum,
    std::vector<int> &columns) {

    initialize_matrix_network();
    min_sum = find_min_cost_max_flow();
    columns.clear();
    for (int i = 0; i < edges.size(); ++i) {
        if (edges[i]->flow > 0 && edges[i]->from != 0 &&
            edges[i]->to != vertices_count - 1) {
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

    matrix_network finder = matrix_network(matrix_data);
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
