#pragma once
#include "core/Carte.h"

/**
 * @class PlateauJoueur
 * @brief Forward déclaration pour éviter l'import d'un gros objet inutile.
 */
class PlateauJoueur;

// Énumération des cibles pour EspritGroupe
enum class CibleGroupe {
    CHAMP,// JAUNE  — groupes connexes de cases jaunes 
    BATIMENT, // ROUGE  — groupes connexes de cases rouges
    EAU, // BLEU   — groupes connexes de cases bleues
};

// Énumération des cibles pour EspritQuantite 
enum class CibleQuantite {
    ARBRE, // VERT
    MONTAGNE, // GRIS
    EAU // BLEU
};

/**
 * @class CarteEsprit
 * @brief Classe abstraite pour les cartes Esprit : calcule un bonus de score selon l'état du plateau.
 */
class CarteEsprit : public Carte {
protected:
    /**
     * @brief Constructeur protégé.
     */
    CarteEsprit(const std::string& nom, const std::string& imageRef, int nbCubes);

public:
    virtual ~CarteEsprit() = default;

    /**
     * @brief Calcule le bonus de score apporté par cette carte selon l'état du plateau.
     */
    virtual int calculerBonus(const PlateauJoueur& p) const = 0;
};

/**
 * @class EspritGroupe
 * @brief Esprit qui score selon la taille des groupes connexes d'un paysage.
 */
class EspritGroupe : public CarteEsprit {
private:
    CibleGroupe _cible;
    int _hauteurCible;//nécessaire pour le chat et la cigogne
    int _tailles[3];
    int _points[3];
    
    int obtenirPoints(int taille) const;

public:
    EspritGroupe(const std::string& nom, const std::string& imageRef, int nbCubes, 
                 CibleGroupe cible, int hauteurCible, int t0, int t1, int t2, int p0, int p1, int p2);

    int calculerBonus(const PlateauJoueur& p) const override;
};

/**
 * @class EspritQuantite
 * @brief Esprit qui score selon la quantité globale d'un élément.
 */
class EspritQuantite : public CarteEsprit {
private:
    CibleQuantite _cible;
    int _hauteurCible;
    int _tailles[3];
    int _points[3];

    int obtenirPoints(int quantite) const;

public:
    /**
     * @brief Constructeur de EspritQuantite.
     */
    EspritQuantite(const std::string& nom, const std::string& imageRef, int nbCubes, 
                   CibleQuantite cible, int hauteurCible, int t0, int t1, int t2, int p0, int p1, int p2);

    int calculerBonus(const PlateauJoueur& p) const override;
};