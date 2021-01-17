#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "Entrez le nom du fichier à ouvrir !" << endl;
		return -1;
	}

	string ligne, binaries = "Binaries\n", subject_to = "\nSubject To\n", z = "Minimize\n z:", resultat = "";
	ifstream fichier(argv[1]);
	size_t limite, i, j, k, indice = 0, numero_capteur = 0, nb_capteurs_cibles = 0;

	size_t  nb_capteurs = 0, nb_cibles = 0;
	size_t* cout_capteur;
	vector <size_t>* cibles, * capteurs;


	if (fichier)
	{
		// On zappe le premier espace.
		getline(fichier, ligne, ' ');
		getline(fichier, ligne, ' ');
		nb_cibles = std::stoi(ligne);
		getline(fichier, ligne, ' ');

		nb_capteurs = std::stoi(ligne);

		cout_capteur = new size_t[nb_capteurs];
		cibles = new vector<size_t>[nb_cibles];
		capteurs = new vector<size_t>[nb_capteurs];

		limite = nb_capteurs / 12;
		// On parcours les coûts de chaque capteurs.
		for (i = 0; i < limite; i++)
		{
			//on zappe le premier espace.
			getline(fichier, ligne, ' ');
			for (j = 0; j < 12; j++)
			{
				getline(fichier, ligne, ' ');
				cout_capteur[indice] = stoi(ligne);
				z += " " + to_string(cout_capteur[indice]) + " x" + to_string(indice + 1);
				binaries += " x" + to_string(indice + 1) + "\n";
				if (j < 12)
				{
					z += " +";
				}

				indice++;
			}
		}
		limite = nb_capteurs % 12;
		//if(limite != 0){ On travaille sur de grandes valeurs, Ce cas n'existe pas (normalement).
		getline(fichier, ligne, ' ');
		for (i = 0; i < limite; i++)
		{
			getline(fichier, ligne, ' ');
			cout_capteur[indice] = stoi(ligne);
			z += " " + to_string(cout_capteur[indice]) + " x" + to_string(indice + 1);
			binaries += " x" + to_string(indice + 1) + "\n";
			if (i < limite - 1)
			{
				z += " +";
			}
			indice++;
		}
		//cout << z << endl;
		binaries += "End";
		//cout << binaries << endl;

		//}
		indice = 0;

		// On parcours toutes les cibles: On voit la liste des capteurs qui les couvrent.
		for (i = 0; i < nb_cibles; i++)
		{
			// On zappe l'espace initial.
			getline(fichier, ligne, ' ');

			// On récupere le nombre de capteurs couvrant la cible j.
			getline(fichier, ligne, ' ');
			nb_capteurs_cibles = std::stoi(ligne);

			limite = nb_capteurs_cibles / 12;
			subject_to += " cible_" + to_string(indice + 1) + ":";
			for (j = 0; j < limite; j++)
			{
				
				//on zappe le premier espace.
				getline(fichier, ligne, ' ');
				for (k = 0; k < 12; k++)
				{
					getline(fichier, ligne, ' ');
					subject_to += " 1 x" + ligne;
					numero_capteur = (size_t)stoi(ligne) - 1;
					cibles[indice].push_back(numero_capteur);
					capteurs[numero_capteur].push_back(indice);

					if ( 12 * j + k < nb_capteurs_cibles - 1)
					{
						subject_to+= " +";
					}
				}
			}
			limite = nb_capteurs_cibles % 12;
			if (limite != 0)
			{
				getline(fichier, ligne, ' ');
				for (j = 0; j < limite; j++)
				{
					getline(fichier, ligne, ' ');
					subject_to += " 1 x" + ligne;
					numero_capteur = (size_t)stoi(ligne) - 1;
					cibles[indice].push_back(numero_capteur);
					capteurs[numero_capteur].push_back(indice);

					if (j < limite - 1)
					{
						subject_to += " +";
					}
				}
			}
			indice++;
			subject_to += " >= 1\n";
		}
	}
	//cout << subject_to;
	resultat += z + subject_to + binaries;
	cout << resultat;
	

	std::ofstream out(string(argv[1]) + ".lp");
	out << resultat;
	out.close();
}