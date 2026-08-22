#pragma once
#include "core/Case.h"
#include <stdexcept>

/**
 * @enum FacePlateau
 * @brief Représente la face choisie pour le plateau du joueur (Face A ou Face B).
 */
enum class FacePlateau {
    FACE_A,
    FACE_B
};

/**
 * @class PlateauJoueur
 * @brief Représente le plateau personnel d'un joueur, contenant une grille hexagonale de Cases.
 * Gère le cycle de vie de ses cases et sa zone de stockage temporaire.
 */
class PlateauJoueur {
private:
    FacePlateau _face;
    int         _nbCases;
    Case** _cases;               // Tableau dynamique de pointeurs sur Case
    Jeton* _jetonsEnAttente[3];  // Zone de stockage temporaire pour le tour courant
    int         _nbJetonsEnAttente;

    /**
     * @brief Méthode utilitaire privée chargeant la géométrie des plateaux.
     */
    void initialiserGrille();

public:
    /**
     * @brief Constructeur du Plateau Joueur.
     * @param face La face (A ou B) déterminant la disposition des cases.
     */
    explicit PlateauJoueur(FacePlateau face);

    /**
     * @brief Destructeur (Garantit la désallocation profonde des cases et jetons en attente).
     */
    ~PlateauJoueur();

    /**
     * @brief Constructeur de recopie.
     */
    PlateauJoueur(const PlateauJoueur& autre);

    /**
     * @brief Opérateur d'affectation.
     */
    PlateauJoueur& operator=(const PlateauJoueur& autre);

    // Getters de base
    FacePlateau getFace() const;
    int getNbCases() const;

    /**
     * @brief Récupère une case spécifique par ses coordonnées cartésiennes.
     * @param x Index de la colonne.
     * @param y Index de la ligne.
     * @return Pointeur vers la Case, ou nullptr si elle est hors-jeu (trous dans la grille).
     */
    Case* getCase(int x, int y) const;

    // ----- GESTION DES JETONS EN ATTENTE -----
    bool stockerJetonEnAttente(Jeton* j);
    Jeton* utiliserJetonEnAttente(int index);
    int getNbJetonsEnAttente() const;

    /**
     * @brief Récupère les voisins d'une case sans aucune allocation dynamique.
     * @param x Coordonnée X de la case cible.
     * @param y Coordonnée Y de la case cible.
     * @param outVoisins Tableau statique de 6 pointeurs fourni par l'appelant qui sera complété.
     */
    void getVoisins(int x, int y, Case* outVoisins[6]) const;

    /**
     * @brief Retourne l'index interne d'une case (utile pour le suivi d'historique de détection).
     */
    int getIndexCase(int x, int y) const;

    /**
     * @brief Retourne la case située à un index précis du tableau interne.
     */
    Case* getCaseParIndex(int i) const;

    const Jeton* getJetonEnAttente(int index) const;
};