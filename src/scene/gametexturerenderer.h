#ifndef GAMETEXTURERENDERER_H
#define GAMETEXTURERENDERER_H

#include <QObject>
#include "gametextures.h"
#include "gamescenegeometry.h"

class GameTextureRenderer : public QObject
{
    Q_OBJECT
public:
    GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
                        GameTextureRepository *repository, QGraphicsScene *scene);

    QGraphicsItem *drawTexture(const QString &name, const Coordinate &c, qreal priority = 0.0);
    QGraphicsItem *moveTexture(QGraphicsItem *item, const QString &name,
                               const Coordinate &c, qreal priority = 0.0);
    QGraphicsItem *drawMoving(GameObject *object);
    QGraphicsWidget *drawControlButtons(const GameObject *object);

    void setupScene();

    QGraphicsScene *scene();
protected:
    QGraphicsItem *drawSelectionRect(GameObject *object);

    qreal zOrder(const Coordinate &c, qreal priority = 0.0) const;
private:
    GameSceneGeometry *geometry_;
    GameTextureRepository *textures_;
    QGraphicsScene *scene_;
};

#endif // GAMETEXTURERENDERER_H
