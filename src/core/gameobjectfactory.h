#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <QObject>
#include "gameobjects.h"

class GameObjectFactory : public QObject
{
    Q_OBJECT
public:
    GameObjectFactory(QObject *parent = nullptr);
    GameObject *createObject(const QString &type, const QString &name);
};

#endif // GAMEOBJECTFACTORY_H
