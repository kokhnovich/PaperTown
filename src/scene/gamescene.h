#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>
#include <QPointF>
#include "../core/gameobjects.h"
#include "../core/gamefield.h"
#include "gametextures.h"
#include "gamefieldrenderer.h"
#include "gamescenegeometry.h"

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
    GameObjectRepository *repository() const;
    GameObjectRenderer *renderer() const;
    
    inline int fieldHeight() const { return field()->height(); }
    inline int fieldWidth() const { return field()->width(); }
protected:
    qreal zOrder(const Coordinate &c, qreal priority = 0.0) const;
    
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
    void setupField();
    
    GameObjectRepository *repository_;
    GameField *field_;
    GameTextureRepository *textures_;

    GameSceneGeometry *geometry_;
    GameObjectRenderer *renderer_;
};

#endif // GAMESCENE_H
