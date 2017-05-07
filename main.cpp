// Copyright 2017 olderor (Yevchenko Bohdan)

#include <iostream>
#include <vector>

struct min_cost_max_flow_finder {
 public:
    struct edge {
        int from, to;
        int cost;
        int flow;
        int capacity;
        int back;
    };

    explicit min_cost_max_flow_finder(const int vertices_count);
    void add_edge(
        const int from,
        const int to,
        const int capacity,
        const int cost);
    const int find_min_cost_max_flow();
    std::vector<edge*> get_edges_state() const;
    const int get_vertices_count() const;

 private:
    int verices_count;
    int flow;
    const int inf = 10000000;
    std::vector<edge*> edges;
    std::vector< std::vector<edge*> > graph;
    std::vector<int> distances;
    std::vector<int> parents;

    const int try_find_flow();
};

struct matrix_min_cost_finder {
 public:
    explicit matrix_min_cost_finder(std::vector< std::vector<int> > &matrix);
    void find_min_sum(int &min_sum, std::vector<int> &columns);
 private:
    min_cost_max_flow_finder *finder;
    int matrix_size;
    std::vector< std::vector<int> > matrix;

    void initialize_finder();
};

void read_data(
    std::istream &_Istr,
    std::vector< std::vector<int> > &matrix_data);

void print_data(
    std::ostream &_Ostr,
    int &sum,
    std::vector<int> &columns);

void solve(
    std::vector< std::vector<int> > &matrix_data,
    int &sum,
    std::vector<int> &columns);

int main();

min_cost_max_flow_finder::min_cost_max_flow_finder(const int vertices_count) {
    this->verices_count = vertices_count;
    graph.resize(verices_count);
    distances.resize(verices_count);
    parents.resize(verices_count);
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
    int flow_delta = 0;
    flow = 0;
    do {
        flow_delta = try_find_flow();
        flow += flow_delta;
    } while (flow_delta != 0);
    return flow;
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
