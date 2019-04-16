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
#include "gamefieldview.h"
#include "gamescenegeometry.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GameScene(QObject *parent, GameObjectRenderRepository *repository,
              GameField *field, GameTextureRepository *textures);
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseHovered(const QPointF &position);

    GameFieldView *fieldView() const;
    GameField *field() const;
    
    friend class GameView;
private:
    GameObjectRenderRepository *repository_;
    GameField *field_;
    GameTextureRepository *textures_;

    GameSceneGeometry *geometry_;
    GameTextureRenderer *renderer_;
    GameFieldView *view_;
};

class GameView : public QGraphicsView {
public:
    GameView(QWidget *parent);
    void startAddingObject(GameObject *object);
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // GAMESCENE_H

