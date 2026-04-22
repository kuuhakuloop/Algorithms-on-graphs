#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Структура для представления возможного участка трубопровода (ребра)
struct Edge {
    int u, v;
    double cost; // Стоимость прокладки трубы между u и v

    // Сортировка по стоимости
    bool operator<(const Edge& other) const {
        return cost < other.cost;
    }
};

// Класс для системы непересекающихся множеств (Disjoint Set Union)
class DSU {
private:
    vector<int> parent;
    vector<int> rank;

public:
    DSU(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; ++i) {
            parent[i] = i; // Изначально каждый узел сам себе родитель
        }
    }

    // Поиск корня с оптимизацией сжатия пути
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }

    // Объединение двух множеств (ранговая эвристика)
    void unite(int i, int j) {
        int rootI = find(i);
        int rootJ = find(j);

        if (rootI != rootJ) {
            if (rank[rootI] < rank[rootJ]) {
                parent[rootI] = rootJ;
            } else if (rank[rootI] > rank[rootJ]) {
                parent[rootJ] = rootI;
            } else {
                parent[rootJ] = rootI;
                rank[rootI]++;
            }
        }
    }
};

// Алгоритм Краскала
void kruskalMST(int vertices, vector<Edge>& edges) {
    // 1. Сортируем все возможные отрезки труб по возрастанию стоимости
    sort(edges.begin(), edges.end());

    DSU dsu(vertices);
    vector<Edge> resultMST;
    double totalCost = 0;

    // 2. Идем по отсортированным ребрам
    for (const auto& edge : edges) {
        int rootU = dsu.find(edge.u);
        int rootV = dsu.find(edge.v);

        // Если концы трубы не связаны (не образуют цикл) - прокладываем трубу
        if (rootU != rootV) {
            resultMST.push_back(edge);
            totalCost += edge.cost;
            dsu.unite(rootU, rootV);
        }
    }

    // Вывод результатов
    cout << "Спроектированная сеть трубопроводов (Остовное дерево):\n";
    for (const auto& edge : resultMST) {
        cout << "Узел " << edge.u << " -- Узел " << edge.v << " (Стоимость: " << edge.cost << ")\n";
    }
    cout << "Общая стоимость сети: " << totalCost << endl;
}

int main() {
    int vertices = 4; // Количество узлов (потребителей/источников)
    
    // Возможные пути прокладки труб (u, v, cost)
    vector<Edge> edges = {
        {0, 1, 10.5},
        {0, 2, 6.0},
        {0, 3, 5.0},
        {1, 3, 15.0},
        {2, 3, 4.0}
    };

    kruskalMST(vertices, edges);
    return 0;
}