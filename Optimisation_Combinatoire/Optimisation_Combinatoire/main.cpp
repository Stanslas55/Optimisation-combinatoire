#pragma once
#include "Glouton.h"
#include "Genetique.h"



int main(int argc, char** argv)
{   	
	string filename, sortie, limite;

	if (argc != 4)
	{
		cout << "usage: ./monAlgo <temps>(s) <chemin_instance> <chemin_resultat>" << endl;
		return -1;
	}
	auto master = std::thread(arbitre, limite);
	limite = argv[1];
	//limite = "3";	


	srand(time(NULL));

	filename = argv[2];
	//filename = "inst41.txt";
	
	char* test = argv[3];
	string resultat = "Oui.";
	std::ofstream out(test);
	out << resultat;
	out.close();


	/*
	std::ofstream out(sortie);
	if (!out.is_open())
	{
		cout << "PROBLEM" << endl;
	}
	out << "101010";
	out.close();*/
	//sortie = "solution11.txt";
	vector <GUERRIER> population(POP_SIZE);

	Genetique arene = Genetique();
	arene.parseFichier(filename, sortie);
	arene.start(filename);
	
	master.join();
	return 0;

}