#pragma once
#include "core/PlateauJoueur.h"
#include "core/Case.h"

/**
 * @class DetecteurPaysage
 * @brief Classe utilitaire pour analyser la grille.
 * N'utilise aucune allocation dynamique interne pour des performances maximales.
 */
class DetecteurPaysage {
public:
    /**
     * @brief Retourne la hauteur de l'arbre présent sur la case.
     */
    static int calculerTailleArbre(const Case* c);

    /**
     * @brief Algorithme de Flood Fill pour trouver la taille d'un champ (JAUNE).
     * @param depart La case de départ.
     * @param plateau Le plateau à analyser.
     * @param casesVisitees Tableau de booléens (alloué par l'appelant) pour mémoriser le parcours.
     */
    static int calculerTailleChamp(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees);

    /**
     * @brief Algorithme de Flood Fill pour les montagnes (GRIS).
     */
    static int calculerTailleMontagne(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees);

    /**
     * @brief [FACE A] Trouve la plus longue rivière continue sur tout le plateau. Mais le plus petit chemin pour y acceder
     */
    static int calculerPlusLongueRiviere(const PlateauJoueur& plateau);

    /**
     * @brief [FACE B] Compte le nombre d'îles (zones non bleues délimitées par l'eau ou les bords).
     */
    static int calculerNombreIles(const PlateauJoueur& plateau);

    /**
     * @brief Compte le nombre de couleurs distinctes adjacentes à un bâtiment (ROUGE).
     */
    static int compterCouleursBatiment(const Case* c, const PlateauJoueur& plateau);

    /**
     * @brief Flood Fill sur les cases BLEU pour trouver la taille d'un groupe d'eau connexe.
     * @param depart La case de départ (doit être BLEU).
     * @param plateau Le plateau à analyser.
     * @param casesVisitees Tableau alloué par l'appelant.
     */

    static int calculerTailleRiviere(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees);

    /**
    * @brief Retourne la hauteur de la montagne sur une case (1, 2 ou 3).
    * Retourne 0 si la case n'est pas une montagne (sommet non GRIS).
    * @param c La case à analyser.
    */
    static int calculerHauteurMontagne(const Case* c);

    /**
     * @brief Flood Fill sur les cases ROUGE d'une hauteur exacte pour trouver la taille d'un groupe de bâtiments connexes.
     * @param depart La case de départ (doit être ROUGE et avoir exactement hauteurCible jetons empilés).
     * @param plateau Le plateau à analyser.
     * @param casesVisitees Tableau alloué par l'appelant.
     * @param hauteurCible La hauteur exacte requise (ici 2 pour Chat/Cigogne). Partie pris : Une case à hauteur 3 ne compte pas.
     */
    static int calculerTailleBatiment(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees, int hauteurCible);

private:
    // Helpers récursifs pour le parcours
    static void dfsIle(const Case* c, const PlateauJoueur& plateau, bool* casesVisitees);
};