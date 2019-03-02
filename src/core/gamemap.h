#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>
#include <QVector>
#include "gameobjects.h"

class GameMap : public QObject
{
    Q_OBJECT
public:
    GameMap(int size_n, int size_m);
    GameObject* at(const Coordinate& pos) const;
    void clear();
    void add(GameObject* object);
    void remove(const Coordinate& pos);
    void remove(GameObject* object);
    bool canPlace(GameObject* object) const;

public slots:
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);

private:
    void internalAdd(GameObject* object);
    QVector<QVector<GameObject*>> map_;
};

#endif // GAMEMAP_H