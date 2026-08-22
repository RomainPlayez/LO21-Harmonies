#pragma once

#include <vector>
#include "core/PlateauCentral.h"
#include "core/Joueur.h"
#include "core/Sac.h"
#include "core/PiocheEsprit.h"
#include "core/EtatPartie.h"

/**
 * @class Jeu
 * @brief Modèle principal de l'état global de la partie.
 * Détient et gère le cycle de vie du Sac, du PlateauCentral et des Joueurs.
 */
class Jeu {
private:
    PlateauCentral* _plateauCentral; /**< Pointeur vers la zone commune */
    Sac* _sac;            /**< Pointeur vers le sac de jetons */
    
    Joueur** _joueurs;        /**< Tableau dynamique de pointeurs sur Joueur  */
    int _nbJoueurs;      /**< Nombre total de joueurs dans la partie */
    
    int  _tourActuel;     /**< Compteur de tours de table */
    int _joueurActifIndex; /**< Index du joueur dont c'est le tour (0 à _nbJoueurs - 1) */

    void genererPiocheCartes();

    bool _avecEsprits;
    PiocheEsprit _piocheEsprits;
    void genererPiocheEsprits();

public:
    /**
     * @brief Constructeur de la structure globale du jeu.
     * Alloue le Sac, le PlateauCentral et le tableau de Joueurs.
     * @param nomsJoueurs Un vecteur contennant les noms des participants.
     * @param face La face de plateau choisie pour toute la partie (FACE_A ou FACE_B).
     * @param avecEsprits booléen pour vérifier si l'option Esprit est activée. 
     */
    Jeu(const std::vector<std::string>& nomsJoueurs, FacePlateau face, bool avecEsprits);

    /**
     * @brief getter pour avoir True ou False en fonction de l'option Esprit. 
     */
    bool isAvecEsprits() const { return _avecEsprits; }

    /**
     * @brief Destructeur profond. Libère le sac, le plateau central et tous les joueurs (et par cascade leurs plateaux).
     */
    ~Jeu();

    /**
     * @brief  La classe Jeu représentant l'état unique de la partie en cours,
     * on interdit explicitement sa copie ou son affectation pour empêcher les fuites.
     */
    Jeu(const Jeu& autre) = delete;
    Jeu& operator=(const Jeu& autre) = delete;

    /**
     * @brief Initialise le plateau central en remplissant l'offre de jetons depuis le sac.
     */
    void initialiserPartie();

    /**
     * @brief Fait progresser l'index du joueur actif. 
     * Incrémente le compteur de tours si tout le monde a joué et complète l'offre centrale.
     */
    void passerAuTourSuivant();

    /**
     * @brief Accès au plateau central commun.
     * @return Pointeur vers le PlateauCentral.
     */
    PlateauCentral* getPlateauCentral() const;

    /**
     * @brief Récupère directement le Joueur dont c'est le tour actuellement.
     * @return Pointeur vers l'objet Joueur actif.
     */
    Joueur* getJoueurActif() const;

    /**
     * @brief Récupère un joueur précis de la partie via son index.
     * @param index Position dans le tableau (0 à nbJoueurs - 1).
     * @return Pointeur vers le Joueur, ou nullptr si l'index est invalide.
     */
    Joueur* getJoueurParIndex(int index) const;

    /**
     * @brief Récupère le nombre total de joueurs.
     */
    int getNbJoueurs() const;

    /**
     * @brief Détermine si les conditions de fin de partie sont réunies (ex: sac vide).
     * @return true si le jeu s'arrête, false sinon.
     */
    bool estFini() const;

    /**
     * @brief Parcourt tous les joueurs et applique l'ensemble des règles de score 
     * pour calculer le classement final.
     */
    void declencherScoringFinal();

    int getTourActuel() const{return _tourActuel;};

    /**
     * @brief Pioche une carte esprit dans la pioche dédiée.
     */
    CarteEsprit* piocherEsprit();

    /**
     * @brief Capture l'état complet du jeu dans un EtatPartie.
     */
    struct EtatPartie sauvegarder() const;

    /**
     * @brief Restaure le jeu depuis un EtatPartie.
     */
    void restaurer(const struct EtatPartie& etat);
};