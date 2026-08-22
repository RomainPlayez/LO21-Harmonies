#pragma once
#include <string>

// Forward declarations pour éviter les inclusions cycliques
class Motif;
class CubeAnimal;

/**
 * @class Carte
 * @brief Classe de base abstraite représentant une carte du jeu (Animal ou Esprit).
 */
class Carte {
protected:
    std::string _nom;
    std::string _imageRef;
    int _nbCubesMax;
    int _nbCubesActuels;
    
    // Attributs remontés ici car communs aux Animaux et aux Esprits
    Motif* _motif;
    CubeAnimal** _cubes; // Tableau dynamique de pointeurs

public:
    /**
     * @brief Constructeur de base.
     */
    Carte(const std::string& nom, const std::string& imageRef, int nbCubes);
    
    /**
     * @brief Destructeur virtuel obligatoire pour sécuriser l'héritage.
     */
    virtual ~Carte();

    // Getters génériques
    const std::string& getNom() const { return _nom; }
    const std::string& getImageRef() const { return _imageRef; }
    int getNbCubesActuels() const { return _nbCubesActuels; }
    int getNbCubesMax() const { return _nbCubesMax; }
    
    // Accesseurs pour le Motif
    Motif* getMotif() const { return _motif; }
    void setMotif(Motif* motif) { _motif = motif; }

    /**
     * @brief Extrait le cube du bas de la carte pour le donner au plateau.
    */
    CubeAnimal* prendreCubeBas();

    /**
     * @brief Force le nombre de cubes restants à une valeur précise.
     */
    void setCubesActuels(int n) { _nbCubesActuels = n; }

    /**
     * @brief Indique si tous les cubes de la carte ont été placés.
     */
    bool estComplete() const { return _nbCubesActuels == 0; }

    // Sécurité anti-copie
    Carte(const Carte&) = delete;
    Carte& operator=(const Carte&) = delete;
};