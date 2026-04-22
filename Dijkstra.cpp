#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

// Структура для хранения ребра: {соседний_узел, вес_ребра}
typedef pair<int, double> Edge;

void dijkstra(int start, int vertices, const vector<vector<Edge>>& adj) {
    // Вектор кратчайших расстояний от старта, инициализируем бесконечностью
    vector<double> dist(vertices, INT_MAX);
    dist[start] = 0;

    // Очередь с приоритетом: {расстояние, узел}. 
    // greater<> нужен, чтобы в топе всегда был узел с наименьшим путем
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({0.0, start});

    while (!pq.empty()) {
        double d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        // Если мы уже нашли путь короче, пропускаем этот
        if (d > dist[u]) continue;

        // Перебираем соседей
        for (auto& edge : adj[u]) {
            int v = edge.first;
            double weight = edge.second;

            // Релаксация ребра
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    // Вывод результатов
    cout << "Кратчайшие расстояния от узла " << start << ":\n";
    for (int i = 0; i < vertices; ++i) {
        cout << "До узла " << i << ": " << (dist[i] == INT_MAX ? -1 : dist[i]) << endl;
    }
}

int main() {
    int vertices = 5;
    vector<vector<Edge>> adj(vertices);

    // Добавляем ребра (граф неориентированный)
    adj[0].push_back({1, 10.0});
    adj[1].push_back({0, 10.0});
    
    adj[0].push_back({2, 3.0});
    adj[2].push_back({0, 3.0});
    
    adj[1].push_back({2, 1.0});
    adj[2].push_back({1, 1.0});
    
    adj[1].push_back({3, 2.0});
    adj[3].push_back({1, 2.0});
    
    adj[2].push_back({3, 9.0});
    adj[3].push_back({2, 9.0});

    dijkstra(0, vertices, adj);

    return 0;
}