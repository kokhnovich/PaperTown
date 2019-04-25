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
const int DATA_KEY_BASE_Z_VALUE = 43;
const int DATA_KEY_IS_OBJECT_ITEM = 44;

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
    void updateObject(GameObject *object, const QList<QGraphicsItem *> &items);
    
    QGraphicsItem *drawMovingItem(GameObject *object);
    void moveMovingItem(GameObject *object, QGraphicsItem *item);
    
    QGraphicsWidget *drawSelectionControl(const GameObject *object);
    void updateSelectionControl(const GameObject *object, QGraphicsWidget *widget);
    void moveSelectionControl(QGraphicsWidget *widget, const Coordinate &old_pos, const Coordinate &new_pos);

    void setupScene();
protected:
    QGraphicsWidget *createSelectionControl(const GameObject *object);
    QGraphicsItem *drawSelectionRect(GameObject *object);
    QGraphicsItem *drawTexture(const QString &name, const Coordinate &c);
    void moveTexture(QGraphicsItem *item, const Coordinate &old_pos, const Coordinate &new_pos);
private:
    GamePropertyRenderer *prop_render_;
};

#endif // GAMETEXTURERENDERER_H
