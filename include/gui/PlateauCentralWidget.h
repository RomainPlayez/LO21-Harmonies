#pragma once
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "gui/CarteWidget.h"
#include "core/PlateauCentral.h"

class PlateauCentralWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlateauCentralWidget(PlateauCentral* modele, bool modeSolo = false, QWidget *parent = nullptr);
    void rafraichirAffichage();
    void setTourActif(bool actif); // Gère les fonds verts/rouges

    bool _cartePriseCeTour = false;
    bool _groupePrisCeTour = false;

    void marquerCartePrise(); // Verrouille et met en rouge
    void resetTour();         // Déverrouille et met en vert

signals:
    void carteChoisie(int indexCarte);
    void groupeJetonChoisi(int indexGroupe);

private:
    PlateauCentral* _modele;
    CarteWidget* _cartesRiviere[5];
    QGraphicsView* _vueJetons;
    QGraphicsScene* _sceneJetons;
    
    QWidget* _conteneurCartes; // Pour colorer le fond

    bool _modeSolo;
};