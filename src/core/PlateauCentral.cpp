#include "core/PlateauCentral.h"
#include <iostream>

PlateauCentral::PlateauCentral(int nbJoueurs) {
    _tailleActive = (nbJoueurs == 1) ? 3 : 5;
    for (int i = 0; i < 5; ++i) {
        _offreJetons[i] = new GroupeJetons();
        _riviereCartes[i] = nullptr;
    }
}

PlateauCentral::~PlateauCentral() {
    for (int i = 0; i < _tailleActive; ++i) {
        delete _offreJetons[i];
        _offreJetons[i] = nullptr;
        if (_riviereCartes[i] != nullptr) {
            delete _riviereCartes[i];
            _riviereCartes[i] = nullptr;
        }
    }
}

void PlateauCentral::remplirOffre(Sac& sac) {
    for (int i = 0; i < _tailleActive; ++i) {
        if (_offreJetons[i]->estVide()) {
            int nbAPiocher = (sac.getNbJetonsRestants() >= 3) ? 3 : sac.getNbJetonsRestants();
            if (nbAPiocher > 0) {
                Jeton** pioche = sac.piocher(nbAPiocher);
                delete _offreJetons[i];
                _offreJetons[i] = new GroupeJetons(pioche, nbAPiocher);
                delete[] pioche;
            }
        }
    }
}

bool PlateauCentral::prendreGroupe(int index, Jeton* result[3]) {
    if (index < 0 || index >= _tailleActive || _offreJetons[index]->estVide()) return false;
    return _offreJetons[index]->prendreJetons(result);
}

Carte* PlateauCentral::prendreCarte(int index) {
    if (index < 0 || index >= _tailleActive || _riviereCartes[index] == nullptr) return nullptr;
    Carte* choisie = _riviereCartes[index];
    _riviereCartes[index] = nullptr;
    return choisie;
}

void PlateauCentral::remplirCartes() {
    for (int i = 0; i < _tailleActive; ++i) {
        if (_riviereCartes[i] == nullptr) {
            if (!_pioche.estVide()) {
                _riviereCartes[i] = _pioche.Piocher();
            }
        }
    }
}

PiocheAnimal& PlateauCentral::getPioche() { return _pioche; }

const GroupeJetons* PlateauCentral::getOffreJetons(int index) const {
    if (index < 0 || index >= _tailleActive) return nullptr;
    return _offreJetons[index];
}

const CarteAnimal* PlateauCentral::getCarteRiviere(int index) const {
    if (index < 0 || index >= _tailleActive) return nullptr;
    return _riviereCartes[index];
}

void PlateauCentral::setGroupe(int index, CouleurJeton couleurs[], int nb) {
    if (index < 0 || index >= _tailleActive || nb <= 0) return;
    delete _offreJetons[index];
    Jeton** jetons = new Jeton*[nb];
    for (int i = 0; i < nb; ++i) jetons[i] = new Jeton(couleurs[i]);
    _offreJetons[index] = new GroupeJetons(jetons, nb);
    delete[] jetons;
}

void PlateauCentral::setCarteRiviere(int index, CarteAnimal* carte) {
    if (index < 0 || index >= _tailleActive) return;
    if (_riviereCartes[index] != nullptr) delete _riviereCartes[index];
    _riviereCartes[index] = carte;
}

void PlateauCentral::defausserCarteOffre(int index) {
    if (index >= 0 && index < _tailleActive && _riviereCartes[index] != nullptr) {
        delete _riviereCartes[index];
        _riviereCartes[index] = nullptr;
    }
}

void PlateauCentral::defausserJetons() {
    for (int i = 0; i < _tailleActive; ++i) {
        delete _offreJetons[i];
        _offreJetons[i] = new GroupeJetons();
    }
}

void PlateauCentral::setTailleActivePourChargement(int nbJoueurs) {
    _tailleActive = (nbJoueurs == 1) ? 3 : 5;
}