#pragma once
#include "Search.h"
#include "tsp_2_opt.h"

#define RENDER_MODE true;
#define CLASSIC_MODE false;

/*Diversificazione e Intensificazione dell'algoritmo Search*/
void taburoute(int lambda, string istanza_input);

/*Gestisce le soluzioni migliori e migliori fattibili globali aggiornandole*/
void aggiorna_parametri(const Soluzione& Soluzione);

/*Genera casualemente una soluzione a partire da un vettore di nodi*/
Soluzione genera_soluzione(const int& m);

/*Taburoute con visualizzazione grafica*/
//void rendered_taburoute(int lambda, SDL_Renderer* renderer);

/*Wrapper per la gestione grafica o meno*/
//void taburoute(int lambda, bool mode);