#ifndef MAXFLOWSOLVER_H
#define MAXFLOWSOLVER_H

#include "MatrixGraph.h"
#include "ListGraph.h"
#include <iostream>

using namespace std;

// Prosta implementacja kolejki FIFO na tablicy dynamicznej do obsługi BFS
class FlowQueue {
    int* arr; //dynamiczna tablica z elementami kolejki
    int frontIdx; //pierwszy index do odczytu
    int rearIdx; //index dodania nowego elementu
    int capacity; //maksymalny rozmiar kolejki
public:
    FlowQueue(int cap) { //konstruktor kolejki o zadanym rozmiarze
        capacity = cap;
        arr = new int[cap];
        frontIdx = 0;
        rearIdx = 0;
    }
    ~FlowQueue() { delete[] arr; }
    void push(int x) { arr[rearIdx++] = x; } //dodanie elementu na koniec kolejki
    int pop() { return arr[frontIdx++]; } //pobranie elementu z poczatku kolejki
    bool isEmpty() { return frontIdx == rearIdx; }
};

class MaxFlowSolver {
private:
    // BFS dla struktury macierzowej - przeszukanie grafu wszerz
    //zadaniem jest znalezienie sciezki od zrodla do ujscia w aktualnej sieci
    //jesli istnije - true, jak nie - false
    static bool bfsMatrix(int** residualMatrix, int V, int source, int sink, int* parent) {
        bool* visited = new bool[V]; //informacja czy dany wierzcholek zostal odwiedzony
        for (int i = 0; i < V; i++) visited[i] = false; //poczatkowo zaden nie zostal odwiedzony

        FlowQueue q(V); //tworzenie kolejki bfs
        q.push(source); //poczatek przeszukania od zrodla
        visited[source] = true; //zrodlo zostalo odwiedzone
        parent[source] = -1; // zrodlo nie ma rodzica

        while (!q.isEmpty()) { //dopoki istnieja wierzcholki do przetworzenia
            int u = q.pop(); //pobranie kolejnego wierzcholka

            for (int v = 0; v < V; v++) { //sprawdzenie wszystkich sasiadow
                if (!visited[v] && residualMatrix[u][v] > 0) {
                    if (v == sink) {
                        parent[v] = u;
                        delete[] visited;
                        return true;
                    }
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                }
            }
        }
        bool pathFound = visited[sink];
        delete[] visited;
        return pathFound;
    }

    // BFS dla struktury listowej (używa pomocniczej macierzy rezydualnej dla łatwości aktualizacji krawędzi wstecznych)
    static bool bfsList(int** residualMatrix, int V, int source, int sink, int* parent, ListGraph* g) {
        bool* visited = new bool[V];
        for (int i = 0; i < V; i++) visited[i] = false;

        FlowQueue q(V);
        q.push(source);
        visited[source] = true;
        parent[source] = -1;

        while (!q.isEmpty()) {
            int u = q.pop();

            // Przeszukujemy tylko istniejące relacje w oparciu o strukturę listową oryginalnego grafu
            // bierzemy pod uwagę krawędzie oryginalne oraz potencjalne wsteczne
            for (int v = 0; v < V; v++) {
                if (!visited[v] && residualMatrix[u][v] > 0) {
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                    if (v == sink) {
                        delete[] visited;
                        return true;
                    }
                }
            }
        }
        bool pathFound = visited[sink];
        delete[] visited;
        return pathFound;
    }

public:
    // Algorytm Forda-Fulkersona (Edmondsa-Karpa) dla reprezentacji macierzowej
    static void fordFulkersonMatrix(MatrixGraph* g, int source, int sink) {
        if (!g) return;
        int V = g->vertices;

        // Alokacja macierzy sieci rezydualnej
        int** residualMatrix = new int*[V];
        for (int i = 0; i < V; i++) {
            residualMatrix[i] = new int[V];
            for (int j = 0; j < V; j++) {
                residualMatrix[i][j] = g->matrix[i][j];
            }
        }

        int* parent = new int[V];
        int maxFlow = 0;

        // Dopóki istnieje ścieżka rozszerzająca od źródła do ujścia
        while (bfsMatrix(residualMatrix, V, source, sink, parent)) {
            // Znajdź maksymalną przepustowość (wąskie gardło) na znalezionej ścieżce
            int pathFlow = 2147483647;
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                if (residualMatrix[u][v] < pathFlow) {
                    pathFlow = residualMatrix[u][v];
                }
            }

            // Aktualizacja przepustowości szczątkowych krawędzi i krawędzi wstecznych
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                residualMatrix[u][v] -= pathFlow;
                residualMatrix[v][u] += pathFlow;
            }

            maxFlow += pathFlow;
        }

        cout << "\n=== Wynik Maksymalnego Przeplywu (Ford-Fulkerson - Macierz) ===" << endl;
        cout << "Maksymalny przeplyw ze zrodla " << source << " do ujscia " << sink << " wynosi: " << maxFlow << endl;

        // Zwolnienie pamięci
        for (int i = 0; i < V; i++) delete[] residualMatrix[i];
        delete[] residualMatrix;
        delete[] parent;
    }

    // Algorytm Forda-Fulkersona (Edmondsa-Karpa) dla reprezentacji listowej
    static void fordFulkersonList(ListGraph* g, int source, int sink) {
        if (!g) return;
        int V = g->vertices;

        // Przepisujemy przepustowości początkowe z listy do struktury roboczej sieci rezydualnej
        int** residualMatrix = new int*[V];
        for (int i = 0; i < V; i++) {
            residualMatrix[i] = new int[V];
            for (int j = 0; j < V; j++) residualMatrix[i][j] = 0;
        }

        for (int u = 0; u < V; u++) {
            ListNode* temp = g->adjList[u];
            while (temp) {
                residualMatrix[u][temp->target] = temp->weight;
                temp = temp->next;
            }
        }

        int* parent = new int[V];
        int maxFlow = 0;

        while (bfsList(residualMatrix, V, source, sink, parent, g)) {
            int pathFlow = 2147483647;
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                if (residualMatrix[u][v] < pathFlow) {
                    pathFlow = residualMatrix[u][v];
                }
            }

            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                residualMatrix[u][v] -= pathFlow;
                residualMatrix[v][u] += pathFlow;
            }

            maxFlow += pathFlow;
        }

        cout << "\n=== Wynik Maksymalnego Przeplywu (Ford-Fulkerson - Lista) ===" << endl;
        cout << "Maksymalny przeplyw ze zrodla " << source << " do ujscia " << sink << " wynosi: " << maxFlow << endl;

        for (int i = 0; i < V; i++) delete[] residualMatrix[i];
        delete[] residualMatrix;
        delete[] parent;
    }
};

#endif