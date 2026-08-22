#include "core/CarteAnimal.h"
#include <stdexcept>

CarteAnimal::CarteAnimal(const std::string& nom, const std::string& imageRef,
                         int nbCubes, const Motif& motif, const int* points)
    : Carte(nom, imageRef, nbCubes) { // Appel explicite au constructeur de la classe mère

    _motif = new Motif(motif);

    _pointsParEmplacement = new int[_nbCubesMax];
    for (int i = 0; i < _nbCubesMax; ++i) {
        _pointsParEmplacement[i] = points[i];
    }
}

CarteAnimal::~CarteAnimal() {
    delete[] _pointsParEmplacement;
}

int CarteAnimal::getPointsEmplacement(int index) const {
    // Sécurité pour éviter de lire en dehors du tableau
    if (index < 0 || index >= _nbCubesMax) {
        throw std::out_of_range("Index hors limites pour les points d'emplacement.");
    }
    return _pointsParEmplacement[index];
}