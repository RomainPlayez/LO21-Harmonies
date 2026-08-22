#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QPushButton>
#include <map>
#include "core/Joueur.h"
#include "gui/HexagonItem.h"

class CarteWidget;
class CarteAnimal;    

class PlateauJoueurWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlateauJoueurWidget(Joueur* joueur, QWidget *parent = nullptr);

    void rafraichirAffichage();
    
    // Permet d'activer ou masquer l'interactivité selon le tour
    void setTourActif(bool actif);

    void deselectionnerTout(); 

signals:
    // Signal émis quand on clique sur le plateau
    void requetePlacementJeton(int x, int y, int indexAttente);
    
    // Signal émis quand on sélectionne un jeton en attente
    void jetonAttenteSelectionne(int index); 

    void actionPlacerCube(int indexCarte, int x, int y);

private slots:
    void onCaseCliquee(int x, int y);
    void onJetonAttenteClique(int index); // Gère le clic sur un jeton en attente
    void onRequetePlacementCube(Carte* carte);
    
protected:
    void mousePressEvent(QMouseEvent *event) override; 

private:
    Joueur* _joueur; 
    QGraphicsView* _vue;
    QGraphicsScene* _scene;
    QWidget* _conteneurAttente; 
    
    CarteWidget* _widgetCartes[4];      // 4 emplacements max pour les cartes actives
    QPushButton* _btnsAttente[3];  // 3 boutons pour les jetons en attente
    int _indexJetonSelectionne;    // Mémorise quel jeton on a cliqué
    
    std::map<int, HexagonItem*> _casesGraphiques;

    void initialiserGrille();
    void construireLayout(); // Fonction pour ranger tout ça proprement
    QString couleurVersTexte(CouleurJeton c);
    
    int _indexCarteSelectionnee = -1;

};