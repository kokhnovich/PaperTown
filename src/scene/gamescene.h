#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMap>
#include <QPixmap>
#include <QPointF>
#include "../core/gameobjects.h"
#include "../core/gamefield.h"
#include "gametextures.h"
#include "gamefieldviews.h"
#include "gamescenegeometry.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GameScene(QObject *parent, GameObjectRepository *repository,
              GameField *field, GameTextureRepository *textures);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    GameObjectRepository *repository_;
    GameField *field_;
    GameTextureRepository *textures_;

    GameSceneGeometry *geometry_;
    GameTextureRenderer *renderer_;
    GameFieldView *view_;
};

class GameView : public QGraphicsView {
public:
    GameView(QWidget *parent);
};

#endif // GAMESCENE_H

