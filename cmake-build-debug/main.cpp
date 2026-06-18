#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "ListGraph.h"
#include "MatrixGraph.h"
#include "MSTSolver.h"
#include "ShortestPathSolver.h"
#include "MaxFlowSolver.h"
#include <ctime>
#include <chrono>
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

void loadFromFile(bool isShortestPath, bool isFlowProblem, int& startV, int& endV) {
    string fileName;
    cout << "Podaj nazwe pliku (np. dane1.txt): ";
    cin >> fileName;
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Blad! Nie udalo sie otworzyc pliku!" << endl;
        return;
    }
    int E = 0, V = 0;
    string firstLine;

    if (getline(file, firstLine)) {
        stringstream ss(firstLine);
        // Probujemy wczytac format dla przeplywu: E, V, startV, endV (4 liczby)
        if (ss >> E >> V >> startV >> endV) {
            if (!isFlowProblem && !isShortestPath) { startV = 0; endV = -1; }
            if (isShortestPath) { endV = -1; } // Dla sciezki endV nie jest potrzebne
        }
        // Jesli sie nie udalo, probujemy format dla najkrotszej sciezki: E, V, startV (3 liczby)
        else {
            stringstream ss2(firstLine);
            if (ss2 >> E >> V >> startV) {
                if (!isShortestPath) startV = 0;
                endV = -1;
            }
            // Jesli to zawiodlo, wczytujemy standardowy format MST: E, V (2 liczby)
            else {
                stringstream ss3(firstLine);
                ss3 >> E >> V;
                startV = 0;
                endV = -1;
            }
        }
    }

    if (V <= 0 || E < 0) {
        cout << "Blad formatu naglowka pliku! Niepoprawna liczba wierzcholkow lub krawedzi." << endl;
        file.close();
        return;
    }
    clearGraphs();
    mGraph = new MatrixGraph(V);
    lGraph = new ListGraph(V);

    int u, v, w;
    for (int i = 0; i < E; i++) {
        if (!(file >> u >> v >> w)) break;

        if (u >= 0 && u < V && v >= 0 && v < V) {
            // Dla problemu przeplywu (isFlowProblem = true) krawedzie sa skierowane
            mGraph->addEdge(u, v, w, isFlowProblem);
            lGraph->addEdge(u, v, w, isFlowProblem);
        } else {
            cout << "Ostrzezenie: Pominieto niepoprawna krawedz: " << u << " -> " << v << endl;
        }
    }
    file.close();

    cout << "Wczytano graf: " << V << " wierzcholkow, " << E << " krawedzi." << endl;
    if (isShortestPath) {
        cout << "Wierzcholek startowy (z pliku): " << startV << endl;
    }
    if (isFlowProblem) {
        cout << "Wierzcholek zrodlowy (z pliku): " << startV << ", docelowy: " << endV << endl;
    }
    mGraph->print();
    cout << endl;
    lGraph->print();
}

void generateRandomGraphSilently(int V, int density, bool isDirected) {
    long long maxEdges = isDirected ? (long long)V * (V - 1) : (long long)V * (V - 1) / 2;
    int targetEdges = (int)(maxEdges * density / 100);

    if (targetEdges < V - 1) targetEdges = V - 1;

    clearGraphs(); //czysci grafy przed stworzeniem nowych
    mGraph = new MatrixGraph(V);
    lGraph = new ListGraph(V);

    // Zapewnienie spójności (łańcuch bazowy) - randomowe wagi i krawedzie
    for (int i = 0; i < V - 1; i++) {
        int weight = rand() % 100 + 1;
        mGraph->addEdge(i, i + 1, weight, isDirected);
        lGraph->addEdge(i, i + 1, weight, isDirected);
    }

    int currentEdges = V - 1;
    while (currentEdges < targetEdges) {
        int u = rand() % V;
        int v = rand() % V;

        if (u != v && mGraph->matrix[u][v] == 0) {
            int weight = rand() % 100 + 1;
            mGraph->addEdge(u, v, weight, isDirected);
            lGraph->addEdge(u, v, weight, isDirected);
            currentEdges++;
        }
    }
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

    // Max krawędzi w grafie nieskierowanym to V*(V-1)/2 -- np. dla 3 wierzcholkow 3 krawedzie
    long long maxEdges = (long long)V * (V - 1) / 2;
    int targetEdges = (int)(maxEdges * density / 100); //gestosc krawedzi

    // Minimalna liczba krawędzi by graf był spójny to V-1 -- sppojny czyli kazda para wierzcholkow jest polaczona jakas droga
    if (targetEdges < V - 1) targetEdges = V - 1;

    clearGraphs(); // Sprzątamy starą pamięć
    mGraph = new MatrixGraph(V);
    lGraph = new ListGraph(V);

    srand(time(NULL)); // Inicjalizacja ziarna losowania

    //Gwarancja spójności (drzewo rozpinające w formie ścieżki (min krawedzi bez cykli))
    for (int i = 0; i < V - 1; i++) { //przejscie przez wszystkie krawedzie do ich minimalnej ilosci
        int weight = rand() % 100 + 1; // Wagi od 1 do 100 random
        mGraph->addEdge(i, i + 1, weight);
        lGraph->addEdge(i, i + 1, weight);
    }

    //Dolosowujemy resztę krawędzi
    int currentEdges = V - 1;
    while (currentEdges < targetEdges) { //obecna liczba krawedzi(zaczynamy od minimalnej) < ostateczna liczba krawedzi z gestosci
        int u = rand() % V; //randomowy z podanej ilosci
        int v = rand() % V; //random z podanej ilosci

        // Nie chcemy pętli własnych (u==v) ani powtórzonych krawędzi (matrix[u][v] > 0)
        if (u != v && mGraph->matrix[u][v] == 0) { //wierzcholki sie nie lacza i sa rozne to je laczymy
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
void runTimeAnalyseModule() {
    cout << "\n======================================================" << endl;
    cout << "  ROZPOCZECIE AUTOMATYCZNEGO MODULU BADANIA CZASU     " << endl;
    cout << "  (Wszystkie wyniki zostana usrednione z 50 prob)     " << endl;
    cout << "======================================================" << endl;

    int sizesV[] = {10, 25, 50, 75, 100, 150, 200};
    int densities[] = {20, 60, 99};
    const int instances = 50;

    srand(time(NULL));

    stringstream buffer;
    streambuf* oldCout = cout.rdbuf(buffer.rdbuf());

    // 1. POMIARY DLA MST
    buffer.str(""); buffer.clear();
    cout.rdbuf(oldCout);
    cout << "\n--- WYNIKI DLA MINIMALNEGO DRZEWA ROZPINACEGO (MST) [w ms] ---" << endl;
    cout << "V\tG%\tPrim_Mac\tPrim_Lis\tKrusk_Mac\tKrusk_Lis" << endl;
    cout.rdbuf(buffer.rdbuf());

    for (int V : sizesV) {
        for (int D : densities) {
            double tPrimM = 0, tPrimL = 0, tKruskM = 0, tKruskL = 0;

            for (int i = 0; i < instances; i++) {
                generateRandomGraphSilently(V, D, false);

                auto s = chrono::high_resolution_clock::now();
                MSTSolver::primMatrix(mGraph);
                auto e = chrono::high_resolution_clock::now();
                tPrimM += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                MSTSolver::primList(lGraph);
                e = chrono::high_resolution_clock::now();
                tPrimL += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                MSTSolver::solveKruskalMatrix(mGraph);
                e = chrono::high_resolution_clock::now();
                tKruskM += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                MSTSolver::solveKruskalList(lGraph);
                e = chrono::high_resolution_clock::now();
                tKruskL += chrono::duration<double, milli>(e - s).count();
            }
            cout.rdbuf(oldCout);
            cout << V << "\t" << D << "%\t"
                 << (tPrimM / instances) << "\t  " << (tPrimL / instances) << "\t"
                 << (tKruskM / instances) << "\t" << (tKruskL / instances) << endl;
            cout.rdbuf(buffer.rdbuf());
        }
    }

    // 2. POMIARY DLA NAJKRÓTSZEJ ŚCIEŻKI
    buffer.str(""); buffer.clear();
    cout.rdbuf(oldCout);
    cout << "\n--- WYNIKI DLA NAJKROTSZEJ SCIEZKI [w ms] ---" << endl;
    cout << "V\tG%\tDijk_Mac\tDijk_Lis\tBell_Mac\tBell_Lis" << endl;
    cout.rdbuf(buffer.rdbuf());

    for (int V : sizesV) {
        for (int D : densities) {
            double tDijkM = 0, tDijkL = 0, tBellM = 0, tBellL = 0;

            for (int i = 0; i < instances; i++) {
                generateRandomGraphSilently(V, D, true);

                auto s = chrono::high_resolution_clock::now();
                ShortestPathSolver::dijkstraMatrix(mGraph, 0);
                auto e = chrono::high_resolution_clock::now();
                tDijkM += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                ShortestPathSolver::dijkstraList(lGraph, 0);
                e = chrono::high_resolution_clock::now();
                tDijkL += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                ShortestPathSolver::fordBellmanMatrix(mGraph, 0);
                e = chrono::high_resolution_clock::now();
                tBellM += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                ShortestPathSolver::fordBellmanList(lGraph, 0);
                e = chrono::high_resolution_clock::now();
                tBellL += chrono::duration<double, milli>(e - s).count();
            }
            cout.rdbuf(oldCout);
            cout << V << "\t" << D << "%\t"
                 << (tDijkM / instances) << "\t  " << (tDijkL / instances) << "\t"
                 << (tBellM / instances) << "\t" << (tBellL / instances) << endl;
            cout.rdbuf(buffer.rdbuf());
        }
    }

    // 3. POMIARY DLA MAKSYMALNEGO PRZEPŁYWU
    buffer.str(""); buffer.clear();
    cout.rdbuf(oldCout);
    cout << "\n--- WYNIKI DLA MAKSYMALNEGO PRZEPLYWU [w ms] ---" << endl;
    cout << "V\tG%\tFordFulk_Mac\tFordFulk_Lis" << endl;
    cout.rdbuf(buffer.rdbuf());

    for (int V : sizesV) {
        for (int D : densities) {
            double tFlowM = 0, tFlowL = 0;

            for (int i = 0; i < instances; i++) {
                generateRandomGraphSilently(V, D, true);

                auto s = chrono::high_resolution_clock::now();
                MaxFlowSolver::fordFulkersonMatrix(mGraph, 0, V - 1);
                auto e = chrono::high_resolution_clock::now();
                tFlowM += chrono::duration<double, milli>(e - s).count();

                s = chrono::high_resolution_clock::now();
                MaxFlowSolver::fordFulkersonList(lGraph, 0, V - 1);
                e = chrono::high_resolution_clock::now();
                tFlowL += chrono::duration<double, milli>(e - s).count();
            }
            cout.rdbuf(oldCout);
            cout << V << "\t" << D << "%\t"
                 << (tFlowM / instances) << "\t  " << (tFlowL / instances) << endl;
            cout.rdbuf(buffer.rdbuf());
        }
    }

    cout.rdbuf(oldCout);
    clearGraphs();
    cout << "\n======================================================" << endl;
    cout << "  POMIARY ZAKONCZONE POMYSLNIE!                       " << endl;
    cout << "======================================================" << endl;
}

void displayMenu1() {
    cout << "\n===========MENU============" << endl;
    cout << "1. Wczytaj dane z pliku" << endl;
    cout << "2. Wygeneruj graf losowo" << endl;
    cout << "3. Wyswietl listowo i macierzowo" << endl;
    cout << "4. Algorytm Prima" << endl;
    cout << "5. Algorytm Kruskala" << endl;
    cout << "0. Powrot" << endl;
    cout << "Wybor: ";
}
void displayMenu2() {
    cout << "\n===========MENU============" << endl;
    cout << "1. Wczytaj dane z pliku" << endl;
    cout << "2. Wygeneruj graf losowo" << endl;
    cout << "3. Wyswietl listowo i macierzowo" << endl;
    cout << "4. Algorytm Dijkstry" << endl;
    cout << "5. Algorytm Forda-Bellmana" << endl;
    cout << "0. Powrot" << endl;
    cout << "Wybor: ";
}
void displayMenu3() {
    cout << "\n===========MENU============" << endl;
    cout << "1. Wczytaj dane z pliku" << endl;
    cout << "2. Wygeneruj graf losowo" << endl;
    cout << "3. Wyswietl listowo i macierzowo" << endl;
    cout << "4. Algorytm Forda Fulkersona" << endl;
    cout << "0. Powrot" << endl;
    cout << "Wybor: ";
}

int main() {
    int problemChoice = -1;
    int fileEndV = -1;
    int fileStartV = -1;
    while (problemChoice != 0) {
        int subChoice = -1;
        cout << "\n=== WYBIERZ PROBLEM ===" << endl;
        cout << "1. MST (Prim, Kruskal)" << endl;
        cout << "2. Najkrotsza sciezka w grafie (Dijkstry, Ford-Bellman)" << endl;
        cout << "3. Wyznaczenie maksymalnego przeplywu (Ford Fulkerson)" << endl;
        cout << "4. URUCHOM AUTOMATYCZNE BADANIA CZASU" << endl;
        cout << "0. Wyjscie" << endl;
        cout<<"Wybor: ";
        cin >> problemChoice;

        if (problemChoice == 4) {
            runTimeAnalyseModule();
            problemChoice = -1;
        }
        else if (problemChoice == 1){
            while (subChoice != 0){
                displayMenu1();
                cin >> subChoice;
                if (subChoice == 1) {
                    loadFromFile(false, false, fileStartV, fileEndV);
                }else if (subChoice == 2) {
                    generateRandomGraph();
                    fileStartV = -1;
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
                displayMenu2();
                cin >> subChoice;
                if (subChoice == 1) {
                    loadFromFile(true, true, fileStartV, fileEndV);
                }else if (subChoice == 2) {
                    generateRandomGraph();
                    fileStartV = -1;
                }else if (subChoice == 3){
                    if (mGraph && lGraph) { mGraph->print(); lGraph->print(); }
                    else cout << "Najpierw wczytaj graf!" << endl;
                }else if (subChoice == 4) {
                    if (mGraph && lGraph) {
                        int currentStart = fileStartV;
                        if (currentStart < 0 || currentStart >= mGraph->vertices) {
                            cout << "Podaj wierzcholek startowy (0 - " << mGraph->vertices - 1 << "): ";
                            cin >> currentStart;
                        }
                        if (currentStart >= 0 && currentStart < mGraph->vertices) {
                            ShortestPathSolver::dijkstraMatrix(mGraph, currentStart);
                            ShortestPathSolver::dijkstraList(lGraph, currentStart);
                        } else {
                            cout << "Niepoprawny wierzcholek!" << endl;
                        }
                    } else {
                        cout << "Najpierw wczytaj lub wygeneruj graf!" << endl;
                    }
                }else if (subChoice == 5) {
                    if (mGraph && lGraph) {
                        int currentStart = fileStartV;
                        if (currentStart < 0 || currentStart >= mGraph->vertices) {
                            cout << "Podaj wierzcholek startowy (0 - " << mGraph->vertices - 1 << "): ";
                            cin >> currentStart;
                        }
                        if (currentStart >= 0 && currentStart < mGraph->vertices) {
                            ShortestPathSolver::fordBellmanMatrix(mGraph, currentStart);
                            ShortestPathSolver::fordBellmanList(lGraph, currentStart);
                        } else {
                            cout << "Niepoprawny wierzcholek!" << endl;
                        }
                    } else {
                        cout << "Najpierw wczytaj graf!" << endl;
                    }
                }
                czyPowrot(subChoice, problemChoice);
            }
}else if(problemChoice == 3){
    while (subChoice != 0){
        displayMenu3();
        cin >> subChoice;
        if (subChoice == 1) {
            loadFromFile(false, true, fileStartV, fileEndV);
        }else if (subChoice == 2) {
            generateRandomGraph();
        }else if (subChoice == 3){
            if (mGraph && lGraph) { mGraph->print(); lGraph->print(); }
            else cout << "Najpierw wczytaj graf!" << endl;
        }else if (subChoice == 4) {
            if (mGraph && lGraph) {
                int source = fileStartV;
                int sink = fileEndV;

                // Jesli wierzcholki sa poprawne, pomijamy wpisywanie manualne
                if (source >= 0 && sink >= 0 && source < mGraph->vertices && sink < mGraph->vertices) {
                    cout << "Uzywam wartosci z pliku -> Zrodlo: " << source << ", Ujscie: " << sink << endl;
                } else {
                    cout << "Podaj wierzcholek zrodlowy (source): ";
                    cin >> source;
                    cout << "Podaj wierzcholek docelowy (sink): ";
                    cin >> sink;
                }

                if (source >= 0 && source < mGraph->vertices && sink >= 0 && sink < mGraph->vertices) {
                    MaxFlowSolver::fordFulkersonMatrix(mGraph, source, sink);
                    MaxFlowSolver::fordFulkersonList(lGraph, source, sink);
                } else {
                    cout << "Niepoprawne numery wierzcholkow!" << endl;
                }
            }else {
                cout << "Najpierw wczytaj graf!" << endl;
            }
        }
    czyPowrot(subChoice, problemChoice);
    }
}
}

    clearGraphs();
    return 0;
}
