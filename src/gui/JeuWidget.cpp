#include "gui/JeuWidget.h"
#include "gui/PlateauJoueurWidget.h"
#include "gui/PlateauCentralWidget.h"
#include "gui/CarteWidget.h" 
#include "core/ControleurPartie.h"
#include "core/Jeu.h"
#include "core/Joueur.h"
#include <QFont>
#include <QCoreApplication>
#include <QDebug>
#include <QVBoxLayout> 
#include <QHBoxLayout> 
#include <QLabel>      
#include <QPushButton> 
#include <QStackedWidget> 
#include <QMessageBox>

JeuWidget::JeuWidget(ControleurPartie* controleur, QWidget *parent)
    : QWidget(parent), _controleur(controleur)
{
    qDebug() << "[JeuWidget] DÉBUT de la construction...";

    if (!_controleur || !_controleur->getJeu()) {
        qDebug() << "[JeuWidget] ERREUR CRITIQUE : Controleur ou Jeu est nullptr !";
        return;
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // BARRE D'ACTION 
    qDebug() << "[JeuWidget] Création de la barre d'action...";
    QHBoxLayout* barreActionLayout = new QHBoxLayout();

    _labelTourActuel = new QLabel("Chargement...", this);
    QFont fontTour = _labelTourActuel->font();
    fontTour.setBold(true);
    fontTour.setPointSize(14);
    _labelTourActuel->setFont(fontTour);

    _labelInstruction = new QLabel("Attente d'une action...", this);
    _labelInstruction->setStyleSheet("color: gray; font-style: italic;");


    _btnFinTour = new QPushButton("FIN DU TOUR", this);
    _btnFinTour->setStyleSheet("background-color: #2C3E50; color: white; font-weight: bold; padding: 10px;");

    _btnSauvegarderQuitter = new QPushButton("Sauvegarder & Quitter", this);
    _btnSauvegarderQuitter->setStyleSheet("background-color: #8E6C46; color: white; font-weight: bold; padding: 10px; border-radius: 4px;");

    barreActionLayout->addWidget(_labelTourActuel);
    barreActionLayout->addStretch();
    barreActionLayout->addWidget(_labelInstruction);
    barreActionLayout->addStretch();
    barreActionLayout->addWidget(_btnFinTour);
    barreActionLayout->addWidget(_btnSauvegarderQuitter);
    mainLayout->addLayout(barreActionLayout);

    //  ZONE CENTRALE
    qDebug() << "[JeuWidget] Création du PlateauCentralWidget...";
    if (_controleur->getJeu()->getPlateauCentral() == nullptr) {
        qDebug() << "[JeuWidget] ERREUR : Le PlateauCentral du backend est nullptr !";
    }

    _zoneCentraleStack = new QStackedWidget(this);

    // Vue A : Le Plateau Central normal
    bool isSolo = (_controleur->getJeu()->getNbJoueurs() == 1);
    _plateauCentralWidget = new PlateauCentralWidget(_controleur->getJeu()->getPlateauCentral(), isSolo, this);
    _zoneCentraleStack->addWidget(_plateauCentralWidget);

    // Vue B : Le Draft des Esprits
    _draftEspritWidget = new QWidget(this);
    QVBoxLayout* draftLayout = new QVBoxLayout(_draftEspritWidget);
    
    QLabel* titreDraft = new QLabel("Extension Esprits : Choisissez votre Esprit protecteur !", _draftEspritWidget);
    titreDraft->setAlignment(Qt::AlignCenter);
    titreDraft->setStyleSheet("font-size: 20px; font-weight: bold; color: #2C3E50;");
    draftLayout->addWidget(titreDraft);

    QHBoxLayout* cartesLayout = new QHBoxLayout();
    cartesLayout->setAlignment(Qt::AlignCenter);
    for(int i = 0; i < 2; ++i) {
        _cartesDraftWidget[i] = new CarteWidget(this);
        _cartesDraftWidget[i]->setModeRiviere(true, i); 
        cartesLayout->addWidget(_cartesDraftWidget[i]);

        // Clic sur une carte du draft
        connect(_cartesDraftWidget[i], &CarteWidget::carteRiviereCliquee, this, [this, i](int) {
            if (_espritsEnCours[i] != nullptr) {
                // Le joueur prend la carte
                _controleur->getJoueurCourant()->prendreCarte(_espritsEnCours[i]);
                
                // L'autre carte est défaussée/détruite
                delete _espritsEnCours[1 - i];
                
                // On nettoie pour le prochain joueur
                _espritsEnCours[0] = nullptr;
                _espritsEnCours[1] = nullptr;
                
                rafraichirUI(); 
            }
        });
    }
    draftLayout->addLayout(cartesLayout);
    _zoneCentraleStack->addWidget(_draftEspritWidget);

    mainLayout->addWidget(_zoneCentraleStack);

    connect(_plateauCentralWidget, &PlateauCentralWidget::carteChoisie, this, [this](int index) {
        //CAS A : ON EST EN TRAIN DE DÉFAUSSER (Mode Solo)
        if (_modeDefausseSolo) {
            Carte* carteADefausser = _controleur->getJeu()->getPlateauCentral()->prendreCarte(index);
            if (carteADefausser) {
                delete carteADefausser; // Poubelle !
            }

            _modeDefausseSolo = false; 
            terminerTourEffectif();    // On finit enfin le tour !
            return;
        }

        //CAS B : LOGIQUE NORMALE (Prendre une carte) 
        Joueur* jActif = _controleur->getJoueurCourant();
        if (jActif && jActif->getNbCartesActives() < 4) {
            Carte* c = _controleur->getJeu()->getPlateauCentral()->prendreCarte(index);
            if (c) {
                jActif->prendreCarte(c);
                _plateauCentralWidget->marquerCartePrise();

                _aPrisCarteCeTour = true; 
                rafraichirUI();
            }
        }
    });

    connect(_plateauCentralWidget, &PlateauCentralWidget::groupeJetonChoisi, this, [this](int index) {
        qDebug() << "[JeuWidget] Signal reçu : groupeJetonChoisi" << index;
        if (_controleur->choisirGroupeJetons(index)) {
            rafraichirUI();
        }
    });

    // ZONE DES JOUEURS 
    qDebug() << "[JeuWidget] Création de la zone des joueurs...";
    _scrollAreaJoueurs = new QScrollArea(this);
    _scrollAreaJoueurs->setWidgetResizable(true);
    _scrollAreaJoueurs->setFrameShape(QFrame::NoFrame);

    _conteneurJoueurs = new QWidget();
    _layoutJoueurs = new QHBoxLayout(_conteneurJoueurs);
    _layoutJoueurs->setAlignment(Qt::AlignLeft);

    int nbJoueurs = _controleur->getJeu()->getNbJoueurs();
    qDebug() << "[JeuWidget] Nombre de joueurs à générer :" << nbJoueurs;

    for (int i = 0; i < nbJoueurs; ++i) {
        Joueur* joueur = _controleur->getJeu()->getJoueurParIndex(i);
        if (joueur == nullptr) {
            qDebug() << "[JeuWidget] ERREUR : Joueur à l'index" << i << "est nullptr !";
            continue;
        }

        QWidget* conteneurJoueurSeul = new QWidget(this);
        QVBoxLayout* layoutJoueurSeul = new QVBoxLayout(conteneurJoueurSeul);
        
        QLabel* nomLabel = new QLabel(QString::fromStdString(joueur->getNom()), this);
        nomLabel->setAlignment(Qt::AlignCenter);
        QFont f = nomLabel->font();
        f.setBold(true);
        nomLabel->setFont(f);
        layoutJoueurSeul->addWidget(nomLabel);

        qDebug() << "[JeuWidget] Création du PlateauJoueurWidget pour :" << QString::fromStdString(joueur->getNom());
        PlateauJoueurWidget* vraiPlateau = new PlateauJoueurWidget(joueur, this);
        layoutJoueurSeul->addWidget(vraiPlateau);
        
        _widgetsJoueurs.push_back(vraiPlateau);
        
        //CONNEXION : PLACEMENT JETON 
        connect(vraiPlateau, &PlateauJoueurWidget::requetePlacementJeton, this, [this, vraiPlateau](int x, int y, int index) {
            if (_controleur->placerJetonDepuisAttente(x, y, index)) {
                rafraichirUI(); 
            } else {
                vraiPlateau->rafraichirAffichage(); 
            }
        });

        //CONNEXION : PLACEMENT CUBE ANIMAL 
        connect(vraiPlateau, &PlateauJoueurWidget::actionPlacerCube, this, [this, vraiPlateau](int indexCarte, int x, int y) {
            if (_controleur->placerCubeAnimal(indexCarte, x, y)) {
                rafraichirUI(); 
            } else {
                vraiPlateau->rafraichirAffichage(); 
            }
        });
        
        _layoutJoueurs->addWidget(conteneurJoueurSeul);
    }

    _scrollAreaJoueurs->setWidget(_conteneurJoueurs);
    mainLayout->addWidget(_scrollAreaJoueurs);

    // CONNEXIONS BOUTONS 
    qDebug() << "[JeuWidget] Création des connexions boutons...";
    connect(_btnFinTour, &QPushButton::clicked, this, &JeuWidget::onBoutonFinTourClique);
    connect(_btnSauvegarderQuitter, &QPushButton::clicked, this, &JeuWidget::onBoutonSauvegarderQuitterClique);

    qDebug() << "[JeuWidget] Premier appel à rafraichirUI()...";
    if (_plateauCentralWidget) _plateauCentralWidget->resetTour();
    rafraichirUI();

    qDebug() << "[JeuWidget] FIN de la construction.";
}

void JeuWidget::rafraichirUI() {
    qDebug() << "[JeuWidget] --- rafraichirUI() DÉBUT ---";

    if (!_controleur || !_controleur->getJeu()) {
        qDebug() << "[JeuWidget] ERREUR: Backend absent pendant rafraichirUI !";
        return;
    }

    Joueur* jActif = _controleur->getJoueurCourant();
    _labelTourActuel->setText("Tour " + QString::number(_controleur->getJeu()->getTourActuel()));

    // MISE À JOUR DU PLATEAU CENTRAL
    if (_plateauCentralWidget) {
        _plateauCentralWidget->rafraichirAffichage();
    }

    //  MISE À JOUR DES JOUEURS 
    qDebug() << "[JeuWidget] Rafraîchissement des" << _widgetsJoueurs.size() << "joueurs...";
    int nbJoueurs = _controleur->getJeu()->getNbJoueurs();
    for (int i = 0; i < nbJoueurs; ++i) {
        Joueur* j = _controleur->getJeu()->getJoueurParIndex(i);
        if (i < _widgetsJoueurs.size() && _widgetsJoueurs[i] != nullptr) {
            bool estSonTour = (j == jActif);
            _widgetsJoueurs[i]->setTourActif(estSonTour);
            _widgetsJoueurs[i]->rafraichirAffichage();
        }
    }

    // GESTION DU DRAFT ESPRIT (Tour 1 Uniquement)
    bool besoinDraft = false;
    if (_controleur->getJeu()->isAvecEsprits() && _controleur->getJeu()->getTourActuel() == 1) {
        bool aUnEsprit = false;
        // On vérifie s'il a déjà pris un Esprit
        for (int i = 0; i < jActif->getNbCartesActives(); ++i) {
            if (dynamic_cast<CarteEsprit*>(jActif->getCarteActive(i)) != nullptr) {
                aUnEsprit = true; 
                break;
            }
        }
        if (!aUnEsprit) besoinDraft = true;
    }

    if (besoinDraft) {
        // On génère le draft s'il est vide
        if (_espritsEnCours[0] == nullptr) {
            _espritsEnCours[0] = _controleur->getJeu()->piocherEsprit();
            _espritsEnCours[1] = _controleur->getJeu()->piocherEsprit();
            _cartesDraftWidget[0]->setCarte(_espritsEnCours[0]);
            _cartesDraftWidget[1]->setCarte(_espritsEnCours[1]);
        }
        
        // On affiche la vue Draft et on bloque les boutons
        _zoneCentraleStack->setCurrentIndex(1); 
        _labelInstruction->setText("Action obligatoire : Choisissez un Esprit de la Nature !");
        _btnFinTour->setEnabled(false);
        return; // ON S'ARRÊTE LÀ, le joueur DOIT choisir.
    } else {
        // Sinon, on affiche le plateau normal
        _zoneCentraleStack->setCurrentIndex(0); 
    }

    int jetonsEnAttente = _controleur->getJoueurCourant()->getPlateau()->getNbJetonsEnAttente();

    if (jetonsEnAttente > 0) {
        _btnSauvegarderQuitter->setEnabled(false); // Impossible de sauvegarder en plein placement !
        _btnSauvegarderQuitter->setToolTip("Terminez de placer vos jetons avant de sauvegarder.");
    } else {
        _btnSauvegarderQuitter->setEnabled(true);
        _btnSauvegarderQuitter->setToolTip("");
    }

    //MISE À JOUR DE L'INSTRUCTION 
    EtatTour etat = _controleur->getEtatActuel();
    switch (etat) {
    case EtatTour::DEBUT_TOUR:
        _labelInstruction->setText("Action obligatoire : Choisissez un groupe de jetons.");
        _btnFinTour->setEnabled(false);
        break;
    case EtatTour::GROUPE_CHOISI:
        _labelInstruction->setText("Placez vos jetons sur votre plateau.");
        _btnFinTour->setEnabled(false);
        break;
    case EtatTour::JETONS_PLACES:
        _labelInstruction->setText("Vous pouvez poser un cube ou terminer votre tour.");
        _btnFinTour->setEnabled(true);
        break;
    default:
        _labelInstruction->setText("...");
        break;
    }

    qDebug() << "[JeuWidget] --- rafraichirUI() FIN ---";
}

void JeuWidget::onBoutonFinTourClique() {
    qDebug() << "[JeuWidget] Clic sur Fin de Tour";

    if (_controleur->getJeu()->getNbJoueurs() == 1 && !_aPrisCarteCeTour) {

        QMessageBox::StandardButton reponse = QMessageBox::question(this, "Mode Solo", 
            "Vous n'avez pris aucune carte ce tour.\nVoulez-vous défausser une carte de la rivière ?", 
            QMessageBox::Yes | QMessageBox::No);

        if (reponse == QMessageBox::Yes) {
            // On active l'état de défausse et on bloque le reste
            _modeDefausseSolo = true;
            _labelInstruction->setText("Mode Solo : Cliquez sur une carte de la rivière pour la détruire !");
            _labelInstruction->setStyleSheet("color: red; font-weight: bold;");
            _btnFinTour->setEnabled(false);
            return; // ON S'ARRÊTE LÀ. On attend que le joueur clique sur une carte !
        }
    }

    terminerTourEffectif();
}

void JeuWidget::terminerTourEffectif() {
    // On tente de passer le tour dans le contrôleur
    bool tourPasse = _controleur->passerTour();
    
    // Si le joueur vient de finir son action et que le jeu est considéré comme fini 
    // (sac vide ou plateau avec 2 cases ou moins)
    if (_controleur->getJeu()->estFini()) {
        qDebug() << "[JeuWidget] Fin de partie détectée ! Bascule immédiate sur l'écran des scores.";
        _partieTermineeSignalee = true;
        
        // Déclencher le calcul des scores (qui va remplir le détail des scores du joueur)
        _controleur->getJeu()->declencherScoringFinal();
        
        emit partieTerminee();
        return; 
    }

    // Si le jeu n'est pas fini, déroulement classique du changement de tour
    if (tourPasse) {
        if (_controleur->getJeu() && _controleur->getJeu()->getPlateauCentral()) {
            _controleur->getJeu()->getPlateauCentral()->remplirCartes();
        }

        if (_plateauCentralWidget) {
            _plateauCentralWidget->resetTour(); 
        }
        
        _aPrisCarteCeTour = false; 
        _modeDefausseSolo = false;
        _labelInstruction->setStyleSheet("color: gray; font-style: italic;");
        
        rafraichirUI(); // Rafraîchissement normal pour le joueur suivant
    }
}

void JeuWidget::onBoutonSauvegarderQuitterClique() {
    qDebug() << "[JeuWidget] Clic sur Sauvegarder & Quitter";
    QString cheminQt = QCoreApplication::applicationDirPath() + "/harmonies_save.txt";
    _controleur->sauvegarderPartie(cheminQt.toStdString());
    emit retourMenu();
}