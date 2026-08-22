#include "core/RegleDeScore.h"
#include "core/PlateauJoueur.h"
#include "core/DetecteurPaysage.h"
#include "core/Carte.h"
#include "core/CarteAnimal.h" 
#include "core/CarteEsprit.h"
#include <iostream>

// SCORE ARBRE
void ScoreArbre::calculerScore(Joueur* j) const {
    if (j == nullptr) return;
    PlateauJoueur& p = *(j->getPlateau());
    int pointsGagnes = 0;

    // On parcourt toutes les cases du plateau
    for (int i = 0; i < p.getNbCases(); ++i) {
        Case* c = p.getCaseParIndex(i);
        if (c != nullptr && !c->estVide() && c->getCouleurSommet() == CouleurJeton::VERT) {
            int hauteur = DetecteurPaysage::calculerTailleArbre(c);
            if (hauteur == 3) pointsGagnes += 5;
            else if (hauteur == 2) pointsGagnes += 3;
            else if (hauteur == 1) pointsGagnes += 1;
        }
    }
    j->ajouterScoreDetail(0, pointsGagnes);
}

// SCORE BÂTIMENT
void ScoreBatiment::calculerScore(Joueur* j) const {
    if (j == nullptr) return;
    PlateauJoueur& p = *(j->getPlateau());
    int pointsGagnes = 0;

    for (int i = 0; i < p.getNbCases(); ++i) {
        Case* c = p.getCaseParIndex(i);
        if (c != nullptr && !c->estVide() && c->getCouleurSommet() == CouleurJeton::ROUGE) {
            int nbCouleurs = DetecteurPaysage::compterCouleursBatiment(c, p);
            // Si le bâtiment a au moins 3 couleurs différentes autour de lui
            if (nbCouleurs >= 3) {
                pointsGagnes += 5;
            }
        }
    }
    j->ajouterScoreDetail(1, pointsGagnes);
}

// SCORE CHAMP
void ScoreChamp::calculerScore(Joueur* j) const {
    if (j == nullptr) return;
    PlateauJoueur& p = *(j->getPlateau());
    int pointsGagnes = 0;

    // Allocation d'un tableau pour mémoriser les champs déjà comptés
    int nbCases = p.getNbCases();
    bool* casesVisitees = new bool[nbCases];
    for (int i = 0; i < nbCases; ++i) casesVisitees[i] = false;

    for (int i = 0; i < nbCases; ++i) {
        Case* c = p.getCaseParIndex(i);
        if (c != nullptr && !c->estVide() && c->getCouleurSommet() == CouleurJeton::JAUNE && !casesVisitees[i]) {
            // L'algorithme va marquer toutes les cases de ce champ comme visitées
            int taille = DetecteurPaysage::calculerTailleChamp(c, p, casesVisitees);
            
            // Un champ doit faire au moins 2 cases pour rapporter des points
            if (taille >= 2) {
                pointsGagnes += 5; 
            }
        }
    }
    delete[] casesVisitees; // Libération de la mémoire
    j->ajouterScoreDetail(2, pointsGagnes);
}

// SCORE MONTAGNE
void ScoreMontagne::calculerScore(Joueur* j) const {
    if (j == nullptr) return;
    PlateauJoueur& p = *(j->getPlateau());
    int pointsGagnes = 0;

    // Vérification de si la montagne à au moins un voisin.
    for (int i = 0; i < p.getNbCases(); ++i) {
        Case* c = p.getCaseParIndex(i);
        if (c != nullptr && !c->estVide() && c->getCouleurSommet() == CouleurJeton::GRIS) {
            
            Case* voisins[6];
            p.getVoisins(c->getX(), c->getY(), voisins);
            bool aUnVoisinMontagne = false;
            
            for (int v = 0; v < 6; ++v) {
                if (voisins[v] != nullptr && !voisins[v]->estVide() && voisins[v]->getCouleurSommet() == CouleurJeton::GRIS) {
                    aUnVoisinMontagne = true;
                    break;
                }
            }

            // Si elle fait partie d'une chaîne, elle score selon sa hauteur
            if (aUnVoisinMontagne) {
                int hauteur = c->getZ();
                if (hauteur == 3) pointsGagnes += 5;
                else if (hauteur == 2) pointsGagnes += 3;
                else if (hauteur == 1) pointsGagnes += 1;
            }
        }
    }
    j->ajouterScoreDetail(3, pointsGagnes);
}

// SCORE EAU (RIVIÈRES ET ÎLES)
void ScoreEau::calculerScore(Joueur* j) const {
    if (j == nullptr) return;
    PlateauJoueur& p = *(j->getPlateau());
    int pointsGagnes = 0;

    if (p.getFace() == FacePlateau::FACE_A) {
        // --- REGLE FACE A : La plus longue rivière ---
        int taille = DetecteurPaysage::calculerPlusLongueRiviere(p);
        
        if (taille == 1) pointsGagnes += 0;
        else if (taille == 2) pointsGagnes += 2;
        else if (taille == 3) pointsGagnes += 5;
        else if (taille == 4) pointsGagnes += 8;
        else if (taille == 5) pointsGagnes += 11;
        else if (taille >= 6) {
            pointsGagnes += 15 + ((taille - 6) * 4); // +4 pts pour chaque jeton au-delà de 6
        }

    } else if (p.getFace() == FacePlateau::FACE_B) {
        // --- REGLE FACE B : Les Îles ---
        int nbIles = DetecteurPaysage::calculerNombreIles(p);
        pointsGagnes += (nbIles * 5); // 5 points par île
    }

    j->ajouterScoreDetail(4, pointsGagnes);
}

// SCORE ANIMAUX
void ScoreAnimal::calculerScore(Joueur* j) const{
    if (j == nullptr) return;

    int pointsAnimaux = 0;

    // On vérifie les cartes actives (cubes partiellement placés)
    for (int i = 0; i < j->getNbCartesActives(); ++i) {
        Carte* c = j->getCarteActive(i);
        CarteAnimal* cA = dynamic_cast<CarteAnimal*>(c);
        
        if (cA != nullptr) {
            int cubesPlaces = cA->getNbCubesMax() - cA->getNbCubesActuels();
            if (cubesPlaces > 0) {
                // L'index du tableau commence à 0, donc on fait (cubesPlaces - 1)
                pointsAnimaux += cA->getPointsEmplacement(cubesPlaces - 1);
            }
        }
    }

    // On vérifie les cartes complétées (tous les cubes sont placés)
    for (int i = 0; i < j->getNbCartesCompletees(); ++i) {
        Carte* c = j->getCarteCompletee(i);
        CarteAnimal* cA = dynamic_cast<CarteAnimal*>(c);
        
        if (cA != nullptr) {
            // S'il est complété, on prend directement le max de points
            pointsAnimaux += cA->getPointsEmplacement(cA->getNbCubesMax() - 1);
        }
    }

    j->ajouterScoreDetail(5, pointsAnimaux);
}

void ScoreEsprit::calculerScore(Joueur* j) const {
    if (j == nullptr) return;

    int pointsEsprits = 0;

    // On cherche les Esprits UNIQUEMENT dans les cartes complétées !
    for (int i = 0; i < j->getNbCartesCompletees(); ++i) {
        Carte* c = j->getCarteCompletee(i);
        
        // On vérifie si cette carte complétée est un Esprit
        CarteEsprit* cE = dynamic_cast<CarteEsprit*>(c);
        
        if (cE != nullptr) {
            int bonus = cE->calculerBonus(*(j->getPlateau()));
            pointsEsprits += bonus;
            
            // POUR LES TESTS :
            std::cout << "  -> L'Esprit '" << cE->getNom() << "' s'active et rapporte " << bonus << " points !" << std::endl;
        }
    }

    j->ajouterScoreDetail(6, pointsEsprits);
}