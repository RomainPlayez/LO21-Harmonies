#include "core/VueConsole.h"
#include <iostream>
#include <limits>
#include <cctype>

// ==============================================================================
// HELPERS LOCAUX (Fonctions libres pour l'ASCII de Wael)
// ==============================================================================
static char obtenirInitialeCouleur(CouleurJeton couleur) {
    switch(couleur) {
        case CouleurJeton::GRIS:   return 'G';
        case CouleurJeton::ROUGE:  return 'R';
        case CouleurJeton::MARRON: return 'M';
        case CouleurJeton::VERT:   return 'V';
        case CouleurJeton::JAUNE:  return 'J';
        case CouleurJeton::BLEU:   return 'B';
        default:                   return '?';
    }
}

static void contenuCoordonnees(const Case* c, char contenu[8]) {
    if(!c) { contenu[0] = '\0'; return; }
    contenu[0] = '0' + c->getX(); contenu[1] = ','; contenu[2] = '0' + c->getY(); contenu[3] = '\0';
}

static void contenuBasCase(const Case* c, char contenu[8]) {
    if(!c) { contenu[0] = '\0'; return; }
    
    if(c->estVide()) {
        contenu[0] = ' '; contenu[1] = ' '; contenu[2] = ' '; contenu[3] = '\0';
        return;
    }
    if(c->aUnCubeAnimal()) {
        contenu[0] = 'A'; contenu[1] = ','; contenu[2] = '0'; contenu[3] = '\0';
        return;
    }
    contenu[0] = obtenirInitialeCouleur(c->getCouleurSommet());
    contenu[1] = ','; contenu[2] = '0' + c->getZ(); contenu[3] = '\0';
}

// ==============================================================================
// UTILITAIRES DE CLASSE
// ==============================================================================
void VueConsole::viderBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string VueConsole::couleurVersTexte(CouleurJeton c) {
    switch (c) {
        case CouleurJeton::GRIS:   return "GRIS";
        case CouleurJeton::ROUGE:  return "ROUGE";
        case CouleurJeton::MARRON: return "MARRON";
        case CouleurJeton::VERT:   return "VERT";
        case CouleurJeton::JAUNE:  return "JAUNE";
        case CouleurJeton::BLEU:   return "BLEU";
        default:                   return "?";
    }
}

// ==============================================================================
// CONFIGURATION INITIALE
// ==============================================================================
int VueConsole::demanderNombreJoueurs() {
    std::cout << R"(
 _   _                                  _            
| | | |                                (_)          
| |_| | __ _ _ __ _ __ ___   ___  _ __  _  ___  ___ 
|  _  |/ _` | '__| '_ ` _ \ / _ \| '_ \| |/ _ \/ __|
| | | | (_| | |  | | | | | | (_) | | | | |  __/\__ \
\_| |_/\__,_|_|  |_| |_| |_|\___/|_| |_|_|\___||___/
    )" << "\n\n";

    std::cout << "Bienvenue dans le jeu Harmonies !\n\n";

    int nbJoueurs = 0;
    while (nbJoueurs < 1 || nbJoueurs > 4) {
        std::cout << "Combien de joueurs vont participer a cette partie ? (choisir entre 1 et 4) : ";
        std::cin >> nbJoueurs;

        if (std::cin.fail()) {
            viderBuffer();
            std::cout << "Saisie invalide, veuillez entrer un chiffre.\n";
            nbJoueurs = 0;
        } else if (nbJoueurs < 1 || nbJoueurs > 4) {
            std::cout << "Veuillez entrer un nombre entre 1 et 4.\n";
        }
    }
    return nbJoueurs;
}

FacePlateau VueConsole::demanderFacePlateau() {
    int choix = 0;
    while (choix != 1 && choix != 2) {
        std::cout << "\nQuelle face du plateau souhaitez-vous utiliser ?\n";
        std::cout << "Face A : plateau a 23 cases - rivieres (choisir 1)\n";
        std::cout << "Face B : plateau a 25 cases - iles (choisir 2)\n";
        std::cout << "Votre choix : ";
        std::cin >> choix;

        if (std::cin.fail() || (choix != 1 && choix != 2)) {
            viderBuffer();
            std::cout << "Saisie invalide, veuillez entrer 1 ou 2.\n";
            choix = 0;
        }
    }
    return (choix == 1) ? FacePlateau::FACE_A : FacePlateau::FACE_B;
}

bool VueConsole::demanderUtiliserCartesEsprit() {
    int choix = 0;
    while (choix != 1 && choix != 2) {
        std::cout << "\nSouhaitez-vous utiliser les cartes Esprit durant cette partie ?\n";
        std::cout << "Oui (choisir 1)\n";
        std::cout << "Non (choisir 2)\n";
        std::cout << "Votre choix : ";
        std::cin >> choix;

        if (std::cin.fail() || (choix != 1 && choix != 2)) {
            viderBuffer();
            std::cout << "Saisie invalide, veuillez entrer 1 ou 2.\n";
            choix = 0;
        }
    }
    return (choix == 1);
}

// ==============================================================================
// AFFICHAGE
// ==============================================================================
void VueConsole::afficherPlateauCentral(ControleurPartie* controleur) {
    PlateauCentral* central = controleur->getJeu()->getPlateauCentral();
    
    int tailleActive = central->getTailleActive(); 
    
    std::cout << "\n=============================\n";
    std::cout << "       PLATEAU CENTRAL\n";
    std::cout << "=============================\n";
    
    std::cout << "--- Groupes de jetons ---\n";
    // On utilise tailleActive
    for (int i = 0; i < tailleActive; ++i) {
        const GroupeJetons* g = central->getOffreJetons(i);
        std::cout << "  [" << i << "] ";
        if (g == nullptr || g->estVide()) {
            std::cout << "(vide)";
        } else {
            for (int j = 0; j < g->getNbJetons(); ++j) {
                if (g->getJeton(j) != nullptr)
                    std::cout << "(" << obtenirInitialeCouleur(g->getJeton(j)->getCouleur()) << ") ";
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "\n--- Riviere de cartes ---\n";
    for (int i = 0; i < tailleActive; ++i) {
        const CarteAnimal* c = central->getCarteRiviere(i);
        std::cout << "  [" << i << "] ";
        if (c == nullptr) {
            std::cout << "(vide)";
        } else {
            std::cout << c->getNom() << " | Cubes : " << c->getNbCubesActuels() << "/" << c->getNbCubesMax();
        }
        std::cout << "\n";
    }
    std::cout << "=============================\n";
}

void VueConsole::afficherMainJoueur(const Joueur* joueur) {
    std::cout << "\n==================================================\n";
    std::cout << "JOUEUR : " << joueur->getNom() << " | SCORE : " << joueur->getScore() << " pts\n";
    std::cout << "==================================================\n";

    std::cout << "--- Cartes actives ---\n";
    if (joueur->getNbCartesActives() == 0) {
        std::cout << "  (aucune carte)\n";
    } else {
        for (int i = 0; i < joueur->getNbCartesActives(); ++i) {
            Carte* c = joueur->getCarteActive(i);
            if (c != nullptr) {
                std::cout << "  [" << i << "] " << c->getNom() << " (" << c->getNbCubesActuels() << " cubes restants)\n";
            }
        }
    }

    const PlateauJoueur* pj = joueur->getPlateau();
    std::cout << "\n--- Jetons en attente ---\n";
    if (pj) {
        int nbAttente = pj->getNbJetonsEnAttente();
        std::cout << nbAttente << " / 3 jetons en attente : ";
        for (int i = 0; i < 3; ++i) {
            const Jeton* j = pj->getJetonEnAttente(i);
            if (j) {
                std::cout << "[" << i << "](" << obtenirInitialeCouleur(j->getCouleur()) << ") ";
            }
        }
        std::cout << "\n";
    }
}

void VueConsole::afficherPlateauJoueur(const PlateauJoueur* pj) {
    if(!pj) return;
    std::cout << "\n=====================================\n";
    std::cout << "PLATEAU JOUEUR " << ((pj->getFace() == FacePlateau::FACE_A) ? "(FACE A)" : "(FACE B)");
    std::cout << "\n=====================================\n\n";

    int maxX = 0; int maxY = 0;
    for(int i = 0; i < pj->getNbCases(); ++i) {
        Case* c = pj->getCaseParIndex(i);
        if(c) {
            if(c->getX() > maxX) maxX = c->getX();
            if(c->getY() > maxY) maxY = c->getY();
        }
    }

    for(int y = 0; y <= maxY; ++y) {
        bool existePaire = false;
        for(int x = 0; x <= maxX; x += 2) {
            if(pj->getCase(x,y)) { existePaire = true; break; }
        }

        if(existePaire) {
            for(int x = 0; x <= maxX; x += 2) {
                if(pj->getCase(x,y)) std::cout << "   ___      ";
                else std::cout << "            ";
            }
            std::cout << "\n";
            for(int x = 0; x <= maxX; x += 2) {
                const Case* c = pj->getCase(x,y);
                if(c) {
                    char txt[8]; contenuCoordonnees(c, txt);
                    std::cout << "  /" << txt << "\\     ";
                } else std::cout << "            ";
            }
            std::cout << "\n";
            for(int x = 0; x <= maxX; x += 2) {
                const Case* c = pj->getCase(x,y);
                if(c) {
                    char txt[8]; contenuBasCase(c, txt);
                    std::cout << "  \\" << txt << "/     ";
                } else std::cout << "            ";
            }
            std::cout << "\n";
        }

        bool existeImpaire = false;
        for(int x = 1; x <= maxX; x += 2) {
            if(pj->getCase(x,y)) { existeImpaire = true; break; }
        }

        if(existeImpaire) {
            std::cout << "      ";
            for(int x = 1; x <= maxX; x += 2) {
                if(pj->getCase(x,y)) std::cout << "   ___      ";
                else std::cout << "            ";
            }
            std::cout << "\n      ";
            for(int x = 1; x <= maxX; x += 2) {
                const Case* c = pj->getCase(x,y);
                if(c) {
                    char txt[8]; contenuCoordonnees(c, txt);
                    std::cout << " /" << txt << "\\     ";
                } else std::cout << "            ";
            }
            std::cout << "\n      ";
            for(int x = 1; x <= maxX; x += 2) {
                const Case* c = pj->getCase(x,y);
                if(c) {
                    char txt[8]; contenuBasCase(c, txt);
                    std::cout << " \\" << txt << "/     ";
                } else std::cout << "           ";
            }
            std::cout << "\n";
        }
    }
    std::cout << std::endl;
}

// ==============================================================================
// LOGIQUE DE JEU INTERACTIVE
// ==============================================================================

void VueConsole::choixEspritPremierTour(ControleurPartie* controleur, Joueur* joueurActuel) {
    Jeu* jeu = controleur->getJeu();
    
    std::cout << "\n==================================================\n";
    std::cout << "  DRAFT ESPRIT (Tour 1) POUR " << joueurActuel->getNom() << "\n";
    std::cout << "==================================================\n";

    CarteEsprit* esprit1 = jeu->piocherEsprit();
    CarteEsprit* esprit2 = jeu->piocherEsprit();

    if (esprit1 == nullptr || esprit2 == nullptr) {
        std::cout << "Information : Plus d'Esprits disponibles dans la pioche.\n";
        return;
    }

    std::cout << "Choisissez votre Esprit Animal pour cette partie :\n";
    std::cout << "1. " << esprit1->getNom() << "\n";
    std::cout << "2. " << esprit2->getNom() << "\n";

    int choix = 0;
    while (true) {
        std::cout << "Votre choix (1 ou 2) : ";
        std::cin >> choix;
        if (std::cin.fail() || (choix != 1 && choix != 2)) {
            viderBuffer();
            std::cout << "Choix invalide ! Veuillez taper 1 ou 2.\n";
        } else {
            break;
        }
    }

    if (choix == 1) {
        joueurActuel->prendreCarte(esprit1);
        std::cout << "-> Vous avez choisi " << esprit1->getNom() << ".\n";
        delete esprit2; 
    } else {
        joueurActuel->prendreCarte(esprit2);
        std::cout << "-> Vous avez choisi " << esprit2->getNom() << ".\n";
        delete esprit1;
    }
}

void VueConsole::menuActionTour(ControleurPartie* controleur, Joueur* joueurActuel, bool avecEsprits) {
    if (avecEsprits && controleur->getJeu()->getTourActuel() == 1) {
        choixEspritPremierTour(controleur, joueurActuel);
    }

    bool groupePris = false;
    bool cartePrise = false;
    bool tourTermine = false;

    PlateauJoueur* pj = joueurActuel->getPlateau();

    // Boucle unifiée : le joueur reste ici tant qu'il n'a pas posé tous ses jetons.
    while (!tourTermine) {
        afficherPlateauCentral(controleur);
        afficherMainJoueur(joueurActuel);
        afficherPlateauJoueur(pj);

        std::cout << "\n=== Tour de " << joueurActuel->getNom() << " ===\n";

        // Choix 1 adaptatif : Prendre OU Placer
        if (!groupePris) {
            std::cout << "1. Prendre un groupe de jetons (Obligatoire)\n";
        } else if (pj->getNbJetonsEnAttente() > 0) {
            std::cout << "1. Placer un jeton en attente (Obligatoire)\n";
        }

        // Choix 2 adaptatif : Max 1 carte par tour
        if (!cartePrise) {
            std::cout << "2. Prendre une carte Animal (Optionnel)\n";
        } else {
            std::cout << "2. Prendre une carte Animal (Fait - Max 1 par tour)\n";
        }

        std::cout << "3. Poser un cube Animal (Optionnel)\n";

        // Option 4 apparaît uniquement quand l'action obligatoire est terminée
        if (groupePris && pj->getNbJetonsEnAttente() == 0) {
            std::cout << "4. Terminer le tour\n";
        }

        std::cout << "Votre choix : ";
        int choix = 0;
        std::cin >> choix;

        if (std::cin.fail()) {
            viderBuffer();
            std::cout << "Saisie invalide.\n";
            continue;
        }

        if (choix == 1) {
            if (!groupePris) {
                std::cout << "Quel groupe de jetons souhaitez-vous selectionner ? (0 a 4) : ";
                int index = -1;
                std::cin >> index;

                if (std::cin.fail() || index < 0 || index > 4) {
                    viderBuffer();
                    std::cout << "Saisie invalide.\n";
                    continue;
                }

                if (controleur->choisirGroupeJetons(index)) {
                    std::cout << "Groupe pris ! Vous devez maintenant placer ces jetons.\n";
                    groupePris = true;
                } else {
                    std::cout << "Impossible de prendre ce groupe (vide ou erreur).\n";
                }
            } 
            else if (pj->getNbJetonsEnAttente() > 0) {
                std::cout << "\nQuel jeton voulez-vous placer ? (Entrez son index 0, 1 ou 2) : ";
                int indexAttente = -1;
                std::cin >> indexAttente;

                if (std::cin.fail() || indexAttente < 0 || indexAttente > 2 || pj->getJetonEnAttente(indexAttente) == nullptr) {
                    viderBuffer();
                    std::cout << "Saisie invalide ou aucun jeton a cet index.\n";
                    continue;
                }

                std::cout << "Coordonnee X cible : ";
                int x; std::cin >> x;
                if (std::cin.fail()) { viderBuffer(); continue; }

                std::cout << "Coordonnee Y cible : ";
                int y; std::cin >> y;
                if (std::cin.fail()) { viderBuffer(); continue; }

                if (controleur->placerJetonDepuisAttente(x, y, indexAttente)) {
                    std::cout << "Jeton place avec succes !\n";
                } else {
                    std::cout << "Erreur : Ce placement est invalide selon les regles.\n";
                }
            }
        }
        else if (choix == 2) {
            if (cartePrise) {
                std::cout << "Vous avez deja pris une carte ce tour (limite a 1 par tour).\n";
            } else {
                std::cout << "Quelle carte de la riviere selectionner ? (0 a 4) : ";
                int index = -1;
                std::cin >> index;

                if (std::cin.fail() || index < 0 || index > 4) {
                    viderBuffer();
                    std::cout << "Saisie invalide.\n";
                    continue;
                }

                Carte* carte = controleur->getJeu()->getPlateauCentral()->prendreCarte(index);
                if (carte == nullptr) {
                    std::cout << "Aucune carte n'est disponible a cet index.\n";
                } else {
                    if (joueurActuel->prendreCarte(carte)) {
                        std::cout << "Carte \"" << carte->getNom() << "\" ajoutee a votre main !\n";
                        cartePrise = true;
                    } else {
                        std::cout << "Impossible de prendre cette carte.\n";
                    }
                }
            }
        }
        else if (choix == 3) {
            placerCubeAnimal(controleur, joueurActuel);
        }
        else if (choix == 4 && groupePris && pj->getNbJetonsEnAttente() == 0) {
            // gestion du mode solo
            if (controleur->getJeu()->getNbJoueurs() == 1) {
                int indexDefausse = -1;
                if (!cartePrise) {
                    char reponse = ' ';
                    while (true) {
                        std::cout << "\n[MODE SOLO] Vous n'avez pris aucune carte Animal ce tour-ci.\n";
                        std::cout << "Voulez-vous defausser une carte de la riviere ? (O/N) : ";
                        std::cin >> reponse;
                        reponse = std::toupper(reponse);
                        if (std::cin.fail() || (reponse != 'O' && reponse != 'N')) {
                            viderBuffer();
                            std::cout << "Saisie invalide.\n";
                            continue;
                        }
                        break;
                    }
                    if (reponse == 'O') {
                        while (true) {
                            std::cout << "Quelle carte de la riviere defausser ? (0 a " << (controleur->getJeu()->getPlateauCentral()->getTailleActive() - 1) << ") : ";
                            std::cin >> indexDefausse;
                            if (std::cin.fail() || indexDefausse < 0 || indexDefausse >= controleur->getJeu()->getPlateauCentral()->getTailleActive()) {
                                viderBuffer();
                                std::cout << "Saisie invalide.\n";
                                continue;
                            }
                            break;
                        }
                    }
                }
                // On envoie les infos au contrôleur
                controleur->gererFinTourSolo(cartePrise, indexDefausse);
            }

            if (controleur->passerTour()) {
                tourTermine = true;
            } else {
                std::cout << "Erreur du controleur pour passer le tour.\n";
            }
        }
        else {
            std::cout << "Choix invalide (ou action non disponible actuellement).\n";
        }
    }
}

void VueConsole::placerCubeAnimal(ControleurPartie* controleur, Joueur* joueurActuel) {
    if (joueurActuel->getNbCartesActives() == 0) {
        std::cout << "Vous n'avez aucune carte active.\n";
        return;
    }

    std::cout << "\nQuel animal souhaitez-vous placer ? (Entrez l'index de la carte) : ";
    int indexCarte;
    std::cin >> indexCarte;
    if (std::cin.fail() || indexCarte < 0 || indexCarte >= joueurActuel->getNbCartesActives()) {
        viderBuffer();
        std::cout << "Erreur : Index invalide.\n";
        return;
    }

    std::cout << "Coordonnee X de la case centrale du motif : ";
    int x; std::cin >> x;
    if (std::cin.fail()) { viderBuffer(); std::cout << "Saisie invalide.\n"; return; }

    std::cout << "Coordonnee Y de la case centrale du motif : ";
    int y; std::cin >> y;
    if (std::cin.fail()) { viderBuffer(); std::cout << "Saisie invalide.\n"; return; }

    if (controleur->placerCubeAnimal(indexCarte, x, y)) {
        std::cout << "Felicitation : l'animal s'est installe sur votre plateau !\n";
    } else {
        std::cout << "Placement invalide : Le motif geometrique n'est pas respecte a ces coordonnees.\n";
    }
}

bool VueConsole::proposerReprise() {
    std::cout << "\nUne partie sauvegardee a ete trouvee.\n";
    int choix = 0;
    while (choix != 1 && choix != 2) {
        std::cout << "Souhaitez-vous la reprendre ?\n";
        std::cout << "Oui (choisir 1)\n";
        std::cout << "Non, nouvelle partie (choisir 2)\n";
        std::cout << "Votre choix : ";
        std::cin >> choix;
        if (std::cin.fail()) {
            viderBuffer();
            std::cout << "Saisie invalide.\n";
            choix = 0;
        } else if (choix != 1 && choix != 2) {
            std::cout << "Entrez 1 ou 2.\n";
        }
    }
    return (choix == 1);
}