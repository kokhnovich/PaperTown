#ifndef GAMETEXTURES_H
#define GAMETEXTURES_H

#include <QObject>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QMap>
#include <QJsonDocument>
#include <QGraphicsScene>
#include <QSharedPointer>
#include "../core/gameobjects.h"

struct GameTexture {
    QPixmap pixmap;
    QPointF offset;
    Coordinate z_offset;
};

class GameTextureRepository : public QObject
{
    Q_OBJECT
public:
    void addTexture(const QString &name, const QPixmap &pixmap, const QPointF &offset, const Coordinate &z_offset);
    void addTexture(const QString &name, const QPointF &offset, const Coordinate &z_offset);
    const GameTexture *getTexture(const QString &name) const;
    void loadFromJson(const QJsonDocument &document);
    void loadFromFile(const QString &file_name);
private:
    QHash<QString, QSharedPointer<GameTexture>> textures_;
};

#endif // GAMETEXTURES_H
