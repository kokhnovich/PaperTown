#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QObject>
#include "gamemap.h"
#include "gamemultimap.h"

struct Cell{
    GameObject* ground_object;
    GameObject* static_object;
    QVector<GameObject*> moving_object;
    //GameList* moving_object;
    Cell() : ground_object(nullptr), static_object(nullptr), moving_object() {}
    Cell(GameObject* ground_object_ = nullptr,
         GameObject* static_object_ = nullptr,
         QVector<GameObject*> moving_object_ = QVector<GameObject*>()) :
        ground_object(ground_object_),
        static_object(static_object_),
        moving_object(moving_object_) {}
};

class GameField : public QObject
{
    Q_OBJECT
public:
    explicit GameField(QObject *parent = nullptr);

    void add(GameObject* object);
    \
    void remove(GameObject* object);

    bool moveObject(GameObject* object, const Coordinate& new_pos);

    GameList* get(const QString& type) const;

    Cell getCell(const Coordinate& pos) const;

private:
    GameMap* ground_map_; // for ground
    GameMap* static_map_; // for buildings
    GameMultimap* moving_map_;

    GameList* ground_list_;
    GameList* static_list_;
    GameList* moving_list_;
};

#endif // GAMEFIELD_H
