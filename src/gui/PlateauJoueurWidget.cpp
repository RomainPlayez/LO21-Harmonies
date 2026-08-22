#include "gui/PlateauJoueurWidget.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QString>
#include "gui/CarteWidget.h"
#include "core/Validateur.h"
#include "core/Carte.h"
#include <QVariantAnimation>
#include <QGraphicsPixmapItem>

// --- VARIABLES DE MAPPING FACE A ---
double ORIGINE_X_A = 112.0; 
double ORIGINE_Y_A = 60.0;
double OFFSET_X_DROITE_A = 42.5; 
double OFFSET_Y_BAS_A    = 49.0; 
double OFFSET_Y_DECALE_A = 24.5; 

// --- VARIABLES DE MAPPING FACE B ---
double ORIGINE_X_B = 69.5; 
double ORIGINE_Y_B = 84.5;  
double OFFSET_X_DROITE_B = 42.5; 
double OFFSET_Y_BAS_B    = 49.0; 
double OFFSET_Y_DECALE_B = 24.5; 

// ---  VARIABLES DE TAILLE ) ---
double ECHELLE_JETONS = 0.2; 
double ECHELLE_CUBES  = 0.4; 
double ECHELLE_LUEUR  = 0.035; 

// ---  VARIABLES DE PERSPECTIVE DES CUBES  ---
double OFF_CUBE_H1 = 0.0;   // Le cube de base, au centre
double OFF_CUBE_H2 = -15.0; // Le 2ème cube, un peu plus haut
double OFF_CUBE_H3 = -30.0; // Le 3ème cube, encore plus haut

//fonction utilitaire 
QString PlateauJoueurWidget::couleurVersTexte(CouleurJeton c) {
    switch(c) {
        case CouleurJeton::GRIS:   return "gris";
        case CouleurJeton::ROUGE:  return "rouge";
        case CouleurJeton::MARRON: return "marron";
        case CouleurJeton::VERT:   return "vert";
        case CouleurJeton::JAUNE:  return "jaune";
        case CouleurJeton::BLEU:   return "bleu";
        default:                   return "";
    }
}

PlateauJoueurWidget::PlateauJoueurWidget(Joueur* joueur, QWidget *parent)
    : QWidget(parent), _joueur(joueur), _indexJetonSelectionne(-1)
{
    _scene = new QGraphicsScene(this);
    _vue = new QGraphicsView(_scene, this);
    
    _vue->setFrameShape(QFrame::NoFrame);
    _vue->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _vue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _vue->setStyleSheet("background: transparent;");

    QString nomImage = (_joueur->getPlateau()->getFace() == FacePlateau::FACE_A) ? ":/assets/plateauA.png" : ":/assets/plateauB.png";
    QPixmap pixFond(nomImage);
    QGraphicsPixmapItem* fondPlateau = _scene->addPixmap(pixFond);
    _scene->setSceneRect(fondPlateau->boundingRect());

    _vue->setAlignment(Qt::AlignLeft | Qt::AlignTop); // On force l'alignement en haut à gauche
    _vue->setFixedSize(pixFond.size());              // On force la vue à faire la taille exacte de l'image

    initialiserGrille();

    construireLayout();

    rafraichirAffichage();
}

void PlateauJoueurWidget::construireLayout() {
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    //  ZONE HAUTE : LES CARTES ACTIVES 
    QHBoxLayout* layoutCartes = new QHBoxLayout();
    for (int i = 0; i < 4; ++i) {
        _widgetCartes[i] = new CarteWidget(this);
        
        connect(_widgetCartes[i], &CarteWidget::requetePlacementCube, 
                this, &PlateauJoueurWidget::onRequetePlacementCube);
                
        layoutCartes->addWidget(_widgetCartes[i]);
    }
    layoutPrincipal->addLayout(layoutCartes);

    //  ZONE BASSE 
    QHBoxLayout* layoutBas = new QHBoxLayout();
    layoutBas->addWidget(_vue); // Plateau à gauche

    // LE CONTENEUR DE DROITE (ZONE ATTENTE)
    _conteneurAttente = new QWidget(this);
    QVBoxLayout* layoutAttente = new QVBoxLayout(_conteneurAttente);
    layoutAttente->setContentsMargins(0, 0, 0, 0);

    QLabel* titreAttente = new QLabel("Jetons à placer:", _conteneurAttente);
    titreAttente->setStyleSheet("font-weight: bold; color: #2C3E50;");
    layoutAttente->addWidget(titreAttente);

    for (int i = 0; i < 3; ++i) {
        _btnsAttente[i] = new QPushButton(_conteneurAttente);
        _btnsAttente[i]->setFixedSize(50, 50);
        _btnsAttente[i]->setStyleSheet("background-color: #E8E4D9; border: 2px solid #B0A896; border-radius: 25px;");
        
        connect(_btnsAttente[i], &QPushButton::clicked, this, [this, i]() {
            onJetonAttenteClique(i);
        });
        layoutAttente->addWidget(_btnsAttente[i]);
    }
    layoutAttente->addStretch();

    layoutBas->addWidget(_conteneurAttente); 

    layoutPrincipal->addLayout(layoutBas);
}

// Fonction pour afficher/masquer la zone d'un coup
void PlateauJoueurWidget::setTourActif(bool actif) {
    if (_conteneurAttente) {
        _conteneurAttente->setVisible(actif);
    }
}

void PlateauJoueurWidget::initialiserGrille() {
    // On détecte la face
    bool estFaceA = (_joueur->getPlateau()->getFace() == FacePlateau::FACE_A);

    // On charge les bonnes variables de mapping
    double origineX = estFaceA ? ORIGINE_X_A : ORIGINE_X_B;
    double origineY = estFaceA ? ORIGINE_Y_A : ORIGINE_Y_B;
    double offsetX  = estFaceA ? OFFSET_X_DROITE_A : OFFSET_X_DROITE_B;
    double offsetY  = estFaceA ? OFFSET_Y_BAS_A : OFFSET_Y_BAS_B;
    double offsetDecale = estFaceA ? OFFSET_Y_DECALE_A : OFFSET_Y_DECALE_B;

    int nbCases = _joueur->getPlateau()->getNbCases();
    for (int i = 0; i < nbCases; ++i) {
        Case* c = _joueur->getPlateau()->getCaseParIndex(i);
        if (c != nullptr) {
            int cx = c->getX();
            int cy = c->getY();

            double posX = origineX + (cx * offsetX);
            double posY = origineY + (cy * offsetY);
            
            if (cx % 2 != 0) {
                posY += offsetDecale;
            }

            HexagonItem* hexItem = new HexagonItem(cx, cy);
            hexItem->setPos(posX, posY);
            
            hexItem->setSurbrillanceLogique(false, ECHELLE_LUEUR);

            connect(hexItem, &HexagonItem::caseCliquee, this, &PlateauJoueurWidget::onCaseCliquee);
            
            _scene->addItem(hexItem);
            _casesGraphiques[(cx * 100) + cy] = hexItem;
        }
    }
}

void PlateauJoueurWidget::onCaseCliquee(int x, int y) {
    Case* c = _joueur->getPlateau()->getCase(x, y);
    if (!c) return;

    // CAS 1 : UN JETON EST SÉLECTIONNÉ
    if (_indexJetonSelectionne != -1) {
        const Jeton* jetonRef = _joueur->getPlateau()->getJetonEnAttente(_indexJetonSelectionne);
        if (!jetonRef || !Validateur::estPlacementValide(*c, *jetonRef)) return;

        
        // Calculer le point de départ (Le bouton)
        QPoint posGlobaleBouton = _btnsAttente[_indexJetonSelectionne]->mapToGlobal(QPoint(0,0));
        QPointF posDepartScene = _vue->mapToScene(_vue->mapFromGlobal(posGlobaleBouton));
        
        // Calculer le point d'arrivée (L'hexagone)
        QPointF posArriveeScene = _casesGraphiques[(x * 100) + y]->pos();

        // Créer le "Faux Jeton" qui va voler
        QString cheminImage = ":/assets/" + couleurVersTexte(jetonRef->getCouleur()) + ".png";
        QGraphicsPixmapItem* jetonVolant = new QGraphicsPixmapItem(QPixmap(cheminImage));

        // On le centre et on lui donne la même échelle que tes jetons sur le plateau
        jetonVolant->setOffset(-jetonVolant->boundingRect().width()/2, -jetonVolant->boundingRect().height()/2);
        jetonVolant->setScale(ECHELLE_JETONS); 
        jetonVolant->setPos(posDepartScene);
        jetonVolant->setZValue(100); // Pour qu'il passe au-dessus de tout
        _scene->addItem(jetonVolant);

        // On gèle la sélection pendant le vol pour éviter les double-clics
        int indexSauvegarde = _indexJetonSelectionne;
        deselectionnerTout(); 

        // On lance le moteur d'animation Qt
        QVariantAnimation* anim = new QVariantAnimation(this);
        anim->setDuration(300); // 300 millisecondes (0.3s) de vol
        anim->setStartValue(posDepartScene);
        anim->setEndValue(posArriveeScene);

        connect(anim, &QVariantAnimation::valueChanged, [jetonVolant](const QVariant& value){
            jetonVolant->setPos(value.toPointF());
        });

        connect(anim, &QVariantAnimation::finished, [this, x, y, indexSauvegarde, jetonVolant]() {
            _scene->removeItem(jetonVolant);
            delete jetonVolant;

            emit requetePlacementJeton(x, y, indexSauvegarde);
        });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
        return;
    }

    // CAS 2 : UN CUBE EST SÉLECTIONN 
    if (_indexCarteSelectionnee != -1) {
        Carte* carteActive = _joueur->getCarteActive(_indexCarteSelectionnee);
        
        if (!carteActive || c->aUnCubeAnimal() || !Validateur::validerMotif(*_joueur->getPlateau(), *carteActive->getMotif(), x, y)) {
            return; 
        }

        //ON DÉTERMINE L'IMAGE À FAIRE VOLER
        bool estEsprit = (dynamic_cast<CarteEsprit*>(carteActive) != nullptr);
        QString cheminCube = estEsprit ? ":/assets/cube_esprit.png" : ":/assets/cube_animal.png";

        QWidget* widgetSource = _widgetCartes[_indexCarteSelectionnee];
        QPoint posGlobaleCarte = widgetSource ? widgetSource->mapToGlobal(widgetSource->rect().center()) : QPoint(0,0);
        QPointF posDepartScene = _vue->mapToScene(_vue->mapFromGlobal(posGlobaleCarte));
        QPointF posArriveeScene = _casesGraphiques[(x * 100) + y]->pos();

        QGraphicsPixmapItem* cubeVolant = new QGraphicsPixmapItem(QPixmap(cheminCube));
        cubeVolant->setOffset(-cubeVolant->boundingRect().width()/2, -cubeVolant->boundingRect().height()/2);
        cubeVolant->setScale(ECHELLE_CUBES); 
        cubeVolant->setPos(posDepartScene);
        cubeVolant->setZValue(100);
        _scene->addItem(cubeVolant);

        int indexSauvegarde = _indexCarteSelectionnee;
        deselectionnerTout();

        QVariantAnimation* anim = new QVariantAnimation(this);
        anim->setDuration(350); 
        anim->setStartValue(posDepartScene);
        anim->setEndValue(posArriveeScene);

        connect(anim, &QVariantAnimation::valueChanged, [cubeVolant](const QVariant& value){
            cubeVolant->setPos(value.toPointF());
        });

        connect(anim, &QVariantAnimation::finished, [this, x, y, indexSauvegarde, cubeVolant]() {
            _scene->removeItem(cubeVolant);
            delete cubeVolant;

            emit actionPlacerCube(indexSauvegarde, x, y);
        });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
        return;
    }
}

void PlateauJoueurWidget::rafraichirAffichage() {
    
    //  MISE À JOUR DES CASES DU PLATEAU
    for (auto const& [id, hexItem] : _casesGraphiques) {
        int cx = id / 100;
        int cy = id % 100;
        
        Case* c = _joueur->getPlateau()->getCase(cx, cy);
        if (!c) continue;

        hexItem->vider(); 
        hexItem->setAUnCube(false);

        // Si la case contient des choses 
        if (!c->estVide()) {
            
            //  ON DESSINE LES JETONS
            QStringList composants;
            for (int i = 0; i < c->getZ(); ++i) {
                composants << couleurVersTexte(c->getJeton(i)->getCouleur());
            }
            QString nomFichier = composants.join("-") + ".png";
            hexItem->setSpriteJetons(":/assets/" + nomFichier, ECHELLE_JETONS); 

            // ON DESSINE LE CUBE
            if (c->aUnCubeAnimal()) {
                std::vector<double> offsets = { OFF_CUBE_H1, OFF_CUBE_H2, OFF_CUBE_H3 };
                QStringList cubes;
                
                if (c->getCube() != nullptr && c->getCube()->isEsprit()) {
                    cubes << ":/assets/cube_esprit.png";
                } else {
                    cubes << ":/assets/cube_animal.png";
                }
                
                hexItem->setPileCubes(cubes, ECHELLE_CUBES, offsets);
                hexItem->setAUnCube(true);
            }
        }
    } // FIn de la boucle des hexagones

    //  MISE À JOUR DES CARTES ACTIVES
    for (int i = 0; i < 4; ++i) {
        if (i < _joueur->getNbCartesActives() && _joueur->getCarteActive(i) != nullptr) {
            _widgetCartes[i]->setCarte(_joueur->getCarteActive(i));
        } else {
            _widgetCartes[i]->setCarte(nullptr);
        }
    }

    // MISE À JOUR DES JETONS EN ATTENTE
    for (int i = 0; i < 3; ++i) {
        const Jeton* j = _joueur->getPlateau()->getJetonEnAttente(i);
        if (j != nullptr) {
            QString chemin = ":/assets/" + couleurVersTexte(j->getCouleur()) + ".png";
            _btnsAttente[i]->setIcon(QIcon(chemin));
            _btnsAttente[i]->setIconSize(QSize(80, 80)); 
        } else {
            _btnsAttente[i]->setIcon(QIcon());
        }
    }
}

void PlateauJoueurWidget::onJetonAttenteClique(int index) {
    // Le jeton existe-t-il vraiment ?
    const Jeton* jetonCible = _joueur->getPlateau()->getJetonEnAttente(index);
    if (jetonCible == nullptr) return; 

    // Si on clique sur le jeton DÉJÀ sélectionné, ça annule !
    if (_indexJetonSelectionne == index) {
        deselectionnerTout();
        return;
    }

    // On enregistre la nouvelle sélection et on nettoie l'UI
    deselectionnerTout(); // Pour reset les anciennes couleurs
    _indexJetonSelectionne = index;

    // Feedback visuel du bouton
    _btnsAttente[index]->setStyleSheet("background-color: #E8E4D9; border: 4px solid #F1C40F; border-radius: 25px;");

    // INTERROGATION POUR CHAQUE CASE
    for (auto const& [id, hexItem] : _casesGraphiques) {
        int cx = id / 100;
        int cy = id % 100;
        
        Case* c = _joueur->getPlateau()->getCase(cx, cy);
        
        if (c != nullptr) {
            if (Validateur::estPlacementValide(*c, *jetonCible)) {
                hexItem->setTypeHover(TypeHover::VALIDE_VERT);
            } else {
                hexItem->setTypeHover(TypeHover::INVALIDE_ROUGE);
            }
        }
    }
}

void PlateauJoueurWidget::deselectionnerTout() {
    _indexJetonSelectionne = -1;
    
    // On éteint les boutons de jetons
    for (int i = 0; i < 3; ++i) {
        _btnsAttente[i]->setStyleSheet("background-color: #E8E4D9; border: 2px solid #B0A896; border-radius: 25px;");
    }

    //  On éteint les hovers du plateau
    for (auto const& [id, hexItem] : _casesGraphiques) {
        hexItem->setTypeHover(TypeHover::DEFAUT_JAUNE);
    }

    // ON ÉTEINT LE CADRE JAUNE DE TOUTES LES CARTES
    for (int i = 0; i < 4; ++i) {
        if (_widgetCartes[i] != nullptr) {
            _widgetCartes[i]->setSelectionnee(false); // Appel au CarteWidget pour enlever le cadre
        }
    }

    _indexCarteSelectionnee = -1; 
    
    update(); 
}

void PlateauJoueurWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        deselectionnerTout(); 
    }
    QWidget::mousePressEvent(event);
}

void PlateauJoueurWidget::onRequetePlacementCube(Carte* carte) {
    // On cherche l'index (0 à 3) de cette carte dans la main du joueur
    int indexTrouve = -1;
    for (int i = 0; i < 4; ++i) {
        if (_joueur->getCarteActive(i) == carte) indexTrouve = i;
    }
    if (indexTrouve == -1) return;

    // Si on clique sur le cube DÉJÀ sélectionné, on annule tout
    if (_indexCarteSelectionnee == indexTrouve) {
        deselectionnerTout();
        return;
    }

    deselectionnerTout();
    _indexCarteSelectionnee = indexTrouve;
    
    // On allume le cadre jaune du cube
    if (_widgetCartes[_indexCarteSelectionnee]) {
        _widgetCartes[_indexCarteSelectionnee]->setSelectionnee(true);
    }

    // LOGIQUE DE HOVER 
    for (auto const& [id, hexItem] : _casesGraphiques) {
        int cx = id / 100;
        int cy = id % 100;
        
        Case* c = _joueur->getPlateau()->getCase(cx, cy);
        bool estValide = false;
        
        if (c != nullptr && !c->aUnCubeAnimal()) {
            estValide = Validateur::validerMotif(*_joueur->getPlateau(), *carte->getMotif(), cx, cy);
        }

        if (estValide) {
            hexItem->setTypeHover(TypeHover::VALIDE_VERT);
        } else {
            hexItem->setTypeHover(TypeHover::INVALIDE_ROUGE);
        }
    }
}