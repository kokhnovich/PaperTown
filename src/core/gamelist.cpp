#include "gamelist.h"

void GameList::remove(GameObject *object) {
    data_.removeOne(object);
}

void GameList::addObject(GameObject *object) {
    data_.append(object);
}
