#include "core/GroupeJetons.h"

GroupeJetons::GroupeJetons() : _nbJetons(0) {
    for (int i = 0; i < 3; ++i) _jetons[i] = nullptr;
}

GroupeJetons::GroupeJetons(Jeton** jetons, int nbJetons) : _nbJetons(nbJetons) {
    for (int i = 0; i < 3; ++i) _jetons[i] = nullptr;
    for (int i = 0; i < _nbJetons && i < 3; ++i) {
        _jetons[i] = jetons[i];
    }
}

GroupeJetons::~GroupeJetons() {
    for (int i = 0; i < 3; ++i) {
        if (_jetons[i] != nullptr) {
            delete _jetons[i];
            _jetons[i] = nullptr;
        }
    }
}

// Copie profonde
GroupeJetons::GroupeJetons(const GroupeJetons& autre) : _nbJetons(autre._nbJetons) {
    for (int i = 0; i < 3; ++i) {
        if (autre._jetons[i] != nullptr) {
            _jetons[i] = new Jeton(*(autre._jetons[i]));
        } else {
            _jetons[i] = nullptr;
        }
    }
}

GroupeJetons& GroupeJetons::operator=(const GroupeJetons& autre) {
    if (this != &autre) {
        // Nettoyage de l'existant
        for (int i = 0; i < 3; ++i) {
            delete _jetons[i];
        }
        // Recopie profonde
        _nbJetons = autre._nbJetons;
        for (int i = 0; i < 3; ++i) {
            if (autre._jetons[i] != nullptr) {
                _jetons[i] = new Jeton(*(autre._jetons[i]));
            } else {
                _jetons[i] = nullptr;
            }
        }
    }
    return *this;
}

bool GroupeJetons::estVide() const {
    return _nbJetons == 0;
}

int GroupeJetons::getNbJetons() const {
    return _nbJetons;
}

const Jeton* GroupeJetons::getJeton(int index) const {
    if (index < 0 || index >= 3) return nullptr;
    return _jetons[index];
}

bool GroupeJetons::prendreJetons(Jeton* result[3]) {
    if (estVide()) return false;
    for (int i = 0; i < 3; ++i) {
        result[i]  = _jetons[i];
        _jetons[i] = nullptr; // Désengagement de la propriété
    }
    _nbJetons = 0;
    return true;
}