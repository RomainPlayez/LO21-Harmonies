#include "core/ControleurPartie.h"
#include "core/Carte.h"
#include "core/Case.h"
#include "core/PlateauJoueur.h"

ControleurPartie::ControleurPartie(const std::vector<std::string>& nomsJoueurs, FacePlateau face, bool avecEsprits)
    : _jeu(nullptr), _etatActuel(EtatTour::DEBUT_TOUR) {
    // Le contrôleur instancie le Jeu, qui lui-même instancie ses joueurs et plateaux
    if (_jeu != nullptr) {
        delete _jeu;
    }
    _jeu = new Jeu(nomsJoueurs, face, avecEsprits);
}

ControleurPartie::~ControleurPartie() {
    // La destruction de _jeu cascade proprement vers les joueurs puis vers les plateaux.
    delete _jeu;
}

void ControleurPartie::demarrerPartie() {
    _jeu->initialiserPartie();
    _etatActuel = EtatTour::DEBUT_TOUR;
}

Joueur* ControleurPartie::getJoueurCourant() const {
    // On récupère directement le joueur concerné depuis l'état du Jeu
    return _jeu->getJoueurActif();
}

bool ControleurPartie::choisirGroupeJetons(int indexGroupe) {
    if (_etatActuel != EtatTour::DEBUT_TOUR) return false;

    Jeton* groupePris[3] = {nullptr, nullptr, nullptr};
    PlateauCentral* central = _jeu->getPlateauCentral();
    
    // On récupère le plateau du joueur courant en passant par l'objet Joueur 
    PlateauJoueur* pActif = getJoueurCourant()->getPlateau();

    if (central->prendreGroupe(indexGroupe, groupePris)) {
        for (int i = 0; i < 3; ++i) {
            if (groupePris[i] != nullptr) {
                pActif->stockerJetonEnAttente(groupePris[i]);
            }
        }
        _etatActuel = EtatTour::GROUPE_CHOISI;
        return true;
    }
    return false;
}

bool ControleurPartie::placerJetonDepuisAttente(int x, int y, int indexAttente) {
    if (_etatActuel != EtatTour::GROUPE_CHOISI && _etatActuel != EtatTour::CARTE_CHOISIE) {
        return false;
    }

    // Accès au plateau via le joueur actif
    PlateauJoueur* pActif = getJoueurCourant()->getPlateau();
    Jeton* j = pActif->utiliserJetonEnAttente(indexAttente);
    
    if (j != nullptr) {
        Case* c = pActif->getCase(x, y);
        
        if (c != nullptr) {
            // L'arbitre externe valide le coup
            if (Validateur::estPlacementValide(*c, *j)) {
                if (c->placerJeton(j)) {
                    if (pActif->getNbJetonsEnAttente() == 0) {
                        _etatActuel = EtatTour::JETONS_PLACES;
                    }
                    return true;
                }
            }
        }
        // Sécurité : On restocke le jeton si le placement échoue
        pActif->stockerJetonEnAttente(j); 
    }
    return false;
}

bool ControleurPartie::passerTour() {
    if (_etatActuel != EtatTour::JETONS_PLACES) {
        return false; 
    }

    if (_jeu->estFini()) {
        return false; 
    }

    // On délègue la progression logistique au modèle de jeu
    _jeu->passerAuTourSuivant();

    // On réinitialise l'état du tour pour le joueur suivant
    _etatActuel = EtatTour::DEBUT_TOUR;
    return true;
}

bool ControleurPartie::placerCubeAnimal(int indexCarte, int x, int y) {
    Joueur* j = this->getJoueurCourant();
    if (j == nullptr) return false;

    // Récupération de la carte active
    Carte* c = j->getCarteActive(indexCarte);
    if (c == nullptr) return false;

    // L'arbitre valide si le motif géométrique de la carte est présent sur le plateau
    if (Validateur::validerMotif(*(j->getPlateau()), *(c->getMotif()), x, y)) {
        
        CubeAnimal* cube = c->prendreCubeBas();
        
        if (cube != nullptr) {
            
            if (dynamic_cast<CarteEsprit*>(c) != nullptr) {
                cube->setEstEsprit(true);
            }

            // Placement physique du cube sur la case du plateau joueur
            j->getPlateau()->getCase(x, y)->placerCube(cube);
            
            // Si tous les cubes de la carte ont été placés, on l'archive
            if (c->estComplete()) {
                j->retirerCarte(indexCarte);
            }
            return true;
        }
    }
    return false;
}

EtatTour ControleurPartie::getEtatActuel() const { return _etatActuel; }
Jeu* ControleurPartie::getJeu() const { return _jeu; }

void ControleurPartie::sauvegarderPartie(const std::string& cheminFichier) const {
    EtatPartie etat = _jeu->sauvegarder();
    etat.etatTour = static_cast<int>(_etatActuel);
    // On utilise le chemin fourni par Qt
    std::ofstream f(cheminFichier);
    if (f.is_open()) {
        etat.ecrire(f);
        f.close();
    }
}

bool ControleurPartie::chargerPartie(const std::string& cheminFichier) {
    std::ifstream f(cheminFichier);
    if (!f.is_open()) return false;
    EtatPartie etat = EtatPartie::lire(f);
    f.close();
    _jeu->restaurer(etat);
    _etatActuel = static_cast<EtatTour>(etat.etatTour);
    return true;
}

bool ControleurPartie::sauvegardeDisponible(const std::string& cheminFichier) {
    std::ifstream f(cheminFichier);
    return f.good();
}

void ControleurPartie::gererFinTourSolo(bool cartePrise, int indexCarteDefausse) {
    if (_jeu->getNbJoueurs() != 1) return; // Sécurité

    PlateauCentral* pc = _jeu->getPlateauCentral();

    // On défausse tous les jetons restants
    pc->defausserJetons();

    // Si aucune carte prise, on peut en défausser une
    if (!cartePrise && indexCarteDefausse >= 0) {
        pc->defausserCarteOffre(indexCarteDefausse);
    }
}