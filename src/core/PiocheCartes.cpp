#include "core/PiocheCartes.h"
#include <iostream>
#include <cstdlib> // Pour rand() et srand()
#include <ctime>   // Pour time()

// Constructeur
PiocheAnimal::PiocheAnimal() : _nbCartesRestantes(0) {
    _PiocheAnimalCartes = new CarteAnimal*[_capaciteMax];
    // On initialise tous les pointeurs à nullptr par sécurité
    for (int i = 0; i < _capaciteMax; ++i) {
        _PiocheAnimalCartes[i] = nullptr;
    }
}

// Destructeur
PiocheAnimal::~PiocheAnimal() {
    // On détruit chaque cartes encore présent dans la PiocheAnimal
    if (_PiocheAnimalCartes != nullptr) {
        for (int i = 0; i < _nbCartesRestantes; ++i) {
            delete _PiocheAnimalCartes[i];
        }
        // On détruit le tableau de pointeurs lui-même
        delete[] _PiocheAnimalCartes;
    }
}

// Constructeur de recopie
PiocheAnimal::PiocheAnimal(const PiocheAnimal& autre) : _nbCartesRestantes(autre._nbCartesRestantes) {
    _PiocheAnimalCartes = new CarteAnimal*[_capaciteMax];
    for (int i = 0; i < _capaciteMax; ++i) {
        _PiocheAnimalCartes[i] = autre._PiocheAnimalCartes[i];
        }
    }

// Opérateur d'affectation =
PiocheAnimal& PiocheAnimal::operator=(const PiocheAnimal& autre) {
    if (this != &autre) {
        // On nettoie l'ancienne mémoire
        for (int i = 0; i < _nbCartesRestantes; ++i) {
            delete _PiocheAnimalCartes[i];
        }
        delete[] _PiocheAnimalCartes;
        //  On copie les nouvelles données
        _nbCartesRestantes = autre._nbCartesRestantes;
        _PiocheAnimalCartes = new CarteAnimal*[_capaciteMax];
        for (int i = 0; i < _capaciteMax; ++i) {
            _PiocheAnimalCartes[i] = autre._PiocheAnimalCartes[i];
        }
    }
    return *this;
}

int PiocheAnimal::getNbCartesRestantes() const {
    return _nbCartesRestantes;
}

bool PiocheAnimal::estVide() const {
    return _nbCartesRestantes == 0;
}

void PiocheAnimal::ajouterCarte(CarteAnimal* carte) {
    if (_nbCartesRestantes < _capaciteMax) {
        _PiocheAnimalCartes[_nbCartesRestantes] = carte;
        _nbCartesRestantes++;
    }
}

CarteAnimal* PiocheAnimal::Piocher() {
    if (estVide()) {
        return nullptr;
    }
    // On récupère la carte du dessus (la dernière du tableau)
    _nbCartesRestantes--; // On diminue le compteur
    CarteAnimal* cartePiocheAnimale = _PiocheAnimalCartes[_nbCartesRestantes];

    _PiocheAnimalCartes[_nbCartesRestantes] = nullptr;

    return cartePiocheAnimale;
}

void PiocheAnimal::melanger() {
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
        CarteAnimal* temp = _PiocheAnimalCartes[i];
        _PiocheAnimalCartes[i] = _PiocheAnimalCartes[j];
        _PiocheAnimalCartes[j] = temp;
    }
}

CarteAnimal* PiocheAnimal::prendreParNom(const std::string& nom) {
    for (int i = 0; i < _nbCartesRestantes; ++i) {
        if (_PiocheAnimalCartes[i]->getNom() == nom) {
            CarteAnimal* carte = _PiocheAnimalCartes[i];
            for (int j = i; j < _nbCartesRestantes - 1; ++j)
                _PiocheAnimalCartes[j] = _PiocheAnimalCartes[j + 1];
            _PiocheAnimalCartes[--_nbCartesRestantes] = nullptr;
            return carte;
        }
    }
    return nullptr;
}
