#include "Search.h"

extern double a;																		//Coefficiente di penalit� per violazioni di capacit�
extern double b;																		//Coefficiente di penalit� per violazioni di tempo
extern int dim;																		//Dimensione del problema 
extern float** adjMatrix;															//Matrice di adiacenza relativa ad i costi
extern vector<Nodo> V;


void tabu_search(Soluzione& S, vector<Nodo>& W, int q, int p1, int p2, int theta_min, int theta_max, int g, int h, int n_max) {

	//STRUTTURA DELL'ALGORITMO DI TABU SEARCH
	
	//Inizializzazione delle variabili locali
	int t = 1;																		//contatore delle iterazioni
	Tabulist listaTabu;																//lista Tabu vuota
	vector<Nodo> PSI;																//Sotto insieme PSI di W contenente i q nodi estratti casualmente	
	
	// Contatori per gestire i coefficienti di penalit�
	int L_ammissibile=0;
	int L_inammissibile=0;
	int C_ammissibile=0;
	int C_inammissibile=0;
	int n_miglioramenti=0;
	

	Soluzione Soluzione_candidata;														//Soluzione Candidata
	vector<Soluzione> Soluzioni_temporanee;												//k*q soluzioni temporanee da utilizzare in valutazione_mossa()  [S']
	Soluzione Soluzione_migliore_fattibile;												//S*
	Soluzione Soluzione_migliore;														//S~*
	float delta_max = 0.0;																
	float f_v = 0.0;
	float F2_candidata_precedente = 0;

	a = 1;
	b = 1;

	//INIZIALIZZAZIONE DELLE SOLUZIONI
	inizializza_soluzione(Soluzione_candidata,S.cluster.size());
	inizializza_soluzione(Soluzione_migliore_fattibile, S.cluster.size());
	inizializza_soluzione(Soluzione_migliore, S.cluster.size());
	if (S.ammissibile) {
		assegna_soluzione(Soluzione_migliore_fattibile, S);
	}
	assegna_soluzione(Soluzione_migliore, S);

	while (n_miglioramenti != n_max && t < STOP) {

		t == 1 ? F2_candidata_precedente = 0 : F2_candidata_precedente = Soluzione_candidata.F2;
		seleziona_vertici(PSI, q, W, W.size());		
		assegna_soluzione(Soluzione_candidata, valutazione_mossa(PSI, q, p1, Soluzioni_temporanee, listaTabu, S, Soluzione_migliore_fattibile, Soluzione_migliore, delta_max, f_v));	
		for (int i = 0; i < Soluzioni_temporanee.size(); i++)
			distruggi_soluzione(Soluzioni_temporanee[i]);
		Soluzioni_temporanee.clear();


		determina_prossima_soluzione(Soluzione_candidata, S, L_ammissibile, L_inammissibile, C_ammissibile, C_inammissibile);
		aggiorna_lista_parametri(Soluzione_candidata, listaTabu, Soluzione_migliore_fattibile, Soluzione_migliore, delta_max, f_v, t, n_miglioramenti, theta_min, theta_max, F2_candidata_precedente);
		correzione_penalita(L_ammissibile, L_inammissibile, C_ammissibile, C_inammissibile, h);

		//print_soluzione(S);
	}

	//ASSEGNA SOLUZIONE
	if (Soluzione_migliore_fattibile.ammissibile)
		assegna_soluzione(S, Soluzione_migliore_fattibile);
	else
		assegna_soluzione(S, Soluzione_migliore);

	//DISTRUZIONE DELLE SOLUZIONI
	
	distruggi_soluzione(Soluzione_migliore_fattibile);
	distruggi_soluzione(Soluzione_migliore);
	distruggi_soluzione(Soluzione_candidata);
}

void seleziona_vertici(vector<Nodo>& PSI, int q, vector<Nodo> W, int dim) {
	//Genera un sottografo PSI di dimensione q a partire dal sottografo W di dimensione dim

	PSI.clear();																	//Svuoto l'insieme PSI

	for (int i = 0; i < q; i++) {													//Per ogni i da 0 a 1		
		int j = (int)(rand() % (dim-i));											//definisco un indice casuale j
		PSI.push_back(W[j]);														//Inserisco in PSI l'elemento di W di posizione j
		W.erase(W.begin() + j);														//Elimino l'elemento dalla copia di W
	}	

}

Soluzione valutazione_mossa(const vector<Nodo>& PSI, int q,int p1, vector<Soluzione>& Soluzioni_temporanee, Tabulist& listaTabu, Soluzione & S, Soluzione& Soluzione_migliore_fattibile, Soluzione& Soluzione_migliore, const float& delta_max, const int& f_v) {
	
	vector<int> N;

	for (int i = 0; i < q; i++) {
		costruzione_vicinato(PSI[i], N, p1);
		identifica_soluzioni_temporanee(N, PSI[i].indice, listaTabu,S,Soluzioni_temporanee, i);  
		N.clear();
	}
	controlla_soluzioni_tabu(Soluzioni_temporanee, listaTabu, S, Soluzione_migliore_fattibile, Soluzione_migliore, delta_max, f_v);
	if (Soluzioni_temporanee.size() > 0)return determina_soluzione_candidata(Soluzioni_temporanee);
	else return S;

}

void costruzione_vicinato(const Nodo& v, vector<int>& N, const int& p1) {

	vector<float> adjVector;													//Vettore contenente tutti i nodi collegati al nodo considerato
	for (int k = 0; k < dim; k++) {
		adjVector.push_back(adjMatrix[v.indice][k]);
	}
	//Inserisco i p1 elementi pi� piccoli di adjVector in N
	for (int j = 0; j < p1; j++) {												
		int min = minimo_indice(adjVector, v.indice);							//Calcolo l'indice del minimo del vettore adjVector
		N.push_back(min);														//Inserisco l'indice in N 
		adjVector[min] = FLOAT_INF;												//Pongo ad infinito l'elemento da adjVector
	}

}

int minimo_indice(vector<float>& A, const int & indice_nodo) {
	float min = FLOAT_INF;
	int k = 0;
	for (int i = 0; i < A.size(); i++) {
		if (A[i] < min && i != indice_nodo && i != 0) {
			min = A[i];
			k = i;
		}
	}
	return k;
}

void identifica_soluzioni_temporanee(const vector<int>& N, const int& indice_nodo, const Tabulist listaTabu, Soluzione & S, vector<Soluzione>& Soluzioni_temporanee, const int & indice_q) {

	int i = 0;
	int indice_intersezione;
	float costo_min = FLOAT_INF;
	vector<Mossa> temp;
	/*dato il nodo V[i], per ogni rotta, diversa da quella di V[i] che intersechi N, calcola la mossa che sposta V[i] in quella rotta*/
	while (i < S.cluster.size()) {																					//Per ogni rotta di S
		if (posizione_nodo(*S.cluster[i], V[indice_nodo]) == -1) {													//Diversa da quella del nodo v	
			if (intersezione(N, S.cluster[i], indice_intersezione) || S.cluster[i]->percorso.size() == 1) {			//Se nella rotta � presente almeno un vicino di V oppure la rotta contiene solo il nodo deposito
				temp.push_back(calcola_mossa(*S.cluster[rotta_del_nodo(S, V[indice_nodo])], *S.cluster[i], indice_nodo, indice_intersezione));  //Calcola la mossa relativa e inseriscila nel vettore delle mosse temporanee
			}
			else {
				Mossa NoMossa;																						//Altrimenti
				NoMossa.costo = FLOAT_INF;																			//La mossa ha costo infinito (nessuna mossa � stata fatta)
				temp.push_back(NoMossa);																			//Inserisci la mossa nel vettore delle mosse temporanee
			}
		}
		i++;
	}
	//Ho cos� costruito un vettore temp composto da k mosse
	//� ora necessario costruire, a partire da queste mosse, le soluzioni S.
	for (int i = 0; i < temp.size(); i++)																			//Per ogni mossa temporanea
		if (temp[i].costo - FLOAT_INF < 0) {																		//Se la mossa � valida
			costruisci_soluzione(Soluzioni_temporanee, temp[i], S);													//Costrusici la soluzione temporanea a partir dalla mossa
		}
		

}

void costruisci_soluzione(vector<Soluzione>& Soluzioni_temporanee, const Mossa & mossa, const Soluzione & S ) {
	Soluzione S_temp ;
	std::list<Nodo>::iterator it_erase, it_insert;

	//ASSEGNAZIONE PARAMETRI STATICI SOLUZIONE

	inizializza_soluzione(S_temp, S.cluster.size());
	assegna_soluzione(S_temp, S);
	S_temp.m = mossa;
	
	//spostamento degli iteratori sulla rotta origine e quella destinazione per posizionarsi sui giusti punti di inserimento ed eliminazione

	it_erase = S_temp.cluster[mossa.rotta_origine]->percorso.begin();
	it_insert = S_temp.cluster[mossa.rotta_destinazione]->percorso.begin();
	
	advance(it_erase, mossa.posizione_in_origine);
	advance(it_insert, mossa.posizione_in_destinazione);

	//Eliminazione del nodo dalla rotta origine

	S_temp.cluster[mossa.rotta_origine]->percorso.erase(it_erase);
	
	//Inserimento del nodo nella rotta destinazione
	if (S_temp.cluster[mossa.rotta_destinazione]->percorso.size() > 1)							
		S_temp.cluster[mossa.rotta_destinazione]->percorso.insert(it_insert, V[mossa.nodo]);
	else { //Se la rotta contiene solo il nodo origine, basta una push
		S_temp.cluster[mossa.rotta_destinazione]->percorso.push_back(V[mossa.nodo]);
		S_temp.m.posizione_in_destinazione = posizione_nodo(*S_temp.cluster[mossa.rotta_destinazione], V[mossa.nodo]);
	}

	//RICALCOLA I PARAMETRI DELLE ROTTE APPENA MODIFICATE
	calcola_lunghezza_percorso(*S_temp.cluster[mossa.rotta_origine]);
	calcola_domanda_percorso(*S_temp.cluster[mossa.rotta_origine]);
	calcola_tempo_percorso(*S_temp.cluster[mossa.rotta_origine]);
	
	calcola_lunghezza_percorso(*S_temp.cluster[mossa.rotta_destinazione]);
	calcola_domanda_percorso(*S_temp.cluster[mossa.rotta_destinazione]);
	calcola_tempo_percorso(*S_temp.cluster[mossa.rotta_destinazione]);
	
	//CALCOLA I PARAMETRI DI FUNZIONE OBIETTIVO DELLA SOLUZIONE E INSERISCILA TRA QUELLE TEMPORANEE

	calcola_F(S_temp); 

	Soluzioni_temporanee.push_back(S_temp);

}

bool intersezione(const vector<int>& N, Rotta * cluster, int& indice_intersezione) {

	for (int i = 0; i < N.size(); i++) {																		//Per ogni nodo vicino in N
		for (std::list<Nodo>::iterator it = cluster->percorso.begin(); it != cluster->percorso.end(); ++it)		//Scorri la rotta
			if (N[i] == it->indice) {																			//Se nella rotta � presente un nodo di N
				indice_intersezione = posizione_nodo(*cluster, *it) ;											//assegna la posizione nella rotta r del nodo trovato
				return true;																					
			}
	}

	return false;
}

Mossa calcola_mossa(Rotta Rr, Rotta Rs, const int& indice_origine, const int& indice_destinazione) {

	Mossa mossa_temporanea;

	/*Inizializzo i valori della mossa a partire dall' indice origine, quello destinazione e le rotte*/
	mossa_temporanea.costo = 0;
	mossa_temporanea.posizione_in_origine = posizione_nodo(Rr, V[indice_origine]);
	mossa_temporanea.nodo = V[indice_origine].indice;
	mossa_temporanea.rotta_origine = Rr.indice;
	mossa_temporanea.rotta_destinazione = Rs.indice;


	if (Rs.percorso.size() > 1)													//Se la rotta contiene altri nodi oltre il nodo deposito
		mossa_temporanea.posizione_in_destinazione = indice_destinazione;
	else	
		mossa_temporanea.posizione_in_destinazione = 1;					

	return mossa_temporanea;
}

void controlla_soluzioni_tabu(vector<Soluzione>& Soluzioni_temporanee, Tabulist& listaTabu, Soluzione& S, Soluzione& Soluzione_migliore_fattibile, Soluzione& Soluzione_migliore, const float& delta_max, const int& f_v) {

	//CONTROLLO DEI TABU E ASSEGNAZIONE DI F_temp

	for (int i = 0; i < Soluzioni_temporanee.size(); i++) {

		bool tabu = cerca_tabu(listaTabu, Soluzioni_temporanee[i].m);
		bool aspirazione = false;

		//Se la soluzione � tabu, verifica i criteri di aspirazione

		if (tabu) {												
			if (Soluzioni_temporanee[i].ammissibile && Soluzioni_temporanee[i].F1 < Soluzione_migliore_fattibile.F1) aspirazione = true;
			else if (!Soluzioni_temporanee[i].ammissibile && Soluzioni_temporanee[i].F2 < Soluzione_migliore.F2) aspirazione = true;
		}

		//Se i criteri di aspirazione sono superati, oppure la soluzione non � tabu, passa al calcolo di F_temp.

		if (!tabu || aspirazione) {
			if (Soluzioni_temporanee[i].F2 < S.F2) Soluzioni_temporanee[i].F_temp = Soluzioni_temporanee[i].F2;
			else Soluzioni_temporanee[i].F_temp = Soluzioni_temporanee[i].F2; + delta_max * f_v * 0.01 * sqrt(S.cluster.size());
		}
		else Soluzioni_temporanee[i].F_temp = FLOAT_INF;

	}

}

bool cerca_tabu(Tabulist& listaTabu, const Mossa& mossa) {
	bool trovato = false;
	int i = 0;

	while (!trovato && i < listaTabu.mossa_tabu.size()) {
		if(listaTabu.mossa_tabu[i].m.costo == mossa.costo &&
		   listaTabu.mossa_tabu[i].m.nodo == mossa.nodo &&
		   listaTabu.mossa_tabu[i].m.posizione_in_destinazione == mossa.posizione_in_destinazione &&
		   listaTabu.mossa_tabu[i].m.posizione_in_origine == mossa.posizione_in_origine &&
		   listaTabu.mossa_tabu[i].m.rotta_destinazione == mossa.rotta_destinazione &&
		   listaTabu.mossa_tabu[i].m.rotta_origine== mossa.rotta_origine)
		trovato = true;
		
	i++;
	}

	return trovato;
}

Soluzione determina_soluzione_candidata(vector<Soluzione>& Soluzioni_temporanee) {
	float min = FLOAT_INF;
	int best_index = 0;

	/*Cerca la soluzione temporanea con il minor valore di F_temp*/
	for (int i = 0; i < Soluzioni_temporanee.size(); i++) {
		if (Soluzioni_temporanee[i].F_temp - min < 0) {
			min = Soluzioni_temporanee[i].F_temp;
			best_index = i;
		}
	}

	//Seleziono il nodo interessato alla mossa e ne aumento il numero di spostamenti

	std::list<Nodo>::iterator it = Soluzioni_temporanee[best_index].cluster[Soluzioni_temporanee[best_index].m.rotta_destinazione]->percorso.begin();
	advance(it, Soluzioni_temporanee[best_index].m.posizione_in_destinazione);
	it->spostamenti++;

	return Soluzioni_temporanee[best_index];
}

void determina_prossima_soluzione(Soluzione& Soluzione_candidata, Soluzione& S, int& L_ammissibile, int& L_inammissibile, int& C_ammissibile, int& C_inammissibile) {
	
	//Gestioni dei contatori : Se la soluzione candidata non � fattibile per capacit� o tempo, o � fattibile, incremento il contatore relativo. 

	if (Soluzione_candidata.penalita_capacita > 0) {
		C_inammissibile++;
		C_ammissibile = 0;
	}
	else {
		C_ammissibile++;
		C_inammissibile = 0;
	}

	if (Soluzione_candidata.penalita_tempo > 0) {
		L_inammissibile++;
		L_ammissibile = 0;
	}
	else {
		L_ammissibile++;
		L_inammissibile = 0;
	}

	//Sostituzione della Soluzione S con la soluzione_candidata se soddisfa i requisiti
	for (int i = 0; i < Soluzione_candidata.cluster.size(); i++)
		tsp_2_opt(*Soluzione_candidata.cluster[i]);
	calcola_F(Soluzione_candidata);
	if (!(Soluzione_candidata.F2 > S.F2 && S.ammissibile)) {
		assegna_soluzione(S, Soluzione_candidata);
	}
	else;
}

void aggiorna_lista_parametri(const Soluzione& Soluzione_candidata, Tabulist& lista_tabu, Soluzione& Soluzione_migliore_fattibile, Soluzione& Soluzione_migliore, float& delta_max, float& f_v, int& t, int& n_miglioramenti, const int& theta_min, const int& theta_max, const float & F2_prec)
{
	/* Iteratore per scorrere il vettore di nodi V */
	std::vector<Nodo>::iterator it;

	/* Inserimento della mossa nella lista tabu */
	srand(t);
	int TTL_temp = rand() % (theta_max - theta_min) + theta_min;
	Tabu tabuTemp;
	tabuTemp.m = Soluzione_candidata.m;
	tabuTemp.TTL = TTL_temp + t;

	//BISOGNEREBBE FARE UNA RICERCA PER VEDERE SE LA MOSSA � GI� PRESENTE IN LISTA, MA NON � NECESSARIA AI FINI DEL FUNZIONAMENTO

	lista_tabu.mossa_tabu.push_back(tabuTemp); // inserisco la mossa tabu nella lista

	/* Aggiornamento S*, S~*, F*, F~* */

	bool soluzione_migliore_aggiornata = 0; // Serve a verificare se la soluzione verr� aggiornata. Utile per la gestione di n_miglioramenti.
	if (Soluzione_candidata.ammissibile == 1) {
		if (Soluzione_candidata.F1 < Soluzione_migliore_fattibile.F1)
		{
			assegna_soluzione(Soluzione_migliore_fattibile, Soluzione_candidata);
			assegna_soluzione(Soluzione_migliore, Soluzione_candidata);
			soluzione_migliore_aggiornata = 1;
		}
	}
	else {
		if (Soluzione_candidata.F2 < Soluzione_migliore.F2)
		{
			assegna_soluzione(Soluzione_migliore, Soluzione_candidata);
			soluzione_migliore_aggiornata = 1;
		}
	}

	/* Aggiornamento di delta_max = |F2(Soluzione_candidata) - F2(Soluzione_candidata_precedente)| */
	delta_max = abs(Soluzione_candidata.F2 - F2_prec);

	/* Aggiornamento di f_v=numero_spostamenti_nodo/t */
	Nodo Nodo_temp = V[Soluzione_candidata.m.nodo];

	f_v = float(Nodo_temp.spostamenti) / float(t);

	/* Aggiornamento di n_miglioramenti per verificare la condizione di stop */
	if (soluzione_migliore_aggiornata == 0)
		n_miglioramenti++; // Se la soluzione migliore non � stata aggiornata il contatore viene aumentato. Arrivati a una certa soglia l'algoritmo terminer�.
	else n_miglioramenti = 0;
	/* Aggiornamento dell'indice del costrutto iterativo */
	t = t + 1;

}

void correzione_penalita(int& L_ammissibile, int& L_inammissibile, int& C_ammissibile, int& C_inammissibile, const int & h) {

	//Se la soluzione migliore calcolata alle ultime h iterazioni � ammissibile o non lo �, modifica relativamente i coefficienti di penalit�.
	if (C_ammissibile == h) {
		a = a / 2;
		C_ammissibile = 0;
	}
	if (C_inammissibile == h) {
		a = a * 2;
		C_inammissibile = 0;
	}
	if (L_ammissibile == h) {
		b = b / 2;
		C_ammissibile = 0;
	}
	if (L_inammissibile == h) {
		b = b * 2;
		C_inammissibile = 0;
	}

}


//VERSIONE RENDERIZZATA DELLA TABU SEARCH

/*void rendered_tabu_search(Soluzione& S, vector<Nodo>& W, int q, int p1, int p2, int theta_min, int theta_max, int g, int h, int n_max, SDL_Renderer* renderer, Frame Screen) {

	//STRUTTURA DELL'ALGORITMO DI TABU SEARCH

	//Inizializzazione delle variabili locali
	int t = 1;																		//contatore delle iterazioni
	Tabulist listaTabu;																//lista Tabu vuota
	vector<Nodo> PSI;																//Sotto insieme PSI di W contenente i q nodi estratti casualmente	

	// Contatori per gestire i coefficienti di penalit�
	int L_ammissibile = 0;
	int L_inammissibile = 0;
	int C_ammissibile = 0;
	int C_inammissibile = 0;
	int n_miglioramenti = 0;


	Soluzione Soluzione_candidata;														//Soluzione Candidata
	vector<Soluzione> Soluzioni_temporanee;												//k*q soluzioni temporanee da utilizzare in valutazione_mossa()  [S']
	Soluzione Soluzione_migliore_fattibile;												//S*
	Soluzione Soluzione_migliore;														//S~*
	float delta_max = 0.0;
	float f_v = 0.0;
	float F2_candidata_precedente = 0;

	a = 1;
	b = 1;

	//INIZIALIZZAZIONE DELLE SOLUZIONI
	inizializza_soluzione(Soluzione_candidata, S.cluster.size());
	inizializza_soluzione(Soluzione_migliore_fattibile, S.cluster.size());
	inizializza_soluzione(Soluzione_migliore, S.cluster.size());

	if (S.ammissibile) {
		assegna_soluzione(Soluzione_migliore_fattibile, S);
	}
	assegna_soluzione(Soluzione_migliore, S);

	while (n_miglioramenti != n_max && t < STOP) {

		t == 1 ? F2_candidata_precedente = 0 : F2_candidata_precedente = Soluzione_candidata.F2;
		seleziona_vertici(PSI, q, W, W.size());
		assegna_soluzione(Soluzione_candidata, valutazione_mossa(PSI, q, p1, Soluzioni_temporanee, listaTabu, S, Soluzione_migliore_fattibile, Soluzione_migliore, delta_max, f_v));
		for (int i = 0; i < Soluzioni_temporanee.size(); i++)
			distruggi_soluzione(Soluzioni_temporanee[i]);
		Soluzioni_temporanee.clear();


		determina_prossima_soluzione(Soluzione_candidata, S, L_ammissibile, L_inammissibile, C_ammissibile, C_inammissibile);
		aggiorna_lista_parametri(Soluzione_candidata, listaTabu, Soluzione_migliore_fattibile, Soluzione_migliore, delta_max, f_v, t, n_miglioramenti, theta_min, theta_max, F2_candidata_precedente);
		correzione_penalita(L_ammissibile, L_inammissibile, C_ammissibile, C_inammissibile, h);

		if (S.ammissibile == 1) {
			ClearRenderSized(renderer, Screen);
			DrawSolutionSized(renderer, Soluzione_candidata, pow(Screen.subscreen,2), Screen.index_frame);
			SDL_RenderPresent(renderer);
		}

	}
	
	//ASSEGNA SOLUZIONE
	if(Soluzione_migliore_fattibile.ammissibile)
		assegna_soluzione(S, Soluzione_migliore_fattibile);
	else
		assegna_soluzione(S, Soluzione_migliore);

	ClearRenderSized(renderer, Screen);
	DrawSolutionSized(renderer, S, pow(Screen.subscreen, 2), Screen.index_frame);
	SDL_RenderPresent(renderer);

	//DISTRUZIONE DELLE SOLUZIONI

	distruggi_soluzione(Soluzione_migliore_fattibile);
	distruggi_soluzione(Soluzione_migliore);
	distruggi_soluzione(Soluzione_candidata);
}*/