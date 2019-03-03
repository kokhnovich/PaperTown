#ifndef GAMEMULTIMAP_H
#define GAMEMULTIMAP_H

#include <QObject>
#include <QVector>
#include "gameobjects.h"
#include "gamelist.h"

class GameMultimap : public QObject
{
    Q_OBJECT
public:
    GameMultimap(int size_n, int size_m);
    GameList* at(const Coordinate& pos) const;
    void add(GameObject* object);
    void remove(GameObject* object);

public slots:
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);

private:
    void internalAdd(GameObject* object);
    QVector<QVector<GameList*>> map_;
};

#endif // GAMEMULTIMAP_H
