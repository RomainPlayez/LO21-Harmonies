#include "core/PlateauJoueur.h"


PlateauJoueur::PlateauJoueur(FacePlateau face) : _face(face), _nbJetonsEnAttente(0) {
    _nbCases = (_face == FacePlateau::FACE_A) ? 23 : 25; 

    _cases = new Case*[_nbCases];
    initialiserGrille();

    for (int i = 0; i < 3; ++i) {
        _jetonsEnAttente[i] = nullptr;
    }
}

void PlateauJoueur::initialiserGrille() {
    if (_face == FacePlateau::FACE_A) {
        // Configuration Face A : 23 cases jouables
        int coordsA[23][2] = {
            {0,0}, {0,1}, {0,2}, {0,3}, {0,4},
            {1,0}, {1,1}, {1,2}, {1,3},
            {2,0}, {2,1}, {2,2}, {2,3}, {2,4},
            {3,0}, {3,1}, {3,2}, {3,3},
            {4,0}, {4,1}, {4,2}, {4,3}, {4,4}
        };
        for (int i = 0; i < 23; ++i) {
            _cases[i] = new Case(coordsA[i][0], coordsA[i][1]);
        }
    } else {
        // Configuration Face B : 25 cases jouables
        int coordsB[25][2] = {
            {0,0}, {0,1}, {0,2}, {0,3},
            {1,0}, {1,1}, {1,2},
            {2,0}, {2,1}, {2,2}, {2,3},
            {3,0}, {3,1}, {3,2},
            {4,0}, {4,1}, {4,2}, {4,3},
            {5,0}, {5,1}, {5,2},
            {6,0}, {6,1}, {6,2}, {6,3}
        };
        for (int i = 0; i < 25; ++i) {
            _cases[i] = new Case(coordsB[i][0], coordsB[i][1]); 
        }
    }
}

PlateauJoueur::~PlateauJoueur() {
    // Libération des cases allouées dynamiquement
    for (int i = 0; i < _nbCases; ++i) {
        delete _cases[i];
    }
    delete[] _cases;

    //  Libération des jetons restés en zone d'attente
    for (int i = 0; i < 3; ++i) {
        if (_jetonsEnAttente[i] != nullptr) {
            delete _jetonsEnAttente[i];
            _jetonsEnAttente[i] = nullptr;
        }
    }
}

PlateauJoueur::PlateauJoueur(const PlateauJoueur& autre)
    : _face(autre._face), _nbCases(autre._nbCases), _nbJetonsEnAttente(autre._nbJetonsEnAttente) {

    // Copie profonde des cases
    _cases = new Case*[_nbCases];
    for (int i = 0; i < _nbCases; ++i) {
        _cases[i] = new Case(*(autre._cases[i]));
    }

    // Copie profonde des jetons en zone d'attente
    for (int i = 0; i < 3; ++i) {
        if (autre._jetonsEnAttente[i] != nullptr) {
            _jetonsEnAttente[i] = new Jeton(*(autre._jetonsEnAttente[i]));
        } else {
            _jetonsEnAttente[i] = nullptr;
        }
    }
}

PlateauJoueur& PlateauJoueur::operator=(const PlateauJoueur& autre) {
    if (this != &autre) {
        // Nettoyage complet de la mémoire actuelle
        for (int i = 0; i < _nbCases; ++i) { delete _cases[i]; }
        delete[] _cases;
        for (int i = 0; i < 3; ++i) { delete _jetonsEnAttente[i]; }

        // Assignation des valeurs de base
        _face = autre._face;
        _nbCases = autre._nbCases;
        _nbJetonsEnAttente = autre._nbJetonsEnAttente;

        // Recopie profonde des nouvelles structures
        _cases = new Case*[_nbCases];
        for (int i = 0; i < _nbCases; ++i) { 
            _cases[i] = new Case(*(autre._cases[i])); 
        }
        for (int i = 0; i < 3; ++i) { 
            if (autre._jetonsEnAttente[i] != nullptr) {
                _jetonsEnAttente[i] = new Jeton(*(autre._jetonsEnAttente[i]));
            } else {
                _jetonsEnAttente[i] = nullptr;
            }
        }
    }
    return *this;
}

FacePlateau PlateauJoueur::getFace() const { return _face; }
int PlateauJoueur::getNbCases() const { return _nbCases; }

Case* PlateauJoueur::getCase(int x, int y) const {
    for (int i = 0; i < _nbCases; ++i) {
        if (_cases[i]->getX() == x && _cases[i]->getY() == y) {
            return _cases[i];
        }
    }
    return nullptr; // Retourne nullptr si coordonnées invalides (trous dans la grille)
}

int PlateauJoueur::getIndexCase(int x, int y) const {
    for (int i = 0; i < _nbCases; ++i) {
        if (_cases[i]->getX() == x && _cases[i]->getY() == y) return i;
    }
    return -1;
}

Case* PlateauJoueur::getCaseParIndex(int i) const {
    if (i < 0 || i >= _nbCases) return nullptr;
    return _cases[i];
}

bool PlateauJoueur::stockerJetonEnAttente(Jeton* j) {
    if (_nbJetonsEnAttente >= 3) return false;
    for (int i = 0; i < 3; ++i) {
        if (_jetonsEnAttente[i] == nullptr) {
            _jetonsEnAttente[i] = j;
            _nbJetonsEnAttente++;
            return true;
        }
    }
    return false;
}

Jeton* PlateauJoueur::utiliserJetonEnAttente(int index) {
    if (index < 0 || index >= 3 || _jetonsEnAttente[index] == nullptr) return nullptr;
    Jeton* j = _jetonsEnAttente[index];
    _jetonsEnAttente[index] = nullptr;
    _nbJetonsEnAttente--;
    return j;
}

int PlateauJoueur::getNbJetonsEnAttente() const { return _nbJetonsEnAttente; }

void PlateauJoueur::getVoisins(int x, int y, Case* outVoisins[6]) const {
    int diffX[2][6] = {
        { 0,  0, -1, -1,  1,  1}, // Si X est Pair
        { 0,  0, -1, -1,  1,  1}  // Si X est Impair
    };
    int diffY[2][6] = {
        {-1,  1, -1,  0, -1,  0}, // Si X est Pair
        {-1,  1,  0,  1,  0,  1}  // Si X est Impair
    };

    int parity = x % 2;

    for (int i = 0; i < 6; ++i) {
        int nx = x + diffX[parity][i];
        int ny = y + diffY[parity][i];
        outVoisins[i] = getCase(nx, ny); // Complète le tableau statique de l'appelant
    }
}

const Jeton* PlateauJoueur::getJetonEnAttente(int index) const {
    if (index < 0 || index >= 3) return nullptr;
    return _jetonsEnAttente[index];
}
