#pragma once
#include "core/Jeton.h"
#include <stdexcept>

/**
 * @struct ElementMotif
 * @brief Représente un jeton précis requis dans le motif, avec sa position relative.
 */
struct ElementMotif {
    int dx;                // Décalage en X (colonne) par rapport au cube animal
    int dy;                // Décalage en Y (ligne) par rapport au cube animal
    CouleurJeton couleur;  // La couleur requise
    int hauteurMinimale;   // La hauteur requise (0 ou 1 pour sol, 2 ou 3 pour montagne/arbre)
};

/**
 * @class Motif
 * @brief Stocke la forme géométrique d'une carte Animal.
 */
class Motif {
private:
    ElementMotif* _elements; // Tableau dynamique contenant les jetons du motif
    int _taille;             // Le nombre de jetons qui composent le motif

public:
    /**
     * @brief Constructeur du motif.
     * @param taille Le nombre de jetons nécessaires pour valider le motif.
     */
    explicit Motif(int taille);

    /**
     * @brief Destructeur
     */
    ~Motif();

    /**
     * @brief Constructeur de recopie
     */
    Motif(const Motif& autre);

    /**
     * @brief Opérateur d'affectation
     */
    Motif& operator=(const Motif& autre);

    /**
     * @brief Définit les caractéristiques d'un élément du motif.
     * @param index La position dans le tableau (0 à _taille - 1).
     * @param dx Décalage relatif X.
     * @param dy Décalage relatif Y.
     * @param couleur La couleur requise.
     * @param hauteurMinimale La hauteur minimale (1 par défaut).
     */
    void setElement(int index, int dx, int dy, CouleurJeton couleur, int hauteurMinimale = 1);

    // Getters
    int getTaille() const;
    const ElementMotif& getElement(int index) const;
};