#pragma once
#include "core/Jeton.h"
#include "core/CubeAnimal.h"
#include <stdexcept>

/**
 * @class Case
 * @brief Représente un emplacement sur le plateau hexagonal.
 * Gère l'empilement des jetons (jusqu'à 3 max) et la présence d'un cube animal.
 */
class Case {
private:
    int _x;
    int _y;
    int _z;                 // Hauteur actuelle de la pile (0, 1, 2 ou 3)
    Jeton* _pile[3];        // Tableau de pointeurs vers les jetons empilés
    CubeAnimal* _cube;      // Pointeur vers le cube animal (nullptr si absent)

public:
    /**
     * @brief Constructeur d'une case vide.
     * @param x Coordonnée X (colonne).
     * @param y Coordonnée Y (ligne).
     */
    Case(int x, int y);

    /**
     * @brief Destructeur (Détruit les jetons empilés et le cube animal).
     */
    ~Case();

    /**
     * @brief Constructeur de recopie (Copie profonde pour sécuriser le PlateauJoueur).
     */
    Case(const Case& autre);

    /**
     * @brief Opérateur d'affectation (Copie profonde).
     */
    Case& operator=(const Case& autre);

    // Getters basiques
    int getX() const;
    int getY() const;
    int getZ() const; 

    /**
     * @brief Vérifie si la case est totalement vide (ni jeton, ni cube).
     */
    bool estVide() const;

    /**
     * @brief Vérifie si la case possède un cube animal.
     */
    bool aUnCubeAnimal() const;
    
    /**
     * @brief Récupère la couleur du jeton au sommet de la pile.
     * @return La couleur du jeton.
     * @throws std::logic_error si la pile de jetons est vide.
     */
    CouleurJeton getCouleurSommet() const;
    
    /**
     * @brief Tente de placer un jeton sur la case.
     * @param j Pointeur vers le jeton à placer.
     * @return true si le placement réussit, false sinon (pile pleine ou cube présent).
     */
    bool placerJeton(Jeton* j);

    /**
     * @brief Tente de placer un cube animal sur la case.
     * @param c Pointeur vers le cube animal à placer.
     * @return true si le placement réussit, false si un cube est déjà présent.
     */
    bool placerCube(CubeAnimal* c);

    /**
     * @brief Récupère un jeton précis dans la pile sans le retirer.
     * @param niveau La hauteur du jeton (0, 1 ou 2).
     * @return Pointeur constant vers le jeton, ou nullptr si inexistant.
     */
    const Jeton* getJeton(int niveau) const;

    void vider();

    const CubeAnimal* getCube() const { return _cube; }
};