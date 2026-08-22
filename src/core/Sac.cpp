#include "core/Sac.h"
#include <stdexcept>
#include <cstdlib> // Pour rand() et srand()
#include <ctime>   // Pour time()

// Constructeur
Sac::Sac() : _nbJetonsRestants(120), _capaciteMax(120) {
    // Allocation du tableau qui va contenir 120 pointeurs de jetons
    _jetonsStockes = new Jeton*[_capaciteMax];

    CouleurJeton couleurs[] = {
        CouleurJeton::BLEU,
        CouleurJeton::GRIS,
        CouleurJeton::MARRON,
        CouleurJeton::VERT,
        CouleurJeton::JAUNE,
        CouleurJeton::ROUGE

    };

    // Définition des quantités respectives (Total = 120)
    int quantites[] = {23, 23, 21, 19, 19, 15};

    int indexGlobal = 0; // Cet index suit la progression des indices de 0 à 119

    for (int c = 0; c < 6; ++c) {             // On boucle sur les 6 couleurs
        for (int q = 0; q < quantites[c]; ++q) { // On boucle autant de fois que la quantité demandée
            _jetonsStockes[indexGlobal] = new Jeton(couleurs[c]);
            indexGlobal++;
        }
    }

    this->melanger();
}

// Destructeur
Sac::~Sac() {
    // On détruit chaque jeton encore présent dans le sac
    for (int i = 0; i < _nbJetonsRestants; ++i) {
        delete _jetonsStockes[i];
    }
    // Puis on détruit le tableau de pointeurs lui-même
    delete[] _jetonsStockes;
}

// Constructeur de recopie 
Sac::Sac(const Sac& autre) : _nbJetonsRestants(autre._nbJetonsRestants), _capaciteMax(autre._capaciteMax) {
    _jetonsStockes = new Jeton*[_capaciteMax];
    for (int i = 0; i < _nbJetonsRestants; ++i) {
        _jetonsStockes[i] = new Jeton(*(autre._jetonsStockes[i])); 
    }
    // On sécurise le reste du sac avec des pointeurs nuls
    for (int i = _nbJetonsRestants; i < _capaciteMax; ++i) {
        _jetonsStockes[i] = nullptr;
    }
}

// Opérateur d'affectation
Sac& Sac::operator=(const Sac& autre) {
    if (this != &autre) { 
        // Libérer la mémoire actuelle
        for (int i = 0; i < _nbJetonsRestants; ++i) {
            delete _jetonsStockes[i];
        }
        delete[] _jetonsStockes;

        // Copier les nouvelles données
        _nbJetonsRestants = autre._nbJetonsRestants;
        _capaciteMax = autre._capaciteMax;
        _jetonsStockes = new Jeton*[_capaciteMax];
        
        for (int i = 0; i < _nbJetonsRestants; ++i) {
            _jetonsStockes[i] = new Jeton(*(autre._jetonsStockes[i]));
        }
    }
    return *this;
}

int Sac::getNbJetonsRestants() const {
    return _nbJetonsRestants;
}

bool Sac::estVide() const {
    return _nbJetonsRestants == 0;
}

// La Pioche
Jeton** Sac::piocher(int quantite) {
    if (quantite > _nbJetonsRestants) {
        throw std::invalid_argument("Pas assez de jetons dans le sac !");
    }

    // Allocation d'un nouveau tableau pour donner les jetons demandés
    Jeton** jetonsPioches = new Jeton*[quantite];
    
    for (int i = 0; i < quantite; ++i) {
        // On transfère le jeton (le pointeur) du sac vers le nouveau tableau
        jetonsPioches[i] = _jetonsStockes[_nbJetonsRestants - 1];
        
        // On retire le jeton du sac en mettant le pointeur à null
        _jetonsStockes[_nbJetonsRestants - 1] = nullptr;
        _nbJetonsRestants--;
    }
    
    return jetonsPioches;
}

void Sac::melanger() {
    // Initialisation de la graine aléatoire (une seule fois)
    static bool seedInitialisee = false;
    if (!seedInitialisee) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seedInitialisee = true;
    }

    // Algorithme de Fisher-Yates pour mélanger le tableau
    for (int i = _nbJetonsRestants - 1; i > 0; --i) {
        int j = std::rand() % (i + 1); // Nombre aléatoire entre 0 et i
        
        // Échange des pointeurs (Swap)
        Jeton* temp = _jetonsStockes[i];
        _jetonsStockes[i] = _jetonsStockes[j];
        _jetonsStockes[j] = temp;
    }
}

int Sac::getNbParCouleur(CouleurJeton c) const {
    int count = 0;
    for (int i = 0; i < _nbJetonsRestants; ++i)
        if (_jetonsStockes[i]->getCouleur() == c) count++;
    return count;
}

void Sac::reinitialiserEtat(int nbParCouleur[6]) {
    for (int i = 0; i < _nbJetonsRestants; ++i) {
        delete _jetonsStockes[i];
        _jetonsStockes[i] = nullptr;
    }
    _nbJetonsRestants = 0;

    CouleurJeton couleurs[] = {
        CouleurJeton::GRIS, CouleurJeton::ROUGE, CouleurJeton::MARRON,
        CouleurJeton::VERT, CouleurJeton::JAUNE, CouleurJeton::BLEU
    };
    for (int c = 0; c < 6; ++c)
        for (int q = 0; q < nbParCouleur[c]; ++q)
            _jetonsStockes[_nbJetonsRestants++] = new Jeton(couleurs[c]);
}

void Sac::vider() {
    // On libère la mémoire de chaque jeton
    for (int i = 0; i < _nbJetonsRestants; ++i) {
        delete _jetonsStockes[i];
        _jetonsStockes[i] = nullptr;
    }
    // On remet le compteur à zéro
    _nbJetonsRestants = 0;
}

void Sac::ajouterJeton(Jeton* j) {
    if (j != nullptr && _nbJetonsRestants < _capaciteMax) {
        _jetonsStockes[_nbJetonsRestants] = j;
        _nbJetonsRestants++;
    }
}

Jeton* Sac::getJetonParIndex(int index) const {
    if (index >= 0 && index < _nbJetonsRestants) {
        return _jetonsStockes[index];
    }
    return nullptr;
}