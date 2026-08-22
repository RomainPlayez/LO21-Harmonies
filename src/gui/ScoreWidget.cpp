#include "gui/ScoreWidget.h"
#include "core/Jeu.h"
#include "core/Joueur.h"

#include <QFont>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <algorithm>
#include <vector>

ScoreWidget::ScoreWidget(ControleurPartie* controleur, QWidget *parent)
    : QWidget(parent), _controleur(controleur)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(80, 50, 80, 50);

    mainLayout->addStretch(1);

    // Titre
    _labelTitre = new QLabel("FIN DE LA PARTIE", this);
    QFont fontTitre = _labelTitre->font();
    fontTitre.setBold(true);
    fontTitre.setPointSize(26);
    _labelTitre->setFont(fontTitre);
    _labelTitre->setAlignment(Qt::AlignCenter);
    _labelTitre->setStyleSheet("color: #2C3E50;");
    mainLayout->addWidget(_labelTitre);

    mainLayout->addSpacing(10);

    // Sous-titre : le nom du vainqueur
    _labelVainqueur = new QLabel(this);
    QFont fontVainqueur = _labelVainqueur->font();
    fontVainqueur.setItalic(true);
    fontVainqueur.setPointSize(14);
    _labelVainqueur->setFont(fontVainqueur);
    _labelVainqueur->setAlignment(Qt::AlignCenter);
    _labelVainqueur->setStyleSheet("color: #8E6C46;");
    mainLayout->addWidget(_labelVainqueur);

    mainLayout->addSpacing(30);

    // LE TABLEAU DÉTAILLÉ
    _tableScores = new QTableWidget(this);
    _tableScores->setColumnCount(10);
    _tableScores->setHorizontalHeaderLabels({
        "Rang", "Joueur", "Arbres", "Montagnes", "Bât.", "Champs", "Eau", "Animaux", "Esprits", "Total"
    });
    
    // Ajustement de la taille des colonnes
    _tableScores->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Les colonnes Rang, Joueur et Total peuvent être un peu plus larges
    _tableScores->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    _tableScores->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
    
    _tableScores->verticalHeader()->setVisible(false);
    _tableScores->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _tableScores->setSelectionMode(QAbstractItemView::NoSelection);
    _tableScores->setFocusPolicy(Qt::NoFocus);
    _tableScores->setAlternatingRowColors(true);
    _tableScores->setStyleSheet(
        "QTableWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "   gridline-color: #C9C2B2;"
        "   font-size: 14px;"
        "   selection-background-color: transparent;"
        "   selection-color: #2C3E50;"
        "}"
        "QTableWidget::item { padding: 10px; }"
        "QHeaderView::section {"
        "   background-color: #2C3E50;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   border: none;"
        "}"
        "QTableWidget::item:alternate { background-color: rgba(244, 241, 234, 160); }"
    );
    _tableScores->setMouseTracking(false);
    _tableScores->viewport()->setAutoFillBackground(false);
    _tableScores->setMinimumHeight(260);
    mainLayout->addWidget(_tableScores);

    // Pour le mode solo
    _resumeSolo = new QWidget(this);
    _resumeSolo->setStyleSheet(
        "background-color: rgba(255, 255, 255, 140);"
        "border: 1px solid #B0A896;"
        "border-radius: 10px;"
    );
    QHBoxLayout* soloLayout = new QHBoxLayout(_resumeSolo);
    soloLayout->setContentsMargins(30, 25, 30, 25);
    soloLayout->setSpacing(20);
    soloLayout->addWidget(creerBlocSolo("JOUEUR", &_labelSoloJoueur));
    soloLayout->addWidget(creerBlocSolo("POINTS", &_labelSoloPoints));
    soloLayout->addWidget(creerBlocSolo("SOLEILS", &_labelSoloSoleils));
    _resumeSolo->setVisible(false);
    mainLayout->addWidget(_resumeSolo);

    mainLayout->addSpacing(30);

    // Retour menu
    _btnRetourMenu = new QPushButton("Retour au menu principal", this);
    _btnRetourMenu->setMinimumHeight(45);
    _btnRetourMenu->setStyleSheet(
        "background-color: #2C3E50; color: white; font-weight: bold;"
        "padding: 10px; border-radius: 6px;"
    );

    QHBoxLayout* boutonLayout = new QHBoxLayout();
    boutonLayout->addStretch();
    boutonLayout->addWidget(_btnRetourMenu);
    boutonLayout->addStretch();
    mainLayout->addLayout(boutonLayout);
    mainLayout->addStretch(1);

    connect(_btnRetourMenu, &QPushButton::clicked, this, &ScoreWidget::retourMenu);

    rafraichirScores();
}

void ScoreWidget::rafraichirScores() {
    if (_controleur == nullptr || _controleur->getJeu() == nullptr) {
        return;
    }

    Jeu* jeu = _controleur->getJeu();
    int nbJoueurs = jeu->getNbJoueurs();

    std::vector<Joueur*> classement;
    for (int i = 0; i < nbJoueurs; ++i) {
        classement.push_back(jeu->getJoueurParIndex(i));
    }
    std::sort(classement.begin(), classement.end(), [](Joueur* a, Joueur* b) {
        return a->getScore() > b->getScore();
    });

    bool modeSolo = (classement.size() == 1);
    _tableScores->setVisible(!modeSolo);
    _resumeSolo->setVisible(modeSolo);

    if (modeSolo) {
        Joueur* j = classement.front();
        _labelSoloJoueur->setText(QString::fromStdString(j->getNom()));
        _labelSoloPoints->setText(QString::number(j->getScore()) + " pts");
        _labelSoloSoleils->setText(QString::number(j->getSoleils()) + " \xE2\x98\x80"); 
        _labelSoloSoleils->setStyleSheet("color: #D35400;"); 
    } else {
        _tableScores->setRowCount(static_cast<int>(classement.size()));

        for (int rang = 0; rang < static_cast<int>(classement.size()); ++rang) {
            Joueur* j = classement[static_cast<size_t>(rang)];

            QTableWidgetItem* itemRang = new QTableWidgetItem(QString::number(rang + 1));
            QTableWidgetItem* itemNom  = new QTableWidgetItem(QString::fromStdString(j->getNom()));
            QTableWidgetItem* itemTotal = new QTableWidgetItem(QString("%1 pts").arg(j->getScore()));

            itemRang->setTextAlignment(Qt::AlignCenter);
            itemNom->setTextAlignment(Qt::AlignCenter);
            itemTotal->setTextAlignment(Qt::AlignCenter);

            // Mise en valeur du vainqueur
            if (rang == 0) {
                QFont fontGagnant = itemNom->font();
                fontGagnant.setBold(true);
                itemRang->setFont(fontGagnant);
                itemNom->setFont(fontGagnant);
                itemTotal->setFont(fontGagnant);

                QBrush fondGagnant(QColor("#F1D9A0"));
                itemRang->setBackground(fondGagnant);
                itemNom->setBackground(fondGagnant);
                itemTotal->setBackground(fondGagnant);
            }

            _tableScores->setItem(rang, 0, itemRang);
            _tableScores->setItem(rang, 1, itemNom);

            // REMPLISSAGE DES 7 COLONNES DE DÉTAILS 
            for(int col = 0; col < 7; ++col) {
                QString valeur = QString::number(j->getScoreDetail(col));
                
                QTableWidgetItem* itemDetail = new QTableWidgetItem(valeur);
                itemDetail->setTextAlignment(Qt::AlignCenter);
                
                if (rang == 0) itemDetail->setBackground(QBrush(QColor("#F1D9A0"))); 
                
                _tableScores->setItem(rang, col + 2, itemDetail);
            }
            _tableScores->setItem(rang, 9, itemTotal);
        }
        _tableScores->setCurrentCell(-1, -1);
        _tableScores->clearSelection();
    }

    if (!classement.empty()) {
        if (modeSolo) {
            _labelVainqueur->setText(
                QString::fromStdString(classement.front()->getNom())
                + " termine la partie avec " + QString::number(classement.front()->getScore())
                + " points !"
            );
        } else {
            _labelVainqueur->setText(
                "Bravo à " + QString::fromStdString(classement.front()->getNom())
                + " qui remporte la partie !"
            );
        }
    } else {
        _labelVainqueur->clear();
    }
}

QWidget* ScoreWidget::creerBlocSolo(const QString& titre, QLabel** labelValeur) {
    QWidget* bloc = new QWidget(_resumeSolo);
    QVBoxLayout* blocLayout = new QVBoxLayout(bloc);
    blocLayout->setContentsMargins(0, 0, 0, 0);
    blocLayout->setSpacing(6);

    QLabel* labelTitre = new QLabel(titre, bloc);
    labelTitre->setAlignment(Qt::AlignCenter);
    labelTitre->setStyleSheet("color: #8E6C46; font-weight: bold; font-size: 12px;");

    QLabel* labelValeurWidget = new QLabel("—", bloc);
    QFont fontValeur = labelValeurWidget->font();
    fontValeur.setBold(true);
    fontValeur.setPointSize(20);
    labelValeurWidget->setFont(fontValeur);
    labelValeurWidget->setAlignment(Qt::AlignCenter);
    labelValeurWidget->setStyleSheet("color: #2C3E50;");

    blocLayout->addWidget(labelTitre);
    blocLayout->addWidget(labelValeurWidget);

    *labelValeur = labelValeurWidget;
    return bloc;
}

void ScoreWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap fond(":/assets/fond-page.png");
    if (!fond.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), fond);
    }
    QWidget::paintEvent(event);
}