#pragma once
#include "HGL.h"

struct Mossa_opt {
	int indice1;
	int indice2;
};

/*Presa una rotta cerca un posizionamento che migliora la lunghezza del percorso*/
void tsp_2_opt(Rotta& r);

/*Scambia 2 nodi in posizione i e j della rotta r*/
void swap_nodi(int i, int j, Rotta& r);

/*Controlla se la mossa è presente in lista tabu*/
bool mossa_in_lista_tabu(Mossa_opt mossa, const vector<Mossa_opt>& V);
