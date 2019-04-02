#include <QGraphicsItem>
#include <QString>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "gametextures.h"

QGraphicsPixmapItem *GameTextureRepository::drawTexture(QGraphicsScene *scene, const QString &name, const QPointF &pos)
{
    Q_ASSERT(textures_.contains(name));
    QSharedPointer<GameTexture> texture = textures_.value(name);
    Q_CHECK_PTR(texture);
    QGraphicsPixmapItem *item = scene->addPixmap(texture->pixmap);
    item->setOffset(pos + texture->offset);
    return item;
}

void GameTextureRepository::addTexture(const QString &name, const QPixmap &pixmap, const QPointF &offset)
{
    Q_ASSERT(!textures_.contains(name));
    QSharedPointer<GameTexture> ptr(new GameTexture {pixmap, offset});
    textures_[name] = ptr;
}

void GameTextureRepository::addTexture(const QString &name, const QPointF &offset)
{
    addTexture(name, QPixmap(QString(":/img/%1.png").arg(name.data())), offset);
}

void GameTextureRepository::loadFromJson(const QJsonDocument &document)
{
    Q_ASSERT(document.isObject());
    auto root_object = document.object();
    for (auto it = root_object.begin(); it != root_object.end(); ++it) {
        QJsonObject obj = it->toObject();
        Q_ASSERT(obj.contains("dx"));
        Q_ASSERT(obj.contains("dy"));
        QPointF offset(obj["dx"].toDouble(), obj["dy"].toDouble());
        addTexture(it.key(), offset);
    }
}

void GameTextureRepository::loadFromFile(const QString &file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    Q_ASSERT(error.error == QJsonParseError::NoError);
    return loadFromJson(document);
}
