#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <list>
#include <ctime>
#include <cstdlib>
#include <fstream>
//#include "pricer_vrp.h"

#define FLOAT_INF 3.40282e+038

using namespace std;

//Strutture dati
struct Nodo;

struct Rotta {
	list<Nodo> percorso;		//Ciclo Hamiltoniano interno alla rotta
	float lunghezza;			//costo totale della rotta in termini di attraversamento di archi
	float tempo_servizio;		//tempo di servizio speso in ogni nodo
	float domanda_totale;		//Totale capacit� richiesta dalla rotta
	int indice;					//Indice della rotta internamente alla soluzione
};

struct Nodo {
	//coordinate
	float x;
	float y;

	int indice;						//indice del nodo nel vettore V di origine
	int domanda;					//quantit� richiesta dal nodo
	int tempo;						//tempo di servizio del nodo
	int spostamenti;				//Numero di spostamenti di un nodo
};

struct Mossa {
	int nodo;						//Nodo interessato dalla mossa
	int rotta_origine;				//Rotta da cui viene preso il nodo
	int rotta_destinazione;			//Rotta in cui viene posto il nodo
	int posizione_in_origine;		//Posizione del nodo nella rotta d'origine
	int posizione_in_destinazione;	//Posizione del nodo nella rotta di destinazione
	float costo;					//Saving della mossa
};

struct Soluzione {
	float F1;						//Valore di funzione obiettivo
	float F2;						//Valore di funzione obiettivo con penalit�
	float F_temp;					//Valore di funzione obiettivo con ulteriore penalit� causata dallo spostamenti di uno stesso nodo
	float penalita_capacita;		//Valore di penalit� relativo a violazione di vincoli di capacit�
	float penalita_tempo;			//Valore di penalit� relativo a violazione di vincoli di lunghezza
	bool ammissibile;				//Ammissibilit� della soluzione
	vector<Rotta*> cluster;			//Insieme di rotte che definiscono la soluzione
	Mossa m;						//Mossa che ha generato la soluzione
};


struct Tabu {
	Mossa m;						//Mossa definita come Tabu
	int TTL;						//Numero di iterazioni per le quali la mossa resta tabu
};

struct Tabulist {
	vector<Tabu> mossa_tabu;		//Lista di mosse tabu
};


//FUNZIONI 

void preleva_dati(const string & nome);
void inizializza_variabili(const float& q, const float& l, const int& veicoli);				//Inizializza le variabili globali del problema
void distruggi_variabili(Soluzione & S);													//Distruzione variabilio globali del problema
void inizializza_soluzione(Soluzione& S,const int &);
void distruggi_soluzione(Soluzione& S);
void assegna_soluzione(Soluzione& S1, const Soluzione& S2);
void calcola_lunghezza_percorso(Rotta&);													//Calcola il tempo totale di percorrenza degli archi del percorso
void calcola_domanda_percorso(Rotta&);														//Calcola la domanda totale dei nodi del percorso
void calcola_tempo_percorso(Rotta&);														//calcola il tempo di servizio dei nodi del percorso
void calcola_F(Soluzione & S);																//calcola le funzioni obiettivo della soluzione S (F1 ed F2), termini di penalit� e ammissibilit�
int posizione_nodo(Rotta&, const Nodo&);													//Identifica la posizione di un nodo in una rotta
int rotta_del_nodo(const Soluzione&, const Nodo&);


//FUNZIONI DI DEBUG 

void random_node(Nodo&);					
void random_set();
void print_node(const Nodo& v);
void print_set(const vector<Nodo>& V);
void print_adjMatrix();
void debug_build_S(Soluzione & S);
void print_soluzione(const Soluzione& S);
void print_rotta(Rotta&);