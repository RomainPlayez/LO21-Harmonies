#pragma once

#include "core/GroupeJetons.h"
#include "core/Carte.h"
#include "core/Sac.h"
#include "core/PiocheCartes.h"

/**
 * @class PlateauCentral
 * @brief Représente la zone commune contenant l'offre de jetons et la rivière de cartes.
 */


class PlateauCentral {
    private:
        GroupeJetons* _offreJetons[5]; // 5 emplacements de groupes de jetons
        CarteAnimal* _riviereCartes[5];
        PiocheAnimal _pioche;
        int _tailleActive;

    public:
        PlateauCentral(int nbJoueurs = 2);
        ~PlateauCentral();

        /**
         * @brief Le PlateauCentral étant unique dans la Partie, on désactive sa copie.
         */
        PlateauCentral(const PlateauCentral&) = delete;
        PlateauCentral& operator=(const PlateauCentral&) = delete;

        /**
         * @brief Remplit les groupes de jetons vides depuis le sac.
         * @param sac Référence vers le sac de jeu.
         */
        void remplirOffre(Sac& sac);

        /**
         * @brief Permet à un joueur de prendre un groupe de 3 jetons.
         * @param index L'index du groupe choisi (0 à 4).
         * @param result Tableau dans lequel les pointeurs des 3 jetons seront placés.
         * @return true si le groupe était disponible, false sinon.
         */
        bool prendreGroupe(int index, Jeton* result[3]);

        /**
         * @brief Retire et renvoie une carte de la rivière.
         */
        Carte* prendreCarte(int index);

        void remplirCartes();

        PiocheAnimal& getPioche();

        /**
         * @brief Accès en lecture seule à un groupe de jetons de l'offre.
         */
        const GroupeJetons* getOffreJetons(int index) const;

        /**
         * @brief Accès en lecture seule à une carte de la rivière.
         */
        const CarteAnimal* getCarteRiviere(int index) const;

        // METHODES SOLO & SAUVEGARDE
        int getTailleActive() const { return _tailleActive; }
        void setGroupe(int index, CouleurJeton couleurs[], int nb);
        void setCarteRiviere(int index, CarteAnimal* carte);
        void defausserCarteOffre(int index);
        void defausserJetons();
        void setTailleActivePourChargement(int nbJoueurs);
};