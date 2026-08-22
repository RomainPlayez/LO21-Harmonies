#include "core/PiocheEsprit.h"
#include <iostream>
#include <cstdlib> // Pour rand() et srand()
#include <ctime>   // Pour time()

// Constructeur
PiocheEsprit::PiocheEsprit() : _nbCartesRestantes(0) {
    _PiocheEspritCartes = new CarteEsprit*[_capaciteMax];
    // On initialise tous les pointeurs à nullptr par sécurité
    for (int i = 0; i < _capaciteMax; ++i) {
        _PiocheEspritCartes[i] = nullptr;
    }
}

// Destructeur
PiocheEsprit::~PiocheEsprit() {
    // On détruit chaque cartes encore présent dans la PiocheEsprit
    if (_PiocheEspritCartes != nullptr) {
        for (int i = 0; i < _nbCartesRestantes; ++i) {
            delete _PiocheEspritCartes[i];
        }
        // On détruit le tableau de pointeurs lui-même
        delete[] _PiocheEspritCartes;
    }
}

// Constructeur de recopie
PiocheEsprit::PiocheEsprit(const PiocheEsprit& autre) : _nbCartesRestantes(autre._nbCartesRestantes) {
    _PiocheEspritCartes = new CarteEsprit*[_capaciteMax];
    for (int i = 0; i < _capaciteMax; ++i) {
        _PiocheEspritCartes[i] = autre._PiocheEspritCartes[i];
        }
    }

// Opérateur d'affectation =
PiocheEsprit& PiocheEsprit::operator=(const PiocheEsprit& autre) {
    if (this != &autre) {
        // On nettoie l'ancienne mémoire
        for (int i = 0; i < _nbCartesRestantes; ++i) {
            delete _PiocheEspritCartes[i];
        }
        delete[] _PiocheEspritCartes;
        //  On copie les nouvelles données
        _nbCartesRestantes = autre._nbCartesRestantes;
        _PiocheEspritCartes = new CarteEsprit*[_capaciteMax];
        for (int i = 0; i < _capaciteMax; ++i) {
            _PiocheEspritCartes[i] = autre._PiocheEspritCartes[i];
        }
    }
    return *this;
}

int PiocheEsprit::getNbCartesRestantes() const {
    return _nbCartesRestantes;
}

bool PiocheEsprit::estVide() const {
    return _nbCartesRestantes == 0;
}

void PiocheEsprit::ajouterCarte(CarteEsprit* carte) {
    if (_nbCartesRestantes < _capaciteMax) {
        _PiocheEspritCartes[_nbCartesRestantes] = carte;
        _nbCartesRestantes++;
    }
}

CarteEsprit* PiocheEsprit::piocherEsprit() {
    if (estVide()) {
        return nullptr;
    }
    // On récupère la carte du dessus (la dernière du tableau)
    _nbCartesRestantes--; // On diminue le compteur
    CarteEsprit* cartePiocheEsprite = _PiocheEspritCartes[_nbCartesRestantes];

    _PiocheEspritCartes[_nbCartesRestantes] = nullptr;

    return cartePiocheEsprite;
}

void PiocheEsprit::melanger() {
    // Initialisation de la graine aléatoire (à ne faire qu'une fois dans le jeu idéalement, mais sécurisé ici)
    static bool seedInitialisee = false;
    if (!seedInitialisee) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seedInitialisee = true;
    }

    // Algorithme de Fisher-Yates pour mélanger le tableau
    for (int i = _nbCartesRestantes - 1; i > 0; --i) {
        int j = std::rand() % (i + 1); // Nombre aléatoire entre 0 et i
        
        // Échange des pointeurs
        CarteEsprit* temp = _PiocheEspritCartes[i];
        _PiocheEspritCartes[i] = _PiocheEspritCartes[j];
        _PiocheEspritCartes[j] = temp;
    }
}

CarteEsprit* PiocheEsprit::prendreParNom(const std::string& nom) {
    for (int i = 0; i < _nbCartesRestantes; ++i) {
        if (_PiocheEspritCartes[i]->getNom() == nom) {
            CarteEsprit* carte = _PiocheEspritCartes[i];
            for (int j = i; j < _nbCartesRestantes - 1; ++j)
                _PiocheEspritCartes[j] = _PiocheEspritCartes[j + 1];
            _PiocheEspritCartes[--_nbCartesRestantes] = nullptr;
            return carte;
        }
    }
    return nullptr;
}