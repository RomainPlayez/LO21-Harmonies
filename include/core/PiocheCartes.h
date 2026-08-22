#pragma once
#include "core/Carte.h"
#include "core/CarteAnimal.h"

/**
 * @class PiocheAnimal
 * @brief Gère la PiocheAnimal des cartes avec un tableau dynamique de pointeurs.
 */
class PiocheAnimal {
private:
    CarteAnimal** _PiocheAnimalCartes;
    int _nbCartesRestantes;
    const int _capaciteMax = 32;

public:
    /**
     * @brief Constructeur par défaut.
     */
    PiocheAnimal();

    /**
     * @brief Destructeur pour libérer le tableau dynamique et les cartes restants
     */
    ~PiocheAnimal();

    /**
     * @brief Constructeur de recopie
     */
    PiocheAnimal(const PiocheAnimal& autre);

    /**
     * @brief Opérateur d'affectation
     */
    PiocheAnimal& operator=(const PiocheAnimal& autre);


    int getNbCartesRestantes() const;
    
    bool estVide() const;

    /**
     * @brief Ajoute une carte sur le dessus de la PiocheAnimal (utile pour l'initialisation)
     */
    void ajouterCarte(CarteAnimal* carte);

    /**
     * @brief Retire et renvoie la carte sur le dessus de la PiocheAnimal.
     * @return Un pointeur vers la carte piochée, ou nullptr si vide.
     */
    CarteAnimal* Piocher();

    /**
     * @brief Mélange les cartes du jeu
     */
    void melanger();

    CarteAnimal* prendreParNom(const std::string& nom);
};