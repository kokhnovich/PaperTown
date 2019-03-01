#ifndef GAMELIST_H
#define GAMELIST_H

#include <QVector>
#include "gameobjects.h"

class GameList : public QObject
{
    Q_OBJECT
public:
    GameList();
    void addObject(GameObject* object);
    void remove(GameObject* object);

signals:

public slots:

signals:
    void update(GameObject*);

private:
    QVector<GameObject*> data_;
    GameFieldBase *field_;
};

#endif // GAMELIST_H
