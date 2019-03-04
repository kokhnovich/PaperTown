#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>
#include <QVector>
#include "gameobjects.h"

class GameMap : public QObject
{
    Q_OBJECT
public:
    GameMap(QObject *parent = nullptr, int size_n = 1, int size_m = 1);
    GameObject *at(const Coordinate &pos) const;
    void add(GameObject *object);
    void remove(GameObject *object);
    bool canPlace(GameObject *object) const;

public slots:
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);

private:
    void internalAdd(GameObject *object);
    QVector<QVector<GameObject *>> map_;
};

#endif // GAMEMAP_H

