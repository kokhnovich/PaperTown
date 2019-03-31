#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QObject>
#include "gamemap.h"
#include "gamelist.h"

class GameField : public GameFieldBase
{
    Q_OBJECT
public:
    GameField(QObject *parent, int height, int width);
    void add(GameObject *object);
    void remove(GameObject *object);
    bool canPlace(GameObject *object, const Coordinate &pos) const override;
    GameList *getByType(const QString &type) const;
    QVector<GameObject *> getCell(const Coordinate &pos) const;
    GameObjectRepository *repository() const override;
signals:
    void added(GameObject *object);
    void removed(GameObject *object);
    void placed(GameObject *object, const Coordinate &pos);
    void moved(GameObject *object, const Coordinate &oldPos, const Coordinate &newPos);
    void updated(GameObject *object);
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
