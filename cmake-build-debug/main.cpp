#include <iostream>
#include <fstream>
#include <string>
#include "ListGraph.h"
#include "MatrixGraph.h"

using namespace std;

MatrixGraph* mGraph = nullptr;
ListGraph* lGraph = nullptr;

void clearGraphs() {
    if (mGraph) { delete mGraph; mGraph = nullptr; }
    if (lGraph) { delete lGraph; lGraph = nullptr; }
}

void loadFromFile() {
    string fileName;
    cout << "Podaj nazwe pliku (np. dane1.txt): ";
    cin >> fileName;
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Blad! Nie udalo sie otworzyc pliku!" << endl;
        return;
    }

    int E, V; // Zmienne na liczbe krawedzi i wierzcholkow
    file >> E >> V; // Wczytanie liczby krawędzi i wierzchołków
    // file >> startV; // Opcjonalnie: startowy wierzchołek (wymagany przy najkrótszej ścieżce)

    clearGraphs(); // Usunięcie starego grafu przed stworzeniem nowego
    mGraph = new MatrixGraph(V);
    lGraph = new ListGraph(V);

    int u, v, w;
    for (int i = 0; i < E; i++) {
        if (!(file >> u >> v >> w)) break; // Wczytanie danych i zabezpieczenie przed koncem pliku
        mGraph->addEdge(u, v, w); // Dodanie do struktury macierzowej
        lGraph->addEdge(u, v, w); // Dodanie do struktury listowej
    }
    file.close();
    cout << "Wczytano graf: " << V << " wierzcholkow, " << E << " krawedzi."<<endl;
    mGraph->print();
    cout << endl;
    lGraph->print();
}

void displayMenu() {
    cout << "\n===========MENU============" << endl;
    cout << "1. Wczytaj dane z pliku" << endl;
    cout << "2. Wygeneruj graf losowo" << endl;
    cout << "3. Wyswietl listowo i macierzowo" << endl;
    cout << "4. Algorytm Prima" << endl;
    cout << "5. Algorytm Kruskala" << endl;
    cout << "0. Powrot" << endl;
    cout << "Wybor: ";
}

int main() {
    int problemChoice = -1;
    char backToMenu;
    while (problemChoice != 0) {
        cout << "\n=== WYBIERZ PROBLEM ===" << endl;
        cout << "1. MST (Prim, Kruskal)" << endl;
        cout << "0. Wyjscie" << endl;
        cout<<"Wybor: ";
        cin >> problemChoice;

        if (problemChoice == 1){
            int subChoice = -1;
            while (subChoice != 0){
                displayMenu();
                cin >> subChoice;
                if (subChoice == 1) {
                    loadFromFile();
                }else if (subChoice == 3){
                        if (mGraph && lGraph) { mGraph->print(); lGraph->print(); }
                        else cout << "Najpierw wczytaj graf!" << endl;
                }
                if (subChoice != 0) {
                    cout<<"Powrot do MENU? (t/n): ";
                    cin >>backToMenu;
                    if (backToMenu == 'n' || backToMenu == 'N') {
                        subChoice = 0;
                        problemChoice = 0;
                    }
                }
            }
        }

    }
    clearGraphs();
    return 0;
}