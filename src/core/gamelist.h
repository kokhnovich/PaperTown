#ifndef GAMELIST_H
#define GAMELIST_H

#include <QVector>
#include "gameobjects.h"

class GameList : public QObject
{
    Q_OBJECT
public:
    GameList(QObject* parent = nullptr);
    bool empty() const;
    void add(GameObject *object);
    void remove(GameObject *object);
    QVector<GameObject *> get() const;

private:
    QVector<GameObject *> data_;
};

#endif // GAMELIST_H
