#pragma once

#include <string>
#include "core/ControleurPartie.h"
#include "core/Joueur.h"
#include "core/PlateauCentral.h"
#include "core/PlateauJoueur.h"
#include "core/Jeton.h"

class VueConsole {
public:
    // CONFIGURATION 
    static int demanderNombreJoueurs();
    static FacePlateau demanderFacePlateau();
    static bool demanderUtiliserCartesEsprit();

    // AFFICHAGE 
    static void afficherPlateauCentral(ControleurPartie* controleur);
    static void afficherMainJoueur(const Joueur* j);
    static void afficherPlateauJoueur(const PlateauJoueur* pj);

    //ACTIONS DE JEU
    /**
     * @brief Boucle principale unifiée du tour d'un joueur.
     */
    static void menuActionTour(ControleurPartie* controleur, Joueur* joueurActuel, bool avecEsprits);

    /**
     * @brief Gère la sélection de la carte Esprit au Tour 1.
     */
    static void choixEspritPremierTour(ControleurPartie* controleur, Joueur* joueurActuel);

    /**
     * @brief Gère le placement optionnel d'un cube animal (sans questions intempestives).
     */
    static void placerCubeAnimal(ControleurPartie* controleur, Joueur* joueurActuel);

    static bool proposerReprise();

private:
    // Utilitaires de sécurisation
    static void viderBuffer();
    static std::string couleurVersTexte(CouleurJeton c);
};