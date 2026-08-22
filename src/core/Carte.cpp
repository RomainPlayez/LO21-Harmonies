#include "core/Carte.h"
#include "core/Motif.h"
#include "core/CubeAnimal.h"

Carte::Carte(const std::string& nom, const std::string& imageRef, int nbCubes)
    : _nom(nom), _imageRef(imageRef), _nbCubesMax(nbCubes), _nbCubesActuels(nbCubes), _motif(nullptr) {
    
    // Allocation dynamique du tableau de pointeurs de cubes 
    _cubes = new CubeAnimal*[_nbCubesMax];
    for (int i = 0; i < _nbCubesMax; ++i) {
        _cubes[i] = new CubeAnimal(_nom); // Instanciation de chaque cube
    }
}

Carte::~Carte() {
    // Libération des cubes restants sur la carte
    for (int i = 0; i < _nbCubesActuels; ++i) {
        delete _cubes[i];
    }
    delete[] _cubes;
    
    // Libération du motif associé
    if (_motif != nullptr) {
        delete _motif;
    }
}

CubeAnimal* Carte::prendreCubeBas() {
    if (_nbCubesActuels <= 0) return nullptr;
    
    // On récupère le pointeur du dernier cube disponible
    CubeAnimal* cubeAExtraire = _cubes[_nbCubesActuels - 1];
    
    // On détache le pointeur de notre tableau pour que la carte n'en soit plus propriétaire
    _cubes[_nbCubesActuels - 1] = nullptr;
    _nbCubesActuels--;
    
    return cubeAExtraire; // On renvoie le vrai pointeur 
}