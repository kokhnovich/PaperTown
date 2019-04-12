#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <QObject>
#include "gameobjects.h"

class GameObjectFactory : public QObject
{
    Q_OBJECT
public:
    GameObjectFactory(GameObjectRepositoryBase *repository, QObject *parent = nullptr);
    GameObject *createObject(const QString &type, const QString &name);
private:
    GameObjectRepositoryBase *repository_;
};

#endif // GAMEOBJECTFACTORY_H
