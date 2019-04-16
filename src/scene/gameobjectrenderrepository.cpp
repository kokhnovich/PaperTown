#include "gameobjectrenderrepository.h"

void GameObjectRenderRepository::addRenderInfo(const QString &type, const QString &name,
        const GameObjectRenderRepository::RenderInfo &info)
{
    QString full_name = fullName(type, name);
    render_info_[full_name] = QSharedPointer<RenderInfo>(new RenderInfo(info));
}

void GameObjectRenderRepository::doLoadObject(const QString& type, const QString& name, const QJsonObject& json)
{
    GameObjectRepository::doLoadObject(type, name, json);
    RenderInfo info;
    auto texture_arr = json.value("textures").toArray();
    info.textures.resize(texture_arr.size());
    for (int i = 0; i < texture_arr.size(); ++i) {
        info.textures[i] = texture_arr[i].toString();
    }
    info.priority = json.value("priority").toDouble(type_priorities_[type]);
    info.caption = json.value("caption").toString(name);
    addRenderInfo(type, name, info);
}

const GameObjectRenderRepository::RenderInfo *GameObjectRenderRepository::getRenderInfo(const QString &type, const QString &name) const
{
    return render_info_[fullName(type, name)].data();
}

const GameObjectRenderRepository::RenderInfo *GameObjectRenderRepository::getRenderInfo(GameObject *object) const
{
    return getRenderInfo(object->type(), object->name());
}

GameObjectRenderRepository::GameObjectRenderRepository(QObject *parent)
    : GameObjectRepository(parent)
{
    type_priorities_["ground"] = 0;
    type_priorities_["static"] = 0.33;
    type_priorities_["moving"] = 0.67;
}
