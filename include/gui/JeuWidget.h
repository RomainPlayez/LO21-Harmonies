#pragma once

#include <QStackedWidget>
#include "core/CarteEsprit.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>
#include "gui/CarteWidget.h"

// Forward declarations pour accélérer la compilation
class ControleurPartie;
class PlateauJoueurWidget;
class PlateauCentralWidget;
class CarteWidget;

class JeuWidget : public QWidget {
    Q_OBJECT

public:
    explicit JeuWidget(ControleurPartie* controleur, QWidget *parent = nullptr);
    ~JeuWidget() = default;

    void rafraichirUI();

signals:
    void partieTerminee();
    void retourMenu();

private slots:
    void onBoutonFinTourClique();
    void onBoutonSauvegarderQuitterClique();

private:
    ControleurPartie* _controleur;
    bool _partieTermineeSignalee = false;

    //  Widgets de la Barre d'Action 
    QLabel* _labelTourActuel;
    QLabel* _labelInstruction;
    QPushButton* _btnFinTour;
    QPushButton* _btnSauvegarderQuitter;

    //  Widgets Centraux et Joueurs 
    QStackedWidget* _zoneCentraleStack;
    PlateauCentralWidget* _plateauCentralWidget;

    QScrollArea* _scrollAreaJoueurs;
    QWidget* _conteneurJoueurs;
    QHBoxLayout* _layoutJoueurs;
    std::vector<PlateauJoueurWidget*> _widgetsJoueurs;

    QWidget* _draftEspritWidget;        // La vue du draft
    CarteWidget* _cartesDraftWidget[2]; // Les deux cartes à choisir
    CarteEsprit* _espritsEnCours[2] = {nullptr, nullptr}; // Les données du draft

    bool _aPrisCarteCeTour = false;
    bool _modeDefausseSolo = false;
    void terminerTourEffectif();
};