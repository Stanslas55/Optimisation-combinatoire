#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <thread>
#include <chrono>
#include <atomic>

using namespace std;



class Glouton
{ 
	public:
		Glouton();
		void parseFichier(string filename);
		bool* glouton();
		bool* gloutonAmeliore();		
		bool* gloutonRand(); // Premiere fonction de test, qu'on a amélioré en goltonr2.
		bool* gloutonRand2();
		
		~Glouton();

	public:
		size_t nb_capteurs, nb_cibles;
		size_t* cout_capteur;
		vector <size_t> *cibles, *capteurs;
};


//void arbitre(fsec limit, chrono::steady_clock::time_point compare);
void arbitre(string chaine);