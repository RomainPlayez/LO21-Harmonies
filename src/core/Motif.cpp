#include "core/Motif.h"

// Constructeur
Motif::Motif(int taille) : _taille(taille) {
    if (_taille <= 0) {
        throw std::invalid_argument("Un motif doit avoir une taille > 0.");
    }
    _elements = new ElementMotif[_taille];
}


Motif::~Motif() {
    delete[] _elements;
}

Motif::Motif(const Motif& autre) : _taille(autre._taille) {
    _elements = new ElementMotif[_taille];
    for (int i = 0; i < _taille; ++i) {
        _elements[i] = autre._elements[i];
    }
}

Motif& Motif::operator=(const Motif& autre) {
    if (this != &autre) {
        delete[] _elements;
        _taille = autre._taille;
        _elements = new ElementMotif[_taille];
        for (int i = 0; i < _taille; ++i) {
            _elements[i] = autre._elements[i];
        }
    }
    return *this;
}


void Motif::setElement(int index, int dx, int dy, CouleurJeton couleur, int hauteurMinimale) {
    if (index < 0 || index >= _taille) {
        throw std::out_of_range("Index hors limites pour le motif.");
    }
    _elements[index].dx = dx;
    _elements[index].dy = dy;
    _elements[index].couleur = couleur;
    _elements[index].hauteurMinimale = hauteurMinimale;
}

int Motif::getTaille() const {
    return _taille;
}

const ElementMotif& Motif::getElement(int index) const {
    if (index < 0 || index >= _taille) {
        throw std::out_of_range("Index hors limites pour le motif.");
    }
    return _elements[index];
}