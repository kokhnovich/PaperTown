#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <QObject>
#include "gameobjects.h"

class GameObjectFactory : public QObject
{
    Q_OBJECT
public:
    GameObjectFactory(GameObjectRepositoryBase *repository, GameIndicators *indicators, QObject *parent = nullptr);
    GameObject *createObject(const QString &type, const QString &name);
private:
    GameObjectRepositoryBase *repository_;
    GameIndicators *indicators_;
};

#endif // GAMEOBJECTFACTORY_H
