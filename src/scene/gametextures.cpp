#include <QGraphicsItem>
#include <QString>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>
#include "gametextures.h"

GameTextureRepository::GameTextureRepository(QObject* parent)
    : QObject(parent)
{
}

const GameTexture *GameTextureRepository::getTexture(const QString &name) const
{
    return textures_.value(name).data();
}

void GameTextureRepository::addTexture(const QString &name, const QPixmap &pixmap,
        const QPointF &offset, const Coordinate &z_offset)
{
    Q_ASSERT(!textures_.contains(name));
    QSharedPointer<GameTexture> ptr(new GameTexture {pixmap, offset, z_offset});
    Q_ASSERT(!ptr->pixmap.isNull());
    textures_[name] = ptr;
}

void GameTextureRepository::addTexture(const QString &name, const QPointF &offset, const Coordinate &z_offset)
{
    addTexture(name, QPixmap(QStringLiteral(":/img/%1.png").arg(name.data())), offset, z_offset);
}

void GameTextureRepository::loadFromJson(const QJsonDocument &document)
{
    Q_ASSERT(document.isObject());
    auto root_object = document.object();
    for (auto it = root_object.begin(); it != root_object.end(); ++it) {
        QJsonObject obj = it->toObject();
        QJsonArray offset = obj.value("offset").toArray();
        QJsonArray z_offset = obj.value("z-offset").toArray();
        addTexture(it.key(),
                   {offset.at(0).toDouble(), offset.at(1).toDouble()},
                   {z_offset.at(0).toInt(), z_offset.at(1).toInt()});
    }
}

void GameTextureRepository::loadFromFile(const QString &file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    qDebug() << "GameTextureRepository: json parsed, error =" << error.errorString();
    Q_ASSERT(error.error == QJsonParseError::NoError);
    return loadFromJson(document);
}
