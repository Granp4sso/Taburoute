#include "tsp_2_opt.h"

void tsp_2_opt(Rotta& r) {
	Rotta* r_migliore = new Rotta();
	*r_migliore = r;										//Rotta che salva il migliore percorso all'iterazione i_esima dell'algoritmo

	vector<Mossa_opt> listaTabu;							//Lista di mosse tabu 

	int n = r.percorso.size();								//Lunghezza del circuito hamiltoniano
	int iterazioni_non_migliorative = 0;					//Numero di iterazioni per la quale non ho trovato miglioramenti
	float migliore_distanza = r.lunghezza;					//Distanza dell'attuale circuito migliore 
	float nuova_distanza = r.lunghezza;						//Distanza calcolata ad ogni tentativo
	bool prima_prova = 1;									//Se è la prima volta che eseguo l'algoritmo vale 1 altrimenti 0
	Mossa_opt mossa;

	while (iterazioni_non_migliorative < 10) {
		r = *r_migliore;										//r sarà all' inizio dell'algoritmo è par alla rotta migliore
		migliore_distanza = r.lunghezza;
		nuova_distanza = r.lunghezza;
		prima_prova = 1;
		for (int i = 0; i < n - 1; i++) {
			for (int j = i + 1; j < n; j++) {
				mossa.indice1 = i;								//Considero la mossa che sto per applicare
				mossa.indice2 = j;
				if (!mossa_in_lista_tabu(mossa, listaTabu)) {	//Se la mossa non è tabu vado avanti altrimenti cambio indici
					swap_nodi(i, j, r);							//Provo a scambiare 2 nodi in posizione i e j
					calcola_lunghezza_percorso(r);				//Calcolo la nuova lunghezza
					nuova_distanza = r.lunghezza;				//Salvo il valore trovato in nuova lunghezza
					if (nuova_distanza < migliore_distanza) {	//Se la nuova lunghezza è migliore della migliore attuale
						*r_migliore = r;						//Salvo la nuova migliore rotta
						migliore_distanza = nuova_distanza;		//Salvo la nuova migliore distanza
						if (prima_prova == 1) {					//Se è la prima volta che eseguo la ricerca
							prima_prova = 0;					//Prima_prova = 0
						}
						else {
							listaTabu.pop_back();				//Altrimenti cancello la precedente mossa che avevo considerato tabu della stessa iterazione 
						}
						listaTabu.push_back(mossa);				//Salvo la mossa che ha generato ma migliore soluzione come mossa tabu
					}
					swap_nodi(i, j, r);							//Faccio tornare r a come era precedentemente
				}
			}
		}
		calcola_lunghezza_percorso(r);							//Ricalcolo la lunghezza del percorso della rotta che aveva all'inizio dell'iterazione					
		if (migliore_distanza == r.lunghezza) {					//Se la migliore lunghezza è pari a quella della rotta all'inizio dell'iterazione
			if (listaTabu.empty()) {							//Se la lista è vuota sicuramente non riuscirò a migliorare la soluzione attuale 
				iterazioni_non_migliorative = 100;
			}
			else {												//Altrimenti provo ad eliminare una mossa tabu e riprovo
				iterazioni_non_migliorative++;					//Incremento il numero di iterazioni non migliorative
				listaTabu.erase(listaTabu.begin());
			}
		}
		else {
			iterazioni_non_migliorative = 0;					//Se in questa iterazione ho avuto miglioramenti allora il numero di iterazioni non migliorative consegutive è 0
		}
	}
	r = *r_migliore;										    //A questo punto r diventa la migliore r trovata
	calcola_domanda_percorso(r);								//Calcolo tutti i valori corretti dell'attuale rotta migliore.
	calcola_lunghezza_percorso(r);
	calcola_tempo_percorso(r);
}

void swap_nodi(int i, int j, Rotta& r) {
	if (i < j) {
		int t = i;
		i = j;
		j = t;
	}
	std::list<Nodo>::iterator iter_i, iter_j;

	iter_i = r.percorso.begin();
	iter_j = r.percorso.begin();

	advance(iter_i, i);
	advance(iter_j, j);

	Nodo temp = *iter_i;
	r.percorso.erase(iter_i);
	iter_i = r.percorso.begin();
	advance(iter_i, i);
	r.percorso.insert(iter_i, *iter_j);

	r.percorso.erase(iter_j);
	iter_j = r.percorso.begin();
	advance(iter_j, j);
	r.percorso.insert(iter_j, temp);
} 

bool mossa_in_lista_tabu(Mossa_opt mossa, const vector<Mossa_opt>& V) {
	bool presente = 0;
	for (int i = 0; i < V.size(); i++) {
		if (V[i].indice1 == mossa.indice1 && V[i].indice2 == mossa.indice2 || V[i].indice1 == mossa.indice2 && V[i].indice2 == mossa.indice1)
			presente = 1;
	}
	return presente;
}