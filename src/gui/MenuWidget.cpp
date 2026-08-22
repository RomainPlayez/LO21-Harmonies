#include "gui/MenuWidget.h"
#include <QPixmap>
#include <QPainter> // Pour dessiner l'image de fond
#include <QInputDialog>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent) {
    // Layout principal horizontal
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50); // Ajoute de l'espace sur les bords

    //  GAUCHE : L'image d'illustration 
    _imageLabelGauche = new QLabel(this);
    QPixmap pixmapIllustration(":/assets/main-page.png");
    if (!pixmapIllustration.isNull()) {
        _imageLabelGauche->setPixmap(pixmapIllustration.scaled(400, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        _imageLabelGauche->setText("Image main-page.png introuvable");
    }
    _imageLabelGauche->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(_imageLabelGauche, 1);

    //  DROITE : Le menu de configuration 
    QVBoxLayout* menuLayout = new QVBoxLayout();
    menuLayout->setAlignment(Qt::AlignVCenter); // Centre le menu verticalement

    // Le Logo officiel
    _logoLabel = new QLabel(this);
    QPixmap pixmapLogo(":/assets/logo.png");
    if (!pixmapLogo.isNull()) {
        _logoLabel->setPixmap(pixmapLogo.scaledToWidth(350, Qt::SmoothTransformation));
    } else {
        _logoLabel->setText("HARMONIES"); // Fallback si l'image manque
    }
    _logoLabel->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(_logoLabel);

    menuLayout->addSpacing(40); // Espace sous le logo

    // Options
    menuLayout->addWidget(new QLabel("Nombre de joueurs :", this));
    _comboNbJoueurs = new QComboBox(this);
    _comboNbJoueurs->addItems({"1 Joueur (Solo)", "2 Joueurs", "3 Joueurs", "4 Joueurs"});
    menuLayout->addWidget(_comboNbJoueurs);

    menuLayout->addWidget(new QLabel("Face du plateau :", this));
    _comboFacePlateau = new QComboBox(this);
    _comboFacePlateau->addItems({"Face A (Rivieres)", "Face B (Iles)"});
    menuLayout->addWidget(_comboFacePlateau);

    _checkCartesEsprit = new QCheckBox("Activer l'extension Cartes Esprits", this);
    menuLayout->addWidget(_checkCartesEsprit);

    menuLayout->addSpacing(30);

    // Boutons
    _btnNouvellePartie = new QPushButton("NOUVELLE PARTIE", this);
    _btnNouvellePartie->setMinimumHeight(50); // Un gros bouton pour jouer

    _btnChargerPartie = new QPushButton("CHARGER LA DERNIERE PARTIE", this);
    _btnChargerPartie->setMinimumHeight(40);

    // On désactive par défaut. La MainWindow mettra à jour ce bouton 
    _btnChargerPartie->setEnabled(false);
    _btnChargerPartie->setText("CHARGEMENT...");

    menuLayout->addWidget(_btnNouvellePartie);
    menuLayout->addWidget(_btnChargerPartie);

    // On ajoute le menu de droite au layout principal
    mainLayout->addLayout(menuLayout, 1); 

    //  CONNEXION DES SIGNAUX 
    connect(_btnNouvellePartie, &QPushButton::clicked, this, &MenuWidget::onBoutonNouvellePartieClique);
    connect(_btnChargerPartie, &QPushButton::clicked, this, &MenuWidget::chargerPartie);
}

void MenuWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap fond(":/assets/fond-page.png");

    if (!fond.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), fond);
    }

    QWidget::paintEvent(event);
}

void MenuWidget::onBoutonNouvellePartieClique() {
    int nbJoueurs = _comboNbJoueurs->currentIndex() + 1;
    FacePlateau face = (_comboFacePlateau->currentIndex() == 0) ? FacePlateau::FACE_A : FacePlateau::FACE_B;
    bool avecEsprits = _checkCartesEsprit->isChecked();

    std::vector<std::string> nomsJoueurs;
    for (int i = 0; i < nbJoueurs; ++i) {
        bool ok;
        QString nomDefaut = QString("Joueur %1").arg(i + 1);
        QString nom = QInputDialog::getText(this, "Nom des joueurs", 
                                            QString("Prénom du joueur %1 :").arg(i + 1), 
                                            QLineEdit::Normal, nomDefaut, &ok);
        
        if (!ok) return; // Si le joueur clique sur "Annuler", on reste sur le menu
        
        if (nom.trimmed().isEmpty()) nom = nomDefaut;
        nomsJoueurs.push_back(nom.trimmed().toStdString());
    }

    emit demarrerNouvellePartie(nomsJoueurs, face, avecEsprits);
}

void MenuWidget::rafraichirBoutonChargement(const std::string& cheminFichier) {
    if (ControleurPartie::sauvegardeDisponible(cheminFichier)) {
        _btnChargerPartie->setEnabled(true);
        _btnChargerPartie->setText("CHARGER LA DERNIERE PARTIE");
    } else {
        _btnChargerPartie->setEnabled(false);
        _btnChargerPartie->setText("AUCUNE SAUVEGARDE DISPONIBLE");
    }
}