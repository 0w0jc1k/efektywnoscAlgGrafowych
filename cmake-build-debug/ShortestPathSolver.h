#ifndef SHORTESTPATHSOLVER_H
#define SHORTESTPATHSOLVER_H

#include "MatrixGraph.h"
#include "ListGraph.h"
#include "MSTSolver.h"
#include <iostream>

using namespace std;

class ShortestPathSolver {
private:
    // Pomocnicza metoda do wyświetlania ścieżki od wierzchołka startowego
    static void printPath(int currentVertex, int* parents) {
        if (currentVertex == -1) return; //osiagnieto poczatek sciezki
        printPath(parents[currentVertex], parents); //wypisanie rodzica
        cout << currentVertex << " "; //wypisanie biezacego wierzcholka
    }

    // Pomocnicza metoda wyświetlająca wyniki
    static void printResults(int startV, int vertices, int* distances, int* parents, const char* algName) {
        cout << "\n=== Wyniki najkrotszych sciezek (" << algName << ") z wierzcholka " << startV << " ===" << endl;
        for (int i = 0; i < vertices; i++) { //przejscie po wszystkich wierzcholkach
            cout << "Do wierzcholka [" << i << "]: ";
            if (distances[i] == 2147483647) { //brak sciezki ze zrodla
                cout << "NIEOSIAGALNY" << endl;
            } else {
                cout << "Dystans: " << distances[i] << " | Sciezka: ";
                printPath(i, parents);
                cout << endl;
            }
        }
    }

public:
    // --- DIJKSTRA DLA MACIERZY (Przeszukanie liniowe O(V^2)) ---
    static void dijkstraMatrix(MatrixGraph* g, int startV) {
        if (!g) return;
        int V = g->vertices;
        int* distances = new int[V];
        int* parents = new int[V]; //poprzednik na najkrotszej sciezce
        bool* visited = new bool[V]; //czy wierzcholek zostal juz przetworzony

        for (int i = 0; i < V; i++) {
            distances[i] = 2147483647; // Nieskończoność
            parents[i] = -1;
            visited[i] = false;
        }
        distances[startV] = 0; //odleglosc od zrodla do samego siebie

        for (int count = 0; count < V - 1; count++) { //maksymalnie V - 1 iteracji
            int minDistance = 2147483647, u = -1;
            
            // Liniowe szukanie minimum z nieodwiedzonego wierzcholka
            for (int v = 0; v < V; v++) {
                if (!visited[v] && distances[v] <= minDistance) {
                    minDistance = distances[v];
                    u = v;
                }
            }

            if (u == -1 || distances[u] == 2147483647) break; //brak kolejnych osiagalnych wierzcholkow
            visited[u] = true; //dodanie wierzcholka do zbioru odwiedzonego

            // Bezpośredni dostęp do macierzy sąsiedztwa - relaksacja
            for (int v = 0; v < V; v++) {
                if (!visited[v] && g->matrix[u][v] > 0 && distances[u] + g->matrix[u][v] < distances[v]) {
                    parents[v] = u;
                    distances[v] = distances[u] + g->matrix[u][v];
                }
            }
        }

        printResults(startV, V, distances, parents, "Dijkstra - Macierz");

        delete[] distances;
        delete[] parents;
        delete[] visited;
    }

    // --- DIJKSTRA DLA LISTY (Z użyciem Kopca Binarnego O(E log V)) ---
    static void dijkstraList(ListGraph* g, int startV) {
        if (!g) return;
        int V = g->vertices;
        int* distances = new int[V];
        int* parents = new int[V];
        bool* visited = new bool[V];

        //inicjalizacja tablic
        for (int i = 0; i < V; i++) {
            distances[i] = 2147483647; //nieskonczonosc
            parents[i] = -1;
            visited[i] = false;
        }

        // Kopiec o pojemności równej liczbie krawędzi * 2 przechowujacy liczbe kandydatow
        MinHeap heap(g->edges * 2 + 1);
        distances[startV] = 0;
        heap.insert({startV, 0, -1}); //dodanie zrodla do kopca

        while (!heap.isEmpty()) {
            HeapNode current = heap.extractMin(); //pobranie najblizszego wierzcholka
            int u = current.v;

            if (visited[u]) continue;
            visited[u] = true;

            // Iteracja bezpośrednio po liście sąsiedztwa wskaźnikami ListNode
            ListNode* temp = g->adjList[u];
            while (temp) {
                int v = temp->target;
                int weight = temp->weight;

                //relaksacja
                if (!visited[v] && distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    parents[v] = u;
                    heap.insert({v, distances[v], u});
                }
                temp = temp->next;
            }
        }

        printResults(startV, V, distances, parents, "Dijkstra - Lista");

        delete[] distances;
        delete[] parents;
        delete[] visited;
    }

    // --- FORD-BELLMAN DLA MACIERZY ---
    static void fordBellmanMatrix(MatrixGraph* g, int startV) {
        if (!g) return;
        int V = g->vertices;
        int* distances = new int[V];
        int* parents = new int[V];

        //ladowanie wartosci
        for (int i = 0; i < V; i++) {
            distances[i] = 2147483647;
            parents[i] = -1;
        }
        distances[startV] = 0;

        // Relaksacja krawędzi V-1 razy bezpośrednio z macierzy dla braku ujemnych wartosci
        for (int i = 1; i <= V - 1; i++) {
            for (int u = 0; u < V; u++) {
                for (int v = 0; v < V; v++) {
                    if (g->matrix[u][v] > 0 && distances[u] != 2147483647) {
                        if (distances[u] + g->matrix[u][v] < distances[v]) {
                            distances[v] = distances[u] + g->matrix[u][v];
                            parents[v] = u;
                        }
                    }
                }
            }
        }

        // Sprawdzenie ujemnych cykli
        for (int u = 0; u < V; u++) {
            for (int v = 0; v < V; v++) {
                if (g->matrix[u][v] > 0 && distances[u] != 2147483647) {
                    if (distances[u] + g->matrix[u][v] < distances[v]) {
                        cout << "[Ford-Bellman Macierz] Graf zawiera ujemny cykl!" << endl;
                        delete[] distances; delete[] parents;
                        return;
                    }
                }
            }
        }

        printResults(startV, V, distances, parents, "Ford-Bellman - Macierz");
        delete[] distances; delete[] parents;
    }

    // --- FORD-BELLMAN DLA LISTY ---
    static void fordBellmanList(ListGraph* g, int startV) {
        if (!g) return;
        int V = g->vertices;
        int* distances = new int[V];
        int* parents = new int[V];

        for (int i = 0; i < V; i++) {
            distances[i] = 2147483647;
            parents[i] = -1;
        }
        distances[startV] = 0;

        // Relaksacja krawędzi V-1 razy bezpośrednio z dynamicznej listy sąsiadów
        for (int i = 1; i <= V - 1; i++) {
            for (int u = 0; u < V; u++) {
                ListNode* temp = g->adjList[u];
                while (temp) {
                    int v = temp->target;
                    int weight = temp->weight;
                    if (distances[u] != 2147483647 && distances[u] + weight < distances[v]) {
                        distances[v] = distances[u] + weight;
                        parents[v] = u;
                    }
                    temp = temp->next;
                }
            }
        }

        // Sprawdzenie ujemnych cykli
        for (int u = 0; u < V; u++) {
            ListNode* temp = g->adjList[u];
            while (temp) {
                int v = temp->target;
                int weight = temp->weight;
                if (distances[u] != 2147483647 && distances[u] + weight < distances[v]) {
                    cout << "[Ford-Bellman Lista] Graf zawiera ujemny cykl!" << endl;
                    delete[] distances; delete[] parents;
                    return;
                }
                temp = temp->next;
            }
        }

        printResults(startV, V, distances, parents, "Ford-Bellman - Lista");
        delete[] distances; delete[] parents;
    }
};

#endif