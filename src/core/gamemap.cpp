#include "gamemap.h"

GameMap::GameMap(int size_n, int size_m) : map_(size_n, QVector<GameObject*>(size_m, nullptr))
{
}

GameObject* GameMap::at(int i, int j) const
{
    return map_.at(i).at(j);
}

void GameMap::clear()
{
    map_.clear();
}

bool GameMap::canPlace(GameObject *object) const
{
    for (const Coordinate& cell : object->cells()) {
        if (map_[cell.x][cell.y] != nullptr) {
            if (map_[cell.x][cell.y] != object) {
                return false;
            }
        }
    }
    return true;
}

void GameMap::add(GameObject* object)
{
    if (canPlace(object)) {
        for (Coordinate& cell : object->cells()) {
            map_[cell.x][cell.y] = object;
        }
    }
}

void GameMap::remove(int x, int y)
{
    if (map_[x][y] != nullptr) {
        remove(map_[x][y]);
    }
}


void GameMap::remove(GameObject *object)
{
    for (const Coordinate& cell : object->cells()) {
        map_[cell.x][cell.y] = nullptr;
    }
}

void GameMap::removeOld(GameObject* object)
{
    for (auto& i : map_) {
        for (auto& j : i) {
            if (j == object) {
                j = nullptr;
            }
        }
    }
}

void GameMap::moved(GameObject* object)
{
    removeOld(object);
    add(object);
}
