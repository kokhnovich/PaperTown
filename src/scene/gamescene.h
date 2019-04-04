#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>
#include <QPointF>
#include "../core/gameobjects.h"
#include "../core/gamefield.h"
#include "gametextures.h"
#include "gameobjectrenderer.h"

const int CELL_SIZE = 25;
const int SLOPE_HEIGHT = 1;
const int SLOPE_WIDTH = 2;
const int TOP_MARGIN = 125;

class GameScene : public RenderScene
{
    Q_OBJECT
public:
    GameScene(QObject *parent, GameObjectRepository *repository,
              GameField *field, GameTextureRepository *textures);
    QGraphicsItem *drawTexture(const QString &name, const Coordinate &c, qreal priority = 0.0) override;
    QGraphicsItem *moveTexture(QGraphicsItem *item, const QString &name,
                               const Coordinate &c, qreal priority = 0.0) override;
    GameField *field() const;
    GameObjectRenderer *renderer() const;
    GameObjectRepository *repository() const;
    
    inline int fieldHeight() const { return field()->height(); }
    inline int fieldWidth() const { return field()->width(); }
protected:
    QPolygonF coordinateToPoly(const Coordinate &c);
    QRectF coordinateToRect(const Coordinate &c);
    QPointF coordinateToTopLeft(const Coordinate &c);
    qreal zOrder(const Coordinate &c, qreal priority = 0.0) const;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
    void setupField();
    
    GameObjectRepository *repository_;
    GameField *field_;
    GameTextureRepository *textures_;

    GameObjectRenderer *renderer_;
};

#endif // GAMESCENE_H
