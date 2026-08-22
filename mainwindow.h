#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QCloseEvent>
#include "gui/MenuWidget.h"
#include "gui/JeuWidget.h"
#include "gui/ScoreWidget.h"
#include "core/ControleurPartie.h"
#include <vector>
#include <string>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Sauvegarde automatiquement la partie en cours (Memento) si l'utilisateur ferme la fenêtre sans passer par "Sauvegarder & Quitter"
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Ce slot est appelé quand le menu émet le signal "Jouer"
    void onLancerPartie(std::vector<std::string> nomsJoueurs, FacePlateau face, bool avecEsprits);

    // Ce slot est appelé quand le menu émet le signal "Charger la dernière partie"
    void onChargerPartie();

    // Appelé quand on doit revenir à l'écran d'accueil
    void onRetourMenu();

    // Appelé quand la partie en cours se termine : affiche l'écran des scores
    void onPartieTerminee();

private:
    std::string _cheminSauvegarde;

    // Nettoie une éventuelle partie déjà en cours avant d'en démarrer/charger une nouvelle, pour éviter les fuites.
    void nettoyerPartieEnCours();


    QStackedWidget* _stackedWidget;
    MenuWidget* _menuWidget;
    JeuWidget* _jeuWidget;
    ScoreWidget* _scoreWidget;

    ControleurPartie* _controleur; // Le backend !
};