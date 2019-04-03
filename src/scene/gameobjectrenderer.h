#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QHash>
#include <QSharedPointer>
#include "../core/gameobjects.h"
#include "gametextures.h"

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
    void addObject(GameObject *object);
    void removeObject(GameObject *object);
protected:
    void putObject(GameObject *object);
    void unputObject(GameObject *object);
protected slots:
    void placeObject(const Coordinate &);
    void moveObject(const Coordinate &, const Coordinate &newPosition);
    void updateObject();
    void destroyObject();
private:
    struct TextureInto {
        QString name;
        QGraphicsItem *item;
        qreal priority;
    };

    RenderScene *scene_;
    GameObjectRepository *repository_;
    QMultiHash<GameObject *, TextureInto> objects_;
};

#endif // GAMEOBJECTRENDERER_H
