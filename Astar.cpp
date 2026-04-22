#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

// Структура для представления узла на сетке
struct Node {
    int x, y;
    double gCost; // Стоимость пути от старта
    double fCost; // gCost + эвристика (оценка до финиша)

    // Перегрузка оператора для priority_queue (нам нужен узел с минимальным fCost)
    bool operator>(const Node& other) const {
        return fCost > other.fCost;
    }
};

// Функция эвристики (Евклидово расстояние)
double heuristic(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// Проверка, находится ли узел в пределах сетки и проходим ли он
bool isValid(int x, int y, const vector<vector<int>>& grid) {
    return (x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() && grid[x][y] != -1);
}

// Основной алгоритм A*
void aStarSearch(const vector<vector<int>>& grid, pair<int, int> start, pair<int, int> goal) {
    // Направления движения (вверх, вниз, влево, вправо)
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    // Очередь с приоритетом для открытых узлов
    priority_queue<Node, vector<Node>, greater<Node>> openList;
    
    // Матрица закрытых узлов (посещенных)
    vector<vector<bool>> closedList(grid.size(), vector<bool>(grid[0].size(), false));

    openList.push({start.first, start.second, 0.0, heuristic(start.first, start.second, goal.first, goal.second)});

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();

        int x = current.x;
        int y = current.y;

        // Если достигли цели
        if (x == goal.first && y == goal.second) {
            cout << "Оптимальный путь найден! Стоимость: " << current.gCost << endl;
            return;
        }

        if (closedList[x][y]) continue;
        closedList[x][y] = true;

        // Проверяем соседей
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (isValid(nx, ny, grid) && !closedList[nx][ny]) {
                // grid[nx][ny] выступает в роли веса ребра (сложность рельефа)
                double newGCost = current.gCost + grid[nx][ny]; 
                double newFCost = newGCost + heuristic(nx, ny, goal.first, goal.second);
                
                openList.push({nx, ny, newGCost, newFCost});
            }
        }
    }
    cout << "Путь не найден." << endl;
}

int main() {
    // Сетка: 1 - обычная проходимость, 5 - сложный рельеф, -1 - препятствие
    vector<vector<int>> grid = {
        {1, 1, 1, 1, 1},
        {1, -1, -1, 5, 1},
        {1, 1, 5, 1, 1},
        {1, -1, 1, -1, 1},
        {1, 1, 1, 1, 1}
    };

    pair<int, int> start = {0, 0};
    pair<int, int> goal = {4, 4};

    aStarSearch(grid, start, goal);
    return 0;
}