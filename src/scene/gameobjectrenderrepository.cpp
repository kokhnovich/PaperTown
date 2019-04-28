#include "gameobjectrenderrepository.h"

void GameObjectRenderRepository::addRenderInfo(const QString &type, const QString &name,
        const GameObjectRenderRepository::RenderInfo &info)
{
    QString full_name = fullName(type, name);
    render_info_[full_name] = QSharedPointer<RenderInfo>(new RenderInfo(info));
}

qreal GameObjectRenderRepository::getTypePriority(const QString &type) const
{
    Q_ASSERT(type_priorities_.contains(type));
    return type_priorities_[type];
}

void GameObjectRenderRepository::doLoadObject(const QString &type, const QString &name, const QJsonObject &json)
{
    GameObjectRepository::doLoadObject(type, name, json);

    Rect bounding_rect = boundingRect(getInfo(type, name)->cells);
    int layer_count = bounding_rect.bottom - bounding_rect.top + 1;

    RenderInfo info;

    auto texture_arr = json.value("textures").toArray();
    for (int i = 0; i < texture_arr.size(); ++i) {
        QString texture_name = texture_arr[i].toString();
        if (texture_name.indexOf("*") >= 0) {
            for (int j = 1; j <= layer_count; ++j) {
                QString cur_name = texture_name;
                cur_name.replace("*", QString::number(j));
                info.textures.append(cur_name);
            }
            continue;
        }
        info.textures.append(texture_name);
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
    type_priorities_["x-backward"] = 0.0;
    type_priorities_["ground"] = 0.2;
    type_priorities_["static"] = 0.4;
    type_priorities_["moving"] = 0.6;
    type_priorities_["x-forward"] = 0.8;
}
