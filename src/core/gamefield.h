#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QObject>
#include "gamemap.h"
#include "gamelist.h"

struct Cell {
    GameObject *ground_obj;
    GameObject *static_obj;
    QVector<GameObject *> moving_obj;

    Cell(GameObject *ground_obj = nullptr,
         GameObject *static_obj = nullptr,
         QVector<GameObject *> moving_obj = QVector<GameObject *>()) :
        ground_obj(ground_obj),
        static_obj(static_obj),
        moving_obj(moving_obj) {}
};

class GameField : public GameFieldBase
{
    Q_OBJECT
public:
    GameField(QObject *parent = nullptr, int size_n = 1, int size_m = 1);
    void add(GameObject *object);
    void remove(GameObject *object);
    bool canPlace(GameObject *object, const Coordinate &pos) const override;
    bool move(GameObject *object, const Coordinate &new_pos);
    GameList *getByType(const QString &type) const;
    Cell getCell(const Coordinate &pos) const;
    GameObjectRepository *repository() const override;

private:
    GameObjectRepository *repository_;

    GameMap *ground_map_;
    GameMap *static_map_;
    GameMultimap *moving_map_;

    GameList *ground_list_;
    GameList *static_list_;
    GameList *moving_list_;
};

#endif // GAMEFIELD_H
