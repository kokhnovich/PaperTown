#ifndef GAMETEXTURERENDERER_H
#define GAMETEXTURERENDERER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include "../core/gameobjects.h"
#include "gametextures.h"
#include "gamescenegeometry.h"
#include "gameobjectrenderrepository.h"

const int DATA_KEY_GAMEOBJECT = 42;
const int DATA_KEY_TEXTURE_NAME = 43;
const int DATA_KEY_PRIORITY = 44;

class GameTextureRenderer : public QObject
{
    Q_OBJECT
public:
    GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
                        GameTextureRepository *textures,
                        GameObjectRenderRepository *repository, QGraphicsScene *scene);

    QList<QGraphicsItem *> drawObject(GameObject *object);
    void moveObject(GameObject *object, const QList<QGraphicsItem *> &items);
    void changeObjectSelectionState(GameObject *object, const QList<QGraphicsItem *> &items,
                                    SelectionState old_state, SelectionState new_state);
    
    QGraphicsItem *drawMoving(GameObject *object);
    QGraphicsWidget *drawSelectionControl(const GameObject *object);

    void setupScene();

    QGraphicsScene *scene() const;
protected:
    QGraphicsItem *drawSelectionRect(GameObject *object);
    QGraphicsItem *drawTexture(const QString &name, const Coordinate &c, qreal priority = 0.0);
    void moveTexture(QGraphicsItem *item, const QString &name,
                     const Coordinate &c, qreal priority = 0.0);
private:
    GameSceneGeometry *geometry_;
    GameTextureRepository *textures_;
    GameObjectRenderRepository *repository_;
    QGraphicsScene *scene_;
};

#endif // GAMETEXTURERENDERER_H
