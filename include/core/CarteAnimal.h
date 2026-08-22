#pragma once
#include "core/Carte.h"
#include "core/Motif.h"

/**
 * @class CarteAnimal
 * @brief Carte Animal concrète. 
 * Ajoute spécifiquement la gestion des points de victoire gagnés à chaque placement de cube.
 */
class CarteAnimal : public Carte {
private:
    // Tableau dynamique : points[i] = points gagnés quand le (i+1)ème cube est posé
    int* _pointsParEmplacement; 

public:
    /**
     * @brief Constructeur de la Carte Animal.
     * @param nom          Nom de l'animal.
     * @param imageRef     Chemin vers l'image de la carte.
     * @param nbCubes      Nombre de cubes disponibles sur la carte (définit _nbCubesMax dans la classe mère).
     * @param motif        Le motif géométrique requis pour valider le placement.
     * @param points       Tableau d'entiers contenant les points gagnés à chaque placement.
     */
    CarteAnimal(const std::string& nom, const std::string& imageRef,
                int nbCubes, const Motif& motif, const int* points);

    /**
     * @brief Destructeur. Libère uniquement les points, le reste est géré par ~Carte().
     */
    ~CarteAnimal() override; 

    /**
     * @brief Renvoie les points gagnés pour le n-ième cube posé (index 0 = premier cube).
     * @param index Position du cube (0 à _nbCubesMax - 1).
     * @return Le nombre de points de victoire correspondants.
     */
    int getPointsEmplacement(int index) const;

    // Blocage de la copie et de l'affectation pour éviter les problèmes de pointeurs
    CarteAnimal(const CarteAnimal&) = delete;
    CarteAnimal& operator=(const CarteAnimal&) = delete;
};