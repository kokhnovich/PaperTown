#include "gamelist.h"

void GameList::remove(GameObject *object) {
    data_.removeOne(object);
    emit update(object);
}

void GameList::addObject(GameObject *object) {
    data_.append(object);
    emit update(object);
}
