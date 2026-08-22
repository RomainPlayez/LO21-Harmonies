#pragma once
#include "core/Case.h"
#include "core/Jeton.h"
#include "core/PlateauJoueur.h"
#include "core/Motif.h"

/**
 * @class Validateur
 * @brief Gère les règles métier strictes du jeu (empilements, motifs).
 */
class Validateur {
public:
    /**
     * @brief Vérifie si un jeton peut être placé sur une case selon les règles de couleur et de hauteur.
     */
    static bool estPlacementValide(const Case& c, const Jeton& j);

    /**
     * @brief Vérifie si un motif complet est présent sur le plateau autour d'une case cible.
     * @param plateau Le plateau du joueur.
     * @param motif Le motif de la carte Animal à vérifier.
     * @param xCentre La ligne de la case où le joueur veut poser le cube.
     * @param yCentre La colonne de la case où le joueur veut poser le cube.
     * @return true si le motif correspond exactement, false sinon.
     */
    static bool validerMotif(const PlateauJoueur& plateau, const Motif& motif, int xCentre, int yCentre);
};