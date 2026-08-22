#include "core/Jeton.h"

Jeton::Jeton(CouleurJeton c, bool enJeu) : _couleur(c), _estEnJeu(enJeu) {
}

CouleurJeton Jeton::getCouleur() const {
    return _couleur;
}

bool Jeton::estEnJeu() const {
    return _estEnJeu;
}

void Jeton::setEnJeu(bool etat) {
    _estEnJeu = etat;
}