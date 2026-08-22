#pragma once
#include "core/Jeton.h"

/**
 * @class Sac
 * @brief Gère la pioche des jetons avec un tableau dynamique.
 */
class Sac {
private:
    Jeton** _jetonsStockes; // Tableau alloué dynamiquement de pointeurs sur Jeton
    int _nbJetonsRestants;
    int _capaciteMax;

public:
    /**
     * @brief Constructeur par défaut.
     */
    Sac();

    /**
     * @brief Destructeur indispensable car on utilise "new" dans la classe.
     */
    ~Sac();

    /**
     * @brief Constructeur de recopie
     * Permet d'éviter que deux sacs pointent vers les mêmes jetons en mémoire.
     */
    Sac(const Sac& autre);

    /**
     * @brief Opérateur d'affectation
     */
    Sac& operator=(const Sac& autre);

    int getNbJetonsRestants() const;
    bool estVide() const;

    /**
     * @brief Pioche un nombre défini de jetons dans le sac.
     * @param quantite Le nombre de jetons à piocher.
     * @return Un tableau dynamique contenant des pointeurs vers les jetons piochés.
     * @note Le code appelant (ex: le PlateauCentral) sera responsable de supprimer ce tableau.
     */
    Jeton** piocher(int quantite);

    /**
     * @brief Mélange aléatoirement les jetons présents dans le sac.
     */
    void melanger();
    
    int getNbParCouleur(CouleurJeton c) const;
    void reinitialiserEtat(int nbParCouleur[6]);

    /**
     * @brief Supprime tous les jetons actuels du sac.
     */
    void vider();

    /**
     * @brief Ajoute un jeton spécifique (utilisé pour la restauration).
     */
    void ajouterJeton(Jeton* j);

    /**
     * @brief Récupère un jeton sans le retirer du sac (pour la sauvegarde).
     */
    Jeton* getJetonParIndex(int index) const;
};