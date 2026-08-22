#include "core/DetecteurPaysage.h"

// ARBRES
int DetecteurPaysage::calculerTailleArbre(const Case* c) {
    if (c == nullptr || c->estVide()) return 0;
    if (c->getCouleurSommet() != CouleurJeton::VERT) return 0;
    
    return c->getZ(); // Z contient la hauteur (1, 2 ou 3)
}

// CHAMPS (JAUNE)
int DetecteurPaysage::calculerTailleChamp(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees) {
    if (depart == nullptr || depart->estVide()) return 0;

    int idx = plateau.getIndexCase(depart->getX(), depart->getY());
    if (idx == -1 || casesVisitees[idx]) return 0; // Sécurité anti-boucle infinie

    // On marque la case comme visitée
    casesVisitees[idx] = true;
    int taille = 1;

    // Utilisation du tableau statique
    Case* voisins[6];
    plateau.getVoisins(depart->getX(), depart->getY(), voisins);

    for (int i = 0; i < 6; ++i) {
        if (voisins[i] != nullptr && !voisins[i]->estVide()) {
            if (voisins[i]->getCouleurSommet() == CouleurJeton::JAUNE) {
                // Appel récursif pour propager la détection
                taille += calculerTailleChamp(voisins[i], plateau, casesVisitees);
            }
        }
    }
    return taille;
}

// MONTAGNES (GRIS)
int DetecteurPaysage::calculerTailleMontagne(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees) {
    if (depart == nullptr || depart->estVide()) return 0;

    int idx = plateau.getIndexCase(depart->getX(), depart->getY());
    if (idx == -1 || casesVisitees[idx]) return 0;

    casesVisitees[idx] = true;
    int taille = 1;

    Case* voisins[6];
    plateau.getVoisins(depart->getX(), depart->getY(), voisins);

    for (int i = 0; i < 6; ++i) {
        if (voisins[i] != nullptr && !voisins[i]->estVide()) {
            if (voisins[i]->getCouleurSommet() == CouleurJeton::GRIS) {
                taille += calculerTailleMontagne(voisins[i], plateau, casesVisitees);
            }
        }
    }
    return taille;
}

// ALGORITHMES POUR L'EAU (RIVIERE & ILES)

// --- FACE A : Le plus court chemin entre les deux extrémités de la rivière ---
int DetecteurPaysage::calculerPlusLongueRiviere(const PlateauJoueur& plateau) {
    int nbCases = plateau.getNbCases();
    int recordRiviere = 0;

    // On va tester chaque case bleue comme "point de départ" de la rivière
    for (int i = 0; i < nbCases; ++i) {
        Case* start = plateau.getCaseParIndex(i);
        if (start == nullptr || start->estVide() || start->getCouleurSommet() != CouleurJeton::BLEU) {
            continue;
        }

        // Tableau des distances (le "plus court chemin" pour atteindre chaque case)
        int* distances = new int[nbCases];
        for (int d = 0; d < nbCases; ++d) distances[d] = 9999; // 9999 = inaccessible
        
        distances[i] = 1; // La case de départ compte pour 1

        // Algorithme de propagation du plus court chemin
        bool changement = true;
        while (changement) {
            changement = false;
            
            for (int c = 0; c < nbCases; ++c) {
                if (distances[c] == 9999) continue; // Si on n'a pas encore atteint cette case, on l'ignore
                
                Case* courant = plateau.getCaseParIndex(c);
                Case* voisins[6];
                plateau.getVoisins(courant->getX(), courant->getY(), voisins);

                for (int v = 0; v < 6; ++v) {
                    if (voisins[v] != nullptr && !voisins[v]->estVide() && voisins[v]->getCouleurSommet() == CouleurJeton::BLEU) {
                        int vIdx = plateau.getIndexCase(voisins[v]->getX(), voisins[v]->getY());
                        
                        // Si passer par "courant" offre un chemin plus court vers le voisin "v"
                        if (distances[c] + 1 < distances[vIdx]) {
                            distances[vIdx] = distances[c] + 1;
                            changement = true; // On a trouvé un raccourci, on continue de propager
                        }
                    }
                }
            }
        }

        // On regarde quelle est la case bleue la plus lointaine atteinte avec ce plus court chemin
        for (int d = 0; d < nbCases; ++d) {
            if (distances[d] != 9999 && distances[d] > recordRiviere) {
                recordRiviere = distances[d];
            }
        }

        delete[] distances; 
    }

    return recordRiviere;
}

// --- FACE B : Le nombre d'îles ---

void DetecteurPaysage::dfsIle(const Case* c, const PlateauJoueur& plateau, bool* casesVisitees) {
    int idx = plateau.getIndexCase(c->getX(), c->getY());
    casesVisitees[idx] = true;

    Case* voisins[6];
    plateau.getVoisins(c->getX(), c->getY(), voisins);

    for (int i = 0; i < 6; ++i) {
        if (voisins[i] != nullptr) {
            int vIdx = plateau.getIndexCase(voisins[i]->getX(), voisins[i]->getY());
            if (!casesVisitees[vIdx]) {
                // Si la case est vide OU si elle a un jeton NON BLEU, elle fait partie de l'île
                if (voisins[i]->estVide() || voisins[i]->getCouleurSommet() != CouleurJeton::BLEU) {
                    dfsIle(voisins[i], plateau, casesVisitees);
                }
            }
        }
    }
}

int DetecteurPaysage::calculerNombreIles(const PlateauJoueur& plateau) {
    int nbCases = plateau.getNbCases();
    bool* casesVisitees = new bool[nbCases];
    for (int i = 0; i < nbCases; ++i) casesVisitees[i] = false;

    int nbIles = 0;

    for (int i = 0; i < nbCases; ++i) {
        Case* c = plateau.getCaseParIndex(i);
        if (c != nullptr && !casesVisitees[i]) {
            // On cherche la première case non parcourue qui N'EST PAS bleue
            if (c->estVide() || c->getCouleurSommet() != CouleurJeton::BLEU) {
                // On a trouvé un bout de terre ! On propage la détection à toute l'île.
                dfsIle(c, plateau, casesVisitees);
                nbIles++; // L'algorithme a fini de peindre cette île, on compte +1.
            }
        }
    }

    delete[] casesVisitees;
    return nbIles;
}

// BÂTIMENTS (ROUGE)
int DetecteurPaysage::compterCouleursBatiment(const Case* c, const PlateauJoueur& plateau) {
    if (c == nullptr || c->estVide() || c->getCouleurSommet() != CouleurJeton::ROUGE) return 0;

    Case* voisins[6];
    plateau.getVoisins(c->getX(), c->getY(), voisins);

    // Tableau pour mémoriser les couleurs trouvées (indexées par la valeur de l'enum)
    bool couleursVues[6] = {false, false, false, false, false, false};
    int nbCouleurs = 0;

    for (int i = 0; i < 6; ++i) {
        if (voisins[i] != nullptr && !voisins[i]->estVide()) {
            int indexCouleur = static_cast<int>(voisins[i]->getCouleurSommet());
            if (!couleursVues[indexCouleur]) {
                couleursVues[indexCouleur] = true;
                nbCouleurs++;
            }
        }
    }
    return nbCouleurs;
}

int DetecteurPaysage::calculerTailleRiviere(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees) {
    if (depart == nullptr || depart->estVide()) return 0;
    if (depart->getCouleurSommet() != CouleurJeton::BLEU) return 0;
    int idx = plateau.getIndexCase(depart->getX(), depart->getY());
    if (idx == -1 || casesVisitees[idx]) return 0;
    casesVisitees[idx] = true;
    int taille = 1;
    Case* voisins[6];
    plateau.getVoisins(depart->getX(), depart->getY(), voisins);
    for (int i = 0; i < 6; ++i) {
        if (voisins[i] != nullptr && !voisins[i]->estVide()
            && voisins[i]->getCouleurSommet() == CouleurJeton::BLEU)
            taille += calculerTailleRiviere(voisins[i], plateau, casesVisitees);
    }
    return taille;
}



int DetecteurPaysage::calculerTailleBatiment(const Case* depart, const PlateauJoueur& plateau, bool* casesVisitees, int hauteurCible) {
    if (depart == nullptr || depart->estVide()) return 0;
    if (depart->getCouleurSommet() != CouleurJeton::ROUGE) return 0;
    if (depart->getZ() != hauteurCible) return 0;  
    int idx = plateau.getIndexCase(depart->getX(), depart->getY());
    if (idx == -1 || casesVisitees[idx]) return 0;
    casesVisitees[idx] = true;
    int taille = 1;
    Case* voisins[6];
    plateau.getVoisins(depart->getX(), depart->getY(), voisins);
    for (int i = 0; i < 6; ++i) {
        if (voisins[i] != nullptr && !voisins[i]->estVide()
            && voisins[i]->getCouleurSommet() == CouleurJeton::ROUGE
            && voisins[i]->getZ() == hauteurCible)  
            taille += calculerTailleBatiment(voisins[i], plateau, casesVisitees, hauteurCible);
    }
    return taille;
}



// MONTAGNES
int DetecteurPaysage::calculerHauteurMontagne(const Case* c) {
    if (c == nullptr || c->estVide()) return 0;
    if (c->getCouleurSommet() != CouleurJeton::GRIS) return 0;
    return c->getZ(); // Z contient la hauteur (1, 2 ou 3)
} 

