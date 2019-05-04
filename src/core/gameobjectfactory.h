#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <QObject>
#include "gameobjects.h"

class GameObjectFactory : public QObject
{
    Q_OBJECT
public:
    GameObjectFactory(GameFieldBase *field, QObject *parent = nullptr);
    GameObject *createObject(const QString &type, const QString &name);
private:
    GameFieldBase *field_;
};

#endif // GAMEOBJECTFACTORY_H
