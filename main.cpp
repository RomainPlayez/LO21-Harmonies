#include <QApplication>
#include <iostream>
#include <string>
#include "mainwindow.h"
#include "core/Jeu.h"
#include "core/ControleurPartie.h"
#include "core/VueConsole.h"


int main(int argc, char *argv[]) {
    std::cout << "=======================================" << std::endl;
    std::cout << "       BIENVENUE DANS HARMONIES        " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Choisissez le mode d'affichage :" << std::endl;
    std::cout << "1. Interface Graphique (Qt)" << std::endl;
    std::cout << "2. Mode Console (Texte)" << std::endl;
    std::cout << "Votre choix (1 ou 2) : ";

    std::string choixStr;
    std::getline(std::cin, choixStr);
    
    int choix = 1; // Graphique par défaut
    try {
        choix = std::stoi(choixStr);
    } catch (...) {
        // Si le joueur entre n'importe quoi, on garde 1
    }

    if (choix == 2) {
        // LANCEMENT DU MODE CONSOLE
        std::cout << "\n[INFO] Lancement du mode console...\n" << std::endl;
        
        bool partieChargee = false;
        std::string cheminSave = "harmonies_save.txt"; 

        // Vérification d'une sauvegarde existante 
        if (ControleurPartie::sauvegardeDisponible(cheminSave)) {
            if (VueConsole::proposerReprise()) {
                partieChargee = true;
            }
        }

        ControleurPartie* controleur = nullptr;
        bool avecCartesEsprit = false; 

        if (partieChargee) {
            std::vector<std::string> nomsDefaut = {"Joueur 1"};
            controleur = new ControleurPartie(nomsDefaut, FacePlateau::FACE_A, false);
            
            if (controleur->chargerPartie(cheminSave)) { 
                std::cout << "-> Partie restauree avec succes !" << std::endl;
            } else {
                std::cout << "-> Erreur de restauration, lancement d'une nouvelle partie." << std::endl;
                partieChargee = false;
            }
        } 
        
        if (!partieChargee) {
            // Nouvelle partie classique
            int nbJoueurs = VueConsole::demanderNombreJoueurs();
            FacePlateau face = VueConsole::demanderFacePlateau();
            avecCartesEsprit = VueConsole::demanderUtiliserCartesEsprit();

            // On génère les noms par défaut pour le constructeur
            std::vector<std::string> nomsJoueurs;
            for (int i = 0; i < nbJoueurs; ++i) {
                nomsJoueurs.push_back("Joueur " + std::to_string(i + 1));
            }

            // On utilise le bon constructeur à 3 paramètres
            controleur = new ControleurPartie(nomsJoueurs, face, avecCartesEsprit);
            std::cout << "-> Demarrage de la partie..." << std::endl;
            controleur->demarrerPartie();
        }

        // Boucle principale de jeu
        bool finDetectee = false;
        
        while (!finDetectee) {
            Joueur* joueur = controleur->getJoueurCourant();

            // Lance le menu complet d'un tour
            VueConsole::menuActionTour(controleur, joueur, avecCartesEsprit);

            // AUTO-SAVE à la fin de l'action du joueur
            controleur->sauvegarderPartie(cheminSave);
            
            if (controleur->getJeu()->estFini()) {
                finDetectee = true;
                break; 
            }

            // Fin du tour normal, on passe au joueur suivant
            controleur->passerTour();
        }

        // Fin de partie et Scoring
        std::cout << "\n==================================================" << std::endl;
        std::cout << "          FIN DE LA PARTIE - SCORING              " << std::endl;
        std::cout << "==================================================" << std::endl;
        
        controleur->getJeu()->declencherScoringFinal();

        delete controleur;
        return 0;
    }
    else {
        // LANCEMENT DU MODE GRAPHIQUE
        std::cout << "\n[INFO] Lancement de l'interface graphique...\n" << std::endl;
        
        //Initialise le moteur graphique
        QApplication a(argc, argv);

        //Crée la fenêtre principale (qui s'occupe de tout le reste)
        MainWindow w;
        w.setWindowTitle("Harmonies");
        w.resize(1280, 720);
        w.show();

        // Lance la boucle infinie de Qt
        return a.exec();
    }
}


