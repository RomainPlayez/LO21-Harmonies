#include <QCoreApplication>
#include "mainwindow.h"
#include <vector>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), _jeuWidget(nullptr), _scoreWidget(nullptr), _controleur(nullptr)
{
    QString cheminQt = QCoreApplication::applicationDirPath() + "/harmonies_save.txt";
    _cheminSauvegarde = cheminQt.toStdString();

    _stackedWidget = new QStackedWidget(this);
    setCentralWidget(_stackedWidget); 

    _menuWidget = new MenuWidget(this);
    
    // On met à jour le bouton de menu avec le bon chemin
    _menuWidget->rafraichirBoutonChargement(_cheminSauvegarde);

    _stackedWidget->addWidget(_menuWidget);

    connect(_menuWidget, &MenuWidget::demarrerNouvellePartie, this, &MainWindow::onLancerPartie);
    connect(_menuWidget, &MenuWidget::chargerPartie, this, &MainWindow::onChargerPartie);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (_controleur != nullptr && _scoreWidget == nullptr) {
        // On passe le chemin absolu
        _controleur->sauvegarderPartie(_cheminSauvegarde);
    }
    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow() {
    // Nettoyage de la mémoire du backend
    if (_controleur != nullptr) {
        delete _controleur;
    }
}

void MainWindow::onLancerPartie(std::vector<std::string> nomsJoueurs, FacePlateau face, bool avecEsprits) {
    // On nettoie une éventuelle partie précédente
    nettoyerPartieEnCours();

    _controleur = new ControleurPartie(nomsJoueurs, face, avecEsprits);
    _controleur->demarrerPartie();

    _jeuWidget = new JeuWidget(_controleur, this);

    _stackedWidget->addWidget(_jeuWidget);

    // Bouton "Sauvegarder & Quitter" et fin de partie
    connect(_jeuWidget, &JeuWidget::retourMenu, this, &MainWindow::onRetourMenu);
    connect(_jeuWidget, &JeuWidget::partieTerminee, this, &MainWindow::onPartieTerminee);

    _stackedWidget->setCurrentWidget(_jeuWidget);
}

void MainWindow::onChargerPartie() {
    nettoyerPartieEnCours();
    std::vector<std::string> nomsParDefaut = {"Joueur 1"};
    _controleur = new ControleurPartie(nomsParDefaut, FacePlateau::FACE_A, false);

    if (!_controleur->chargerPartie(_cheminSauvegarde)) {
        delete _controleur;
        _controleur = nullptr;
        return;
    }

    // On crée la page de jeu à partir de la partie restaurée
    _jeuWidget = new JeuWidget(_controleur, this);
    _stackedWidget->addWidget(_jeuWidget);

    connect(_jeuWidget, &JeuWidget::retourMenu, this, &MainWindow::onRetourMenu);
    connect(_jeuWidget, &JeuWidget::partieTerminee, this, &MainWindow::onPartieTerminee);

    _stackedWidget->setCurrentWidget(_jeuWidget);
}

void MainWindow::nettoyerPartieEnCours() {
    if (_scoreWidget != nullptr) {
        _stackedWidget->removeWidget(_scoreWidget);
        delete _scoreWidget;
        _scoreWidget = nullptr;
    }
    if (_jeuWidget != nullptr) {
        _stackedWidget->removeWidget(_jeuWidget);
        delete _jeuWidget;
        _jeuWidget = nullptr;
    }
    if (_controleur != nullptr) {
        delete _controleur;
        _controleur = nullptr;
    }
}

void MainWindow::onRetourMenu() {
    _menuWidget->rafraichirBoutonChargement(_cheminSauvegarde);
    _stackedWidget->setCurrentWidget(_menuWidget);
}

void MainWindow::onPartieTerminee() {
    if (_scoreWidget != nullptr) {
        _stackedWidget->removeWidget(_scoreWidget);
        delete _scoreWidget;
        _scoreWidget = nullptr;
    }

    _scoreWidget = new ScoreWidget(_controleur, this);
    _stackedWidget->addWidget(_scoreWidget);

    connect(_scoreWidget, &ScoreWidget::retourMenu, this, &MainWindow::onRetourMenu);

    _stackedWidget->setCurrentWidget(_scoreWidget);
}