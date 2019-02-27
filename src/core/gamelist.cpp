#include "gamelist.h"

void GameList::remove(int pos, int count) {
    data_.remove(pos, count);
}

void GameList::removeEqual(GameObject* const & object) {
    data_.removeAll(object);
}
