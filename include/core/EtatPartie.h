#pragma once
#include <fstream>
#include <string>
#include "core/Jeton.h"
#include "core/PlateauJoueur.h"

static const char* SAVE_FILE = "harmonies.save";

/**
 * @brief État d'une case du plateau joueur.
 * Stocke la position et les jetons empilés.
 */
struct EtatCase {
    int x, y;
    int nbJetons;
    CouleurJeton jetons[3];
    bool aUnCube;
    bool estCubeEsprit;
};

/**
 * @brief État complet d'un plateau joueur.
 */
struct EtatPlateauJoueur {
    EtatCase cases[25];
    int nbCases;
};

/**
 * @brief État d'une carte (nom + cubes restants).
 */
struct EtatCarte {
    char nom[64];
    int nbCubesActuels;
};

/**
 * @brief État d'un groupe de jetons du plateau central.
 */
struct EtatGroupe {
    CouleurJeton couleurs[3];
    int nbJetons;
};

/**
 * @brief État complet d'un joueur.
 */
struct EtatJoueur {
    char nom[64];
    int score;
    int nbCartesActives;
    EtatCarte cartesActives[4];
    int nbCartesCompletees; 
    EtatCarte cartesCompletees[20]; 
    EtatPlateauJoueur plateau;
};

/**
 * @class EtatPartie
 * @brief Memento complet de la partie, il peut être écrit et lu depuis un fichier texte.
 */
struct EtatPartie {
    int nbJoueurs;
    int face;               
    int tourActuel;
    int joueurActifIndex;
    int etatTour;
    int nbJetonsSac;
    CouleurJeton jetonsSac[120];
    EtatGroupe groupes[5];
    EtatCarte cartesRiviere[5];
    EtatJoueur joueurs[4];

    /**
     * @brief Écrit l'état dans un fichier texte.
     */
    void ecrire(std::ofstream& f) const;

    /**
     * @brief Lit l'état depuis un fichier texte.
     */
    static EtatPartie lire(std::ifstream& f);

    /**
     * @brief Vérifie si un fichier de sauvegarde existe.
     */
    static bool fichierExiste();
};