#ifndef GAMEMULTIMAP_H
#define GAMEMULTIMAP_H

#include <QObject>
#include <QVector>
#include "gameobjects.h"
#include "gamelist.h"

class GameMultimap : public QObject
{
    Q_OBJECT
public:
    GameMultimap(QObject *parent = nullptr, int size_n = 1, int size_m = 1);
    const GameList *at(const Coordinate &pos) const;
    void add(GameObject *object);
    void remove(GameObject *object);

public slots:
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);

protected:
    GameList *atMut(const Coordinate &pos);
private:
    void internalAdd(GameObject *object);
    QVector<QVector<GameList *>> map_;
};

#endif // GAMEMULTIMAP_H
