//
// Created by Ola on 22.04.2026.
//
#ifndef MATRIXGRAPH_H
#define MATRIXGRAPH_H
#include <iostream>
using namespace std;

class MatrixGraph {
public:
    int** matrix; //wskaźnik do tablicy dwuwymiarowej
    int vertices; //liczba wierzchołków
    int edges; //liczba krawędzi

    MatrixGraph(int V) { // Konstruktor
        vertices = V;
        edges = 0;
        matrix = new int*[V]; //dynamiczna alokacja bo macierz V x V
        for (int i = 0; i < V; i++) {
            matrix[i] = new int[V];
            for (int j = 0; j < V; j++) matrix[i][j] = 0;
        }
    }

    ~MatrixGraph() { // Destruktor
        for (int i = 0; i < vertices; i++) delete[] matrix[i];
        delete[] matrix;
    }

    void addEdge(int u, int v, int weight, bool isDirected = false) {//dodanie odpowiedniej ilosci krawedzi w zaleznosci od tego czy
        //skierowane czy nie
        matrix[u][v] = weight; // Krawedz u -> v
        if (!isDirected) {
            matrix[v][u] = weight; // Krawedz v -> u tylko dla grafu nieskierowanego (w obie strony)
        }
        edges++;
    }

    void print() {
        cout << "Macierz sasiedztwa:" << endl;
        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) cout << matrix[i][j] << " ";
            cout <<endl;
        }
    }
};
#endif