//
// Created by Ola on 22.04.2026.
//Klasa dla listy sasiadow
#ifndef LISTGRAPH_H
#define LISTGRAPH_H
#include "Structures.h"
#include <iostream>
using namespace std;

class ListGraph {
public:
    ListNode** adjList; //tablica wskaźników na listy
    int vertices; // Liczba wierzchołków
    int edges; // Liczba krawędzi

    ListGraph(int V) { // Konstruktor
        vertices = V;
        edges = 0;
        adjList = new ListNode*[V]; //dynamiczna alokacja tablicy wskaźników
        for (int i = 0; i < V; i++) adjList[i] = nullptr; // Każda lista pusta na start
    }

    ~ListGraph() { // Destruktor
        for (int i = 0; i < vertices; i++) {
            ListNode* current = adjList[i];
            while (current) {
                ListNode* next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] adjList;
    }

    void addEdge(int u, int v, int weight) {
        ListNode* newNode = new ListNode;
        newNode->target = v;
        newNode->weight = weight;
        newNode->next = adjList[u]; // Wskaż na dotychczasowy początek
        adjList[u] = newNode; // Stań się nowym początkiem

        ListNode* newNode2 = new ListNode; // Powrotny dla nieskierowanego
        newNode2->target = u; // Cel: u
        newNode2->weight = weight;
        newNode2->next = adjList[v]; // Wskazanie
        adjList[v] = newNode2; // Przypisanie
        edges++;
    }

    void print() {
        cout << "Lista sasiadow:" << endl;
        for (int i = 0; i < vertices; i++) {
            cout << "[" << i <<"]: "; //
            ListNode* curr = adjList[i];
            while (curr) {
                cout << "-> " << curr->target << "(" << curr->weight << ") ";
                curr = curr->next;
            }
            cout<<endl;
        }
    }
};
#endif

