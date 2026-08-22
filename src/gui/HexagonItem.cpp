#include "gui/HexagonItem.h"

HexagonItem::HexagonItem(int coordX, int coordY, QGraphicsItem *parent)
    : QGraphicsObject(parent), _x(coordX), _y(coordY), 
      _nbCubes(0), _echelleJetons(1.0), _echelleCubes(1.0), _echelleSurbrillance(1.0),
      _estEnSurbrillanceLogique(false), 
      _estSurvole(false), _typeHover(TypeHover::DEFAUT_JAUNE) 
{
    _aUnCube = false;
    _pixmapCubeCache = QPixmap(":/assets/cube_animal.png");

    _hoverJaune = QPixmap(":/assets/hover-jaune.png");
    _hoverVert  = QPixmap(":/assets/hover-vert.png");
    _hoverRouge = QPixmap(":/assets/hover-rouge.png");
    
    _surbrillance = QPixmap(":/assets/surbrillance_hexagone.png"); 
    
    setAcceptedMouseButtons(Qt::LeftButton);
    
    setAcceptHoverEvents(true); 
}

QRectF HexagonItem::boundingRect() const {
    return QRectF(-45, -45, 90, 90); 
}

void HexagonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);

    //DESSINER LA SURBRILLANCE LOGIQUE FIXE (ex: les cases possibles)
    if (_estEnSurbrillanceLogique && !_surbrillance.isNull()) {
        double w = _surbrillance.width() * _echelleSurbrillance;
        double h = _surbrillance.height() * _echelleSurbrillance;
        painter->drawPixmap(-w/2, -h/2, w, h, _surbrillance);
    }

    //DESSINER LE HOVER DE LA SOURIS (Au-dessus de la surbrillance fixe, sous les jetons)
    if (_estSurvole) {
        QPixmap* imgActuelle = &_hoverJaune; // Par défaut
        
        if (_typeHover == TypeHover::VALIDE_VERT) {
            imgActuelle = &_hoverVert;
        } else if (_typeHover == TypeHover::INVALIDE_ROUGE) {
            imgActuelle = &_hoverRouge;
        }

        if (!imgActuelle->isNull()) {
            double w = imgActuelle->width() * _echelleSurbrillance; 
            double h = imgActuelle->height() * _echelleSurbrillance;
            painter->drawPixmap(-w/2, -h/2, w, h, *imgActuelle);
        }
    }

    // DESSINER LES JETONS (Sprite unique)
    if (!_spriteJetons.isNull()) {
        double w = _spriteJetons.width() * _echelleJetons;
        double h = _spriteJetons.height() * _echelleJetons;
        painter->drawPixmap(-w/2, -h/2, w, h, _spriteJetons);
    }

    //  DESSIN DU CUBE 
    if (_aUnCube && !_pixmapCubeCache.isNull()) {
        // On récupère la taille du cube avec l'échelle
        double cw = _pixmapCubeCache.width() * _echelleCubes; 
        double ch = _pixmapCubeCache.height() * _echelleCubes;
        
        double decalageY = -15.0; 

        painter->drawPixmap(-cw/2, (-ch/2) + decalageY, cw, ch, _pixmapCubeCache);
    }
}

void HexagonItem::setSpriteJetons(const QString& cheminImage, double echelle) {
    _echelleJetons = echelle;
    _spriteJetons = QPixmap(cheminImage);
    update();
}

void HexagonItem::setPileCubes(const QStringList& cheminsImages, double echelle, const std::vector<double>& offsetsY) {
    _echelleCubes = echelle;
    _offsetsYCubes = offsetsY;
    
    if (!cheminsImages.isEmpty()) {
        _spriteCube = QPixmap(cheminsImages[0]);
        _nbCubes = cheminsImages.size();
    }
    update();
}

void HexagonItem::vider() {
    _spriteJetons = QPixmap(); 
    _spriteCube = QPixmap();   
    _nbCubes = 0;
    _aUnCube = false;      
    update();
}

void HexagonItem::setSurbrillanceLogique(bool active, double echelle) {
    _estEnSurbrillanceLogique = active;
    _echelleSurbrillance = echelle;
    update();
}

void HexagonItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit caseCliquee(_x, _y);
    QGraphicsObject::mousePressEvent(event);
}

QPainterPath HexagonItem::shape() const {
    QPainterPath chemin;
    QPolygonF polygone;

    // --- VARIABLES DE HITBOX ---
    double w = 55.0; // Largeur totale de l'hexagone
    double h = 50.0; // Hauteur totale de l'hexagone

    polygone << QPointF(-w/4, -h/2)  // Haut-Gauche
             << QPointF(w/4, -h/2)   // Haut-Droite
             << QPointF(w/2, 0)      // Pointe Droite
             << QPointF(w/4, h/2)    // Bas-Droite
             << QPointF(-w/4, h/2)   // Bas-Gauche
             << QPointF(-w/2, 0);    // Pointe Gauche

    chemin.addPolygon(polygone);
    return chemin;
}

void HexagonItem::setTypeHover(TypeHover type) {
    _typeHover = type;
    if (_estSurvole) update(); // Si la souris est déjà dessus, on change la couleur direct
}

void HexagonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    _estSurvole = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void HexagonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    _estSurvole = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void HexagonItem::setAUnCube(bool aUnCube) {
    _aUnCube = aUnCube;
    update();
}