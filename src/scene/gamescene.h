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
const int FIELD_HEIGHT = 48;
const int FIELD_WIDTH = 64;
const int SLOPE_HEIGHT = 1;
const int SLOPE_WIDTH = 2;
const int TOP_MARGIN = 125;

class GameScene : public RenderScene
{
    Q_OBJECT
public:
    GameScene(QObject *parent = nullptr);
    QGraphicsItem *drawTexture(const QString &name, const Coordinate &c, qreal priority = 0.0) override;
    QGraphicsItem *moveTexture(QGraphicsItem *item, const QString &name,
                               const Coordinate &c, qreal priority = 0.0) override;
    GameField *field() const;
    GameObjectRenderer *renderer() const;
    GameObjectRepository *repository() const;
protected:
    QPolygonF coordinateToPoly(const Coordinate &c);
    QRectF coordinateToRect(const Coordinate &c);
    QPointF coordinateToTopLeft(const Coordinate &c);
    qreal zOrder(const Coordinate &c, qreal priority = 0.0) const;
private:
    void setupField();
    void initObjects();

    GameObjectRepository *repository_;
    GameField *field_;
    GameObjectRenderer *renderer_;
    GameTextureRepository *textures_;
};

#endif // GAMESCENE_H
