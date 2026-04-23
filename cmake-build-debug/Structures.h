//
// Created by Ola on 22.04.2026.
//Struktury pomocnicze
#ifndef STRUCTURES_H
#define STRUCTURES_H

struct ListNode { //struktura elementu listy sąsiedztwa
    int target; //numer wierzchołka docelowego
    int weight; //waga krawędzi
    ListNode* next; //wskaźnik na kolejny element w liście
};

struct Edge { //pomocnicza struktura krawędzi - kruskal i mst
    int u,v,weight;
};

struct HeapNode { //element kopca dla prima
    int v,weight, parent; //wierzcholek, aktualna najkrotsza droga dojscia do drzewa, rodzic z ktorego przyszlismy
};

#endif //STRUCTURES_H
