#include "gui/CarteWidget.h"
#include <QPainter>
#include <QPixmap>
#include <QCursor>
#include <QGraphicsDropShadowEffect>

// --- VARIABLES GLOBALES DE MAPPING (CARTES) ---
double TAILLE_CARTE_W = 125.0; 
double TAILLE_CARTE_H = 185.0; 

double ECHELLE_CUBE_CARTE = 0.50; 
double ORIGINE_CUBE_X = 95.0; 
double ORIGINE_CUBE_Y = 2.0; 
double OFFSET_CUBE_Y = 28.0; 

CarteWidget::CarteWidget(QWidget *parent)
    : QWidget(parent), _carte(nullptr)
{
    setFixedSize(TAILLE_CARTE_W, TAILLE_CARTE_H);
    setMouseTracking(true); 
    
}

void CarteWidget::setCarte(Carte* carte) {
    _carte = carte;
    if (_carte != nullptr) {
        // Charge l'image de la carte
        QString cheminCarte = ":/assets/" + QString::fromStdString(_carte->getImageRef());
        _pixCarteCachee = QPixmap(cheminCarte);

        if (dynamic_cast<CarteEsprit*>(_carte) != nullptr) {
            _pixCubeCache = QPixmap(":/assets/cube_esprit.png");
        } else {
            _pixCubeCache = QPixmap(":/assets/cube_animal.png");
        }
    } else {
        _pixCarteCachee = QPixmap();
        _pixCubeCache = QPixmap(); // Pas de cube sur un emplacement vide
    }
    update();
}

void CarteWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (_carte == nullptr) {
        painter.setPen(QPen(QColor("#B0A896"), 2, Qt::DashLine));
        painter.setBrush(QColor(255, 255, 255, 100));
        painter.drawRoundedRect(1, 1, width() - 2, height() - 2, 8, 8);
        
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, "Emplacement\nLibre");
        return;
    }

    // DESSINER LA CARTE EN FOND
    if (!_pixCarteCachee.isNull()) {
        double scale = _estSurvole ? 1.05 : 1.0; 
        painter.drawPixmap(0, 0, width() * scale, height() * scale, _pixCarteCachee);
    }

    // DESSINER LES CUBES EMPILÉS
    int nbCubes = _carte->getNbCubesActuels();
    if (nbCubes > 0 && !_pixCubeCache.isNull() && !_estModeRiviere) {
        double cw = _pixCubeCache.width() * ECHELLE_CUBE_CARTE;
        double ch = _pixCubeCache.height() * ECHELLE_CUBE_CARTE;

        for (int i = 0; i < nbCubes; ++i) {
            double cx = ORIGINE_CUBE_X;
            double cy = ORIGINE_CUBE_Y + (i * OFFSET_CUBE_Y);

            if (i == nbCubes - 1) {
                _rectCubeCliquable = QRect(cx, cy, cw, ch);
            }
            painter.drawPixmap(cx, cy, cw, ch, _pixCubeCache);
        }
    }

    // ENCADRÉ DE SÉLECTION JAUNE SUR LE CUBE 
    // On vérifie qu'il y a au moins un cube à encadrer
    if (_estSelectionnee && nbCubes > 0) { 
        painter.setPen(QPen(QColor("#F1C40F"), 4)); // Jaune or
        painter.setBrush(Qt::NoBrush);
        
        painter.drawRect(_rectCubeCliquable.adjusted(-2, -2, 2, 2)); 
    }
}

// GESTION DU CLIC SOURIS
void CarteWidget::mousePressEvent(QMouseEvent *event) {
    if (_estModeRiviere && _carte != nullptr) {
        emit carteRiviereCliquee(_indexRiviere);
    } else if (_carte != nullptr && _carte->getNbCubesActuels() > 0) {
        if (_rectCubeCliquable.contains(event->pos())) {
            emit requetePlacementCube(_carte);
        }
    }
    QWidget::mousePressEvent(event);
}

// GESTION DU CURSEUR 
void CarteWidget::mouseMoveEvent(QMouseEvent *event) {
    if (_estModeRiviere && _carte != nullptr && _hoverAutorise) {
        if (!_estSurvole) {
            _estSurvole = true;
            QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
            shadow->setBlurRadius(15);
            shadow->setColor(QColor(0, 0, 0, 150));
            shadow->setOffset(0, 5);
            setGraphicsEffect(shadow);
            update();
        }
    }
    QWidget::mouseMoveEvent(event);
}

void CarteWidget::setModeRiviere(bool actif, int index) {
    _estModeRiviere = actif;
    _indexRiviere = index;
}

void CarteWidget::leaveEvent(QEvent *event) {
    if (_estModeRiviere && _estSurvole) {
        _estSurvole = false;
        setGraphicsEffect(nullptr);
        update();
    }
    QWidget::leaveEvent(event);
}

void CarteWidget::setHoverAutorise(bool autorise) {
    _hoverAutorise = autorise;
    // Si on coupe le hover alors que la souris était déjà dessus, on nettoie l'ombre immédiatement
    if (!_hoverAutorise && _estSurvole) {
        _estSurvole = false;
        setGraphicsEffect(nullptr);
        update();
    }
}

void CarteWidget::setSelectionnee(bool selectionnee) {
    _estSelectionnee = selectionnee;
    update(); // Force le redessin
}

