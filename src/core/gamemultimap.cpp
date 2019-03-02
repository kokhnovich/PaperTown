#include "gamemultimap.h"

GameList& GameMultimap::at(const Coordinate &pos)
{
    return map_[pos.x][pos.y];
}

void GameMultimap::clear()
{
    map_.clear();
}

void GameMultimap::add(GameObject *object)
{
    connect(object, SIGNAL(move(Coordinate,Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    internalAdd(object);
}

void GameMultimap::remove(GameObject *object)
{
    disconnect(object, SIGNAL(move(Coordinate,Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    for (const Coordinate& cell : object->cells()) {
        at(cell).remove(object);
    }
}

void GameMultimap::moved(const Coordinate &oldPosition, const Coordinate &newPosition)
{
    auto object = qobject_cast<GameObject*>(sender());
    auto res = object->cellsRelative();
    for (const Coordinate &coord : res) {
        at(coord + oldPosition).remove(object);
    }
    internalAdd(object);
}

void GameMultimap::internalAdd(GameObject *object)
{
    for(const Coordinate cell : object->cells()) {
        at(cell).addObject(object);
    }
}
