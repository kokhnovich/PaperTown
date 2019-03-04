#include "gamemultimap.h"

GameMultimap::GameMultimap(QObject *parent, int size_n, int size_m) :
    QObject(parent), map_(size_n, QVector<GameList*>(size_m, new GameList()))
{
}

const GameList *GameMultimap::at(const Coordinate &pos) const
{
    return map_[pos.x][pos.y];
}

GameList *GameMultimap::atMut(const Coordinate &pos)
{
    return map_[pos.x][pos.y];
}

void GameMultimap::add(GameObject *object)
{
    connect(object, SIGNAL(move(Coordinate, Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    internalAdd(object);
}

void GameMultimap::remove(GameObject *object)
{
    disconnect(object, SIGNAL(move(Coordinate, Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    for (const Coordinate &cell : object->cells()) {
        atMut(cell)->remove(object);
    }
}

void GameMultimap::moved(const Coordinate &oldPosition, const Coordinate &)
{
    auto object = qobject_cast<GameObject *>(sender());
    auto res = object->cellsRelative();
    for (const Coordinate &coord : res) {
        atMut(coord + oldPosition)->remove(object);
    }
    internalAdd(object);
}

void GameMultimap::internalAdd(GameObject *object)
{
    for (const Coordinate &cell : object->cells()) {
        atMut(cell)->add(object);
    }
}
