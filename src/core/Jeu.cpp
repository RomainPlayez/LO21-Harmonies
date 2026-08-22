#include "core/Jeu.h"
#include "core/RegleDeScore.h"
#include "core/Motif.h"
#include "core/CarteAnimal.h"
#include "core/PiocheCartes.h"
#include "core/PiocheEsprit.h"
#include "core/CarteEsprit.h"
#include <iostream>
#include <cstring>

Jeu::Jeu(const std::vector<std::string>& nomsJoueurs, FacePlateau face, bool avecEsprits) 
    : _nbJoueurs(nomsJoueurs.size()), _tourActuel(1), _joueurActifIndex(0), _avecEsprits(avecEsprits) {
    _sac = new Sac();
    _plateauCentral = new PlateauCentral(_nbJoueurs);
    genererPiocheCartes();
    genererPiocheEsprits();
    
    // Alloue le tableau de Joueurs
    _joueurs = new Joueur*[_nbJoueurs];
    for (int i = 0; i < _nbJoueurs; ++i) {
        _joueurs[i] = new Joueur(nomsJoueurs[i], face); 
    }
}

Jeu::~Jeu() {
    for (int i = 0; i < _nbJoueurs; ++i) {
        delete _joueurs[i]; // Nettoie chaque joueur (qui détruira son propre PlateauJoueur)
    }
    delete[] _joueurs;
    
    delete _sac;
    delete _plateauCentral;
}

Joueur* Jeu::getJoueurActif() const { return _joueurs[_joueurActifIndex]; }
Joueur* Jeu::getJoueurParIndex(int index) const {
    if (index < 0 || index >= _nbJoueurs) return nullptr;
    return _joueurs[index];
}
PlateauCentral* Jeu::getPlateauCentral() const { return _plateauCentral; }
int Jeu::getNbJoueurs() const {
    return _nbJoueurs;
}

void Jeu::initialiserPartie() {
    // La Partie demande au plateau central de se remplir depuis le sac
    _plateauCentral->remplirOffre(*_sac);
    _plateauCentral->remplirCartes();
}

void Jeu::passerAuTourSuivant() {
    _joueurActifIndex = (_joueurActifIndex + 1) % _nbJoueurs;
    
    if (_joueurActifIndex == 0) {
        _tourActuel++;
    }
    
    // En solo, on défausse tous les jetons restants avant de remplir
    if (_nbJoueurs == 1) {
        _plateauCentral->defausserJetons();
    }
    
    // On remplit l'offre à chaque fin de tour
    _plateauCentral->remplirOffre(*_sac);
}

bool Jeu::estFini() const {
    // Condition 1 : Le sac de jetons est vide
    if (_sac->estVide()) {
        return true;
    }

    // Condition 2 : Un joueur a 2 cases ou moins innocupées
    for (int i = 0; i < _nbJoueurs; ++i) {
        PlateauJoueur* plateau = _joueurs[i]->getPlateau();
        if (plateau != nullptr) {
            int casesVides = 0;
            
            // On compte les cases vides de ce joueur
            for (int c = 0; c < plateau->getNbCases(); ++c) {
                Case* cas = plateau->getCaseParIndex(c);
                if (cas != nullptr && cas->estVide()) {
                    casesVides++;
                }
            }
            
            // Si 2 cases ou moins sont vides
            if (casesVides <= 2) {
                return true;
            }
        }
    }

    // Si aucune condition n'est remplie, on continue de jouer
    return false;
}


void Jeu::declencherScoringFinal() {
    // Création du tableau
    RegleDeScore* regles[7] = {
        new ScoreArbre(),
        new ScoreMontagne(),
        new ScoreBatiment(),
        new ScoreChamp(),
        new ScoreEau(),
        new ScoreAnimal(), 
        new ScoreEsprit()  
    };

    //  Application des règles pour chaque joueur de la partie
    for (int i = 0; i < _nbJoueurs; ++i) {
        std::cout << "\n--- Calcul du score pour " << _joueurs[i]->getNom() << " ---" << std::endl;

        for (int r = 0; r < 7; ++r) { // Boucle jusqu'à 7
            regles[r]->calculerScore(_joueurs[i]);
        }

        std::cout << "Score final : " << _joueurs[i]->getScore() << " points !" << std::endl;
    }

    // POUR LE MODE SOLO 
    if (_nbJoueurs == 1) {
        Joueur* soloJoueur = _joueurs[0];
        int pv = soloJoueur->getScore();
        int soleils = 0;

        // Barème officiel du mode Solo (Points -> Soleils)
        if (pv >= 160)      soleils = 8;
        else if (pv >= 150) soleils = 7;
        else if (pv >= 140) soleils = 6;
        else if (pv >= 130) soleils = 5;
        else if (pv >= 110) soleils = 4;
        else if (pv >= 90)  soleils = 3;
        else if (pv >= 70)  soleils = 2;
        else if (pv >= 40)  soleils = 1;
        else                soleils = 0;

        // Modificateur : Face du plateau (Face A = +1 Soleil)
        if (soloJoueur->getPlateau() && soloJoueur->getPlateau()->getFace() == FacePlateau::FACE_A) {
            soleils += 1;
        }

        int totalEsprits = 0;
        bool poseeEspritGroupe = false;

        // Cherhcer cartes activés
        for (int i = 0; i < soloJoueur->getNbCartesActives(); ++i) {
            Carte* c = soloJoueur->getCarteActive(i);
            if (c) {
                if (dynamic_cast<CarteEsprit*>(c) != nullptr) {
                    totalEsprits++;
                }
                if (dynamic_cast<EspritGroupe*>(c) != nullptr) {
                    poseeEspritGroupe = true;
                }
            }
        }

        // Chercher cartes completés
        for (int i = 0; i < soloJoueur->getNbCartesCompletees(); ++i) {
            Carte* c = soloJoueur->getCarteCompletee(i);
            if (c) {
                if (dynamic_cast<CarteEsprit*>(c) != nullptr) {
                    totalEsprits++;
                }
                if (dynamic_cast<EspritGroupe*>(c) != nullptr) {
                    poseeEspritGroupe = true;
                }
            }
        }

        // Modificateur : Sans carte Esprit (+2 Soleils)
        if (totalEsprits == 0) {
            soleils += 2;
        }

        // Modificateur : Esprit de type "Groupe" (+1 Soleil)
        if (poseeEspritGroupe) {
            soleils += 1;
        }

        // Garder le rang final au joueur 
        soloJoueur->setSoleils(soleils);
        
        std::cout << "Rang final en Mode Solo : " << soloJoueur->getSoleils() << " Soleils !" << std::endl;
    }

    // Nettoyage obligatoire de la mémoire
    for (int r = 0; r < 7; ++r) { // Boucle jusqu'à 7
        delete regles[r];
    }
}

void Jeu::genererPiocheCartes() {
    PiocheAnimal& pioche = _plateauCentral->getPioche();
    // ====================================================================
    // 1. La Loutre
    // ====================================================================
    Motif motifLoutre(3);
    motifLoutre.setElement(0, 0, 0, CouleurJeton::BLEU,1);
    motifLoutre.setElement(1, 1, 0, CouleurJeton::VERT,1);
    motifLoutre.setElement(1, 2, 0, CouleurJeton::VERT,1);
    int ptsLoutre[] = {5, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Loutre", "loutre.jpg", 3, motifLoutre, ptsLoutre));

    // ====================================================================
    // 2. Le Lézard
    // ====================================================================
    Motif motifLezard(3);
    motifLezard.setElement(0, 0, 0, CouleurJeton::ROUGE,2);
    motifLezard.setElement(1, 1, 0, CouleurJeton::JAUNE,1);
    motifLezard.setElement(1, 2, 0, CouleurJeton::JAUNE,1);
    int ptsLezard[] = {5, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Lezard", "lezard.jpg", 3, motifLezard, ptsLezard));

    // ====================================================================
    // 3. Le Sanglier
    // ====================================================================
    Motif motifSanglier(2);
    motifSanglier.setElement(0,  0,  0, CouleurJeton::VERT, 2);
    motifSanglier.setElement(1, 1, 0, CouleurJeton::ROUGE,2);
    int ptsSanglier[] = {4, 8, 13};
    pioche.ajouterCarte(new CarteAnimal("Sanglier", "sanglier.jpg", 3, motifSanglier, ptsSanglier));

    // ====================================================================
    // 4. LE MARTIN-PÊCHEUR
    // ====================================================================
    Motif motifMartinPecheur(3);
    motifMartinPecheur.setElement(0, 0,  0, CouleurJeton::VERT, 3);
    motifMartinPecheur.setElement(1, -1, 0, CouleurJeton::BLEU, 1);
    motifMartinPecheur.setElement(2, 1, -1, CouleurJeton::BLEU, 1);
    int ptsMartinPecheur[] = {5, 11, 18};
    pioche.ajouterCarte(new CarteAnimal("Martin Pecheur", "martin-pecheur.jpg", 3, motifMartinPecheur, ptsMartinPecheur));

    // ====================================================================
    // 5. LE POISSON
    // ====================================================================
    Motif motifPoisson(2);
    motifPoisson.setElement(0, 0,  0, CouleurJeton::BLEU, 1);
    motifPoisson.setElement(1, 0, -1, CouleurJeton::GRIS, 3);
    int ptsPoisson[] = {3, 6, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Poisson", "poisson.jpg", 4, motifPoisson, ptsPoisson));

    // ====================================================================
    // 6. LE LAPIN
    // ====================================================================
    Motif motifLapin(3);
    motifLapin.setElement(0, 0, 0, CouleurJeton::VERT, 1);
    motifLapin.setElement(1, 1, 0,  CouleurJeton::VERT, 1);
    motifLapin.setElement(2, 2, 0, CouleurJeton::ROUGE, 2);
    int ptsLapin[] = {5, 10, 17};
    pioche.ajouterCarte(new CarteAnimal("Lapin", "lapin.jpg", 3, motifLapin, ptsLapin));

    // ====================================================================
    // 7. L'ABEILLE
    // ====================================================================
    Motif motifAbeille(4);
    motifAbeille.setElement(0,  0,  0, CouleurJeton::VERT, 2);
    motifAbeille.setElement(1, -1, 0, CouleurJeton::JAUNE, 1);
    motifAbeille.setElement(2,  0,  -1, CouleurJeton::JAUNE, 1);
    motifAbeille.setElement(3,  1,  -1, CouleurJeton::JAUNE, 1);
    int ptsAbeille[] = {8,18};
    pioche.ajouterCarte(new CarteAnimal("Abeille", "abeille.jpg", 2, motifAbeille, ptsAbeille));

    // ====================================================================
    // 8. LE GUÉPARD
    // ====================================================================
    Motif motifGuepard(3);
    motifGuepard.setElement(0,  0,  0, CouleurJeton::JAUNE, 1);
    motifGuepard.setElement(1,  1, 0, CouleurJeton::VERT, 2);
    motifGuepard.setElement(2, 2,  0, CouleurJeton::VERT, 2);
    int ptsGuepard[] = {5, 11};
    pioche.ajouterCarte(new CarteAnimal("Guepard", "guepard.jpg", 2, motifGuepard, ptsGuepard));

    // ====================================================================
    // 9. LE KOALA
    // ====================================================================
    Motif motifKoala(2);
    motifKoala.setElement(0,  0,  0, CouleurJeton::VERT, 2);
    motifKoala.setElement(1,  -1, 0, CouleurJeton::VERT, 1);
    int ptsKoala[] = {3, 6, 10, 15};
    pioche.ajouterCarte(new CarteAnimal("Koala", "koala.jpg", 4, motifKoala, ptsKoala));

    // ====================================================================
    // 10. LE FENNEC
    // ====================================================================
    Motif motifFennec(3);
    motifFennec.setElement(0,  0,  0, CouleurJeton::GRIS, 1);
    motifFennec.setElement(1, 1, 0, CouleurJeton::GRIS, 1);
    motifFennec.setElement(2,  2,  0, CouleurJeton::JAUNE, 1);
    int ptsFennec[] = {4, 9, 16};
    pioche.ajouterCarte(new CarteAnimal("Fennec", "fennec.jpg", 3, motifFennec, ptsFennec));

    // ====================================================================
    // 11. L'ÉCUREUIL
    // ====================================================================
    Motif motifEcureuil(2);
    motifEcureuil.setElement(0,  0,  0, CouleurJeton::ROUGE, 2);
    motifEcureuil.setElement(1,  -1,  0, CouleurJeton::VERT, 3);
    int ptsEcureuil[] = {4, 9, 15};
    pioche.ajouterCarte(new CarteAnimal("Ecureuil", "ecureuil.jpg", 3, motifEcureuil, ptsEcureuil));

    // ====================================================================
    // 12. LA COCCINELLE
    // ====================================================================
    Motif motifCoccinelle(2);
    motifCoccinelle.setElement(0,  0,  0, CouleurJeton::JAUNE, 1);
    motifCoccinelle.setElement(1,  -1, 0, CouleurJeton::VERT, 1);
    int ptsCoccinelle[] = {2, 5, 8, 12, 17};
    pioche.ajouterCarte(new CarteAnimal("Coccinelle", "coccinelle.jpg", 5, motifCoccinelle, ptsCoccinelle));

    // ====================================================================
    // 13. LE FLAMANT ROSE
    // ====================================================================
    Motif motifFlamantRose(3);
    motifFlamantRose.setElement(0,  0,  0, CouleurJeton::BLEU, 1);
    motifFlamantRose.setElement(1,  -1,  0, CouleurJeton::JAUNE, 1);
    motifFlamantRose.setElement(2,  -1,  1, CouleurJeton::JAUNE, 1);
    int ptsFlamantRose[] = {4, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Flamant Rose", "flamant_rose.jpg", 3, motifFlamantRose, ptsFlamantRose));

    // ====================================================================
    // 14. L'AIGLE
    // ====================================================================
    Motif motifAigle(2);
    motifAigle.setElement(0,  0,  0, CouleurJeton::GRIS, 3);
    motifAigle.setElement(1, -1,  0, CouleurJeton::JAUNE, 1);
    int ptsAigle[] = {5, 11};
    pioche.ajouterCarte(new CarteAnimal("Aigle", "aigle.jpg", 2, motifAigle, ptsAigle));

    // ====================================================================
    // 15. LE RATON LAVEUR
    // ====================================================================
    Motif motifRatonLaveur(4);
    motifRatonLaveur.setElement(0,  0,  0, CouleurJeton::JAUNE, 1);
    motifRatonLaveur.setElement(1, -1, 0, CouleurJeton::BLEU, 1);
    motifRatonLaveur.setElement(2, 0, -1, CouleurJeton::BLEU, 1);
    motifRatonLaveur.setElement(3, 1, -1, CouleurJeton::BLEU, 1);
    int ptsRatonLaveur[] = {4, 12};
    pioche.ajouterCarte(new CarteAnimal("Raton Laveur", "raton_laveur.jpg", 2, motifRatonLaveur, ptsRatonLaveur));

    // ====================================================================
    // 16. L'OURS
    // ====================================================================
    Motif motifOurs(3);
    motifOurs.setElement(0,  0,  0, CouleurJeton::VERT, 1);
    motifOurs.setElement(1,  -1,  0, CouleurJeton::GRIS, 2);
    motifOurs.setElement(2,  -1,  1, CouleurJeton::GRIS, 2);
    int ptsOurs[] = {5, 11};
    pioche.ajouterCarte(new CarteAnimal("Ours", "ours.jpg", 2, motifOurs, ptsOurs));

    // ====================================================================
    // 17. LE CROCODILE
    // ====================================================================

    Motif motifCrocodile(3);
    motifCrocodile.setElement(0,  0,  0, CouleurJeton::BLEU, 1);
    motifCrocodile.setElement(1,  1,  0, CouleurJeton::BLEU, 1);
    motifCrocodile.setElement(2,  2,  0, CouleurJeton::VERT, 3);
    int ptsCrocodile[] = {4, 9, 15};
    pioche.ajouterCarte(new CarteAnimal("Crocodile", "crocodile.jpg", 3, motifCrocodile, ptsCrocodile));

    // ====================================================================
    // 18. LA GRENOUILLE
    // ====================================================================

    Motif motifGrenouille(2);
    motifGrenouille.setElement(0,  0,  0, CouleurJeton::BLEU, 1);
    motifGrenouille.setElement(1,  -1,  0, CouleurJeton::VERT, 1);
    int ptsGrenouille[] = {2, 4, 6, 10 ,15};
    pioche.ajouterCarte(new CarteAnimal("Grenouille", "grenouille.jpg", 5, motifGrenouille, ptsGrenouille));

    // ====================================================================
    // 19. LE HÉRISSON
    // ====================================================================

    Motif motifHerisson(3);
    motifHerisson.setElement(0,  0,  0, CouleurJeton::ROUGE, 2);
    motifHerisson.setElement(1, -1, 0, CouleurJeton::VERT, 2);
    motifHerisson.setElement(2, -1, 1, CouleurJeton::VERT, 2);
    int ptsHerisson[] = {5, 12};
    pioche.ajouterCarte(new CarteAnimal("Herisson", "herisson.jpg", 2, motifHerisson, ptsHerisson));

    // ====================================================================
    // 20. LE CORBEAU
    // ====================================================================

    Motif motifCorbeau(3);
    motifCorbeau.setElement(0,  0,  0, CouleurJeton::JAUNE, 1);
    motifCorbeau.setElement(1,  -1,  1, CouleurJeton::ROUGE, 2);
    motifCorbeau.setElement(2,  1,  0, CouleurJeton::ROUGE, 2);
    int ptsCorbeau[] = {4, 9};
    pioche.ajouterCarte(new CarteAnimal("Corbeau", "corbeau.jpg", 2, motifCorbeau, ptsCorbeau));

    // ====================================================================
    // 21. LE PAON
    // ====================================================================

    Motif motifPaon(3);
    motifPaon.setElement(0,  0,  0, CouleurJeton::ROUGE, 2);
    motifPaon.setElement(1, -1,  1, CouleurJeton::BLEU, 1);
    motifPaon.setElement(2,  1, 0, CouleurJeton::BLEU, 1);
    int ptsPaon[] = {5, 10, 17};
    pioche.ajouterCarte(new CarteAnimal("Paon", "paon.jpg", 3, motifPaon, ptsPaon));

    // ====================================================================
    // 22. LE LOUP
    // ====================================================================

    Motif motifLoup(3);
    motifLoup.setElement(0,  0,  0, CouleurJeton::VERT, 3);
    motifLoup.setElement(1,  -1, 1, CouleurJeton::JAUNE, 1);
    motifLoup.setElement(2,  -1, 0, CouleurJeton::JAUNE, 1);
    int ptsLoup[] = {4, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Loup", "loup.jpg", 3, motifLoup, ptsLoup));

    // ====================================================================
    // 23. LE CANARD
    // ====================================================================

    Motif motifCanard(2);
    motifCanard.setElement(0,  0,  0, CouleurJeton::BLEU, 1);
    motifCanard.setElement(1,  -1, 1, CouleurJeton::ROUGE, 2);
    int ptsCanard[] = {2, 4, 8, 13};
    pioche.ajouterCarte(new CarteAnimal("Canard", "canard.jpg", 4, motifCanard, ptsCanard));

    // ====================================================================
    // 24. LE SURICATE
    // ====================================================================
    Motif motifSuricate(2);
    motifSuricate.setElement(0,  0,  0, CouleurJeton::GRIS, 1);
    motifSuricate.setElement(1,  -1, 1, CouleurJeton::JAUNE, 1);
    int ptsSuricate[] = {2, 5, 9, 14};
    pioche.ajouterCarte(new CarteAnimal("Suricate", "suricate.jpg", 4, motifSuricate, ptsSuricate));

    // ====================================================================
    // 25. LA RAIE
    // ====================================================================
    Motif motifRaie(3);
    motifRaie.setElement(0,  0,  0, CouleurJeton::BLEU, 1);
    motifRaie.setElement(1, -1,  1, CouleurJeton::GRIS, 1);
    motifRaie.setElement(2,  -1, 0, CouleurJeton::GRIS, 1);
    int ptsRaie[] = {4, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Raie", "raie.jpg", 3, motifRaie, ptsRaie));

    // ====================================================================
    // 26. LA MUSARAIGNE
    // ====================================================================
    Motif motifMusaraigne(3);
    motifMusaraigne.setElement(0,  0,  0, CouleurJeton::ROUGE, 2);
    motifMusaraigne.setElement(1,  -1,  1, CouleurJeton::JAUNE, 1);
    motifMusaraigne.setElement(2,  1,  0, CouleurJeton::JAUNE, 1);
    int ptsMusaraigne[] = {5, 10, 17};
    pioche.ajouterCarte(new CarteAnimal("Musaraigne", "musaraigne.jpg", 3, motifMusaraigne, ptsMusaraigne));

    // ====================================================================
    // 27. LE LAMA
    // ====================================================================
    Motif motifLama(3);
    motifLama.setElement(0,  0,  0, CouleurJeton::JAUNE, 1);
    motifLama.setElement(1,  1, 0, CouleurJeton::JAUNE, 1);
    motifLama.setElement(2,  2, 0, CouleurJeton::GRIS, 2);
    int ptsLama[] = {5, 12};
    pioche.ajouterCarte(new CarteAnimal("Lama", "lama.jpg", 2, motifLama, ptsLama));

    // ====================================================================
    // 28. LE SINGE
    // ====================================================================
    Motif motifSinge(3);
    motifSinge.setElement(0,  0,  0, CouleurJeton::GRIS, 2);
    motifSinge.setElement(1,  -1, 1, CouleurJeton::BLEU, 1);
    motifSinge.setElement(2, -1, 0, CouleurJeton::BLEU, 1);
    int ptsSinge[] = {5, 11};
    pioche.ajouterCarte(new CarteAnimal("Singe", "singe.jpg", 2, motifSinge, ptsSinge));

    // ====================================================================
    // 29. LA CHAUVE-SOURIS
    // ====================================================================
    Motif motifChauveSouris(2);
    motifChauveSouris.setElement(0,  0,  0, CouleurJeton::GRIS, 1);
    motifChauveSouris.setElement(1, -1, 1, CouleurJeton::VERT, 3);
    int ptsChauveSouris[] = {3, 6, 10, 15};
    pioche.ajouterCarte(new CarteAnimal("Chauve Souris", "chauve_souris.jpg", 4, motifChauveSouris, ptsChauveSouris));

    // ====================================================================
    // 30. LE PERROQUET
    // ====================================================================
    Motif motifPerroquet(3);
    motifPerroquet.setElement(0,  0,  0, CouleurJeton::VERT, 2);
    motifPerroquet.setElement(1, -1,  0, CouleurJeton::BLEU, 1);
    motifPerroquet.setElement(2,  -1, 1, CouleurJeton::BLEU, 1);
    int ptsPerroquet[] = {4, 9, 14};
    pioche.ajouterCarte(new CarteAnimal("Perroquet", "perroquet.jpg", 3, motifPerroquet, ptsPerroquet));

    // ====================================================================
    // 31. LE RENARD DES NEIGES
    // ====================================================================
    Motif motifRenardDesNeiges(3);
    motifRenardDesNeiges.setElement(0,  0,  0, CouleurJeton::JAUNE, 1);
    motifRenardDesNeiges.setElement(1,  -1, 0, CouleurJeton::VERT, 2);
    motifRenardDesNeiges.setElement(2,  1,  -1, CouleurJeton::VERT, 2);
    int ptsRenardDesNeiges[] = {5, 10, 17};
    pioche.ajouterCarte(new CarteAnimal("Renard des Neiges", "renard_neiges.jpg", 3, motifRenardDesNeiges, ptsRenardDesNeiges));

    // ====================================================================
    // 32. LE MANCHOT
    // ====================================================================
    Motif motifManchot(3);
    motifManchot.setElement(0,  0,  0, CouleurJeton::GRIS, 1);
    motifManchot.setElement(1,  -1,  0, CouleurJeton::BLEU, 1);
    motifManchot.setElement(2,  1,  -1, CouleurJeton::BLEU, 1);
    int ptsManchot[] = {4, 10, 16};
    pioche.ajouterCarte(new CarteAnimal("Manchot", "manchot.jpg", 3, motifManchot, ptsManchot));

    //Mélange des cartes : 
    pioche.melanger();
}

void Jeu::genererPiocheEsprits() {

    // ==========================================
    // 1. LA LIBELLULE (Esprit Groupe)
    // ==========================================
    EspritGroupe* libellule = new EspritGroupe(
        "Libellule", "libellule.jpg", 1,
        CibleGroupe::EAU, 0, // hauteurCible ignorée pour l'eau
        2, 0, 0,             // Seuil : groupe de 2 minimum
        7, 0, 0              // 7 points
    );
    Motif* motifLibellule = new Motif(3);
    motifLibellule->setElement(0, 0, 0, CouleurJeton::BLEU, 1);  // Centre (Cube)
    motifLibellule->setElement(1, -1, 0, CouleurJeton::VERT, 2); // Arbre opposé 1
    motifLibellule->setElement(2, 1, 0, CouleurJeton::VERT, 2);  // Arbre opposé 2
    libellule->setMotif(motifLibellule);
    _piocheEsprits.ajouterCarte(libellule);

    // ==========================================
    // 2. LE PAPILLON (Esprit Groupe)
    // ==========================================
    EspritGroupe* papillon = new EspritGroupe(
        "Papillon", "papillon.jpg", 1,
        CibleGroupe::CHAMP, 0, 
        1, 0, 0,             // Seuil : groupe de 1 (donc chaque champ compte)
        5, 0, 0              // 5 points par champ
    );
    Motif* motifPapillon = new Motif(4);
    motifPapillon->setElement(0, 0, 0, CouleurJeton::JAUNE, 1);  // Centre (Cube)
    motifPapillon->setElement(1, -1, 1, CouleurJeton::BLEU, 1);   // Rivière adjacente 1
    motifPapillon->setElement(2, 0, 1, CouleurJeton::BLEU, 1);   // Rivière adjacente 2
    motifPapillon->setElement(3, -1, 2, CouleurJeton::JAUNE, 1);  // Champ symétrique
    papillon->setMotif(motifPapillon);
    _piocheEsprits.ajouterCarte(papillon);

    // ==========================================
    // 3. LE LION (Esprit Groupe)
    // ==========================================
    EspritGroupe* lion = new EspritGroupe(
        "Lion", "lion.jpg", 1,
        CibleGroupe::CHAMP, 0, 
        1, 2, 3,             // Seuils : taille 1, 2, ou 3+
        2, 2, 10             // Points associés
    );
    Motif* motifLion = new Motif(3);
    motifLion->setElement(0, 0, 0, CouleurJeton::JAUNE, 1); // Centre (Cube)
    motifLion->setElement(1, -1, 0, CouleurJeton::JAUNE, 1); // Champ opposé
    motifLion->setElement(2, 1, 0, CouleurJeton::VERT, 2);   // Arbre opposé
    lion->setMotif(motifLion);
    _piocheEsprits.ajouterCarte(lion);

    // ==========================================
    // 4. LE CERF (Esprit Quantité)
    // ==========================================
    EspritQuantite* cerf = new EspritQuantite(
        "Cerf", "cerf.jpg", 1,
        CibleQuantite::ARBRE, 0, 
        1, 2, 3,             // Seuils de hauteur
        1, 2, 4              // Points associés par hauteur
    );
    Motif* motifCerf = new Motif(3);
    motifCerf->setElement(0, 0, 0, CouleurJeton::VERT, 2);   // Centre (Cube, Arbre h=2)
    motifCerf->setElement(1, -1, 0, CouleurJeton::VERT, 3);  // Grand arbre
    motifCerf->setElement(2, 1, 0, CouleurJeton::VERT, 1);   // Petit arbuste
    cerf->setMotif(motifCerf);
    _piocheEsprits.ajouterCarte(cerf);

    // ==========================================
    // 5. LE HIBOU (Esprit Quantité)
    // ==========================================
    EspritQuantite* hibou = new EspritQuantite(
        "Hibou", "hibou.jpg", 1,
        CibleQuantite::ARBRE, 0, 
        1, 2, 3,             // Seuils de hauteur
        1, 3, 1              // Points associés (3 pts pour hauteur 2)
    );
    Motif* motifHibou = new Motif(3);
    motifHibou->setElement(0, 0, 0, CouleurJeton::VERT, 3);  // Centre (Cube, Arbre h=3)
    motifHibou->setElement(1, 1, 0, CouleurJeton::VERT, 1);  // Arbuste 1
    motifHibou->setElement(2, 0, 1, CouleurJeton::VERT, 1);  // Arbuste 2 
    hibou->setMotif(motifHibou);
    _piocheEsprits.ajouterCarte(hibou);

    // ==========================================
    // 6. LA TORTUE (Esprit Quantité)
    // ==========================================
    EspritQuantite* tortue = new EspritQuantite(
        "Tortue", "tortue.jpg", 1,
        CibleQuantite::EAU, 0, 
        1, 0, 0,             // Seuil : 1 (comptage unitaire)
        2, 0, 0              // 2 points par case eau
    );
    Motif* motifTortue = new Motif(3);
    motifTortue->setElement(0, 0, 0, CouleurJeton::BLEU, 1); // Centre (Cube)
    motifTortue->setElement(1, -1, 0, CouleurJeton::GRIS, 2);// Montagne h=2
    motifTortue->setElement(2, 1, 0, CouleurJeton::BLEU, 1); // Case eau
    tortue->setMotif(motifTortue);
    _piocheEsprits.ajouterCarte(tortue);

    // ==========================================
    // 7. LE CHAT (Esprit Groupe)
    // ==========================================
    EspritGroupe* chat = new EspritGroupe(
        "Chat", "chat.jpg", 1,
        CibleGroupe::BATIMENT, 2, // Les bâtiments exigent une hauteur de 2
        1, 0, 0,             // Seuil 1
        4, 0, 0              // 4 points
    );
    Motif* motifChat = new Motif(3);
    motifChat->setElement(0, 0, 0, CouleurJeton::ROUGE, 2);  // Centre (Cube, Bâtiment)
    motifChat->setElement(1, 1, 0, CouleurJeton::VERT, 1);   // Arbuste
    motifChat->setElement(2, 2, 0, CouleurJeton::ROUGE, 2);  // Bâtiment de l'autre côté
    chat->setMotif(motifChat);
    _piocheEsprits.ajouterCarte(chat);

    // ==========================================
    // 8. LA CIGOGNE (Esprit Groupe)
    // ==========================================
    EspritGroupe* cigogne = new EspritGroupe(
        "Cigogne", "cigogne.jpg", 1,
        CibleGroupe::BATIMENT, 2, 
        2, 0, 0,             // Seuil 2
        6, 0, 0              // 6 points
    );
    Motif* motifCigogne = new Motif(3);
    motifCigogne->setElement(0, 0, 0, CouleurJeton::ROUGE, 2); // Centre (Cube, opposé au champ)
    motifCigogne->setElement(1, 1, 0, CouleurJeton::ROUGE, 2); // Bâtiment adjacent
    motifCigogne->setElement(2, 2, 0, CouleurJeton::JAUNE, 1); // Champ en bout de ligne
    cigogne->setMotif(motifCigogne);
    _piocheEsprits.ajouterCarte(cigogne);

    // ==========================================
    // 9. LE BOUQUETIN (Esprit Quantité)
    // ==========================================
    EspritQuantite* bouquetin = new EspritQuantite(
        "Bouquetin", "bouquetin.jpg", 1,
        CibleQuantite::MONTAGNE, 0, 
        1, 2, 3,             // Seuils de hauteur
        1, 2, 4              // Points associés
    );
    Motif* motifBouquetin = new Motif(2);
    motifBouquetin->setElement(0, 0, 0, CouleurJeton::GRIS, 3); // Centre (Cube, h=3)
    motifBouquetin->setElement(1, 1, 0, CouleurJeton::GRIS, 2); // Montagne adjacente h=2
    bouquetin->setMotif(motifBouquetin);
    _piocheEsprits.ajouterCarte(bouquetin);

    // ==========================================
    // 10. LA MARMOTTE (Esprit Quantité)
    // ==========================================
    EspritQuantite* marmotte = new EspritQuantite(
        "Marmotte", "marmotte.jpg", 1,
        CibleQuantite::MONTAGNE, 0, 
        1, 2, 3,             // Seuils de hauteur
        1, 3, 1              // Points associés
    );
    Motif* motifMarmotte = new Motif(3);
    motifMarmotte->setElement(0, 0, 0, CouleurJeton::GRIS, 2); // Centre (Cube, h=2)
    motifMarmotte->setElement(1, 1, 0, CouleurJeton::GRIS, 1); // Montagne h=1 adjacente
    motifMarmotte->setElement(2, -1, 1, CouleurJeton::GRIS, 1); 
    marmotte->setMotif(motifMarmotte);
    _piocheEsprits.ajouterCarte(marmotte);


    // ==========================================
    // Mélange final de la pioche
    // ==========================================
    _piocheEsprits.melanger();
}

CarteEsprit* Jeu::piocherEsprit() {
    return _piocheEsprits.piocherEsprit(); 
}

EtatPartie Jeu::sauvegarder() const {
    EtatPartie etat;
    std::memset(&etat, 0, sizeof(EtatPartie));

    etat.nbJoueurs = _nbJoueurs;
    etat.face = (_joueurs[0]->getPlateau()->getFace() == FacePlateau::FACE_A) ? 0 : 1;
    etat.tourActuel = _tourActuel;
    etat.joueurActifIndex = _joueurActifIndex;

    // Sac
    etat.nbJetonsSac = _sac->getNbJetonsRestants(); // Utilise ta méthode existante
    for (int i = 0; i < etat.nbJetonsSac; ++i) {
        etat.jetonsSac[i] = _sac->getJetonParIndex(i)->getCouleur(); 
    }

    // Plateau central : groupes
    for (int i = 0; i < 5; ++i) {
        const GroupeJetons* g = _plateauCentral->getOffreJetons(i);
        if (g && !g->estVide()) {
            etat.groupes[i].nbJetons = g->getNbJetons();
            for (int j = 0; j < g->getNbJetons(); ++j)
                if (g->getJeton(j)) etat.groupes[i].couleurs[j] = g->getJeton(j)->getCouleur();
        }
    }

    // Plateau central : rivière
    for (int i = 0; i < 5; ++i) {
        const CarteAnimal* c = _plateauCentral->getCarteRiviere(i);
        if (c) {
            strncpy(etat.cartesRiviere[i].nom, c->getNom().c_str(), 63);
            etat.cartesRiviere[i].nbCubesActuels = c->getNbCubesActuels();
        }
    }

    // Joueurs
    for (int p = 0; p < _nbJoueurs; ++p) {
        Joueur* j = _joueurs[p];
        EtatJoueur& ej = etat.joueurs[p];
        ej.score = j->getScore();
        strncpy(ej.nom, j->getNom().c_str(), 63);
        ej.nom[63] = '\0';
        ej.nbCartesActives = j->getNbCartesActives();
        for (int c = 0; c < j->getNbCartesActives(); ++c) {
            Carte* carte = j->getCarteActive(c);
            if (carte) {
                strncpy(ej.cartesActives[c].nom, carte->getNom().c_str(), 63);
                ej.cartesActives[c].nbCubesActuels = carte->getNbCubesActuels();
            }
        }
        ej.nbCartesCompletees = j->getNbCartesCompletees();
        for (int c = 0; c < j->getNbCartesCompletees(); ++c) {
            Carte* carte = j->getCarteCompletee(c);
            if (carte) {
                strncpy(ej.cartesCompletees[c].nom, carte->getNom().c_str(), 63);
                ej.cartesCompletees[c].nbCubesActuels = carte->getNbCubesActuels();
            }
        }
        // Plateau joueur
        PlateauJoueur* plateau = j->getPlateau();
        ej.plateau.nbCases = 0;
        for (int c = 0; c < plateau->getNbCases(); ++c) {
            Case* cas = plateau->getCaseParIndex(c);
            if (cas && (!cas->estVide() || cas->aUnCubeAnimal())) {
                EtatCase& ec = ej.plateau.cases[ej.plateau.nbCases++];
                ec.x = cas->getX(); ec.y = cas->getY();
                ec.nbJetons = cas->getZ();
                
                ec.aUnCube = cas->aUnCubeAnimal();
                if (ec.aUnCube && cas->getCube() != nullptr) {
                    ec.estCubeEsprit = cas->getCube()->isEsprit();
                } else {
                    ec.estCubeEsprit = false;
                }
                
                for (int k = 0; k < cas->getZ(); ++k)
                    if (cas->getJeton(k)) ec.jetons[k] = cas->getJeton(k)->getCouleur();
            }
        }
    }
    return etat;
}

void Jeu::restaurer(const EtatPartie& etat) {
    // On réajuste le nombre de joueurs si la sauvegarde est différente de l'initialisation
    if (_nbJoueurs != etat.nbJoueurs) {
        for(int i = 0; i < _nbJoueurs; ++i) delete _joueurs[i];
        delete[] _joueurs;
        
        _nbJoueurs = etat.nbJoueurs;
        FacePlateau face = (etat.face == 0) ? FacePlateau::FACE_A : FacePlateau::FACE_B;
        _joueurs = new Joueur*[_nbJoueurs];
        for(int i = 0; i < _nbJoueurs; ++i) _joueurs[i] = new Joueur("Joueur " + std::to_string(i+1), face);
    }

    _plateauCentral->setTailleActivePourChargement(etat.nbJoueurs);

    _sac->vider(); // Fait place nette !
    for (int i = 0; i < etat.nbJetonsSac; ++i) {
        _sac->ajouterJeton(new Jeton(etat.jetonsSac[i]));
    }
    _sac->melanger();
    
    for (int i = 0; i < 5; ++i) {
        if (etat.groupes[i].nbJetons > 0) {
            CouleurJeton couleurs[3];
            for (int j = 0; j < etat.groupes[i].nbJetons; ++j) couleurs[j] = etat.groupes[i].couleurs[j];
            _plateauCentral->setGroupe(i, couleurs, etat.groupes[i].nbJetons);
        }
    }
    
    PiocheAnimal& pioche = _plateauCentral->getPioche();
    for (int i = 0; i < 5; ++i) {
        if (strlen(etat.cartesRiviere[i].nom) > 0) {
            CarteAnimal* carte = pioche.prendreParNom(etat.cartesRiviere[i].nom);
            if (carte) {
                carte->setCubesActuels(etat.cartesRiviere[i].nbCubesActuels);
                _plateauCentral->setCarteRiviere(i, carte);
            }
        }
    }
    
    for (int p = 0; p < _nbJoueurs; ++p) {
        Joueur* joueur = _joueurs[p];
        const EtatJoueur& ej = etat.joueurs[p];
        joueur->setNom(ej.nom);
        joueur->ajouterScore(ej.score);

        // Restaurer le plateau
        PlateauJoueur* plateau = joueur->getPlateau();
        for (int c = 0; c < ej.plateau.nbCases; ++c) {
            const EtatCase& ec = ej.plateau.cases[c];
            Case* cas = plateau->getCase(ec.x, ec.y);
            if (cas) {
                cas->vider(); // Nettoie par sécurité
                
                for (int k = 0; k < ec.nbJetons; ++k)
                    cas->placerJeton(new Jeton(ec.jetons[k]));
                    
                if (ec.aUnCube) {
                    CubeAnimal* nouveauCube = new CubeAnimal("null",ec.estCubeEsprit);
                    cas->placerCube(nouveauCube);
                }
            }
        }

        // Fonction de recherche universelle (Animaux ou Esprit) ---
        auto trouverCarteUniverselle = [&](const char* nom, int cubesActuels) -> Carte* {
            Carte* carte = pioche.prendreParNom(nom); // Cherche en Animal
            if (!carte) carte = _piocheEsprits.prendreParNom(nom); // Cherche en Esprit
            if (carte) carte->setCubesActuels(cubesActuels);
            return carte;
        };

        // Restaurer les cartes actives (Animaux + Esprits)
        for (int c = 0; c < ej.nbCartesActives; ++c) {
            Carte* carte = trouverCarteUniverselle(ej.cartesActives[c].nom, ej.cartesActives[c].nbCubesActuels);
            if (carte) joueur->prendreCarte(carte);
        }

        // Restaurer les cartes complétées
        for (int c = 0; c < ej.nbCartesCompletees; ++c) {
            Carte* carte = trouverCarteUniverselle(ej.cartesCompletees[c].nom, ej.cartesCompletees[c].nbCubesActuels);
            if (carte) joueur->gererCartesCompletees(carte);
        }
    }
    _tourActuel       = etat.tourActuel;
    _joueurActifIndex = etat.joueurActifIndex;
}