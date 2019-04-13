#include <algorithm>
#include "gameobjectrepository.h"

GameObjectRepository::GameObjectRepository(QObject *parent)
    : GameObjectRepositoryBase(parent)
{}

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
        QString type = local_object.value("type").toString();
        doLoadObject(type, it.key(), local_object);
    }
}

void GameObjectRepository::hideKey(const QString &type, const QString &name)
{
    is_hidden_[fullName(type, name)] = true;
}

void GameObjectRepository::unhideKey(const QString &type, const QString &name)
{
    is_hidden_[fullName(type, name)] = false;
}

bool GameObjectRepository::isKeyHidden(const GameObjectKey &key)
{
    return is_hidden_.value(fullName(key.type, key.name), false);
}

QVector<GameObjectKey> GameObjectRepository::keys(bool show_hidden)
{
    auto keys = GameObjectRepositoryBase::keys();
    if (show_hidden) {
        return keys;
    }
    keys.erase(std::remove_if(keys.begin(), keys.end(), [this](const GameObjectKey &key) {
        return isKeyHidden(key);
    }), keys.end());
    return keys;
}

void GameObjectRepository::doLoadObject(const QString &type, const QString &name, const QJsonObject &json)
{
    auto cell_arr = json.value("cells").toArray();
    GameObjectRepositoryBase::GameObjectInfo info;
    info.cells.resize(cell_arr.size());
    for (int i = 0; i < cell_arr.size(); ++i) {
        info.cells[i].x = cell_arr.at(i).toArray().at(0).toInt();
        info.cells[i].y = cell_arr.at(i).toArray().at(1).toInt();
    }
    auto keys_obj = json.value("keys");
    if (keys_obj.isObject()) {
        info.keys = keys_obj.toVariant().toMap();
    }
    addObject(type, name, info);
    if (json.value("hidden").toBool(false)) {
        hideKey(type, name);
    }
}

