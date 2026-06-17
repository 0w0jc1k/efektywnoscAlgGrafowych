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

    void addEdge(int u, int v, int weight, bool isDirected = false) { //funkcja dodajaca krawedzie pomiedzy wierzcholkami o danej wadze
        ListNode* newNode = new ListNode;
        newNode->target = v;
        newNode->weight = weight;
        newNode->next = adjList[u];
        adjList[u] = newNode;

        if (!isDirected) { //jesli nieskierowana to dodajemy jeszcze jedna w druga strone
            ListNode* newNode2 = new ListNode;
            newNode2->target = u;
            newNode2->weight = weight;
            newNode2->next = adjList[v];
            adjList[v] = newNode2;
        }
        edges++;
    }

    void print() {
        cout << "Lista sasiadow:" << endl;
        for (int i = 0; i < vertices; i++) {
            cout << "[" << i <<"]: "; //
            ListNode* curr = adjList[i]; //obecny wierzcholek
            while (curr) { //jesli istnieje wierzcholek
                cout << "-> " << curr->target << "(" << curr->weight << ") "; //zrodlo, cel, waga
                curr = curr->next;
            }
            cout<<endl;
        }
    }
};
#endif

