#ifndef GAMETEXTURES_H
#define GAMETEXTURES_H

#include <QObject>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QMap>

struct GameTexture {
    QPixmap pixmap;
    QPointF offset;
};

class GameTextureRespository : public QObject
{
    Q_OBJECT
public:
    
private:
};

#endif // GAMETEXTURES_H
