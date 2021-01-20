#pragma once
#include "Genetique.h"


Genetique::Genetique() :
    sortie(""), nb_capteurs(0), nb_cibles(0), g_cout_capteur(nullptr), cibles(nullptr), capteurs(nullptr), population(vector<GUERRIER>(taille_population)), puissance(nullptr), proba_mutation(0)
{
}

void Genetique::parseFichier(string filename, string chemin_sortie)
{
    string ligne;
    ifstream fichier(filename);
    size_t limite, i, j, k, indice = 0, numero_capteur = 0, N_capteurs = 0;

    sortie = chemin_sortie;
    if (fichier)
    {
        // On zappe le premier espace.
        getline(fichier, ligne, ' ');
        getline(fichier, ligne, ' ');
        nb_cibles = (size_t)stoi(ligne);
        getline(fichier, ligne, ' ');

        nb_capteurs = (size_t)stoi(ligne);

        g_cout_capteur = new size_t[nb_capteurs];
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
                g_cout_capteur[indice] = (size_t)stoi(ligne);
                indice++;
            }
        }
        limite = nb_capteurs % 12;
        //if(limite != 0){ On travaille sur de grandes valeurs, Ce cas n'existe pas (normalement).
        getline(fichier, ligne, ' ');
        for (i = 0; i < limite; i++)
        {
            getline(fichier, ligne, ' ');
            g_cout_capteur[indice] = (size_t)stoi(ligne);
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
            N_capteurs = (size_t)stoi(ligne);

            limite = N_capteurs / 12;

            for (j = 0; j < limite; j++)
            {
                //on zappe le premier espace.
                getline(fichier, ligne, ' ');
                for (k = 0; k < 12; k++)
                {
                    getline(fichier, ligne, ' ');
                    numero_capteur = ((size_t)(stoi(ligne)) - 1);
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
                    numero_capteur = ((size_t)(stoi(ligne)) - 1);
                    cibles[indice].push_back(numero_capteur);
                    capteurs[numero_capteur].push_back(indice);
                }
            }
            indice++;
        }
    }
    proba_mutation = 2;
    puissance = new scoreG[taille_population]; // Le tableau qui contient le score de chaque guerrier. Vide pour le moment.   
}

void Genetique::start(string filename)
{
    int generation_count = 0;

    Glouton armee = Glouton();
    armee.parseFichier(filename);

    initPopulation(&armee);
    // Calcul du score initial dans initPopulation
    // Le batman.
    get<0>(batman) = puissance[0];
    get<1>(batman) = population[0];

    trouverBatman();

    for (generation_count = 0; generation_count < NB_GENERATION; generation_count++)
    {

        nextPopulation();
        trouverBatman();
    }
    std::cout << "score : " << get<0>(batman) << endl; // Penser à afficher la liste des capteurs allumés. (numéro par exemple).
    exit(0);
}

void Genetique::initPopulation(Glouton *armee)
{   
    size_t indice = 0;
    vector <GUERRIER> pop(taille_population);     

    
    for (size_t i = 0; i < 5; i++)
    {
        pop[i] = armee->gloutonAmeliore();
    }
    for (indice = 5; indice < taille_population; indice++)
    {
        pop[indice] = armee->gloutonRand2();
    }
    population = pop;

    for (indice = 0; indice < taille_population; indice++)
    {
        puissance[indice] = calculScore(population[indice]);
    }
}

void Genetique::nextPopulation()
{
    size_t i = 0;
    int MIN = 0, MAX = 0;
    // indice des n combattants dans l'arene.
    int combattant1 = -1;
    int combattant2 = -1;
    int combattant3 = -1;
    int combattant4 = -1;

    tuple <GUERRIER, GUERRIER> elus;  

    vector<GUERRIER> newPopulation(taille_population);
    
    for (i = 0; i < taille_population / 2; i++)
    {
        combattant1 = rand() % (taille_population - 1);
        combattant2 = rand() % (taille_population - 1);
        combattant3 = rand() % (taille_population - 1);
        combattant4 = rand() % (taille_population - 1);

        while (combattant2 == combattant1)
        {
            combattant2 = rand() % (taille_population - 1);
        }
        while (combattant3 == combattant1 || combattant3 == combattant2)
        {
            combattant3 = rand() % (taille_population - 1);
        }
        while (combattant4 == combattant1 || combattant4 == combattant2 || combattant4 == combattant3)
        {
            combattant4 = rand() % (taille_population - 1);
        }
        // On a verifié qu'ils sont tous différents.
        //cout << puissance[combattant1] << " " << puissance[combattant2] << " " << puissance[combattant3] << " " << puissance[combattant4] << endl;


        GUERRIER elu1 = population[compareCombattants(combattant1, combattant2)];
        GUERRIER elu2 = population[compareCombattants(combattant3, combattant4)];

        elus = make_tuple(elu1, elu2);    
        //cout << calculScore(get<0>(elus)) << " " << calculScore(get<1>(elus)) << endl;
        
       
        elus = croisement(elus);
        
        
        elus = mutation(elus);
        
            
        
        //réparation
     
        //cout << calculScore(get<0>(elus)) << " " << calculScore(get<1>(elus)) << endl;
        get<0>(elus) = el_reparator(get<0>(elus));
        get<1>(elus) = el_reparator(get<1>(elus));
        
        

        newPopulation[i] = get<0>(elus);
        newPopulation[taille_population - 1 - i] = get<1>(elus);

        //cout << endl;
    }
   
    population = newPopulation; 
}

// Selection par tournois parce que Batman les bat tous.
tuple<GUERRIER, GUERRIER> Genetique::croisement(tuple<GUERRIER, GUERRIER> elus) 
{
    int i = 0;
    
    for (i = 0; i < nb_capteurs; i++)
    {
        int random = rand() % 2;
        if (random == 1) // A voir si les performances sont OK.
        {
            //cout << "switch:" << get<0>(elus)[i] << get<1>(elus)[i] << endl;
            bool temp = get<0>(elus)[i];
            get<0>(elus)[i] = get<1>(elus)[i];
            get<1>(elus)[i] = temp;

           // cout << "result:" << get<0>(elus)[i] << get<1>(elus)[i] << endl;


        }
        
    }    
    return elus;
}

tuple<GUERRIER, GUERRIER> Genetique::mutation(tuple<GUERRIER, GUERRIER> elus)
{
    int random1 = 0;
    int random2 = 0;
    int i = 0;
    for (i = 0; i < nb_capteurs; i++)
    {
        
        random1 = rand() % 1000;
        random2 = rand() % 1000;
        
        if (random1 < proba_mutation)
        {
            get<0>(elus)[i] = !get<0>(elus)[i];
            
        }
        if (random2 < proba_mutation)
        {
            get<1>(elus)[i] = !get<1>(elus)[i];
            
        }
    }

    return elus;
}

GUERRIER Genetique::el_reparator(GUERRIER G)
{
    //cout << "Début glotona()" << endl;
    // Booléens.   
    bool isCapteurRedondant = true;

    // Listes et tableaux.
    bool* etats_cibles = new bool[nb_cibles]; // Au départ, tous les capteurs sont désactivés.
    
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

    // On initialise le score de chaque capteur, qui pour chacun d’eux, vaut le nombre de cibles activées.
    // Score de chaque capteur = nb de nouvelles cibles activées par chaque capteurs.    
    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        scores[indice_capteur] = (float)capteurs[indice_capteur].size() / (float)g_cout_capteur[indice_capteur];
    }

    for (indice_capteur = 0; indice_capteur < nb_capteurs; indice_capteur++)
    {
        if (G[indice_capteur]) {
            nbCibles = capteurs[indice_capteur].size();
            for (indice_cible = 0; indice_cible < nbCibles; indice_cible++) {
                // Si la cible n'est pas allumée.
                if (!etats_cibles[capteurs[indice_capteur][indice_cible]]) {

                    etats_cibles[capteurs[indice_capteur][indice_cible]] = true;
                    nbContraintesOk++;

                    // Quels sont les capteurs pointant sur cette cibles ?
                    limite_nb_capteurs = cibles[capteurs[indice_capteur][indice_cible]].size();
                    for (capteur_dans_cible = 0; capteur_dans_cible < limite_nb_capteurs; capteur_dans_cible++)
                    {
                        score_capteur = 0;
                        // On calcule le score de ce capteur.

                        limite_nb_cibles = capteurs[cibles[capteurs[indice_capteur][indice_cible]][capteur_dans_cible]].size();
                        for (cible_active_dans_capteur = 0; cible_active_dans_capteur < limite_nb_cibles; cible_active_dans_capteur++)
                        {
                            // Augmenter le score pour les capteurs non allumés.
                            if (!etats_cibles[capteurs[cibles[capteurs[indice_capteur][indice_cible]][capteur_dans_cible]][cible_active_dans_capteur]]) {
                                score_capteur++;
                            }
                        }
                        if (score_capteur != 0) {
                            scores[cibles[capteurs[indice_capteur][indice_cible]][capteur_dans_cible]] = score_capteur / g_cout_capteur[cibles[capteurs[indice_capteur][indice_cible]][capteur_dans_cible]];
                        }
                        else {
                            scores[cibles[capteurs[indice_capteur][indice_cible]][capteur_dans_cible]] = score_capteur;
                        }
                    }
                }
            }

         capteurs_actifs.push_back(indice_capteur);
            
        }
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
                        scores[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]] = score_capteur / g_cout_capteur[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]];
                    }
                    else {
                        scores[cibles[capteurs[indiceCapteurChoisi][indice_cible]][capteur_dans_cible]] = score_capteur;
                    }
                }
            }
        }
        capteurs_actifs.push_back(indiceCapteurChoisi);
        G[indiceCapteurChoisi] = true;
        solution += g_cout_capteur[indiceCapteurChoisi];
    }
    //on determine le nombre de capteurs par cibles
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
    //on determine les capteurs dedondants
    nbCapteursActifs = capteurs_actifs.size();
    for (indice_capteur = 0; indice_capteur < nbCapteursActifs; indice_capteur++)
    {
        isCapteurRedondant = true;
        limite_nb_cibles = capteurs[capteurs_actifs[indice_capteur]].size();
        for (indice_cible = 0; indice_cible < limite_nb_cibles; indice_cible++)
        {
            //si le capteur est le seul a couvrir une cible on met le bool a false et on break
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
        //choix du pire capteur
        indiceCapteurUseless = 0;
        nbCapteurReccurent = capteursRedondantsIndices.size();
        for (indice_capteur = 1; indice_capteur < nbCapteurReccurent; indice_capteur++)
        {
            if (g_cout_capteur[capteurs_actifs[capteursRedondantsIndices[indice_capteur]]] > g_cout_capteur[capteurs_actifs[capteursRedondantsIndices[indiceCapteurUseless]]]) {
                indiceCapteurUseless = indice_capteur;
            }
        }
        solution -= g_cout_capteur[capteurs_actifs[capteursRedondantsIndices[indiceCapteurUseless]]];
        G[capteurs_actifs[capteursRedondantsIndices[indiceCapteurUseless]]] = false;
        capteurs_actifs.erase(capteurs_actifs.begin() + capteursRedondantsIndices[indiceCapteurUseless]);
        
        //calcul du nombre de capteurs par cible
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
        // calcul des capteurs redondants
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
    //cout << "Fin glotona()" << endl;
    return G;
    
    
}



size_t Genetique::calculScore(GUERRIER G)
{
    size_t indice = 0, score = 0;

    for (indice = 0; indice < nb_capteurs; indice++)
    {
        //cout << g_cout_capteur[indice]<< " ";
        if (G[indice]) {
            
            score += g_cout_capteur[indice];
        }

    }
    //cout << endl;
    return score;
}

size_t Genetique::compareCombattants(size_t combattant1, size_t combattant2)
{
    size_t MIN = 0, MAX = 0;    
    size_t alea = rand() % 1000;
    if (puissance[combattant1] > puissance[combattant2])
    {
        MAX = combattant1;
        MIN = combattant2;
    }
    else {
        MAX = combattant2;
        MIN = combattant1;
    }
   
     if(alea <= PRESSION_EVOL){
    
        return MIN;
    }
    else {
        return MAX;
    }
}

void Genetique::trouverBatman()
{
    bool test = true;
    size_t* nbCapteurParCible = new size_t[nb_cibles];
    size_t indice = 0, indice_capteur = 0, indice_cible = 0, indice_nb_capteur = 0;
    size_t limite_nb_cibles = 0;

    string resultat;
    size_t indice_resultat;

    

    for (indice = 0; indice < taille_population; indice++)
    {
      
      
        puissance[indice] = calculScore(population[indice]);
       
        if (puissance[indice] < get<0>(batman) && test)
        {
            std::ofstream out(sortie);
            if (!out.is_open())
            {
                cout << "PROBLEM" << endl;
            }
            get<0>(batman) = puissance[indice];
            get<1>(batman) = population[indice];

            for (indice_resultat = 0; indice_resultat < nb_capteurs; indice_resultat++)
            {
                resultat += (get<1>(batman)[indice_resultat]) ? "1":"0";
            }          
            out << resultat;
            out.close();
            cout << resultat;
            
        }
    }
    cout << " ";
}

Genetique::~Genetique()
{
    delete[] g_cout_capteur;
    delete[] cibles;
    delete[] capteurs;
}