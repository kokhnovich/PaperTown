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

struct GameTexture {
    QPixmap pixmap;
    QPointF offset;
};

class GameTextureRepository : public QObject
{
    Q_OBJECT
public:
    void addTexture(const QString &name, const QPixmap &pixmap, const QPointF &offset);
    void addTexture(const QString &name, const QPointF &offset);
    QGraphicsPixmapItem *drawTexture(QGraphicsScene *scene, const QString &name, const QPointF &pos);
    void loadFromJson(const QJsonDocument &document);
    void loadFromFile(const QString &file_name);
private:
    QMap<QString, QSharedPointer<GameTexture>> textures_;
};

#endif // GAMETEXTURES_H
