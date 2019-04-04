#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>
#include "gameobjectrenderer.h"

void GameObjectRenderer::changeObjectSelectionState(GameObject* object, bool selected)
{
    iterateTextures(object, [=](const TextureInfo &info) {
        info.item->setOpacity(selected ? 0.75 : 1.0);
    });
}

void GameObjectRenderer::selectionMoved(const Coordinate&, const Coordinate& newPosition)
{
    // TODO
}

void GameObjectRenderer::selectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, true);
    // TODO
}

void GameObjectRenderer::unselectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, false);
    // TODO
}

void GameObjectRenderer::addObject(GameObject *object)
{
    connect(object, &GameObject::placed, this, &GameObjectRenderer::placeObject);
    connect(object, &GameObject::moved, this, &GameObjectRenderer::moveObject);
    connect(object, &GameObject::updated, this, &GameObjectRenderer::updateObject);
    connect(object, &GameObject::selected, this, &GameObjectRenderer::selectObject);
    connect(object, &GameObject::unselected, this, &GameObjectRenderer::unselectObject);
    connect(object, &GameObject::selectMoved, this, &GameObjectRenderer::selectionMoved);
}

void GameObjectRenderer::putObject(GameObject *object)
{
    auto info = repository_->getRenderInfo(object);
    for (const QString &texture_name : info->textures) {
        QGraphicsItem *item = scene_->drawTexture(texture_name, object->position(), info->priority);
        item->setData(DATA_KEY_GAMEOBJECT, QVariant::fromValue(object));
        objects_.insert(object, {
            texture_name,
            item,
            info->priority
        });
    }
}

void GameObjectRenderer::moveObject(const Coordinate &, const Coordinate &newPosition)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    iterateTextures(object, [=](const TextureInfo &info) {
        scene_->moveTexture(info.item, info.name, newPosition);
    });
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
    iterateTextures(object, [&](const TextureInfo &info) {
        scene_->removeItem(info.item);
        delete info.item;
    });
    objects_.remove(object);
}

void GameObjectRenderer::removeObject(GameObject *object)
{
    disconnect(object, &GameObject::placed, this, &GameObjectRenderer::placeObject);
    disconnect(object, &GameObject::moved, this, &GameObjectRenderer::moveObject);
    disconnect(object, &GameObject::updated, this, &GameObjectRenderer::updateObject);
    disconnect(object, &GameObject::selected, this, &GameObjectRenderer::selectObject);
    disconnect(object, &GameObject::unselected, this, &GameObjectRenderer::unselectObject);
    disconnect(object, &GameObject::selectMoved, this, &GameObjectRenderer::selectionMoved);

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
    qDebug() << "GameObjectRepository: json parsed, error =" << error.errorString();
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
        auto cell_arr = local_object.value("cells").toArray();
        QVector<Coordinate> cells(cell_arr.size());
        for (int i = 0; i < cell_arr.size(); ++i) {
            cells[i].x = cell_arr.at(i).toArray().at(0).toInt();
            cells[i].y = cell_arr.at(i).toArray().at(1).toInt();
        }
        info.priority = local_object.value("priority").toDouble(type_priorities_[type]);
        addRenderInfo(type, it.key(), info);
        addObject(type, it.key(), cells);
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

