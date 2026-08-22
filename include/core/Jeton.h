#pragma once

/**
 * @enum CouleurJeton
 * @brief Représente les 6 couleurs possibles d'un jeton physique.
 */
enum class CouleurJeton {
    GRIS,
    ROUGE,
    MARRON,
    VERT,
    JAUNE,
    BLEU
};

/**
 * @class Jeton
 * @brief Représente un jeton du jeu Harmonies.
 */
class Jeton {
private:
    CouleurJeton _couleur;
    bool _estEnJeu;

public:
    /**
     * @brief Constructeur de la classe Jeton.
     * @param c La couleur du jeton.
     * @param enJeu Indique si le jeton est en jeu (par défaut à false).
     */
    Jeton(CouleurJeton c, bool enJeu = false);

    /**
     * @brief Récupère la couleur du jeton.
     * @return La couleur du jeton.
     */
    CouleurJeton getCouleur() const;

    /**
     * @brief Vérifie si le jeton est actuellement en jeu (piochable).
     * @return true si le jeton est en jeu, false sinon.
     */
    bool estEnJeu() const;
    
    /**
     * @brief Modifie l'état de jeu du jeton.
     * @param etat Le nouvel état du jeton.
     */
    void setEnJeu(bool etat);
};