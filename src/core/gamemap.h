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
    GameObject* at(int i, int j) const;
    void clear();
    void add(GameObject* object);
    void remove(int x, int y);
    void remove(GameObject* object);
    bool canPlace(GameObject* object) const;

public slots:
    void removeOld(GameObject* object);
    void moved(GameObject* object);

private:
    QVector<QVector<GameObject*>> map_;
};

#endif // GAMEMAP_H
