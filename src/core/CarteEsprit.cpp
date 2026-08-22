#include "core/CarteEsprit.h"
#include "core/DetecteurPaysage.h"

CarteEsprit::CarteEsprit(const std::string& nom, const std::string& imageRef, int nbCubes)
    : Carte(nom, imageRef, nbCubes){}


EspritGroupe::EspritGroupe(const std::string& nom, const std::string& imageRef, int nbCubes,
    CibleGroupe cible, int hauteurCible, int t0, int t1, int t2, int p0, int p1, int p2)
    : CarteEsprit(nom, imageRef, nbCubes), _cible(cible), _hauteurCible(hauteurCible) {
    _tailles[0] = t0; _tailles[1] = t1; _tailles[2] = t2;
    _points[0] = p0; _points[1] = p1; _points[2] = p2;
}

int EspritGroupe::obtenirPoints(int taille) const {
    int pts = 0;
    for (int i = 0; i < 3; ++i) {
        if (_tailles[i] > 0 && taille >= _tailles[i])
            pts = _points[i];
    }
    return pts;
}

int EspritGroupe::calculerBonus(const PlateauJoueur& p) const {
    int nbCases = p.getNbCases();
    bool* visited = new bool[nbCases];
    for (int i = 0; i < nbCases; ++i) visited[i] = false;

    int total = 0;

    for (int i = 0; i < nbCases; ++i) {
        Case* c = p.getCaseParIndex(i);
        if (c == nullptr || c->estVide() || visited[i]) continue;

        int taille = 0;

        switch (_cible) {
            case CibleGroupe::CHAMP:
                if (c->getCouleurSommet() == CouleurJeton::JAUNE)
                    taille = DetecteurPaysage::calculerTailleChamp(c, p, visited);
                break;

            case CibleGroupe::EAU:
                if (c->getCouleurSommet() == CouleurJeton::BLEU)
                    taille = DetecteurPaysage::calculerTailleRiviere(c, p, visited);
                break;

            case CibleGroupe::BATIMENT:
                if (c->getCouleurSommet() == CouleurJeton::ROUGE && c->getZ() == _hauteurCible)
                    taille = DetecteurPaysage::calculerTailleBatiment(c, p, visited, _hauteurCible);
                break;

            default:
                break;
        }

        if (taille > 0)
            total += obtenirPoints(taille);
    }

    delete[] visited;
    return total;
}

EspritQuantite::EspritQuantite(const std::string& nom, const std::string& imageRef, int nbCubes,
    CibleQuantite cible, int hauteurCible, int t0, int t1, int t2, int p0, int p1, int p2)
    : CarteEsprit(nom, imageRef, nbCubes), _cible(cible), _hauteurCible(hauteurCible) {
    _tailles[0] = t0; _tailles[1] = t1; _tailles[2] = t2;
    _points[0] = p0; _points[1] = p1; _points[2] = p2;
}

int EspritQuantite::obtenirPoints(int quantite) const {
    int pts = 0;
    for (int i = 0; i < 3; ++i) {
        if (_tailles[i] > 0 && quantite == _tailles[i])
            pts = _points[i];
    }
    return pts;
}

int EspritQuantite::calculerBonus(const PlateauJoueur& p) const {
    int nbCases = p.getNbCases();
    int total = 0;

    for (int i = 0; i < nbCases; ++i) {
        Case* c = p.getCaseParIndex(i);
        if (c == nullptr || c->estVide()) continue;

        switch (_cible) {
            case CibleQuantite::ARBRE: {
                if (c->getCouleurSommet() == CouleurJeton::VERT) {
                    int taille = DetecteurPaysage::calculerTailleArbre(c);
                    total += obtenirPoints(taille);
                }
                break;
            }
            case CibleQuantite::MONTAGNE: {
                int taille = DetecteurPaysage::calculerHauteurMontagne(c);
                total += obtenirPoints(taille);
                break;
            }
            case CibleQuantite::EAU: {
                if (c->getCouleurSommet() == CouleurJeton::BLEU)
                    total += _points[0];
                break;
            }
            default:
                break;
            }
    }

    return total;
}