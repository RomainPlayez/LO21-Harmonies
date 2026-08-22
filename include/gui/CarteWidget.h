#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include "core/Carte.h" 
#include "core/CarteAnimal.h"
#include "core/CarteEsprit.h" 

class CarteWidget : public QWidget {
    Q_OBJECT
public:
    explicit CarteWidget(QWidget *parent = nullptr);
    
    // Met à jour la carte à afficher
    void setCarte(Carte* carte);

    void setModeRiviere(bool actif, int index = -1); // Active le comportement Rivière

    void setHoverAutorise(bool autorise);

    void setSelectionnee(bool selectionnee);

signals:
    // Émis quand le joueur clique spécifiquement sur le bon cube
    void requetePlacementCube(Carte* carte);
    void carteRiviereCliquee(int index);

protected:
    // C'est ici qu'on dessine la carte et les cubes
    void paintEvent(QPaintEvent *event) override;
    
    // C'est ici qu'on vérifie si on a cliqué sur le cube
    void mousePressEvent(QMouseEvent *event) override;
    
    void mouseMoveEvent(QMouseEvent *event) override;

    void leaveEvent(QEvent *event) override;

private:
    Carte* _carte;
    QRect _rectCubeCliquable; // La "Hitbox" invisible du cube tout en bas

    QPixmap _pixCarteCachee; 
    QPixmap _pixCubeCache;

    bool _estModeRiviere = false;
    int _indexRiviere = -1;
    bool _estSurvole = false;

    bool _hoverAutorise = true; // Par défaut, le hover est permis

    bool _estSelectionnee = false;
};