#include "gamelist.h"

GameList::GameList() {}

bool GameList::empty() const
{
    return data_.size() == 0;
}

void GameList::remove(GameObject *object) {
    data_.removeOne(object);
}

void GameList::addObject(GameObject *object) {
    data_.append(object);
}
