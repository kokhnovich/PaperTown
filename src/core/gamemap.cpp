#include "gamemap.h"

GameMap::GameMap(int size_n, int size_m) : map_(size_n, QVector<GameObject*>(size_m, nullptr)) {
}

GameObject* GameMap::At(int i, int j) const {
    return map_.at(i).at(j);
}

void GameMap::clear() {
    map_.clear();
    update(0, 0);
}

void GameMap::addObject(GameObject* object, int x, int y) {
    map_[x][y] = object;
    update(x, y);
}

void GameMap::deleteObject(int x, int y) {
    map_[x][y] = nullptr;
    update(x, y);
}
