#ifndef GAMETEXTURERENDERER_H
#define GAMETEXTURERENDERER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include "../core/gameobjects.h"
#include "gametextures.h"
#include "gamescenegeometry.h"
#include "gameobjectrenderrepository.h"
#include "gamepropertyrenderer.h"

const int DATA_KEY_GAMEOBJECT = 42;

class GameTextureRenderer : public GameTextureRendererBase
{
    Q_OBJECT
public:
    GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
                        GameTextureRepository *textures,
                        GameObjectRenderRepository *repository, QGraphicsScene *scene);

    QList<QGraphicsItem *> drawObject(GameObject *object);
    void moveObject(GameObject *object, const Coordinate &old_pos, const QList<QGraphicsItem *> &items);
    void changeObjectSelectionState(GameObject *object, const QList<QGraphicsItem *> &items,
                                    SelectionState old_state, SelectionState new_state);
    
    QGraphicsItem *drawMoving(GameObject *object);
    QGraphicsWidget *drawSelectionControl(const GameObject *object);

    void setupScene();
protected:
    QGraphicsItem *drawSelectionRect(GameObject *object);
    QGraphicsItem *drawTexture(const QString &name, const Coordinate &c);
    void moveTexture(QGraphicsItem *item, const Coordinate &old_pos, const Coordinate &new_pos);
private:
    GamePropertyRenderer *prop_render_;
};

#endif // GAMETEXTURERENDERER_H
