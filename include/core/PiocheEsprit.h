#pragma once
#include "core/Carte.h"
#include "core/CarteEsprit.h"

/**
 * @class PiocheEsprit
 * @brief Gère la PiocheEsprit des cartes avec un tableau dynamique de pointeurs.
 */
class PiocheEsprit {
private:
    CarteEsprit** _PiocheEspritCartes;
    int _nbCartesRestantes;
    const int _capaciteMax = 10;

public:
    /**
     * @brief Constructeur par défaut.
     */
    PiocheEsprit();

    /**
     * @brief Destructeur pour libérer le tableau dynamique et les cartes restants
     */
    ~PiocheEsprit();

    /**
     * @brief Constructeur de recopie
     */
    PiocheEsprit(const PiocheEsprit& autre);

    /**
     * @brief Opérateur d'affectation
     */
    PiocheEsprit& operator=(const PiocheEsprit& autre);


    int getNbCartesRestantes() const;
    
    bool estVide() const;

    /**
     * @brief Ajoute une carte sur le dessus de la PiocheEsprit (utile pour l'initialisation)
     */
    void ajouterCarte(CarteEsprit* carte);

    /**
     * @brief Retire et renvoie la carte sur le dessus de la PiocheEsprit.
     * @return Un pointeur vers la carte piochée, ou nullptr si vide.
     */
    CarteEsprit* piocherEsprit();

    /**
     * @brief Mélange les cartes du jeu
     */
    void melanger();

    /**
     * @brief Cherche et retire une carte esprit par son nom.
     */
    CarteEsprit* prendreParNom(const std::string& nom);
};