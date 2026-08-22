#pragma once
#include "core/Jeton.h"

/**
 * @class GroupeJetons
 * @brief Représente un groupe de 3 jetons exposés sur le PlateauCentral.
 * Le groupe possède ses jetons et les libère à sa destruction.
 */
class GroupeJetons {
private:
    Jeton* _jetons[3];
    int    _nbJetons;

public:
    /**
     * @brief Constructeur — crée un groupe vide.
     */
    GroupeJetons();

    /**
     * @brief Constructeur — crée un groupe à partir d'un tableau de pointeurs.
     * @param jetons Tableau de nbJetons pointeurs.
     * @param nbJetons Nombre de jetons (1 à 3).
     */
    GroupeJetons(Jeton** jetons, int nbJetons);

    ~GroupeJetons();

    /**
     * @brief Vérifie si le groupe ne contient plus aucun jeton.
     */
    bool estVide() const;

    int getNbJetons() const;

    /**
     * @brief Accès à un jeton par son index.
     * @param index Position (0 à 2).
     * @return Pointeur vers le jeton, ou nullptr si absent.
     */
    const Jeton* getJeton(int index) const;

    /**
     * @brief Retire tous les jetons du groupe et les transfère dans un tableau externe.
     * @param result Tableau de 3 pointeurs qui recevra les jetons.
     * @return true si le groupe n'était pas vide.
     */
    bool prendreJetons(Jeton* result[3]);

    /**
     * @brief Opérateur d'affectation et de recopie.
     */
    GroupeJetons(const GroupeJetons& autre);
    GroupeJetons& operator=(const GroupeJetons& autre);
};
