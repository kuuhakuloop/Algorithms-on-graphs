#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// Константа для обозначения бесконечности (если пути нет)
const double INF = 1e9;

// Функция алгоритма Прима
void primMST(int vertices, const vector<vector<pair<int, double>>>& adj) {
    // Очередь с приоритетом: хранит пары {стоимость_прокладки, узел_назначения}
    // greater используется для того, чтобы наверху очереди всегда был путь с минимальной стоимостью
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    // Вектор для отслеживания узлов, уже включенных в трубопроводную сеть (MST)
    vector<bool> inMST(vertices, false);

    // Вектор для хранения стоимости подключения каждого узла (для вывода)
    vector<double> minCost(vertices, INF);

    // Вектор для восстановления самой структуры сети (какой узел к какому подключен)
    vector<int> parent(vertices, -1);

    // Начинаем с нулевого узла (например, главного распределительного центра)
    int startNode = 0;
    pq.push({0.0, startNode});
    minCost[startNode] = 0.0;

    double totalCost = 0.0;

    while (!pq.empty()) {
        // Извлекаем узел с минимальной стоимостью подключения
        int u = pq.top().second;
        double cost = pq.top().first;
        pq.pop();

        // Если узел уже в сети, пропускаем его (чтобы избежать циклов)
        if (inMST[u]) continue;

        // Добавляем узел в сеть
        inMST[u] = true;
        totalCost += cost;

        // Перебираем всех соседей добавленного узла
        for (auto& edge : adj[u]) {
            int v = edge.first;        // Соседний узел
            double weight = edge.second; // Стоимость трубы до соседа

            // Если сосед еще не в сети и новая труба дешевле, чем известные ранее варианты
            if (!inMST[v] && weight < minCost[v]) {
                minCost[v] = weight;
                pq.push({weight, v});
                parent[v] = u; // Запоминаем, что узел v выгоднее всего подключить к узлу u
            }
        }
    }

    // Вывод результатов проектирования сети
    cout << "Спроектированная сеть (Алгоритм Прима):\n";
    for (int i = 1; i < vertices; ++i) {
        if (parent[i] != -1) {
            cout << "Узел " << parent[i] << " -- Узел " << i << " (Стоимость: " << minCost[i] << ")\n";
        }
    }
    cout << "Общая стоимость прокладки сети: " << totalCost << endl;
}

int main() {
    int vertices = 5; // Количество узлов в сети

    // Граф представлен списком смежности: adj[u] содержит пары {v, cost}
    // Это оптимально с точки зрения управления памятью
    vector<vector<pair<int, double>>> adj(vertices);

    // Добавление возможных участков трубопровода (граф неориентированный, трубы идут в обе стороны)
    // Узел 0 соединен с узлом 1 (стоимость 2.0) и узлом 3 (стоимость 6.0)
    adj[0].push_back({1, 2.0});
    adj[1].push_back({0, 2.0});
    
    adj[0].push_back({3, 6.0});
    adj[3].push_back({0, 6.0});

    adj[1].push_back({2, 3.0});
    adj[2].push_back({1, 3.0});

    adj[1].push_back({3, 8.0});
    adj[3].push_back({1, 8.0});

    adj[1].push_back({4, 5.0});
    adj[4].push_back({1, 5.0});

    adj[2].push_back({4, 7.0});
    adj[4].push_back({2, 7.0});

    adj[3].push_back({4, 9.0});
    adj[4].push_back({3, 9.0});

    primMST(vertices, adj);

    return 0;
}