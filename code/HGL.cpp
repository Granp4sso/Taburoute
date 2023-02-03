#include "HGL.h"

//SPAZIO DI INIZIALIZZAZIONE PER LE VARIABILI GLOBALI AL PROBLEMA

double a;																		//Coefficiente di penalit� per violazioni di capacit�
double b;																		//Coefficiente di penalit� per violazioni di tempo
int dim;																		//Dimensione del problema 
float** adjMatrix;																//Matrice di adiacenza relativa ad i costi
vector<Nodo> V;																	//Insieme dei Nodi del grafo
vector<Soluzione> S;															//Vettore delle lambda soluzioni iniziali
int N_VEICOLI;
int n_veicoli;

float Q;																	//Capacit� per veicolo
float L;																	//Lunghezza di attraversamento per veicolo

void inizializza_variabili(const float& q, const float& l, const int & veicoli) {

	a = 1;																	//Inizializzazione coefficiente di penalit� di capacit�
	b = 1;																	//Inizializzazione coefficiente di penalit� di tempo(lunghezza)
	n_veicoli = veicoli;													//Inizializzazione numero di veicoli
	dim = V.size();															//Dimensione del problema

	//Costruzione della matrice di adiacenza 
	adjMatrix = new float* [dim];											//alloca lo spazio per dim puntatori
	for (int i = 0; i < dim; i++)											//per ognuno dei dim puntatori 
		adjMatrix[i] = new float[dim];										//alloca un vettore di dimensione dim

	for (int i = 0; i < dim; i++) {
		for (int j = i; j < dim; j++) {										//Riempimento della matrice con i costi euclidei per andare da un nodo i ad uno j
			adjMatrix[i][j] = sqrt(pow(V[i].y - V[j].y, 2) + pow(V[i].x - V[j].x, 2));
		}
	}

	for (int i = 0; i < dim; i++) {											//Riempimento della matrice con i costi euclidei per andare da un nodo j ad uno i
		for (int j = i; j < dim; j++) {
			adjMatrix[j][i] = adjMatrix[i][j];
		}
	}

	Q = q;
	L = l;

}

void distruggi_variabili(Soluzione& S)
{
	for (int i = 0; i < dim; i++) delete adjMatrix[i];

	for (int i = 0; i < 4; i++) delete S.cluster[i];
}

void inizializza_soluzione(Soluzione& S, const int& n) {
	for (int i = 0; i < n; i++)
		S.cluster.push_back(new Rotta());

	S.F1 = FLOAT_INF;
	S.F2 = FLOAT_INF;
	S.F_temp = FLOAT_INF;
	S.penalita_capacita = 0;
	S.penalita_tempo = 0;
	S.ammissibile = 0;
}
void distruggi_soluzione(Soluzione& S) {

	int n = S.cluster.size();

	for (int i = 0; i < n; i++)
		delete S.cluster[i];

}
void assegna_soluzione(Soluzione& S1, const Soluzione& S2) {
	S1.F1 = S2.F1;
	S1.F2 = S2.F2;
	S1.ammissibile = S2.ammissibile;
	S1.penalita_capacita = S2.penalita_capacita;
	S1.penalita_tempo = S2.penalita_tempo;
	S1.F_temp = S2.F_temp;
	S1.m = S2.m;

	for (int i = 0; i < S2.cluster.size(); i++) {
		*S1.cluster[i] = *S2.cluster[i];
	}
}

void calcola_lunghezza_percorso(Rotta& R) { //TESTATA E FUNZIONANTE

	int prev = 0;
	float costo = 0;
	float tempo = 0;

	for (std::list<Nodo>::iterator it = R.percorso.begin(); it != R.percorso.end(); ++it) {
		costo += adjMatrix[prev][it->indice];
		prev = it->indice;
	}
	costo += adjMatrix[prev][0];
	R.lunghezza = costo;

}

void calcola_domanda_percorso(Rotta& R) {

	float domanda = 0;

	for (std::list<Nodo>::iterator it = R.percorso.begin(); it != R.percorso.end(); ++it) {
		domanda += it->domanda;
	}

	R.domanda_totale = domanda;
}

void calcola_tempo_percorso(Rotta& R) {

	float tempo = 0;

	for (std::list<Nodo>::iterator it = R.percorso.begin(); it != R.percorso.end(); ++it) {
		tempo += it->tempo;
	}

	R.tempo_servizio = tempo;
}

void calcola_F(Soluzione& S) {

	float Penalita_Capacita = 0;
	float Penalita_Tempo = 0;

	//CALCOLO DI F1

	S.F1 = 0;
	S.F2 = 0;

	for (int j = 0; j < S.cluster.size(); j++)
		S.F1 += S.cluster[j]->lunghezza;

	//CALCOLO DI F2

	for (int i = 0; i < S.cluster.size(); i++) {
		S.cluster[i]->domanda_totale - Q > 0 ? Penalita_Capacita += S.cluster[i]->domanda_totale - Q : Penalita_Capacita += 0;
		S.cluster[i]->tempo_servizio + S.cluster[i]->lunghezza - L > 0 ? Penalita_Tempo += S.cluster[i]->tempo_servizio + S.cluster[i]->lunghezza - L : Penalita_Tempo += 0;
	}

	S.penalita_capacita = Penalita_Capacita;
	S.penalita_tempo = Penalita_Tempo;

	S.F2 += S.F1 + a * Penalita_Capacita + b * Penalita_Tempo;

	S.F1 == S.F2 ? S.ammissibile = true : S.ammissibile = false;

}

int posizione_nodo(Rotta& R, const Nodo& v) { //TESTATA E FUNZIONANTE

	int i = 0;

	for (std::list<Nodo>::iterator it = R.percorso.begin(); it != R.percorso.end(); ++it) {
		if (it->indice == v.indice) return i;
		else i++;
	}

	return -1;
}

int rotta_del_nodo(const Soluzione& S, const Nodo& N) {
	for (int i = 0; i < S.cluster.size(); i++) {
		for (std::list<Nodo>::iterator it = S.cluster[i]->percorso.begin(); it != S.cluster[i]->percorso.end(); ++it) {
			if (it->indice == N.indice) return i;
		}
	}
}

void preleva_dati(const string& nome) {
	fstream myfile;
	float indice;
	float x;
	float y;
	float domanda;
	Nodo temp;

	myfile.open(nome, ios::in);

	if(myfile)
	{
		
		while (myfile >> indice >> x >> y >> domanda) {
			if (indice > 0) {
				temp.indice = indice - 1;
				temp.x = x;
				temp.y = y;
				temp.domanda = domanda;
				temp.tempo = 0;
				temp.spostamenti = 0;
				V.push_back(temp);
			}
			else {
				N_VEICOLI = -1 * indice;
			}

		}



	}

	myfile.close();


}



//FUNZIONI DI DEBUG E TESTING

void random_node(Nodo& v)
{
	v.domanda = rand() % 100;
	v.tempo = rand() % 10;
	v.x = rand() % 100;
	v.y = rand() % 100;
}

void random_set() //PER INIZIALIZZARE RANDOMICAMENTE IL VETTORE V
{
	srand(time(NULL));
	dim = 20;
	for (int i = 0; i < dim; i++)
	{
		Nodo v;
		v.indice = i;
		random_node(v);
		V.push_back(v);
	}

	V[0].domanda = 0;
	V[0].tempo = 0;
}

void print_node(const Nodo& v) {
	cout << "Node: " << v.indice << " x: " << v.x << " y: " << v.y << " tempo: " << v.tempo << " domanda: " << v.domanda << endl;
}

void print_set(const vector<Nodo>& V) {
	for (int i = 0; i < V.size(); i++) print_node(V[i]);
}

void print_adjMatrix() {
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			cout << adjMatrix[i][j] << " ";
		}
		cout << endl;
	}
}

void debug_build_S(Soluzione& S) {

	int N = 4; //Numero di clusters per questa soluzione dummy

	for (int i = 0; i < N; i++) S.cluster.push_back(new Rotta());

	for (int i = 0; i < N; i++) { S.cluster[i]->percorso.push_back(V[0]); S.cluster[i]->indice = i; }

	for (int i = 1; i < 4; i++) { S.cluster[0]->percorso.push_back(V[i]); }
	for (int i = 4; i < 7; i++) { S.cluster[1]->percorso.push_back(V[i]); }
	for (int i = 7; i < 10; i++) { S.cluster[2]->percorso.push_back(V[i]); }
	for (int i = 10; i < 13; i++) { S.cluster[3]->percorso.push_back(V[i]); }

	for (int i = 0; i < N; i++) calcola_lunghezza_percorso(*S.cluster[i]);
	calcola_F(S);
	S.F_temp = FLOAT_INF;
}

void print_soluzione(const Soluzione& S) {

	int j = 0;

	cout << "Soluzione:\n";
	cout << "Rotte:\n";
	for (int i = 0; i < S.cluster.size(); i++) {
		cout << i << " [" << S.cluster[i]->percorso.size() << "] : ";
		for (std::list<Nodo>::iterator it = S.cluster[i]->percorso.begin(); it != S.cluster[i]->percorso.end(); ++it)
		cout << it->indice << "->";
		cout << "0" << "\tDomanda:" << S.cluster[i]->domanda_totale;
		cout << "\tTempo di servizio:" << S.cluster[i]->tempo_servizio;
		cout << "\tLunghezza:" << S.cluster[i]->lunghezza;
		cout << "\tLunghezza + Tempo: " << S.cluster[i]->lunghezza + S.cluster[i]->tempo_servizio << endl;
	}

	cout << "Penalita - T :  " << S.penalita_tempo << "\t C : " << S.penalita_capacita << endl;
	cout << "\nF1:" << S.F1;
	cout << "\nF2:" << S.F2;
	cout << "\nF:" << S.F_temp;
	cout << "\nAmmissibile : " << S.ammissibile;

	cout << "\na: " << a << "\tb: " << b << endl;
	cout << endl;

}

void print_rotta(Rotta& r) {
	for (std::list<Nodo>::iterator it = r.percorso.begin(); it != r.percorso.end(); ++it)
		cout << it->indice << "->";
	cout << "0" << "\tDomanda:" << r.domanda_totale;
	cout << "\tTempo di servizio:" << r.tempo_servizio;
	cout << "\tLunghezza:" << r.lunghezza;
	cout << "\tLunghezza + Tempo: " << r.lunghezza + r.tempo_servizio << endl;
}