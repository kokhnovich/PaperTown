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
    for (Coordinate cell : object->cells()) {
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
        bool is_existed = isExists(object);
        for (Coordinate cell : object->cells()) {
            map_[cell.x][cell.y] = object;
        }
        if (is_existed) {
            emit moved(object);
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
    for (Coordinate cell : object->cells()) {
        map_[cell.x][cell.y] = nullptr;
    }
}

bool GameMap::isExists(GameObject *object) const
{
    for (const auto& i : map_) {
        for(const auto& j : i) {
            if (j == object) {
                return true;
            }
        }
    }
    return false;
}
