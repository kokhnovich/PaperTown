#include "gameobjectfactory.h"

GameObjectFactory::GameObjectFactory(QObject *parent)
    : QObject(parent)
{}

GameObject *GameObjectFactory::createObject(const QString &type, const QString &name)
{
    if (type == "ground") {
        return new GroundObject(name);
    }
    if (type == "static") {
        return new StaticObject(name);
    }
    if (type == "moving") {
        return new MovingObject(name);
    }
}
