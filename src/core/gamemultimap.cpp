#include "gamemultimap.h"

GameMultimap::GameMultimap(int size_n, int size_m) :
    map_(size_n, QVector<GameList*>(size_m, new GameList()))
{
}

GameList* GameMultimap::at(const Coordinate &pos) const
{
    return map_[pos.x][pos.y];
}

void GameMultimap::add(GameObject *object)
{
    connect(object, SIGNAL(move(Coordinate,Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    internalAdd(object);
}

void GameMultimap::remove(GameObject *object)
{
    disconnect(object, SIGNAL(move(Coordinate, Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    for (const Coordinate& cell : object->cells()) {
        at(cell)->remove(object);
    }
}

void GameMultimap::moved(const Coordinate &oldPosition, const Coordinate&)
{
    auto object = qobject_cast<GameObject*>(sender());
    auto res = object->cellsRelative();
    for (const Coordinate &coord : res) {
        at(coord + oldPosition)->remove(object);
    }
    internalAdd(object);
}

void GameMultimap::internalAdd(GameObject *object)
{
    for(const Coordinate cell : object->cells()) {
        at(cell)->add(object);
    }
}
