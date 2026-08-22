#pragma once
#include "Jeu.h"
#include "Joueur.h"
#include "Validateur.h"
#include <vector>
#include <string>

/**
 * @enum EtatTour
 * @brief Machine à états stricte pour le flux d'un tour de jeu.
 */
enum class EtatTour {
    DEBUT_TOUR,
    GROUPE_CHOISI,  
    CARTE_CHOISIE,  
    JETONS_PLACES,  
    FIN_TOUR
};

/**
 * @class ControleurPartie
 * @brief Pilote le déroulement de la partie en interrogeant le Modèle (Jeu/Joueur).
 */
class ControleurPartie {
private:
    Jeu* _jeu;              // Pointeur vers l'état global du jeu
    EtatTour _etatActuel;   // État courant du tour

public:
    /**
     * @brief Constructeur du contrôleur.
     * @param nbJoueurs Nombre de participants.
     * @param face Face du plateau choisie.
     * @param avecEsprits, booléean pour indiquer si extension oui ou non
     */
    ControleurPartie(const std::vector<std::string>& nomsJoueurs, FacePlateau face, bool avecEsprits);

    /**
     * @brief Destructeur — Libère uniquement le jeu.
     */
    ~ControleurPartie();

    //Empêche la duplication accidentelle du contrôleur
    ControleurPartie(const ControleurPartie&) = delete;
    ControleurPartie& operator=(const ControleurPartie&) = delete;

    /**
     * @brief Démarre la partie et initialise le cycle.
     */
    void demarrerPartie();

    /**
     * @brief Termine le tour actuel et passe au joueur suivant.
     * @return true si le changement est valide, false sinon.
     */
    bool passerTour();

    /**
     * @brief Permet au joueur courant de sélectionner un groupe de jetons.
     */
    bool choisirGroupeJetons(int indexGroupe);

    /**
     * @brief Place un jeton depuis la zone d'attente du joueur actif vers son plateau.
     */
    bool placerJetonDepuisAttente(int x, int y, int indexAttente);

    /**
    * @brief Permet au joueur de placer un cube d'une carte animal sur une case de son plateau.
    * @return true si le placement est valide, false sinon.
    */
    bool placerCubeAnimal(int indexCarte, int x, int y);
    
    // Getters
    EtatTour getEtatActuel() const;
    Jeu* getJeu() const;
    Joueur* getJoueurCourant() const; // Renvoie directement le joueur actif depuis l'objet Jeu

    /**
     * @brief Sauvegarde l'état complet de la partie dans un fichier.
     */
    void sauvegarderPartie(const std::string& cheminFichier) const;

    /**
     * @brief Charge et restaure une partie depuis le fichier de sauvegarde.
     * @return true si la restauration a réussi.
     */
    bool chargerPartie(const std::string& cheminFichier);

    /**
     * @brief Vérifie si un fichier de sauvegarde est disponible.
     */
    static bool sauvegardeDisponible(const std::string& cheminFichier);  

    void gererFinTourSolo(bool cartePrise, int indexCarteDefausse = -1);
};