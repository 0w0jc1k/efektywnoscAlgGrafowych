#include <iostream>
#include <fstream>
#include <string>
#include "ListGraph.h"
#include "MatrixGraph.h"
#include "MSTSolver.h"
#include <ctime>
#include <cstdlib>

using namespace std;

MatrixGraph* mGraph = nullptr; //wskaznik na macierzowy graf
ListGraph* lGraph = nullptr; //wskaznik na listowy graf

void czyPowrot(int &subChoice, int &problemChoice){
    if (subChoice == 0) return;
    char backToMenu;
        cout<<"Powrot do MENU? (t/n): ";
        cin >>backToMenu;
        if (backToMenu == 'n' || backToMenu == 'N') {
            subChoice = 0;
            problemChoice = 0;
        }
    }

void clearGraphs() { //czysci stare grafy
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

    int E, V; //liczba krawedzi i wierzcholkow
    file >> E >> V; //wczytanie liczby krawędzi i wierzchołków z pliku

    clearGraphs(); // Usunięcie starego grafu przed stworzeniem nowego
    mGraph = new MatrixGraph(V);
    lGraph = new ListGraph(V);

    int u, v, w; //wierzcholek startowy, koncowy, dlugosc sciezki
    for (int i = 0; i < E; i++) {
        if (!(file >> u >> v >> w)) break; //zabezpieczenie przed koncem pliku
        mGraph->addEdge(u, v, w); // Dodanie do struktury macierzowej
        lGraph->addEdge(u, v, w); // Dodanie do struktury listowej
    }
    file.close();
    cout << "Wczytano graf: " << V << " wierzcholkow, " << E << " krawedzi."<<endl;
    mGraph->print();
    cout << endl;
    lGraph->print();
}

// Funkcja generująca graf losowo
void generateRandomGraph() {
    int V, density;
    cout << "Podaj liczbe wierzcholkow: ";
    cin >> V;
    cout << "Podaj gestosc grafu (w %): ";
    cin >> density;

    if (V < 2) {
        cout << "Zbyt mala liczba wierzcholkow!" << endl;
        return;
    }

    // Max krawędzi w grafie nieskierowanym to V*(V-1)/2
    long long maxEdges = (long long)V * (V - 1) / 2;
    int targetEdges = (int)(maxEdges * density / 100);

    // Minimalna liczba krawędzi by graf był spójny to V-1
    if (targetEdges < V - 1) targetEdges = V - 1;

    clearGraphs(); // Sprzątamy starą pamięć
    mGraph = new MatrixGraph(V);
    lGraph = new ListGraph(V);

    srand(time(NULL)); // Inicjalizacja ziarna losowania

    // KROK 1: Gwarantujemy spójność (drzewo rozpinające w formie ścieżki)
    for (int i = 0; i < V - 1; i++) {
        int weight = rand() % 100 + 1; // Wagi od 1 do 100
        mGraph->addEdge(i, i + 1, weight);
        lGraph->addEdge(i, i + 1, weight);
    }

    // KROK 2: Dolosowujemy resztę krawędzi
    int currentEdges = V - 1;
    while (currentEdges < targetEdges) {
        int u = rand() % V;
        int v = rand() % V;

        // Nie chcemy pętli własnych (u==v) ani powtórzonych krawędzi (matrix[u][v] > 0)
        if (u != v && mGraph->matrix[u][v] == 0) {
            int weight = rand() % 100 + 1;
            mGraph->addEdge(u, v, weight);
            lGraph->addEdge(u, v, weight);
            currentEdges++;
        }
    }

    cout << "\n>>> Wygenerowano spojny graf o gestosci " << density << "% (" << targetEdges << " krawedzi)." << endl;

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
    while (problemChoice != 0) {
        int subChoice = -1;
        cout << "\n=== WYBIERZ PROBLEM ===" << endl;
        cout << "1. MST (Prim, Kruskal)" << endl;
        cout << "2. Najkrotsza sciezka w grafie (Dijkstry, Ford-Bellman)" << endl;
        cout << "3. Wyznaczenie maksymalnego przeplywu (Ford Fulkerson)" << endl;
        cout << "0. Wyjscie" << endl;
        cout<<"Wybor: ";
        cin >> problemChoice;

        if (problemChoice == 1){
            while (subChoice != 0){
                displayMenu();
                cin >> subChoice;
                if (subChoice == 1) {
                    loadFromFile();
                }else if (subChoice == 2) {
                    generateRandomGraph();
                }else if (subChoice == 3){
                        if (mGraph && lGraph) { mGraph->print(); lGraph->print(); }
                        else cout << "Najpierw wczytaj graf!" << endl;
                }else if (subChoice == 4) {
                    if (mGraph && lGraph) {
                        MSTSolver::primMatrix(mGraph);
                        MSTSolver::primList(lGraph);
                    }else {
                        cout << "Najpierw wczytaj graf!" << endl;
                        break;
                    }
                }else if (subChoice == 5) {
                    if (mGraph && lGraph) {
                        MSTSolver::solveKruskalMatrix(mGraph);
                        MSTSolver::solveKruskalList(lGraph);
                    }else {
                        cout << "Najpierw wczytaj graf!" << endl;
                        break;
                    }
                }
                czyPowrot(subChoice, problemChoice);
            }
        }else if(problemChoice == 2){
            while (subChoice != 0){
                displayMenu();
                cin >> subChoice;
                if (subChoice == 1) {
                    loadFromFile();
                }else if (subChoice == 2) {
                    generateRandomGraph();
                }else if (subChoice == 3){
                    if (mGraph && lGraph) { mGraph->print(); lGraph->print(); }
                    else cout << "Najpierw wczytaj graf!" << endl;
                }else if (subChoice == 4) {
                    if (mGraph && lGraph) {
                        MSTSolver::primMatrix(mGraph);
                        MSTSolver::primList(lGraph);
                    }else {
                        cout << "Najpierw wczytaj graf!" << endl;
                        break;
                    }
                }else if (subChoice == 5) {
                    if (mGraph && lGraph) {
                        MSTSolver::solveKruskalMatrix(mGraph);
                        MSTSolver::solveKruskalList(lGraph);
                    }else {
                        cout << "Najpierw wczytaj graf!" << endl;
                        break;
                    }
                }
                czyPowrot(subChoice, problemChoice);
            }
}else if(problemChoice == 3){
    while (subChoice != 0){
        displayMenu();
        cin >> subChoice;
        if (subChoice == 1) {
            loadFromFile();
        }else if (subChoice == 2) {
            generateRandomGraph();
        }else if (subChoice == 3){
            if (mGraph && lGraph) { mGraph->print(); lGraph->print(); }
            else cout << "Najpierw wczytaj graf!" << endl;
        }else if (subChoice == 4) {
            if (mGraph && lGraph) {
                MSTSolver::primMatrix(mGraph);
                MSTSolver::primList(lGraph);
            }else {
                cout << "Najpierw wczytaj graf!" << endl;
                break;
            }
        }else if (subChoice == 5) {
            if (mGraph && lGraph) {
                MSTSolver::solveKruskalMatrix(mGraph);
                MSTSolver::solveKruskalList(lGraph);
            }else {
                cout << "Najpierw wczytaj graf!" << endl;
                break;
            }
        }
    czyPowrot(subChoice, problemChoice);
    }
}
}

    clearGraphs();
    return 0;
}
