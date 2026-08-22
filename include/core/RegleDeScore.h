#pragma once
#include "core/Joueur.h"

/**
 * @class RegleDeScore
 * @brief Interface (Design Pattern Strategy) pour le calcul des points.
 */
class RegleDeScore {
public:
    virtual ~RegleDeScore() = default;
    
    /**
     * @brief Calcule et ajoute les points au joueur en analysant son plateau.
     * @param j Pointeur vers le joueur à évaluer.
     */
    virtual void calculerScore(Joueur* j) const = 0;
};

class ScoreArbre : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};

class ScoreMontagne : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};

class ScoreBatiment : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};

class ScoreEau : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};

class ScoreChamp : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};

/**
 * @class ScoreAnimal
 * @brief Calcule les points rapportés par les cubes placés sur les cartes Animaux.
 */
class ScoreAnimal : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};

/**
 * @class ScoreEsprit
 * @brief Calcule les points bonus accordés par les cartes Esprits de la Nature.
 */
class ScoreEsprit : public RegleDeScore {
public:
    void calculerScore(Joueur* j) const override;
};