#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QHash>
#include <QSharedPointer>
#include "../core/gameobjects.h"
#include "gametextures.h"
#include "gamescenegeometry.h"
#include "../objects/gameobjectrepository.h"

const int DATA_KEY_GAMEOBJECT = 42;

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

class GameObjectRenderRepository : public GameObjectRepository
{
    Q_OBJECT
public:
    struct RenderInfo {
        QVector<QString> textures;
        qreal priority;
        QString caption;
    };

    GameObjectRenderRepository(QObject *parent = nullptr);
    void addRenderInfo(const QString &type, const QString &name, const RenderInfo &info);
    const RenderInfo *getRenderInfo(const QString &type, const QString &name) const;
    const RenderInfo *getRenderInfo(GameObject *object) const;
protected:
    void doLoadObject(const QString & type, const QString & name, const QJsonObject & json) override;
private:
    QHash<QString, QSharedPointer<RenderInfo>> render_info_;
    QHash<QString, qreal> type_priorities_;
};


class GameFieldView : public QObject
{
    Q_OBJECT
public:
    enum SelectionState {
        None,
        Selected,
        Moving
    };
    Q_ENUM(SelectionState)
    
    GameFieldView(QObject *parent, GameTextureRenderer *renderer, GameObjectRenderRepository *repository);
public slots:
    void addObject(GameObject *object);
    void removeObject(GameObject *object);
protected:
    void putObject(GameObject *object);
    void unputObject(GameObject *object);
    void changeObjectSelectionState(GameObject *object, SelectionState state);

    qreal getStateZDelta(SelectionState state) const;
protected slots:
    void placeObject(const Coordinate &);
    void moveObject(const Coordinate &, const Coordinate &newPosition);
    void startMovingObject();
    void endMovingObject();
    void updateObject();
    void selectObject();
    void movingPositionChanged(const Coordinate &, const Coordinate &newPosition);
    void unselectObject();
private:
    struct TextureInfo {
        QString name;
        QGraphicsItem *item;
        qreal priority;
    };

    template<typename T>
    void iterateTextures(GameObject *object, const T &func)
    {
        auto it = objects_.find(object);
        while (it != objects_.end() && it.key() == object) {
            func(it.value());
            ++it;
        }
    }

    GameTextureRenderer *renderer_;
    QGraphicsScene *scene_;
    GameObjectRenderRepository *repository_;
    QMultiHash<GameObject *, TextureInfo> objects_;
    QGraphicsItem *moving_item_;
    QGraphicsWidget *control_buttons_;
    SelectionState last_state_;
};

#endif // GAMEOBJECTRENDERER_H
