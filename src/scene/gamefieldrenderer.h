#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QHash>
#include <QSharedPointer>
#include "../core/gameobjects.h"
#include "gametextures.h"

const int DATA_KEY_GAMEOBJECT = 42;

class RenderScene : public QGraphicsScene
{
    Q_OBJECT
public:
    virtual QGraphicsItem *drawTexture(const QString &name, const Coordinate &c, qreal priority = 0.0) = 0;
    virtual QGraphicsItem *moveTexture(QGraphicsItem *item, const QString &name,
                                       const Coordinate &c, qreal priority = 0.0) = 0;
    RenderScene(QObject *parent = nullptr) : QGraphicsScene(parent) {}
};

class GameObjectRepository : public GameObjectRepositoryBase
{
    Q_OBJECT
public:
    struct RenderInfo {
        QVector<QString> textures;
        qreal priority;
    };

    GameObjectRepository(QObject *parent = nullptr);
    void addRenderInfo(const QString &type, const QString &name, const RenderInfo &info);
    const RenderInfo *getRenderInfo(const QString &type, const QString &name) const;
    const RenderInfo *getRenderInfo(GameObject *object) const;
    void loadFromJson(const QJsonDocument &document);
    void loadFromFile(const QString &file_name);
private:
    QHash<QString, QSharedPointer<RenderInfo>> render_info_;
    QHash<QString, qreal> type_priorities_;
};

class GameObjectRenderer : public QObject
{
    Q_OBJECT
public:
    GameObjectRenderer(RenderScene *scene, GameObjectRepository *repository);
public slots:
    void addObject(GameObject *object);
    void removeObject(GameObject *object);
protected:
    void putObject(GameObject *object);
    void unputObject(GameObject *object);
    void changeObjectSelectionState(GameObject *object, bool selected);
protected slots:
    void placeObject(const Coordinate &);
    void moveObject(const Coordinate &, const Coordinate &newPosition);
    void updateObject();
    void selectObject();
    void selectionMoved(const Coordinate &, const Coordinate &newPosition);
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

    RenderScene *scene_;
    GameObjectRepository *repository_;
    QMultiHash<GameObject *, TextureInfo> objects_;
};

#endif // GAMEOBJECTRENDERER_H
