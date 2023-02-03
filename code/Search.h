#pragma once
#include "HGL.h"
#include "tsp_2_opt.h"
//#include "GraphEngine.h"
#define STOP 1000000



void tabu_search(Soluzione& S,vector<Nodo>& W, int q, int p1, int p2, int theta_min, int theta_max, int g, int h, int n_max);


//FUNZIONI COMPONENTI SEARCH

/*Seleziona q vertici casualmente dal vettore W e costruisce l'insieme PSI*/
void seleziona_vertici(vector<Nodo>& PSI, int q, vector<Nodo> W, int dim);

/*Genera una soluzione candidata a nuova Soluzione migliore*/
Soluzione valutazione_mossa(const vector<Nodo>& PSI, int q, int p1, vector<Soluzione> & Soluzioni_temporanee, Tabulist&, Soluzione & S, Soluzione& Soluzione_migliore_fattibile, Soluzione& Soluzione_migliore, const float &, const int &);

/*Verifica se la soluzione candidata sostituir� la vecchia Soluzione migliore*/
void determina_prossima_soluzione(Soluzione& Soluzione_candidata, Soluzione& S, int& L_ammissibile, int& L_inammissibile, int& C_ammissibile, int& C_inammissibile);

/*Aggiorna tutti i parametri di tempo, costo , la lista tab� e la migliore soluzione ammissibile e non*/
void aggiorna_lista_parametri(const Soluzione& Soluzione_candidata, Tabulist& lista_tabu, Soluzione& Soluzione_migliore_fattibile, Soluzione& Soluzione_migliore, float& delta_max, float& f_v, int& t, int& n_miglioramenti, const int& theta_min, const int& theta_max, const float& F2_prec);	//Aggiorna la lista tabu con la mossa effettuata e i parametri della search

/*Aggiorna i coefficienti di penalit�*/
void correzione_penalita(int& L_ammissibile, int& L_inammissibile, int& C_ammissibile, int& C_inammissibile, const int & h);
 

//SOTTOFUNZIONI VALUTAZIONE_MOSSA

/*Dato il nodo v, ne genera un intorno N di dimensione p1*/
void costruzione_vicinato(const Nodo& v, vector<int> & N, const int &p1);

/*Dato un nodo v in PSI, e il suo intorno N, costruisce le soluzioni temporanee*/
void identifica_soluzioni_temporanee(const vector<int> & N, const int & indice_nodo, const Tabulist, Soluzione& S, vector<Soluzione>& Soluzioni_temporanee, const int& indice_q);

/*Controlla se una soluzione � generata a partire da una mossa tabu*/
void controlla_soluzioni_tabu(vector<Soluzione>& Soluzioni_temporanee, Tabulist& listaTabu, Soluzione& S, Soluzione& Soluzione_migliore_fattibile, Soluzione& Soluzione_migliore,const float& delta_max, const int& f_v);

/*Stabilisce la soluzione candidata a partire da tutte le soluzioni temporanee identificate*/
Soluzione determina_soluzione_candidata(vector<Soluzione>& Soluzioni_temporanee);

//SOTTOFUNZIONI DI COSTRUZIONE_VICINATO

int minimo_indice(vector<float>& V, const int& indice_nodo);


//SOTTOFUNZIONI DI IDENTIFICA_SOLUZIONI_TEMPORANEE

/*Verifica se la rotta R contiene almeno un nodo appartenente ad N, insieme dei vicini*/
bool intersezione(const vector<int>& N, Rotta* cluster, int& indice_intersezione);

/*Costruzione della mossa che sposta il nodo origine al vicino destinazione dalla rotta Rr a quella Rs*/
Mossa calcola_mossa(Rotta Rr, Rotta Rs, const int& indice_origine, const int& indice_destinazione);

/*Costruzione della soluzione temporanea a partire dalla mossa*/
void costruisci_soluzione(vector<Soluzione>& Soluzioni_temporanee, const Mossa& mossa, const Soluzione& S);


//SOTTOFUNZIONI DI CONTROLLA_SOLUZIONI_TABU

/*Controlla se la mossa m � tabu, e quindi � presente nella lista*/
bool cerca_tabu(Tabulist& listaTabu, const Mossa& mossa);


//RENDERED TABU SEARCH

//void rendered_tabu_search(Soluzione& S, vector<Nodo>& W, int q, int p1, int p2, int theta_min, int theta_max, int g, int h, int n_max, SDL_Renderer* renderer, Frame Screen);