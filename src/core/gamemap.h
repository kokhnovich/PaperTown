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
    GameObject* At(int i, int j) const;
    void clear();
signals:

public slots:

private:
    QVector<QVector<GameObject*>> map_;
};

#endif // GAMEMAP_H
