#include "core/EtatPartie.h"
#include <cstring>
#include <cstdio>

static const char* couleurVersStr(CouleurJeton c) {
    switch (c) {
        case CouleurJeton::GRIS: return "GRIS";
        case CouleurJeton::ROUGE: return "ROUGE";
        case CouleurJeton::MARRON: return "MARRON";
        case CouleurJeton::VERT: return "VERT";
        case CouleurJeton::JAUNE: return "JAUNE";
        case CouleurJeton::BLEU: return "BLEU";
        default: return "?";
    }
}

static CouleurJeton strVersCouleur(const std::string& s) {
    if (s == "GRIS") return CouleurJeton::GRIS;
    if (s == "ROUGE") return CouleurJeton::ROUGE;
    if (s == "MARRON") return CouleurJeton::MARRON;
    if (s == "VERT") return CouleurJeton::VERT;
    if (s == "JAUNE") return CouleurJeton::JAUNE;
    return CouleurJeton::BLEU;
}

// Écriture (sauvegarde)
void EtatPartie::ecrire(std::ofstream& f) const {
    f << "HARMONIES_SAVE_V1\n";
    f << "nbJoueurs=" << nbJoueurs << "\n";
    f << "face=" << face << "\n";
    f << "tourActuel=" << tourActuel << "\n";
    f << "joueurActifIndex=" << joueurActifIndex << "\n";
    f << "etatTour=" << etatTour << "\n";

    // Sac
    f << "SAC " << nbJetonsSac;
    for (int i = 0; i < nbJetonsSac; ++i) {
        f << " " << couleurVersStr(jetonsSac[i]);
    }
    f << "\n";

    // Groupes
    for (int i = 0; i < 5; ++i) {
        f << "GROUPE " << i << " " << groupes[i].nbJetons;
        for (int j = 0; j < groupes[i].nbJetons; ++j)
            f << " " << couleurVersStr(groupes[i].couleurs[j]);
        f << "\n";
    }

    // Rivière
    for (int i = 0; i < 5; ++i) {
        if (strlen(cartesRiviere[i].nom) == 0)
            f << "RIVIERE " << i << " vide 0\n";
        else
            f << "RIVIERE " << i << " " << cartesRiviere[i].nom
              << " " << cartesRiviere[i].nbCubesActuels << "\n";
    }

    // Joueurs
    for (int p = 0; p < nbJoueurs; ++p) {
        const EtatJoueur& ej = joueurs[p];
        f << "JOUEUR " << p << "\n";
        f << "nom=" << ej.nom << "\n";
        f << "score=" << ej.score << "\n";
        f << "nbCartesActives=" << ej.nbCartesActives << "\n";
        for (int c = 0; c < ej.nbCartesActives; ++c)
            f << "CARTE " << c << " " << ej.cartesActives[c].nom
              << " " << ej.cartesActives[c].nbCubesActuels << "\n";
        f << "nbCartesCompletees=" << ej.nbCartesCompletees << "\n";
        for (int c = 0; c < ej.nbCartesCompletees; ++c)
            f << "CARTEC " << c << " " << ej.cartesCompletees[c].nom
              << " " << ej.cartesCompletees[c].nbCubesActuels << "\n";
        f << "CASES_START\n";
        for (int c = 0; c < ej.plateau.nbCases; ++c) {
            const EtatCase& ec = ej.plateau.cases[c];
            if (ec.nbJetons == 0 && !ec.aUnCube) continue; 
            
            f << "CASE " << ec.x << " " << ec.y << " " << ec.nbJetons;
            for (int j = 0; j < ec.nbJetons; ++j)
                f << " " << couleurVersStr(ec.jetons[j]);
            
            int codeCube = 0;
            if (ec.aUnCube) {
                codeCube = ec.estCubeEsprit ? 2 : 1;
            }
            f << " " << codeCube << "\n";
        }
        f << "CASES_END\n";
    }
}

// Lecture (chargement)
EtatPartie EtatPartie::lire(std::ifstream& f) {
    EtatPartie etat;
    std::memset(&etat, 0, sizeof(EtatPartie));

    std::string ligne;
    std::getline(f, ligne);

    auto lireInt = [&](const std::string& label) -> int {
        std::getline(f, ligne);
        return std::stoi(ligne.substr(label.size() + 1));
    };

    etat.nbJoueurs = lireInt("nbJoueurs");
    etat.face = lireInt("face");
    etat.tourActuel = lireInt("tourActuel");
    etat.joueurActifIndex = lireInt("joueurActifIndex");
    etat.etatTour = lireInt("etatTour");

    // Sac
    std::getline(f, ligne);
    {
        int pos = ligne.find(' ');
        int pos2 = ligne.find(' ', pos + 1);
        int nbJ = 0;
        
        if (pos2 == (int)std::string::npos) {
            // S'il n'y a pas de jetons : "SAC 0"
            nbJ = std::stoi(ligne.substr(pos + 1));
        } else {
            nbJ = std::stoi(ligne.substr(pos + 1, pos2 - pos - 1));
        }
        
        etat.nbJetonsSac = nbJ;
        pos = pos2;
        
        for (int i = 0; i < nbJ; ++i) {
            int next = ligne.find(' ', pos + 1);
            std::string c = (next == (int)std::string::npos)
                ? ligne.substr(pos + 1)
                : ligne.substr(pos + 1, next - pos - 1);
            etat.jetonsSac[i] = strVersCouleur(c);
            pos = next;
        }
    }

    // Groupes
    for (int i = 0; i < 5; ++i) {
        std::getline(f, ligne);
        int pos = ligne.find(' ');          
        pos = ligne.find(' ', pos + 1);
        int nbJ = std::stoi(ligne.substr(pos + 1));
        etat.groupes[i].nbJetons = nbJ;
        pos = ligne.find(' ', pos + 1);
        for (int j = 0; j < nbJ; ++j) {
            int next = ligne.find(' ', pos + 1);
            std::string c = (next == (int)std::string::npos)
                ? ligne.substr(pos + 1)
                : ligne.substr(pos + 1, next - pos - 1);
            etat.groupes[i].couleurs[j] = strVersCouleur(c);
            pos = next;
        }
    }

    // Rivière
    for (int i = 0; i < 5; ++i) {
        std::getline(f, ligne);
        int pos1 = ligne.find(' ');         
        int pos2 = ligne.find(' ', pos1+1); 
        int pos3 = ligne.rfind(' ');        
        std::string nom = ligne.substr(pos2 + 1, pos3 - pos2 - 1);
        int cubes = std::stoi(ligne.substr(pos3 + 1));
        if (nom != "vide") {
            strncpy(etat.cartesRiviere[i].nom, nom.c_str(), 63);
            etat.cartesRiviere[i].nbCubesActuels = cubes;
        }
    }

    // Joueurs
    for (int p = 0; p < etat.nbJoueurs; ++p) {
        std::getline(f, ligne);
        EtatJoueur& ej = etat.joueurs[p];
        std::getline(f, ligne); // Lit la ligne "nom=LePrenom"
        std::string nomCharge = ligne.substr(4); // Coupe les 4 premiers caractères ("nom=")
        strncpy(ej.nom, nomCharge.c_str(), 63);
        ej.nom[63] = '\0'; // Sécurité string C
        ej.score = lireInt("score");
        ej.nbCartesActives = lireInt("nbCartesActives");
        for (int c = 0; c < ej.nbCartesActives; ++c) {
            std::getline(f, ligne);
            int pos1 = ligne.find(' ');       
            int pos2 = ligne.find(' ', pos1+1); 
            int pos3 = ligne.rfind(' ');
            std::string nom = ligne.substr(pos2+1, pos3-pos2-1);
            int cubes = std::stoi(ligne.substr(pos3+1));
            strncpy(ej.cartesActives[c].nom, nom.c_str(), 63);
            ej.cartesActives[c].nbCubesActuels = cubes;
        }
        ej.nbCartesCompletees = lireInt("nbCartesCompletees");
        for (int c = 0; c < ej.nbCartesCompletees; ++c) {
            std::getline(f, ligne);
            int pos1 = ligne.find(' ');       
            int pos2 = ligne.find(' ', pos1+1); 
            int pos3 = ligne.rfind(' ');
            std::string nom = ligne.substr(pos2+1, pos3-pos2-1);
            int cubes = std::stoi(ligne.substr(pos3+1));
            strncpy(ej.cartesCompletees[c].nom, nom.c_str(), 63);
            ej.cartesCompletees[c].nbCubesActuels = cubes;
        }

        std::getline(f, ligne);
        ej.plateau.nbCases = 0;
        while (std::getline(f, ligne) && ligne != "CASES_END") {
            EtatCase& ec = ej.plateau.cases[ej.plateau.nbCases++];
            int pos = ligne.find(' ');
            int pos2 = ligne.find(' ', pos+1);
            ec.x = std::stoi(ligne.substr(pos+1, pos2-pos-1));
            pos = pos2;
            pos2 = ligne.find(' ', pos+1);
            ec.y = std::stoi(ligne.substr(pos+1, pos2-pos-1));
            pos = pos2;
            pos2 = ligne.find(' ', pos+1);
            int nbJ = std::stoi(ligne.substr(pos+1, pos2-pos-1));
            ec.nbJetons = nbJ;
            pos = pos2;

            for (int j = 0; j < nbJ; ++j) {
                pos2 = ligne.find(' ', pos+1);
                std::string c = (pos2 == (int)std::string::npos)
                    ? ligne.substr(pos+1)
                    : ligne.substr(pos+1, pos2-pos-1);
                ec.jetons[j] = strVersCouleur(c);
                pos = pos2;
            }
            
            ec.aUnCube = false;
            ec.estCubeEsprit = false;
            
            if (pos != (int)std::string::npos && pos + 1 < ligne.size()) {
                int codeCube = std::stoi(ligne.substr(pos + 1));
                if (codeCube == 1) {
                    ec.aUnCube = true;
                    ec.estCubeEsprit = false;
                } else if (codeCube == 2) {
                    ec.aUnCube = true;
                    ec.estCubeEsprit = true;
                }
            }
        }
    }
    return etat;
}

bool EtatPartie::fichierExiste() {
    std::ifstream f(SAVE_FILE);
    return f.good();
}