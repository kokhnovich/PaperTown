#include "gamelist.h"

GameList::GameList(QObject *parent) : QObject(parent) {}

bool GameList::empty() const
{
    return data_.size() == 0;
}

void GameList::remove(GameObject *object)
{
    data_.removeOne(object);
}

void GameList::add(GameObject *object)
{
    data_.append(object);
}

QVector<GameObject *> GameList::get() const
{
    return data_;
}
