#include "core/Validateur.h"
#include <iostream>

bool Validateur::estPlacementValide(const Case& c, const Jeton& j) {
    int hauteur = c.getZ();
    CouleurJeton couleurAJouer = j.getCouleur();

    // Règle 1 : Hauteur max = 3 (donc index 0, 1, 2 max)
    if (hauteur >= 3 || c.aUnCubeAnimal()) {
        return false;
    }

    // Règle 2 : Le sol (Niveau 1) accepte n'importe quel jeton
    if (c.estVide()) {
        return true; 
    }

    CouleurJeton sommet = c.getCouleurSommet();

    // Règle 3 : Placement au Niveau 2 (hauteur actuelle == 1)
    if (hauteur == 1) {
        switch (sommet) {
            case CouleurJeton::BLEU:
            case CouleurJeton::JAUNE:
            case CouleurJeton::VERT:
                return false; // Rien ne pousse sur l'eau, les champs ou les arbres
            
            case CouleurJeton::ROUGE:
                // Sur un bâtiment, on ne peut mettre qu'un autre bâtiment
                return couleurAJouer == CouleurJeton::ROUGE;
            
            case CouleurJeton::GRIS:
                // Sur une montagne (niv 1), on peut mettre Montagne ou Bâtiment
                return couleurAJouer == CouleurJeton::GRIS || couleurAJouer == CouleurJeton::ROUGE;
            
            case CouleurJeton::MARRON:
                // Sur du bois (niv 1), on peut mettre Bois, Arbre ou Bâtiment
                return couleurAJouer == CouleurJeton::MARRON || 
                       couleurAJouer == CouleurJeton::VERT || 
                       couleurAJouer == CouleurJeton::ROUGE;
                       
            default:
                return false;
        }
    }

    // Règle 4 : Placement au Niveau 3 (hauteur actuelle == 2)
    if (hauteur == 2) {
        // Seules deux combinaisons sont possibles au sommet
        if (sommet == CouleurJeton::GRIS && couleurAJouer == CouleurJeton::GRIS) return true;
        if (sommet == CouleurJeton::MARRON && couleurAJouer == CouleurJeton::VERT) return true;
        
        return false; // Tout le reste est interdit 
    }

    return false; // Par sécurité
}

bool Validateur::validerMotif(const PlateauJoueur& plateau, const Motif& motif, int xCentre, int yCentre) {
    int taille = motif.getTaille();
    bool trouve = false;

    int qCentre = xCentre;
    int rCentre = yCentre - (xCentre / 2);

    // On teste les 6 rotations (de 0 à 5)
    for (int rotation = 0; rotation < 6 && !trouve; ++rotation) {
        bool match = true;

        for (int i = 0; i < taille && match; ++i) {
            const ElementMotif& elem = motif.getElement(i);

            // Coordonnées d'origine du motif (en Axial)
            int dq = elem.dx;
            int dr = elem.dy;

            // Formule de rotation 60° dans le sens horaire
            for (int r = 0; r < rotation; ++r) {
                int temp_dq = dq;
                dq = -dr;
                dr = temp_dq + dr;
            }

            // Position absolue en géométrie Axiale
            int qAbs = qCentre + dq;
            int rAbs = rCentre + dr;

            // Reconvertir la case testée en coordonnées
            int xCheck = qAbs;
            
            // On sécurise le décalage pour éviter qu'un test hors-plateau négatif ne fasse crasher le calcul
            int offsetQ = (qAbs < 0) ? (qAbs - 1) / 2 : qAbs / 2;
            int yCheck = rAbs + offsetQ;

            // Vérification physique
            Case* c = plateau.getCase(xCheck, yCheck);

            if (c == nullptr || c->estVide()) {
                match = false;
            } else {
                if (c->getCouleurSommet() != elem.couleur || c->getZ() < elem.hauteurMinimale) {
                    match = false;
                }
            }
        }

        if (match) {
            trouve = true; // Trouvé dans cette orientation !
        }
    }

    return trouve;
}