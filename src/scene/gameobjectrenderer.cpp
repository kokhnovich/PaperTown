#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include "gameobjectrenderer.h"

void GameObjectRenderer::addObject(GameObject *object)
{
    connect(object, &GameObject::placed, this, &GameObjectRenderer::placeObject);
    connect(object, &GameObject::moved, this, &GameObjectRenderer::moveObject);
    connect(object, &GameObject::updated, this, &GameObjectRenderer::updateObject);
    connect(object, &GameObject::destroyed, this, &GameObjectRenderer::destroyObject);
}

void GameObjectRenderer::putObject(GameObject *object)
{
    auto info = repository_->getRenderInfo(object);
    for (const QString &texture_name : info->textures) {
        objects_.insert(object, {
            texture_name,
            scene_->drawTexture(texture_name, object->position(), info->priority),
            info->priority
        });
    }
}

void GameObjectRenderer::destroyObject()
{
    removeObject(qobject_cast<GameObject *>(sender()));
}

void GameObjectRenderer::moveObject(const Coordinate &, const Coordinate &newPosition)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    auto it = objects_.find(object);
    while (it != objects_.end() && it.key() == object) {
        const auto &info = it.value();
        scene_->moveTexture(info.item, info.name, newPosition);
        ++it;
    }
}

void GameObjectRenderer::placeObject(const Coordinate &)
{
    putObject(qobject_cast<GameObject *>(sender()));
}

void GameObjectRenderer::updateObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    if (objects_.contains(object)) {
        unputObject(object);
        putObject(object);
    }
}

GameObjectRenderer::GameObjectRenderer(RenderScene *scene, GameObjectRepository *repository)
    : scene_(scene), repository_(repository), objects_()
{}

void GameObjectRenderer::unputObject(GameObject *object)
{
    Q_ASSERT(objects_.contains(object));
    auto it = objects_.find(object);
    while (it != objects_.end() && it.key() == object) {
        const auto &info = it.value();
        scene_->removeItem(info.item);
        delete info.item;
        ++it;
    }
    objects_.remove(object);
}

void GameObjectRenderer::removeObject(GameObject *object)
{
    disconnect(object, &GameObject::placed, this, &GameObjectRenderer::placeObject);
    disconnect(object, &GameObject::moved, this, &GameObjectRenderer::moveObject);
    disconnect(object, &GameObject::updated, this, &GameObjectRenderer::updateObject);
    disconnect(object, &GameObject::destroyed, this, &GameObjectRenderer::destroyObject);

    if (objects_.contains(object)) {
        unputObject(object);
    }
}

void GameObjectRepository::addRenderInfo(const QString &type, const QString &name,
    const GameObjectRepository::RenderInfo &info)
{
    QString full_name = fullName(type, name);
    render_info_[full_name] = QSharedPointer<RenderInfo>(new RenderInfo(info));
}

void GameObjectRepository::loadFromFile(const QString &file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    Q_ASSERT(error.error == QJsonParseError::NoError);
    return loadFromJson(document);    
}

void GameObjectRepository::loadFromJson(const QJsonDocument &document)
{
    Q_ASSERT(document.isObject());
    auto root_object = document.object();
    for (auto it = root_object.begin(); it != root_object.end(); ++it) {
        Q_ASSERT(it->isObject());
        auto local_object = it->toObject();
        RenderInfo info;
        QString type = local_object.value("type").toString();
        auto texture_arr = local_object.value("textures").toArray();
        info.textures.resize(texture_arr.size());
        for (int i = 0; i < texture_arr.size(); ++i) {
            info.textures[i] = texture_arr[i].toString();
        }
        info.priority = local_object.value("priority").toDouble(type_priorities_[type]);
        addRenderInfo(type, it.key(), info);
    }
}

const GameObjectRepository::RenderInfo *GameObjectRepository::getRenderInfo(const QString &type, const QString &name) const
{
    return render_info_[fullName(type, name)].get();
}

const GameObjectRepository::RenderInfo *GameObjectRepository::getRenderInfo(GameObject *object) const
{
    return getRenderInfo(object->type(), object->name());
}

GameObjectRepository::GameObjectRepository(QObject* parent)
    : GameObjectRepositoryBase(parent)
{
    type_priorities_["ground"] = 0;
    type_priorities_["static"] = 0.33;
    type_priorities_["moving"] = 0.67;
}

