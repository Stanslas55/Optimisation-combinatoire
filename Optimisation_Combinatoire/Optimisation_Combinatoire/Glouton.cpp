#pragma once
#include "Glouton.h"


Glouton::Glouton() :
    nb_capteurs(0), nb_cibles(0), cout_capteur(nullptr), cibles(nullptr), capteurs(nullptr)
{
}

void Glouton::parseFichier(string filename)
{
    string ligne;
    ifstream fichier(filename);
    size_t limite, i, j, k, indice = 0, numero_capteur = 0, N_capteurs = 0;

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
            indice++;
        }

        //}
        indice = 0;

        // On parcours toutes les cibles: On voit la liste des capteurs qui les couvrent.
        for (i = 0; i < nb_cibles; i++)
        {
            // On zappe l'espace initial.
            getline(fichier, ligne, ' ');

            // On récupere le nombre de capteurs couvrant la cible j.
            getline(fichier, ligne, ' ');
            N_capteurs = std::stoi(ligne);

            limite = N_capteurs / 12;

            for (j = 0; j < limite; j++)
            {
                //on zappe le premier espace.
                getline(fichier, ligne, ' ');
                for (k = 0; k < 12; k++)
                {
                    getline(fichier, ligne, ' ');
                    numero_capteur = (size_t)stoi(ligne) - 1;
                    cibles[indice].push_back(numero_capteur);
                    capteurs[numero_capteur].push_back(indice);
                }
            }
            limite = N_capteurs % 12;
            if (limite != 0)
            {
                getline(fichier, ligne, ' ');
                for (j = 0; j < limite; j++)
                {
                    getline(fichier, ligne, ' ');
                    numero_capteur = (size_t)stoi(ligne) - 1;
                    cibles[indice].push_back(numero_capteur);
                    capteurs[numero_capteur].push_back(indice);
                }
            }
            indice++;
        }
    }    
}

bool* Glouton::glouton()
{
    
    // Tableaux.
    bool* etats_cibles = new bool[nb_cibles];
    bool* etat_capteurs = new bool[nb_capteurs]; // Au départ, tous les capteurs sont désactivés.
    float* scores = new float[nb_capteurs];

    // Compteurs.
    //size_t solution = 0;
    size_t nb_contraintes_ok = 0;
    float score_capteur = 0;

    // Indices de boucles.
    size_t indice = 0;
    size_t indice_cible = 0;
    size_t indice_capteur = 0;
    size_t capteur_dans_cible = 0;
    size_t cible_active_dans_capteur = 0;
    size_t indice_capteur_score_max = 0;

    // Conditions d'arrêts.
    size_t nb_cibles_capteur_score_max = 0;
    size_t limite_nb_capteurs = 0;
    size_t limite_nb_cibles = 0;

    // On initialise l'état de toutes les cibles à false.
    for (indice_cible = 0; indice_cible < nb_cibles; indice_cible++) {
        etats_cibles[indice_cible] = false;
    }
    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        etat_capteurs[indice_capteur] = false;
    }

    // On initialise le score de chaque capteur.
    for (indice = 0; indice < nb_capteurs; indice++)
    {
        // Le score est défini par nombre de cibles non couvertes qui seront couvertes par le capteur,
        // nombre ensuite divisé par le coût de déploiement du capteur.
        scores[indice] = (float)capteurs[indice].size() / (float)cout_capteur[indice];
    }

    // Tant qu'il reste des cibles à couvrir...
    while (nb_contraintes_ok < nb_cibles) {
        // On recherche le capteur ayant le score le plus grand.
        indice_capteur_score_max = 0;
        // Pour chaque capteur...
        for (indice = 1; indice < nb_capteurs; indice++)
        {
            // Si le score du capteur itéré est supérieur au plus grand score précédemment trouvé...
            if (scores[indice] > scores[indice_capteur_score_max]) {
                // On stocke l'indice de ce capteur.
                indice_capteur_score_max = indice;
            }
        }
        // On récupère le nombre de cibles couvertes par le capteur ayant le score maximal.
        nb_cibles_capteur_score_max = capteurs[indice_capteur_score_max].size();
        // Pour chaque cible couverte par le capteur choisi...
        for (indice_cible = 0; indice_cible < nb_cibles_capteur_score_max; indice_cible++) {
            // Si la cible n'est pas couverte...
            if (etats_cibles[capteurs[indice_capteur_score_max][indice_cible]] == false) {
                // On marque la cible comme couverte et on comptabilise une nouvelle contrainte satisfaite.
                etats_cibles[capteurs[indice_capteur_score_max][indice_cible]] = true;
                nb_contraintes_ok++;
                // On récupère le nombre de capteurs couvrant la cible itérée.
                limite_nb_capteurs = cibles[capteurs[indice_capteur_score_max][indice_cible]].size();
                // Pour chaque capteur couvrant cette cible...
                for (capteur_dans_cible = 0; capteur_dans_cible < limite_nb_capteurs; capteur_dans_cible++)
                {
                    // On initialise le score de ce capteur.
                    score_capteur = 0;
                    // On récupère le nombre de cibles couvertes par ce capteur.
                    limite_nb_cibles = capteurs[cibles[capteurs[indice_capteur_score_max][indice_cible]][capteur_dans_cible]].size();
                    // Pour chaque cible couverte par ce capteur...
                    for (cible_active_dans_capteur = 0; cible_active_dans_capteur < limite_nb_cibles; cible_active_dans_capteur++)
                    {
                        //  Si la cible n'est pas couverte, on incrémente le score du capteur.
                        if (!etats_cibles[capteurs[cibles[capteurs[indice_capteur_score_max][indice_cible]][capteur_dans_cible]][cible_active_dans_capteur]]) {
                            score_capteur++;
                        }
                    }
                    // On stocke le score du capteur, en le divisant par son coût.
                    scores[cibles[capteurs[indice_capteur_score_max][indice_cible]][capteur_dans_cible]] = (float)score_capteur / (float)cout_capteur[cibles[capteurs[indice_capteur_score_max][indice_cible]][capteur_dans_cible]];
                }
            }
        }
        // On augmente le score avec le coût du capteur choisi et on l'active
        //solution += cout_capteur[indice_capteur_score_max];
        etat_capteurs[indice_capteur_score_max] = true;
    }

    // On libère la mémoire et on retourne le score final.
    delete[] etats_cibles;
    delete[] scores;
    return etat_capteurs;
}

bool* Glouton::gloutonAmeliore()
{  
    
    // Booléens.   
    bool isCapteurRedondant = true;

    // Listes et tableaux.
    bool* etats_cibles = new bool[nb_cibles]; // Au départ, tous les capteurs sont désactivés.
    bool* etat_capteurs = new bool[nb_capteurs]; // Au départ, tous les capteurs sont désactivés.
    float* scores = new float[nb_capteurs];
    size_t* nbCapteurParCible = new size_t[nb_cibles]; // Nombre de capteurs par cibles.
    vector <size_t> capteurs_actifs; // Liste des capteurs activés. 
    vector <size_t> capteursRedondantsIndices; // Contient l'indice es capteurs redondant dans le tableau des capteurs actifs.    

    // Compteurs.
    size_t solution = 0;
    size_t nbContraintesOk = 0;
    size_t nbCapteurRedondant = 0;
    size_t nbCapteursActifs = 0;
    size_t nbCapteurReccurent = 0;
    float score_capteur = 0;    

    // Indices de boucles.
    size_t indice_capteur = 0;
    size_t indice_cible = 0;
    size_t capteur_dans_cible = 0;
    size_t cible_active_dans_capteur = 0;
    size_t indiceCapteurChoisi = 0;
    size_t indiceCapteurUseless = 0; 

    // Conditions d'arrêts.
    size_t nbCibles = 0;
    size_t limite_nb_capteurs = 0;
    size_t limite_nb_cibles = 0;

    // Nécessité absolue d'initialiser l'état des cibles.
    for (indice_cible = 0; indice_cible < nb_cibles; indice_cible++)
    {
        etats_cibles[indice_cible] = false;
    }
    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        etat_capteurs[indice_capteur] = false;
    }

    // On initialise le score de chaque capteur, qui pour chacun d’eux, vaut le nombre de cibles activées.
    // Score de chaque capteur = nb de nouvelles cibles activées par chaque capteurs.    
    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        scores[indice_capteur] = (float)capteurs[indice_capteur].size() / (float)cout_capteur[indice_capteur];
    }

    while (nbContraintesOk < nb_cibles) {
        // On choisit le capteur ayant le score le plus grand.
        indiceCapteurChoisi = 0;
        for (indice_capteur = 1; indice_capteur < nb_capteurs; indice_capteur++)
        {
            //cout << "capteur " << i << ": " << scores[i] << " vs " << "capteur " << indiceCapteurChoisi << ": " << scores[indiceCapteurChoisi] << endl;
            if (scores[indice_capteur] > scores[indiceCapteurChoisi]) {
                indiceCapteurChoisi = indice_capteur;
            }
        }
        // on allume les cibles et on modifie le score
        nbCibles = capteurs[indiceCapteurChoisi].size();
        for (indice_cible = 0; indice_cible < nbCibles; indice_cible++) {
            // Si la cible n'est pas allumée.
            if (!etats_cibles[capteurs[indiceCapteurChoisi][indice_cible]]) {

                etats_cibles[capteurs[indiceCapteurChoisi][indice_cible]] = true;
                nbContraintesOk++;

                // Quels sont les capteurs pointant sur cette cibles ?
                limite_nb_capteurs = cibles[capteurs[indiceCapteurChoisi][indice_cible]].size();
                for (capteur_dans_cible = 0; capteur_dans_cible < limite_nb_capteurs; capteur_dans_cible++)
                {                   
                    score_capteur = 0;
                    // On calcule le score de ce capteur.
                    
                    limite_nb_cibles = capteurs[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]].size();
                    for (cible_active_dans_capteur = 0; cible_active_dans_capteur < limite_nb_cibles; cible_active_dans_capteur++)
                    {
                        // Augmenter le score pour les capteurs non allumés.
                        if (!etats_cibles[capteurs[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]][cible_active_dans_capteur]]) {
                            score_capteur++;
                        }
                    }
                    if (score_capteur != 0) {
                        scores[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]] = score_capteur / cout_capteur[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]];
                    }
                    else {
                        scores[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]] = score_capteur;
                    }
                }
            }
        }
        capteurs_actifs.push_back(indiceCapteurChoisi);
        etat_capteurs[indiceCapteurChoisi] = true;
        solution += cout_capteur[indiceCapteurChoisi];
    }
   
    for (indice_capteur = 0; indice_capteur < nb_cibles; indice_capteur++)
    {
        nbCapteurParCible[indice_capteur] = 0;
        limite_nb_cibles = cibles[indice_capteur].size();
        for (indice_cible = 0; indice_cible < limite_nb_cibles; indice_cible++)
        {
            if (find(capteurs_actifs.begin(), capteurs_actifs.end(), cibles[indice_capteur][indice_cible]) != capteurs_actifs.end()) {
                nbCapteurParCible[indice_capteur]++;
            }
        }
        
    }
    
    nbCapteursActifs = capteurs_actifs.size();
    for (indice_capteur = 0; indice_capteur < nbCapteursActifs; indice_capteur++)
    {
        isCapteurRedondant = true;
        limite_nb_cibles = capteurs[capteurs_actifs[indice_capteur]].size();
        for (indice_cible = 0; indice_cible < limite_nb_cibles; indice_cible++)
        {
            if (nbCapteurParCible[capteurs[capteurs_actifs[indice_capteur]][indice_cible]] < 2) {
                isCapteurRedondant = false;
                break;
            }
        }
        if (isCapteurRedondant) {
            nbCapteurRedondant++;
            capteursRedondantsIndices.push_back(indice_capteur);
            
        }
    }
    
    while (nbCapteurRedondant > 0)
    {
        indiceCapteurUseless = 0;
        nbCapteurReccurent = capteursRedondantsIndices.size();
        for (indice_capteur = 1; indice_capteur < nbCapteurReccurent; indice_capteur++)
        {
            if (cout_capteur[capteurs_actifs[capteursRedondantsIndices[indice_capteur]]] > cout_capteur[capteurs_actifs[capteursRedondantsIndices[indiceCapteurUseless]]]) {
                indiceCapteurUseless = indice_capteur;
            }
        }
        solution -= cout_capteur[capteurs_actifs[capteursRedondantsIndices[indiceCapteurUseless]]];
        etat_capteurs[capteurs_actifs[capteursRedondantsIndices[indiceCapteurUseless]]] = false;
        capteurs_actifs.erase(capteurs_actifs.begin() + capteursRedondantsIndices[indiceCapteurUseless]);
        
        
        for (indice_cible = 0; indice_cible < nb_cibles; indice_cible++)
        {
            nbCapteurParCible[indice_cible] = 0;
            limite_nb_capteurs = cibles[indice_cible].size();
            for (indice_capteur = 0; indice_capteur < limite_nb_capteurs; indice_capteur++)
            {
                if (find(capteurs_actifs.begin(), capteurs_actifs.end(), cibles[indice_cible][indice_capteur]) != capteurs_actifs.end()) {
                    nbCapteurParCible[indice_cible]++;
                }
            }
            
        }
        
        capteursRedondantsIndices.erase(capteursRedondantsIndices.begin(), capteursRedondantsIndices.end());
        nbCapteurRedondant = 0;
        limite_nb_capteurs = capteurs_actifs.size();
        for (indice_capteur = 0; indice_capteur < limite_nb_capteurs; indice_capteur++)
        {
            isCapteurRedondant = true;
            limite_nb_cibles = capteurs[capteurs_actifs[indice_capteur]].size();
            for (indice_cible = 0; indice_cible < limite_nb_cibles; indice_cible++)
            {
                if (nbCapteurParCible[capteurs[capteurs_actifs[indice_capteur]][indice_cible]] < 2) {
                    isCapteurRedondant = false;
                }
            }
            if (isCapteurRedondant) {
                nbCapteurRedondant++;
                capteursRedondantsIndices.push_back(indice_capteur);
            }
        }
    } 
    delete[] etats_cibles;
    delete[] scores;
    delete[] nbCapteurParCible;
    
    capteurs_actifs.clear();
    capteursRedondantsIndices.clear();  
    
    return etat_capteurs;
}

// POUR LE GENETIQUE.
bool* Glouton::gloutonRand()
{
    // Listes et tableaux.
    bool* etats_capteurs = new bool[nb_capteurs];
    bool* etats_cibles = new bool[nb_cibles]; // Au départ, tous les capteurs sont désactivés.

    size_t* nbCapteurParCible = new size_t[nb_cibles]; // Nombre de capteurs par cibles

    // Compteurs.
    size_t nbContraintesOk = 0;

    // Indices de boucles.
    size_t indice_capteur = 0;
    size_t indice_cible = 0;
    size_t indiceCapteurChoisi = 0;

    // Conditions d'arrêts.
    size_t nbCibles = 0;

    // Nécessité absolue d'initialiser l'état des cibles.
    for (indice_cible = 0; indice_cible < nb_cibles; indice_cible++) {
        etats_cibles[indice_cible] = false;
    }
    for (indice_cible = 0; indice_cible < nb_capteurs; indice_cible++) {
        etats_capteurs[indice_cible] = false;
    }  
    while (nbContraintesOk < nb_cibles) {
        // On choisit le capteur ayant le score le plus grand.

        indiceCapteurChoisi = rand() % nb_capteurs;
        if (!etats_capteurs[indiceCapteurChoisi]) {

            // on allume les cibles et on modifie le score
            nbCibles = capteurs[indiceCapteurChoisi].size();
            for (indice_cible = 0; indice_cible < nbCibles; indice_cible++) {
                // Si la cible n'est pas allumée.
                if (!etats_cibles[capteurs[indiceCapteurChoisi][indice_cible]]) {

                    etats_cibles[capteurs[indiceCapteurChoisi][indice_cible]] = true;
                    nbContraintesOk++;
                }
            }
            etats_capteurs[indiceCapteurChoisi] = true;
        }
    }   
    delete[] etats_cibles;  
    return etats_capteurs;
}
// POUR LE GENETIQUE.
bool* Glouton::gloutonRand2()
{
    //cout << "Début glotona()" << endl;
   // Booléens.   
    bool isCapteurRedondant = true;

    // Listes et tableaux.
    bool* etats_cibles = new bool[nb_cibles]; // Au départ, tous les capteurs sont désactivés.
    bool* etat_capteurs = new bool[nb_capteurs]; // Au départ, tous les capteurs sont désactivés.
    //float* scores = new float[nb_capteurs];
    //float* scores_choix = new float[nb_capteurs];
    size_t* nbCapteurParCible = new size_t[nb_cibles]; // Nombre de capteurs par cibles.
    vector <size_t> capteurs_actifs; // Liste des capteurs activés. 
    vector <float> scores; // Liste des capteurs activés. 
    vector <float> scores_choix; // Liste des capteurs activés. 
    vector <size_t> capteursRedondantsIndices; // Contient l'indice es capteurs redondant dans le tableau des capteurs actifs.    

    // Compteurs.
    size_t solution = 0;
    size_t nbContraintesOk = 0;
    size_t nbCapteurRedondant = 0;
    size_t nbCapteursActifs = 0;
    size_t nbCapteurReccurent = 0;
    float score_capteur = 0;

    // Indices de boucles.
    size_t indice_capteur = 0;
    size_t indice = 0;
    size_t indice_cible = 0;
    size_t capteur_dans_cible = 0;
    size_t cible_active_dans_capteur = 0;
    size_t indiceCapteurChoisi = 0;
    size_t indiceCapteurUseless = 0;
    size_t elimination = 0;

    // Conditions d'arrêts.
    size_t nbCibles = 0;
    size_t limite_nb_capteurs = 0;
    size_t limite_nb_cibles = 0;

    // Nécessité absolue d'initialiser l'état des cibles.
    for (indice_cible = 0; indice_cible < nb_cibles; indice_cible++)
    {
        etats_cibles[indice_cible] = false;
    }
    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        etat_capteurs[indice_capteur] = false;
    }

    // On initialise le score de chaque capteur, qui pour chacun d’eux, vaut le nombre de cibles activées.
    // Score de chaque capteur = nb de nouvelles cibles activées par chaque capteurs.    
    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        scores.push_back((float)capteurs[indice_capteur].size() / (float)cout_capteur[indice_capteur]);
    }

    while (nbContraintesOk < nb_cibles) {
        // On choisit un capteur parmi les 10 premiers (classement sur le score) 
        elimination = rand() % 10;
        scores_choix = scores;
        //on elimine les n premier capteurs (0<= n <= 9)
        for ( indice = 0; indice < elimination; indice++)
        {
            indiceCapteurChoisi = 0;
            for (indice_capteur = 1; indice_capteur < nb_capteurs - indice; indice_capteur++)
            {
                if (scores_choix[indice_capteur] > scores_choix[indiceCapteurChoisi]) {
                    indiceCapteurChoisi = indice_capteur;
                }
            }
            scores_choix.erase(scores_choix.begin() + indiceCapteurChoisi);
        }
        //on choisi le capteur ayant le score le plus élevé parmi les capteurs restant
        indiceCapteurChoisi = 0;
        for (indice_capteur = 1; indice_capteur < scores_choix.size(); indice_capteur++)
        {
            if (scores_choix[indice_capteur] > scores_choix[indiceCapteurChoisi]) {
                indiceCapteurChoisi = indice_capteur;
            }
        }
        // on récupère ce capteur dans la liste complète
        for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
        {
            if (scores[indice_capteur] == scores_choix[indiceCapteurChoisi]) {
                indiceCapteurChoisi = indice_capteur;
                break;
            }
        }
      
        // on allume les cibles et on modifie le score
        nbCibles = capteurs[indiceCapteurChoisi].size();
        for (indice_cible = 0; indice_cible < nbCibles; indice_cible++) {
            // Si la cible n'est pas allumée.
            if (!etats_cibles[capteurs[indiceCapteurChoisi][indice_cible]]) {

                etats_cibles[capteurs[indiceCapteurChoisi][indice_cible]] = true;
                nbContraintesOk++;

                // Quels sont les capteurs pointant sur cette cibles ?
                limite_nb_capteurs = cibles[capteurs[indiceCapteurChoisi][indice_cible]].size();
                for (capteur_dans_cible = 0; capteur_dans_cible < limite_nb_capteurs; capteur_dans_cible++)
                {
                    score_capteur = 0;
                    // On calcule le score de ce capteur.

                    limite_nb_cibles = capteurs[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]].size();
                    for (cible_active_dans_capteur = 0; cible_active_dans_capteur < limite_nb_cibles; cible_active_dans_capteur++)
                    {
                        // Augmenter le score pour les capteurs non allumés.
                        if (!etats_cibles[capteurs[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]][cible_active_dans_capteur]]) {
                            score_capteur++;
                        }
                    }
                    if (score_capteur != 0) {
                        scores[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]] = score_capteur / cout_capteur[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]];
                    }
                    else {
                        scores[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]] = score_capteur;
                    }
                }
            }
        }
        capteurs_actifs.push_back(indiceCapteurChoisi);
        etat_capteurs[indiceCapteurChoisi] = true;
        solution += cout_capteur[indiceCapteurChoisi];
    }

    
    delete[] etats_cibles;
    
    delete[] nbCapteurParCible;

    capteurs_actifs.clear();
    capteursRedondantsIndices.clear();
    
    return etat_capteurs;
}

Glouton::~Glouton()
{
    delete[] cout_capteur;
    delete[] cibles;
    delete[] capteurs;
}

//void arbitre(fsec limit, chrono::steady_clock::time_point compare)
void arbitre(string chaine)
{ 
    long long limit = (long long)std::stoi(chaine);
    chrono::steady_clock::time_point tempsAvant = chrono::steady_clock::now();
    chrono::steady_clock::time_point tempsApres;
    long long tempsExecution = 0;
    cout << "Debut du timer :)";
   
    do {
        tempsApres = chrono::steady_clock::now();
       
        tempsExecution = chrono::duration_cast<chrono::seconds>(tempsApres - tempsAvant).count();
    } while (tempsExecution < limit);
   //} while (d < (ms)1000); // A enlever
    cout << "Aurevoir :)";
    exit(0);           
}


