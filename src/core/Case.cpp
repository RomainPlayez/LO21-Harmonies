#include "core/Case.h"

Case::Case(int x, int y) : _x(x), _y(y), _z(0), _cube(nullptr) {
    for (int i = 0; i < 3; ++i) {
        _pile[i] = nullptr;
    }
}

Case::~Case() {
    // La case est propriétaire de ses jetons et de son cube.
    for (int i = 0; i < _z; ++i) {
        delete _pile[i];
    }
    delete _cube;
}

Case::Case(const Case& autre) : _x(autre._x), _y(autre._y), _z(autre._z) {
    // Copie profonde des jetons
    for (int i = 0; i < 3; ++i) {
        if (autre._pile[i] != nullptr) {
            _pile[i] = new Jeton(*(autre._pile[i]));
        } else {
            _pile[i] = nullptr;
        }
    }
    
    // Copie profonde du cube animal
    if (autre._cube != nullptr) {
        _cube = new CubeAnimal(*(autre._cube)); 
    } else {
        _cube = nullptr;
    }
}

Case& Case::operator=(const Case& autre) {
    if (this != &autre) {
        // Nettoyage de la mémoire actuelle
        for (int i = 0; i < _z; ++i) {
            delete _pile[i];
        }
        delete _cube;

        // Copie des attributs simples
        _x = autre._x;
        _y = autre._y;
        _z = autre._z;

        // Copie profonde des pointeurs
        for (int i = 0; i < 3; ++i) {
            if (autre._pile[i] != nullptr) {
                _pile[i] = new Jeton(*(autre._pile[i]));
            } else {
                _pile[i] = nullptr;
            }
        }
        
        if (autre._cube != nullptr) {
            _cube = new CubeAnimal(*(autre._cube));
        } else {
            _cube = nullptr;
        }
    }
    return *this;
}

int Case::getX() const { return _x; }
int Case::getY() const { return _y; }
int Case::getZ() const { return _z; }

bool Case::estVide() const {
    return _z == 0 && _cube == nullptr;
}

bool Case::aUnCubeAnimal() const {
    return _cube != nullptr;
}

const Jeton* Case::getJeton(int niveau) const {
    if (niveau >= 0 && niveau < 3) {
        return _pile[niveau];
    }
    return nullptr;
}

CouleurJeton Case::getCouleurSommet() const {
    if (_z == 0) { // Plus sécurisé que estVide() si jamais il y a un cube mais pas de jeton
        throw std::logic_error("Erreur : Impossible de recuperer la couleur, aucun jeton sur la case.");
    }
    return _pile[_z - 1]->getCouleur();
}

bool Case::placerJeton(Jeton* j) {
    // Interdit de placer un jeton si un cube animal est déjà présent ou pile pleine
    if (_cube != nullptr || _z >= 3) {
        return false;
    }

    _pile[_z] = j; // On place le jeton au niveau actuel de z
    _z++;          // La hauteur augmente
    j->setEnJeu(true);
    return true;
}

bool Case::placerCube(CubeAnimal* c) {
    // Un seul cube par case
    if (_cube != nullptr) {
        return false;
    }
    _cube = c;
    return true;
}

void Case::vider() {
    for (int i = 0; i < _z; ++i) {
        delete _pile[i];
        _pile[i] = nullptr;
    }
    _z = 0;
    _cube = nullptr;
}