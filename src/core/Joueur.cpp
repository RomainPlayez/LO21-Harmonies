#include "core/Joueur.h"
#include "core/Validateur.h"
#include <cstdbool>


Joueur::Joueur(const std::string& nom, FacePlateau face) : _nom(nom), _score(0), _nbCartesActives(0), _nbCartesCompletees(0) , _soleils(0), _cartesCompletees(nullptr) {
    _plateau = new PlateauJoueur(face);

    _cartesActives    = new Carte*[MAX_CARTES_ACTIVES];

    for (int i = 0; i < MAX_CARTES_ACTIVES;    ++i) _cartesActives[i]    = nullptr;

    for (int i = 0; i < 7; ++i) _detailsScore[i] = 0;
}

Joueur::~Joueur() {
    delete _plateau;

    for (int i = 0; i < _nbCartesActives;    ++i) delete _cartesActives[i];
    for (int i = 0; i < _nbCartesCompletees; ++i) delete _cartesCompletees[i];

    delete[] _cartesActives;
    delete[] _cartesCompletees;
}

const std::string& Joueur::getNom() const { return _nom; }
int Joueur::getScore() const { return _score; }
PlateauJoueur* Joueur::getPlateau() const { return _plateau; }
int Joueur::getNbCartesActives() const { return _nbCartesActives; }
int Joueur::getNbCartesCompletees() const { return _nbCartesCompletees; }

//soleils
int Joueur::getSoleils() const { return _soleils;}
void Joueur::setSoleils(int soleils) {_soleils = soleils; }

Carte* Joueur::getCarteActive(int index) const {
    if (index < 0 || index >= _nbCartesActives) return nullptr;
    return _cartesActives[index];
}

bool Joueur::prendreCarte(Carte* c) {
    if (_nbCartesActives >= MAX_CARTES_ACTIVES || c == nullptr) return false;
    for (int i = 0; i < _nbCartesActives; i++) {
        //comme retirer carte change la valeur contenue par le tableau en nullptr, on vérifie 
        //qu'il n'y a pas d'emplacement libre avant la fin du tableau
        if (_cartesActives[i] == nullptr) {
            _cartesActives[i] = c;
            return true;
        }
    }
    _cartesActives[_nbCartesActives++] = c;
    return true;
}

bool Joueur::placerJeton(Jeton* j, Case* c) {
    if (j == nullptr || c == nullptr) return false;
    if (!Validateur::estPlacementValide(*c, *j)) return false;
    return c->placerJeton(j);
}

void Joueur::ajouterScore(int points) {
    _score += points;
}

bool Joueur::finDeTour() const {
    return _plateau->getNbJetonsEnAttente() == 0;
}

void Joueur::gererCartesCompletees(Carte* c) {
    if (_nbCartesCompletees == MAX_CARTES_COMPLETEES) {
        Carte** newtab = new Carte * [2 * MAX_CARTES_COMPLETEES + 1];
        for (size_t i = 0; i < _nbCartesCompletees; i++) newtab[i] = _cartesCompletees[i];
        auto old = _cartesCompletees;
        _cartesCompletees = newtab;
        delete[] old;
    }
    _cartesCompletees[_nbCartesCompletees++] = c;
}

bool Joueur::retirerCarte(int index) {
    Carte* c = this->getCarteActive(index);
    if (c->estComplete()) {
        _cartesActives[index] = nullptr;
        gererCartesCompletees(c);
        return true;
    }
    return false;
}

Carte* Joueur::getCarteCompletee(int index) const {
    if (index < 0 || index >= _nbCartesCompletees) return nullptr;
    return _cartesCompletees[index];
}

void Joueur::setNom(const std::string& nom) {
    _nom = nom;
}

void Joueur::ajouterScoreDetail(int indexCategorie, int points) {
    if (indexCategorie >= 0 && indexCategorie < 7) {
        _detailsScore[indexCategorie] += points;
    }
    ajouterScore(points); 
}

int Joueur::getScoreDetail(int indexCategorie) const {
    if (indexCategorie >= 0 && indexCategorie < 7) {
        return _detailsScore[indexCategorie];
    }
    return 0;
}