#pragma once

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include "core/ControleurPartie.h"

class MenuWidget : public QWidget {
    Q_OBJECT

public:
    explicit MenuWidget(QWidget *parent = nullptr);
    void rafraichirBoutonChargement(const std::string& cheminFichier);

signals:
    void demarrerNouvellePartie(std::vector<std::string> nomsJoueurs, FacePlateau face, bool avecEsprits);
    void chargerPartie();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onBoutonNouvellePartieClique();

private:
    QLabel* _imageLabelGauche;
    QLabel* _logoLabel;
    QComboBox* _comboNbJoueurs;
    QComboBox* _comboFacePlateau;
    QCheckBox* _checkCartesEsprit;
    QPushButton* _btnNouvellePartie;
    QPushButton* _btnChargerPartie;
};