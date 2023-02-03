#include "Taburoute.h"

extern int dim;																		//Dimensione del problema 
extern float Q;																		//Capacit� massima dei veicoli
extern float L;																		//Lunghezza massima percorribile dai veicoli
extern float** adjMatrix;															//Matrice di adiacenza relativa ad i costi
extern vector<Nodo> V;
extern double a;																		//Coefficiente di penalit� per violazioni di capacit�
extern double b;																		//Coefficiente di penalit� per violazioni di tempo
extern int n_veicoli;																		//Numero di veicoli(rotte)
Soluzione S_migliore_fattibile;	// La soluzione migliore globale � necessaria; essa servir� per tenere traccia della migliore soluzione per successive chiamate di search();
Soluzione S_migliore; 
Soluzione S_temporanea;	// La soluzione temporanea viene calcolata lambda volte nel ciclo iniziale della taburoute, come input a SEARCH(P1)

const string ISTANZA = "Istanza0.txt";
extern int N_VEICOLI;


void taburoute(int lambda, string istanza_input) {
	// Inizializzazione 
	preleva_dati(istanza_input);

	inizializza_variabili(100, FLOAT_INF,N_VEICOLI);
	inizializza_soluzione(S_migliore,n_veicoli);
	inizializza_soluzione(S_migliore_fattibile,n_veicoli);
	inizializza_soluzione(S_temporanea, n_veicoli);

// Configurazione parametri d'ingresso per SEARCH(P1) 
	vector<Nodo> W = V; W.erase(W.begin()); // Eliminazione del nodo deposito;
	int q = 5 * n_veicoli;
	int p_1 = V.size() / n_veicoli; 
	int p_2 = 0;
	int theta_min = 5;
	int theta_max = 10;
	float g = 0.01;
	int h = 10;
	int nmax = 100;

	//DIVERSIFICAZIONE

	for (int i = 0; i < lambda; i++)
	{
		assegna_soluzione(S_temporanea,genera_soluzione(n_veicoli));

		aggiorna_parametri(S_temporanea); // La soluzione generata deve eventualmente sostituire quanto memorizzato per la soluzione migliore.
		tabu_search(S_temporanea, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax); // SEARCH(P1)
		aggiorna_parametri(S_temporanea); // L'output di tabu_search() potrebbe aver generato una soluzione migliore.
		print_soluzione(S_temporanea);
	}

	cout << "SOLUZIONI MIGLIORI 1\n";
	print_soluzione(S_migliore_fattibile);
	print_soluzione(S_migliore);

	// if abbiamo S_migliore_fattibile chiamala su questa altrimenti su S_migliore
	if (S_migliore_fattibile.F1 - FLOAT_INF < 0) tabu_search(S_migliore_fattibile, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax); // SEARCH(P2)
	else tabu_search(S_migliore, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax); // SEARCH(P2)

	cout << "SOLUZIONI MIGLIORISSIME\n";
	print_soluzione(S_migliore_fattibile);
	print_soluzione(S_migliore);

	//INTENSIFICAZIONE

	// Configurazione parametri d'ingresso per SEARCH(P3) 
	// Per W � necessario prendere i V/2 nodi col maggior numero di spostamenti subiti.
	vector<Nodo> V_temp = W;
	// Insertion sort, ordinamento crescente 
	for (int j = 1; j < V_temp.size(); j++) {
		Nodo key = V_temp[j];
		int i = j - 1;
		while (i > 0 && V_temp[i].spostamenti > key.spostamenti) {
			V_temp[i + 1] = V_temp[i];
			i = i - 1;
		}
		V_temp[i + 1] = key;
	}
	W.clear();

	// Vengono sfruttati due indici, a causa dell'ordinamento crescente 
	for (int j = V_temp.size()-1; j >= V_temp.size() / 2; j--)
		W.push_back(V_temp[j]);

	cout << W.size() << endl;
	q = W.size();
	nmax = 50 * dim;

	if (S_migliore_fattibile.F1 - FLOAT_INF < 0) tabu_search(S_migliore_fattibile, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax); // SEARCH(P3)
	else tabu_search(S_migliore, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax); // SEARCH(P3)

	if (S_migliore_fattibile.F1 - FLOAT_INF < 0) {
		cout << "SOLUZIONE OTTIMA TROVATA:\n";
		print_soluzione(S_migliore_fattibile);
	}
	else cout << "IL PROBLEMA NON AMMETTE SOLUZIONI\n";
	//DISTRUZIONE

	distruggi_soluzione(S_migliore);
	distruggi_soluzione(S_migliore_fattibile);
	distruggi_soluzione(S_temporanea);
};

void aggiorna_parametri(const Soluzione& Soluzione_in) {

	// In questa funzione viene aggiornata la soluzione migliore globale 
	if (Soluzione_in.ammissibile == 0)
	{
		if (Soluzione_in.F2 < S_migliore.F2)
			assegna_soluzione(S_migliore,Soluzione_in);
	}
	else if (Soluzione_in.ammissibile == 1)
	{
		if (Soluzione_in.F1 < S_migliore_fattibile.F1)
		{
			assegna_soluzione(S_migliore,Soluzione_in);
			assegna_soluzione(S_migliore_fattibile,Soluzione_in); // In questo caso avremo che S* e S*~ coincideranno
		}
	}
}

Soluzione genera_soluzione(const int& m) {
	Soluzione S;								//Soluzione da creare
	inizializza_soluzione(S, m);

	vector <Nodo> V_Copy = V;					//Copia del vettore V
	V_Copy.erase(V_Copy.begin());				//Elimino il nodo sorgente
	vector <Nodo> Copy = V_Copy;           		//Copia del vettore dei nodi V senza l'origine
	vector <Nodo> V_casuale;					//Voglio ordinare i nodi in maniera casuale
	V_casuale.clear();

	srand(time(NULL));
	//inzializzo un vettore inserendo casualmente i Nodi di V
	for (int i = 0; i < dim - 1; i++) {														//Per ogni i da 0 a 1		
		int j = (int)(rand() % (dim - i - 1));												//definisco un indice casuale j decrescente con i
		V_casuale.push_back(Copy[j]);														//Inserisco un elemento della copia di V in V_casuale
		Copy.erase(Copy.begin() + j);														//Elimino l'elemento inserito
	}

	float domanda = 0;				//Domanda parziale durante la costruzione del cluster 		
	float lunghezza_percorso = 0;	//Lunghezza parziale del cluster durante la costruzione
	int indice_prec = 0;			//Indice del nodo inserito precedentemente. Serve per calcolare la distanza del nodo da quello precedente
	int n_nodi = dim - 2;			//Numero dei nodi che non sono ancora entrati in un cluster
	int c = 0;						//Numero cluster creati


	S.cluster[c]->indice = c;
	S.cluster[c]->percorso.push_back(V[0]);																		//Inserisco il nodo sorgente nel cluster
	while (n_nodi >= 0 && c < m - 1) {																			//Se ho inserito tutti i nodi oppure mi manca solo un cluster mi fermo
		if ((domanda + V_casuale[n_nodi].domanda <= Q) && (lunghezza_percorso + V_casuale[n_nodi].tempo + adjMatrix[indice_prec][V_casuale[n_nodi].indice] + adjMatrix[0][V_casuale[n_nodi].indice] <= L)) {	//Se far entrare il nodo nel cluster rispetta i vincoli di capacit� e lunghezza lo faccio entrare. lunghezza= lunghezza corrente + arco per inserire il nodo + arco per tornare al nodo sorgente
			domanda += V_casuale[n_nodi].domanda;																//tengo conto della domanda attuale
			lunghezza_percorso += V_casuale[n_nodi].tempo + adjMatrix[indice_prec][V_casuale[n_nodi].indice];   //tengo conto della lunghezza attuale
			//cout << "Lunghezza percorso: " << lunghezza_percorso << endl;
			S.cluster[c]->percorso.push_back(V_casuale[n_nodi]);												//Inserisco il nodo nella rotta
			indice_prec = V_casuale[n_nodi].indice;																//Salvo l'indice del nodo precedentemente inserito
			V_casuale.pop_back();
			n_nodi--;
		}
		else {																									//Altrimenti il cluster � pieno e calcolo tutti i valori della rotta creata
			calcola_lunghezza_percorso(*S.cluster[c]);
			calcola_domanda_percorso(*S.cluster[c]);
			calcola_tempo_percorso(*S.cluster[c]);
			indice_prec = 0;
			domanda = 0;
			lunghezza_percorso = 0;
			c++;
			S.cluster[c]->indice = c;
			S.cluster[c]->percorso.push_back(V[0]);																//Inserisco il nodo sorgente nel cluster
		}
	}

	//Se non ho ancora caricato tutti i nodi e mi rimane solo un cluster Inserisco tutti i nodi nell'ultimo cluster (La funzione calcola F capir� se la solzione � ammissibile o meno)
	if (n_nodi >= 0) {
		while (n_nodi >= 0) {
			S.cluster[c]->percorso.push_back(V_casuale[n_nodi]);
			V_casuale.pop_back();
			n_nodi--;
		}
		calcola_lunghezza_percorso(*S.cluster[c]);
		calcola_domanda_percorso(*S.cluster[c]);
		calcola_tempo_percorso(*S.cluster[c]);
		c++;
	}

	//Se ho inserito tutti i nodi ma ho ancora dei camion liberi inizializzo le rotte con il solo nodo sorgente
	if (c < m - 1) {
		calcola_lunghezza_percorso(*S.cluster[c]);			//Prima di inizializzare devo calcolare i valori dell'ultimo cluster
		calcola_domanda_percorso(*S.cluster[c]);
		calcola_tempo_percorso(*S.cluster[c]);
		c++;
		while (c < m) {										//Per ogni cluster mancante inserisco il nodo sorgente e inizalizzo i valori a 0
			S.cluster[c]->indice = c;
			S.cluster[c]->percorso.push_back(V[0]);
			S.cluster[c]->domanda_totale = 0;
			S.cluster[c]->lunghezza = 0;
			S.cluster[c]->tempo_servizio = 0;
			c++;
		}
	}

	//Calcolo tutti i valori della Soluzione dato l'insieme delle rotte 
	calcola_F(S);

	return S;
}
/*
void rendered_taburoute(int lambda, SDL_Renderer* renderer) {
	// Inizializzazione 
	preleva_dati(ISTANZA);
	cout << "V: \n";
	print_set(V);

	inizializza_variabili(100, 250, N_VEICOLI);
	inizializza_soluzione(S_migliore, n_veicoli);
	inizializza_soluzione(S_migliore_fattibile, n_veicoli);
	inizializza_soluzione(S_temporanea, n_veicoli);

	// Configurazione parametri d'ingresso per SEARCH(P1) 
	vector<Nodo> W = V; W.erase(W.begin()); // Eliminazione del nodo deposito;
	int q = 5 * n_veicoli;
	int p_1 = V.size() / n_veicoli;
	int p_2 = 0;
	int theta_min = 5;
	int theta_max = 10;
	float g = 0.01;
	int h = 10;
	int nmax = 100;

	//Oggetti di rendering

	Frame Screen;
	Screen.subscreen = sqrt(lambda);

	//DIVERSIFICAZIONE

	for (int i = 0; i < lambda; i++)
	{
		Screen.index_frame = i;
		assegna_soluzione(S_temporanea, genera_soluzione(n_veicoli));
		
		//RENDERING SOLUZIONE INIZIALE
		if (S_temporanea.ammissibile == 1) {
			ClearRenderSized(renderer, Screen);
			DrawSolutionSized(renderer, S_temporanea, pow(Screen.subscreen, 2), Screen.index_frame);
			SDL_RenderPresent(renderer);
		}
		//FINE RENDERING SOLUZIONE INIZIALE

		aggiorna_parametri(S_temporanea); // La soluzione generata deve eventualmente sostituire quanto memorizzato per la soluzione migliore.
		rendered_tabu_search(S_temporanea, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax, renderer, Screen); // SEARCH(P1)
		aggiorna_parametri(S_temporanea); // L'output di tabu_search() potrebbe aver generato una soluzione migliore.
		print_soluzione(S_temporanea);
	}

	//INIZIO SEZIONE RENDER 

	Screen.index_frame = 0;
	Screen.subscreen = 1;

	SDL_Delay(500);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	DrawFrameSized(renderer, 1);
	DrawSolutionSized(renderer, S_migliore_fattibile, Screen.subscreen, Screen.index_frame);
	SDL_RenderPresent(renderer);
	SDL_Delay(500);

	//FINE SEZIONE RENDER

	cout << "FINE DIVERSIFICAZIONE\n";
	print_soluzione(S_migliore_fattibile);
	print_soluzione(S_migliore);

	// if abbiamo S_migliore_fattibile chiamala su questa altrimenti su S_migliore
	if (S_migliore_fattibile.F1 - FLOAT_INF < 0) rendered_tabu_search(S_migliore_fattibile, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax, renderer, Screen); // SEARCH(P2)
	else rendered_tabu_search(S_migliore, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax, renderer,Screen); // SEARCH(P2)

	cout << "INTENSIFICAZIONE\n";
	print_soluzione(S_migliore_fattibile);
	print_soluzione(S_migliore);

	//INTENSIFICAZIONE

	// Configurazione parametri d'ingresso per SEARCH(P3) 
	// Per W � necessario prendere i V/2 nodi col maggior numero di spostamenti subiti.
	vector<Nodo> V_temp = W;
	// Insertion sort, ordinamento crescente 
	for (int j = 1; j < V_temp.size(); j++) {
		Nodo key = V_temp[j];
		int i = j - 1;
		while (i > 0 && V_temp[i].spostamenti > key.spostamenti) {
			V_temp[i + 1] = V_temp[i];
			i = i - 1;
		}
		V_temp[i + 1] = key;
	}
	W.clear();

	// Vengono sfruttati due indici, a causa dell'ordinamento crescente 
	for (int j = V_temp.size() - 1; j >= V_temp.size() / 2; j--)
		W.push_back(V_temp[j]);

	cout << W.size() << endl;
	q = W.size();
	nmax = 50 * dim;

	if (S_migliore_fattibile.F1 - FLOAT_INF < 0) rendered_tabu_search(S_migliore_fattibile, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax,renderer,Screen); // SEARCH(P3)
	else rendered_tabu_search(S_migliore, W, q, p_1, p_2, theta_min, theta_max, g, h, nmax, renderer, Screen); // SEARCH(P3)
	
	//aggiorna_parametri(S_temporanea); // Viene determinata la miglior soluzione.
	if (S_migliore_fattibile.ammissibile==1) {
		cout << "SOLUZIONE OTTIMA TROVATA:\n";
		print_soluzione(S_migliore_fattibile);
	}
	else cout << "IL PROBLEMA NON AMMETTE SOLUZIONI\n";
	//DISTRUZIONE

	distruggi_soluzione(S_migliore);
	distruggi_soluzione(S_migliore_fattibile);
	distruggi_soluzione(S_temporanea);
}

void taburoute(int lambda, bool mode) {

	if (mode) {

		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_Window* window = SDL_CreateWindow("SDL Prova", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800 + 200, SDL_WINDOW_OPENGL);
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_Event event;
		bool running = false;

		InitGraphEngine(renderer, window, 800, 800);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		DrawFrameSized(renderer, sqrt(lambda));
		SDL_RenderPresent(renderer);
		rendered_taburoute(lambda,renderer);
		while (!running) {
			while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) running = true;
		}

		DestroyGraphEngine(renderer, window);

	}
	else taburoute(lambda);
}
*/
