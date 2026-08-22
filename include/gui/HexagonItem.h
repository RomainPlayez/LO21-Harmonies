#pragma once
#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QStringList>

enum class TypeHover {
    DEFAUT_JAUNE,
    VALIDE_VERT,
    INVALIDE_ROUGE
};

class HexagonItem : public QGraphicsObject {
    Q_OBJECT
public:
    HexagonItem(int coordX, int coordY, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    
    // Pour afficher les sprites de jetons empilés 
    void setSpriteJetons(const QString& cheminImage, double echelle);
    
    // Pour afficher des cubes empilés manuellement
    void setPileCubes(const QStringList& cheminsImages, double echelle, const std::vector<double>& offsetsY);
    
    // Pour nettoyer la case
    void vider();

    // Gestion de la surbrillance logique (validateur)
    void setSurbrillanceLogique(bool active, double echelle);

    // méthode pour que le plateau puisse changer la couleur du hover
    void setTypeHover(TypeHover type);

    void setAUnCube(bool aUnCube);

signals:
    void caseCliquee(int x, int y);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    

private:
    int _x, _y;
    QPixmap _surbrillance;
    QPixmap _spriteJetons;
    
    // Pour les cubes
    QPixmap _spriteCube; // Image générique du cube
    int _nbCubes;        // Hauteur de la pile
    std::vector<double> _offsetsYCubes; // Décalages de perspective fournis par le parent

    // Échelles séparées
    double _echelleJetons;
    double _echelleCubes;
    double _echelleSurbrillance;

    bool _estEnSurbrillanceLogique;

    // --- GESTION DU HOVER ---
    bool _estSurvole;
    TypeHover _typeHover;
    
    QPixmap _hoverJaune;
    QPixmap _hoverVert;
    QPixmap _hoverRouge;

    bool _aUnCube;
    QPixmap _pixmapCubeCache;
};