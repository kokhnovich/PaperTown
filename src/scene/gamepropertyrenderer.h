#ifndef GAMEPROPERTYRENDERER_H
#define GAMEPROPERTYRENDERER_H

#include <QObject>
#include <QGraphicsItem>
#include "gamescenegeometry.h"
#include "gametextures.h"
#include "gameobjectrenderrepository.h"
#include "../objects/gameobjectproperties.h"

const int DATA_KEY_PROPERTY = 128;

class GameTextureRendererBase : public QObject
{
    Q_OBJECT
public:
    inline GameSceneGeometry *geometry() const { return geometry_; }
    inline GameTextureRepository *textures() const { return textures_; }
    inline GameObjectRenderRepository *repository() const { return repository_; }
    inline QGraphicsScene *scene() const { return scene_; }

    GameTextureRendererBase(QObject *parent, GameSceneGeometry *geometry,
                            GameTextureRepository *textures,
                            GameObjectRenderRepository *repository, QGraphicsScene *scene);
private:
    GameSceneGeometry *geometry_;
    GameTextureRepository *textures_;
    GameObjectRenderRepository *repository_;
    QGraphicsScene *scene_;
};

class GameAbstractPropertyRenderer : public QObject
{
    Q_OBJECT
public:
    virtual QList<QGraphicsItem *> drawProperty(GameObjectProperty *property);
    virtual QWidget *createControlWidget(GameObjectProperty *property);
    virtual void updateControlWidget(GameObjectProperty *property, QWidget *widget);
    virtual void updatePropertyItem(QGraphicsItem *item, GameObjectProperty *property);

    inline GameSceneGeometry *geometry() const { return renderer_->geometry(); }
    inline GameTextureRepository *textures() const { return renderer_->textures(); }
    inline GameObjectRenderRepository *repository() const { return renderer_->repository(); }
    inline QGraphicsScene *scene() const { return renderer_->scene(); }

    GameAbstractPropertyRenderer(GameTextureRendererBase *renderer, QObject *parent = nullptr);
protected:
    virtual QList<QGraphicsItem *> doDrawProperty(GameObjectProperty *property);
private:
    GameTextureRendererBase *renderer_;
};

class GamePropertyRenderer: public GameAbstractPropertyRenderer
{
    Q_OBJECT
public:
    void addRenderer(const QString &property_class, GameAbstractPropertyRenderer *renderer);

    QList<QGraphicsItem *> drawProperty(GameObjectProperty *property) override;
    QWidget *createControlWidget(GameObjectProperty *property) override;
    void updateControlWidget(GameObjectProperty *property, QWidget *widget) override;
    void updatePropertyItem(QGraphicsItem *item, GameObjectProperty *property) override;

    GamePropertyRenderer(GameTextureRendererBase *renderer, QObject *parent = nullptr);
private:
    QString mangleWidgetName(GameObjectProperty *property);
    
    QHash<QString, GameAbstractPropertyRenderer *> renderers_;
};

#endif // GAMEPROPERTYRENDERER_H

