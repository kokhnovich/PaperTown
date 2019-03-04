#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QObject>
#include "gamemap.h"
#include "gamemultimap.h"

class GameField : public QObject
{
    Q_OBJECT
public:
    explicit GameField(QObject *parent = nullptr);

signals:

public slots:

private:
    GameMap* ground_map_; // for ground
    GameMap* static_map_; // for buildings
    GameMultimap moving_map_;

    GameList* ground_list_;
    GameList* static_list_;
    GameList* moving_list_;
};

#endif // GAMEFIELD_H
