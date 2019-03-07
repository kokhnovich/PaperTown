#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QObject>
#include "gamemap.h"
#include "gamemultimap.h"

struct Cell{
    GameObject* ground_object;
    GameObject* static_object;
    QVector<GameObject*> moving_object;

    Cell(GameObject* ground_object_ = nullptr,
         GameObject* static_object_ = nullptr,
         QVector<GameObject*> moving_object_ = QVector<GameObject*>()) :
        ground_object(ground_object_),
        static_object(static_object_),
        moving_object(moving_object_) {}
};

class GameField : public GameFieldBase
{
    Q_OBJECT
public:
    GameField(QObject *parent = nullptr, int size_n = 1, int size_m = 1);

    void add(GameObject* object);

    void remove(GameObject* object);

    bool canPlace(GameObject* object) const;

    bool canMoveObject(GameObject *object, const Coordinate &pos) const override;

    bool move(GameObject* object, const Coordinate& new_pos);

    GameList* get(const QString& type) const;

    Cell getCell(const Coordinate& pos) const;

    GameObjectRepository* repository() const;

private:
    GameMap* ground_map_;
    GameMap* static_map_;
    GameMultimap* moving_map_;

    GameList* ground_list_;
    GameList* static_list_;
    GameList* moving_list_;

    GameObjectRepository *repository_;
};

#endif // GAMEFIELD_H
