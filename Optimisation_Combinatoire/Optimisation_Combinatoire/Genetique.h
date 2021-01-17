#pragma once

#include "Glouton.h"

#include <vector>
#include <string>
#include <tuple>
 
#define POP_SIZE 140 // Penser à regarder la formule adaptée à la taille de la population pour les mutations.
#define PRESSION_EVOL 800 // 80% pour l'évolution.
#define NB_GENERATION 10// TODO: Voir un nombre adapté.

typedef bool* GUERRIER; 
typedef size_t scoreG;



class Genetique
{
public:
	Genetique();
	void parseFichier(string filename, string sortie);	
	void start(string filename);
	
	~Genetique();

	string sortie;

private:
	size_t nb_capteurs, nb_cibles;
	size_t* g_cout_capteur;
	vector <size_t>* cibles, * capteurs;

	size_t taille_population = POP_SIZE;
	vector<GUERRIER> population;
	scoreG* puissance;
	size_t proba_mutation;
	tuple <scoreG, GUERRIER> batman; // Meilleur guerrier de tous les temps.

	void initPopulation(Glouton *armee);
	void nextPopulation();
	
	tuple<GUERRIER, GUERRIER> croisement(tuple<GUERRIER, GUERRIER> elus);	
	tuple<GUERRIER, GUERRIER> mutation(tuple<GUERRIER, GUERRIER> elus);	
	GUERRIER el_reparator(GUERRIER G);
	
	size_t calculScore(GUERRIER G);
	size_t compareCombattants(size_t combattant1, size_t combattant2);
	void trouverBatman();


};