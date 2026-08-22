#include "gui/PlateauCentralWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

// Variables globales
double TAILLE_PLATEAU_CENTRAL = 0.8; 

static QString getNomFichierCouleur(CouleurJeton c) {
    switch(c) {
        case CouleurJeton::GRIS:   return "gris";
        case CouleurJeton::ROUGE:  return "rouge";
        case CouleurJeton::MARRON: return "marron";
        case CouleurJeton::VERT:   return "vert";
        case CouleurJeton::JAUNE:  return "jaune";
        case CouleurJeton::BLEU:   return "bleu";
        default: return "gris";
    }
}

class GroupeJetonsItem : public QGraphicsObject {
public:
    int index;
    const GroupeJetons* groupeRef;
    QObject* parentWidget; 
    
    GroupeJetonsItem(int idx, const GroupeJetons* ref, QObject* parent) 
        : index(idx), groupeRef(ref), parentWidget(parent) {
        setAcceptHoverEvents(true);
    }
    
    QRectF boundingRect() const override { return QRectF(-50, -50, 100, 100); }
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        if (!groupeRef || groupeRef->estVide()) return;
        
        QPointF offsets[3] = { QPointF(0, -10), QPointF(-15, 4), QPointF(15, 4) };
        for (int i = 0; i < groupeRef->getNbJetons(); ++i) {
            const Jeton* j = groupeRef->getJeton(i); 
            if (j != nullptr) {
                QString chemin = ":/assets/" + getNomFichierCouleur(j->getCouleur()) + ".png";
                QPixmap pix(chemin);
                painter->drawPixmap(offsets[i].x() - 22.5, offsets[i].y() - 22.5, 70, 70, pix);
            }
        }
    }
    
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        QMetaObject::invokeMethod(parentWidget, "groupeJetonChoisi", Q_ARG(int, index));
    }
};

PlateauCentralWidget::PlateauCentralWidget(PlateauCentral* modele, bool modeSolo, QWidget *parent) 
    : QWidget(parent), _modeSolo(modeSolo), _modele(modele) 
{
    QHBoxLayout* layoutPrincipal = new QHBoxLayout(this);

    // LA RIVIÈRE DE CARTES
    _conteneurCartes = new QWidget(this);
    QHBoxLayout* layoutCartes = new QHBoxLayout(_conteneurCartes);
    for (int i = 0; i < 5; ++i) {
        _cartesRiviere[i] = new CarteWidget(this);
        _cartesRiviere[i]->setModeRiviere(true, i);
        connect(_cartesRiviere[i], &CarteWidget::carteRiviereCliquee, this, [this](int idx) {
            if (!_cartePriseCeTour) {
                emit carteChoisie(idx);
            }
        });
        layoutCartes->addWidget(_cartesRiviere[i]);
    }

    // LE PLATEAU DES JETONS (Adaptatif Solo/Multi)
    _sceneJetons = new QGraphicsScene(this);
    _vueJetons = new QGraphicsView(_sceneJetons, this);
    _vueJetons->setStyleSheet("background: transparent; border: none;");
    
    bool estSolo = (_modele->getTailleActive() == 3);
    QString imagePlateau = estSolo ? ":/assets/pioche-jetons-solo.png" : ":/assets/pioche-jetons.png";
    
    QGraphicsPixmapItem* fondPlateau = _sceneJetons->addPixmap(QPixmap(imagePlateau));
    fondPlateau->setScale(TAILLE_PLATEAU_CENTRAL);
    
    layoutPrincipal->addWidget(_conteneurCartes, 6);
    layoutPrincipal->addWidget(_vueJetons, 4);
}

void PlateauCentralWidget::rafraichirAffichage() {
    if (_modele == nullptr) return;

    for(int i = 0; i < 5; ++i) {
        const CarteAnimal* carte = _modele->getCarteRiviere(i);
        _cartesRiviere[i]->setCarte(const_cast<CarteAnimal*>(carte));
        _cartesRiviere[i]->setHoverAutorise(!_cartePriseCeTour);
    }
    
    for (QGraphicsItem* item : _sceneJetons->items()) {
        if (dynamic_cast<GroupeJetonsItem*>(item)) {
            _sceneJetons->removeItem(item);
            delete item;
        }
    }

    // LES DEUX SETS DE COORDONNÉES
    QPointF posMulti[5] = { QPointF(135, 45), QPointF(215, 120), QPointF(160, 215), QPointF(55, 200), QPointF(40, 90) };
    QPointF posSolo[3]  = { QPointF(120, 120), QPointF(205, 140), QPointF(140, 200) }; 

    bool estSolo = (_modele->getTailleActive() == 3);
    QPointF* posGroupes = estSolo ? posSolo : posMulti;

    // On boucle jusqu'à _tailleActive (3 ou 5)
    for(int i = 0; i < _modele->getTailleActive(); ++i) {
        const GroupeJetons* g = _modele->getOffreJetons(i);
        if (g != nullptr && !g->estVide()) {
            GroupeJetonsItem* item = new GroupeJetonsItem(i, g, this); 
            item->setPos(posGroupes[i]);
            _sceneJetons->addItem(item);
        }
    }
}

void PlateauCentralWidget::setTourActif(bool actif) {
    if (!actif) {
        _conteneurCartes->setStyleSheet("background-color: transparent;");
        _cartePriseCeTour = false;
        _groupePrisCeTour = false;
    } else {
        if (!_cartePriseCeTour) {
            _conteneurCartes->setStyleSheet("background-color: rgba(46, 204, 113, 0.2); border-radius: 10px;");
        } else {
            _conteneurCartes->setStyleSheet("background-color: rgba(231, 76, 60, 0.2); border-radius: 10px;");
        }
    }
}

void PlateauCentralWidget::resetTour() {
    _cartePriseCeTour = false;
    _groupePrisCeTour = false;
    setTourActif(true);
}

void PlateauCentralWidget::marquerCartePrise() {
    _cartePriseCeTour = true;
    setTourActif(true);
}