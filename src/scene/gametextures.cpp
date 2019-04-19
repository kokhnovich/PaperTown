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
    if (!textures_.contains(name)) {
        qCritical() << "could not get texture with name = " << name;
        Q_ASSERT(textures_.contains(name));
    }
    return textures_.value(name).data();
}

void GameTextureRepository::addTexture(const QString &name, const QPixmap &pixmap,
        const QPointF &offset, const Coordinate &z_offset)
{
    Q_ASSERT(!textures_.contains(name));
    QSharedPointer<GameTexture> ptr(new GameTexture {pixmap, offset, z_offset});
    if (ptr->pixmap.isNull()) {
        qWarning() << "texture" << name << "was not found";
    }
    Q_ASSERT(!ptr->pixmap.isNull());
    textures_[name] = ptr;
}

void GameTextureRepository::addTexture(const QString &name, const QPointF &offset, const Coordinate &z_offset)
{
    addTexture(name, QPixmap(QStringLiteral(":/img/%1.png").arg(name)), offset, z_offset);
}

void GameTextureRepository::loadFromJson(const QJsonDocument &document)
{
    Q_ASSERT(document.isObject());
    auto root_object = document.object();
    for (auto it = root_object.begin(); it != root_object.end(); ++it) {
        QJsonObject obj = it->toObject();
        QJsonArray offset_arr = obj.value("offset").toArray();
        QJsonArray z_offset_arr = obj.value("z-offset").toArray();
        
        QString key = it.key();
        QPointF offset {offset_arr.at(0).toDouble(), offset_arr.at(1).toDouble()};
        Coordinate z_offset {z_offset_arr.at(0).toInt(), z_offset_arr.at(1).toInt()};
        
        addTexture(key, offset, z_offset);
        
        int layers = obj.value("layers").toInt();
        for (int i = 0; i < layers; ++i) {
            addTexture(QStringLiteral("%1.l%2").arg(key).arg(i+1), offset, z_offset + Coordinate(i, 0));
        }
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
