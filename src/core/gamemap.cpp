#include "gamemap.h"

GameMap::GameMap(int size_n, int size_m) : map_(size_n, QVector<GameObject*>(size_m, nullptr))
{
}

GameObject* GameMap::at(const Coordinate &pos) const
{
    return map_.at(pos.x).at(pos.y);
}

void GameMap::clear()
{
    map_.clear();
}

bool GameMap::canPlace(GameObject *object) const
{
    for (const Coordinate& cell : object->cells()) {
        if (at(cell) != nullptr) {
            if (at(cell) != object) {
                return false;
            }
        }
    }
    return true;
}

void GameMap::add(GameObject* object)
{
    connect(object, SIGNAL(move(Coordinate,Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    internalAdd(object);
}

void GameMap::remove(const Coordinate& pos)
{
    if (map_[pos.x][pos.y] != nullptr) {
        remove(map_[pos.x][pos.y]);
    }
}


void GameMap::remove(GameObject *object)
{
    disconnect(object, SIGNAL(move(Coordinate,Coordinate)), this, SLOT(moved(Coordinate, Coordinate)));
    for (const Coordinate& cell : object->cells()) {
        map_[cell.x][cell.y] = nullptr;
    }
}

void GameMap::moved(const Coordinate &oldPosition, const Coordinate &newPosition)
{
    auto object = at(oldPosition);
    auto res = object->cellsRelative();
    for (const Coordinate &coord : res) {
        map_[coord.x + oldPosition.x][coord.y + oldPosition.y] = nullptr;
    }
    internalAdd(object);
}

void GameMap::internalAdd(GameObject *object)
{
    if (!canPlace(object)) return;
    for (const Coordinate& cell : object->cells()) {
        map_[cell.x][cell.y] = object;
    }
}
