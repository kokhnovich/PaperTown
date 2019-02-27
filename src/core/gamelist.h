#ifndef GAMELIST_H
#define GAMELIST_H

#include <QVector>
#include "gameobjects.h"

class GameList
{
public:
    GameList();
    void addObject(const GameObject& object);
    void remove(int pos, int count = 1);
    int removeEqual(GameObject* const& object);
    GameObject* operator[](int pos);

private:
    QVector<GameObject*> data_;
};

#endif // GAMELIST_H
