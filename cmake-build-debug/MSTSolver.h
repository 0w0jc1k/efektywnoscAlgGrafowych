#ifndef MSTSOLVER_H
#define MSTSOLVER_H //minimalne drzewo rozpinajace - podzbior krawedzi laczacych wszystkie wierzcholki przy uzyciu minimalnej mozliwej sumy wag krawedzi
//nie tworzac przy tym zadnych cykli

#include "MatrixGraph.h"
#include "ListGraph.h"
#include <iostream>

class MinHeap { //implementacja kopca binarnego dla Prima
    HeapNode* data;
    int capacity; // Maksymalny rozmiar
    int size; // Aktualny rozmiar
public:
    MinHeap(int cap) { capacity = cap; size = 0; data = new HeapNode[cap]; } // Konstruktor
    ~MinHeap() { delete[] data; } // Destruktor
    bool isEmpty() { return size == 0; }
    void insert(HeapNode node) { // Wstawianie do kopca
        int i = size++;
        while (i > 0 && data[(i - 1) / 2].weight > node.weight) { //budowa kopca
            data[i] = data[(i - 1) / 2]; //rodzic idzie na miejsce potomka
            i = (i - 1) / 2; //potomek na miejsce rodzica
        }
        data[i] = node; //wstaw wierzcholek w miejsce
    }
    HeapNode extractMin() { // Wyciaganie minimum
        HeapNode min = data[0]; //najpierw minimum to korzen
        data[0] = data[--size]; //ostatni na miejsce korzenia
        int i = 0, j; // Indeksy do naprawy kopca w dol
        while ((j = 2 * i + 1) < size) { // Dopoki ma dzieci
            if (j + 1 < size && data[j + 1].weight < data[j].weight) j++; // Wybierz mniejsze dziecko
            if (data[i].weight <= data[j].weight) break; //koniec jesli jest okej
            HeapNode tmp = data[i]; data[i] = data[j]; data[j] = tmp; // Zamiana
            i = j; //idziemy nizej
        }
        return min; // Zwroc najmniejszy
    }
};

class DSU {//zbiory rozlaczne dla kruskala
    int* parent;
public:
    DSU(int n) { parent = new int[n]; for (int i = 0; i < n; i++) parent[i] = i; } // Każdy jest swoim rodzicem
    ~DSU() { delete[] parent; }
    int find(int i) { // Znajdowanie reprezentanta z kompresja sciezki
        if (parent[i] == i) return i; // Korzen znaleziony
        return parent[i] = find(parent[i]); // Kompresja: przypisanie bezposrednio do korzenia
    }
    void unite(int i, int j) { // Laczenie zbiorow
        int root_i = find(i); // Znajdz korzen i
        int root_j = find(j); // Znajdz korzen j
        if (root_i != root_j) parent[root_i] = root_j; // Polacz pod jedno drzewo
    }
};

void sortEdges(Edge* edges, int count) { // Proste sortowanie przez wstawianie dla krawedzi (Kruskal)
    for (int i = 1; i < count; i++) { // Petla po krawedziach
        Edge key = edges[i]; // Zapamietaj krawedz
        int j = i - 1; // Indeks poprzednika
        while (j >= 0 && edges[j].weight > key.weight) { // Przesuwaj wieksze wagi
            edges[j + 1] = edges[j]; // Przesuniecie
            j--; // Dalej w lewo
        }
        edges[j + 1] = key; // Wstaw w odpowiednie miejsce
    }
}

class MSTSolver {
public:
    // ALGORYTM PRIMA DLA MACIERZY
    static void primMatrix(MatrixGraph* g) {
        if (!g) return;
        bool* visited = new bool[g->vertices]; // Tablica odwiedzin
        for (int i = 0; i < g->vertices; i++) visited[i] = false; // Zerowanie
        MinHeap heap(g->vertices * g->vertices); // Kopiec na krawedzie
        int totalWeight = 0; // Suma wag MST

        cout << "\nKrawedzie MST (Prim - Macierz):" << endl;
        heap.insert({0, 0, -1}); // Zacznij od wierzcholka 0

        while (!heap.isEmpty()) { // Dopoki sa krawedzie w kopcu
            HeapNode current = heap.extractMin(); // Wybierz najtansza
            if (visited[current.v]) continue; // Jesli juz w drzewie, pomin
            
            visited[current.v] = true; // Dodaj do drzewa
            totalWeight += current.weight; // Zwieksz wage
            if (current.parent != -1) // Jesli to nie pierwszy wezel
                std::cout << current.parent << " - " << current.v << " w: " << current.weight << std::endl;

            for (int v = 0; v < g->vertices; v++) { // Sprawdz sasiadow w macierzy
                if (g->matrix[current.v][v] > 0 && !visited[v]) { // Jesli krawedz istnieje i sasiad wolny
                    heap.insert({v, g->matrix[current.v][v], current.v}); // Dodaj do kopca
                }
            }
        }
        std::cout << "Suma MST: " << totalWeight << std::endl;
        delete[] visited; // Sprzatanie
    }

    // ALGORYTM PRIMA DLA LISTY
    static void primList(ListGraph* g) {
        if (!g) return;
        bool* visited = new bool[g->vertices];
        for (int i = 0; i < g->vertices; i++) visited[i] = false;
        MinHeap heap(g->edges * 2 + 1); // Kopiec rozmiaru liczby krawedzi
        int totalWeight = 0;

        std::cout << "\nKrawedzie MST (Prim - Lista):" << std::endl;
        heap.insert({0, 0, -1});

        while (!heap.isEmpty()) {
            HeapNode current = heap.extractMin();
            if (visited[current.v]) continue;

            visited[current.v] = true;
            totalWeight += current.weight;
            if (current.parent != -1)
                std::cout << current.parent << " - " << current.v << " w: " << current.weight << std::endl;

            ListNode* temp = g->adjList[current.v]; // Przeszukaj liste sasiadow
            while (temp) { // Przechodz po liscie
                if (!visited[temp->target]) { // Jesli sasiad nieodwiedzony
                    heap.insert({temp->target, temp->weight, current.v}); // Dodaj do kopca
                }
                temp = temp->next; // Nastepny sasiad
            }
        }
        std::cout << "Suma MST: " << totalWeight << std::endl;
        delete[] visited;
    }

    // ALGORYTM KRUSKALA (Wspolny dla obu, bo operuje na liscie krawedzi)
    static void kruskal(int V, int E, Edge* allEdges, const char* label) {
        sortEdges(allEdges, E); // Sortuj krawedzie rosnaco
        DSU dsu(V); // Inicjalizuj strukture zbiorow
        int totalWeight = 0; // Suma wag
        int edgesCount = 0; // Licznik krawedzi w MST

        std::cout << "\nKrawedzie MST (Kruskal - " << label << "):" << std::endl;
        for (int i = 0; i < E; i++) { // Przegladaj posortowane krawedzie
            if (dsu.find(allEdges[i].u) != dsu.find(allEdges[i].v)) { // Jesli nie tworza cyklu
                dsu.unite(allEdges[i].u, allEdges[i].v); // Polacz zbiory
                std::cout << allEdges[i].u << " - " << allEdges[i].v << " w: " << allEdges[i].weight << std::endl;
                totalWeight += allEdges[i].weight; // Dodaj wage
                edgesCount++; // Kolejna krawedz
            }
            if (edgesCount == V - 1) break; // Drzewo gotowe
        }
        std::cout << "Suma MST: " << totalWeight << std::endl;
    }

    // Pomocnik zbierajacy krawedzie z macierzy
    static void solveKruskalMatrix(MatrixGraph* g) {
        if (!g) return;
        Edge* edges = new Edge[g->edges]; // Tablica na krawedzie
        int k = 0;
        for (int i = 0; i < g->vertices; i++) {
            for (int j = i + 1; j < g->vertices; j++) { // i+1 zeby nie dublowac w nieskierowanym
                if (g->matrix[i][j] > 0) edges[k++] = {i, j, g->matrix[i][j]};
            }
        }
        kruskal(g->vertices, k, edges, "Macierz");
        delete[] edges;
    }

    // Pomocnik zbierajacy krawedzie z listy
    static void solveKruskalList(ListGraph* g) {
        if (!g) return;
        Edge* edges = new Edge[g->edges];
        int k = 0;
        for (int i = 0; i < g->vertices; i++) {
            ListNode* temp = g->adjList[i];
            while (temp) {
                if (i < temp->target) { // Dodaj tylko raz (u < v)
                    edges[k++] = {i, temp->target, temp->weight};
                }
                temp = temp->next;
            }
        }
        kruskal(g->vertices, k, edges, "Lista");
        delete[] edges;
    }
};

#endif