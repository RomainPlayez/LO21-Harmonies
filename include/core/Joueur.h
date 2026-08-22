#pragma once
#include <string>
#include <cstdbool>
#include "PlateauJoueur.h"
#include "Carte.h"
#include "Jeton.h"
#include "Case.h"

/**
 * @class Joueur
 * @brief Représente un joueur : possède un plateau personnel, des cartes et un score.
 */
class Joueur {
private:
    std::string _nom;
    int _score;
    int _soleils;
    int _detailsScore[7];

    PlateauJoueur* _plateau;

    Carte** _cartesActives;
    int _nbCartesActives;

    Carte** _cartesCompletees;
    int _nbCartesCompletees;

    static const int MAX_CARTES_ACTIVES    = 4;
    int MAX_CARTES_COMPLETEES = 0;

    

public:
    /**
     * @brief Constructeur principal.
     * @param nom Le nom du joueur.
     * @param face La face du plateau personnel (A ou B).
     */
    Joueur(const std::string& nom, FacePlateau face);

    /**
     * @brief Destructeur — libère le plateau et les cartes possédées.
     */
    ~Joueur();

    /**
     * @brief Retourne le nom du joueur.
     */
    const std::string& getNom() const;

    /**
     * @brief Modifie le nom du joueur (utilisé notamment lors du chargement d'une sauvegarde).
     * @param nom Le nouveau nom à attribuer.
     */
    void setNom(const std::string& nom);

    /**
     * @brief Retourne le score courant.
     */
    int getScore() const;


    // getter et setter pour soleil
    int getSoleils() const;
    void setSoleils(int soleils);

    /**
     * @brief Retourne une référence vers le plateau personnel.
     */
    PlateauJoueur* getPlateau() const;

    int getNbCartesActives() const;
    int getNbCartesCompletees() const;
    Carte* getCarteCompletee(int index) const;

    /**
     * @brief Accède à une carte active par son index.
     * @param index Position dans la main (0 à nbCartesActives-1).
     * @return Pointeur vers la carte, ou nullptr si index invalide.
     */
    Carte* getCarteActive(int index) const;

    /**
     * @brief Ajoute une carte à la main active.
     * @param c Pointeur vers la carte transférée depuis le PlateauCentral.
     * @return true si ajoutée, false si la main est déjà pleine (5 cartes max).
     */
    bool prendreCarte(Carte* c);

    /**
     * @brief Place un jeton sur une case après validation des règles du Validateur.
     * @param j Le jeton à placer (doit être en attente sur le plateau).
     * @param c La case cible.
     * @return true si le placement est valide et a été effectué.
     */
    bool placerJeton(Jeton* j, Case* c);

    /**
     * @brief Ajoute des points au score du joueur.
     * @param points Nombre de points à ajouter.
     */
    void ajouterScore(int points);

    /**
     * @brief Vérifie si le joueur a bien placé tous ses jetons en attente (fin de tour possible).
     * @return true si aucun jeton n'est encore en attente de placement.
     */
    bool finDeTour() const;

    /**
     * @brief Une fois qu'une carte est complétée, la fonction la retire du tableau des cartesActives
     * et l'ajoute à celui des cartesCompletees.
     * @return true si la carte a bien été retirée, false sinon.
     */
    bool retirerCarte(int index);

    //@brief : permet la gestion du tableau des cartes complétées (ajout, adaptation taille du tableau)
    void gererCartesCompletees(Carte* c);

    // Blocage de la copie pour éviter les doubles libérations (Double Free)
    Joueur(const Joueur&) = delete;
    Joueur& operator=(const Joueur&) = delete;

    /**
     * @brief Ajoute des points au score total ET au détail d'une catégorie.
     * @param indexCategorie (0=Arbre, 1=Montagne, 2=Bâtiment, 3=Champ, 4=Eau, 5=Animaux, 6=Esprits)
     * @param points Le nombre de points à ajouter.
     */
    void ajouterScoreDetail(int indexCategorie, int points);
    
    /**
     * @brief Récupère le score d'une catégorie spécifique.
     */
    int getScoreDetail(int indexCategorie) const;
};
