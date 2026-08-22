#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include "core/ControleurPartie.h"

/**
 * @class ScoreWidget
 * @brief Écran de fin de partie : affiche le classement final des joueurs
 * le nom + le total des points et on peut revenir au menu principal.
 */
class ScoreWidget : public QWidget {
    Q_OBJECT

public:
    explicit ScoreWidget(ControleurPartie* controleur, QWidget *parent = nullptr);

    // Reconstruit entièrement le tableau des scores à partir du contrôleur.
    // Utile si le widget est ré-affiché pour une nouvelle partie.
    void rafraichirScores();

signals:
    // Émis lorsque l'utilisateur souhaite retourner à l'écran d'accueil.
    void retourMenu();

protected:
    // l'image de fond de la page d'accueil
    void paintEvent(QPaintEvent *event) override;

private:
    ControleurPartie* _controleur;

    QLabel* _labelTitre;
    QLabel* _labelVainqueur;
    QTableWidget* _tableScores;
    QPushButton* _btnRetourMenu;
    QWidget* _resumeSolo;
    QLabel* _labelSoloJoueur;
    QLabel* _labelSoloPoints;
    QLabel* _labelSoloSoleils;
    QWidget* creerBlocSolo(const QString& titre, QLabel** labelValeur);
};

#endif // SCOREWIDGET_H