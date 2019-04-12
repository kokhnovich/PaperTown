#include "gameobjectfactory.h"

GameObjectFactory::GameObjectFactory(GameObjectRepositoryBase *repository, QObject *parent)
    : QObject(parent), repository_(repository)
{}

GameObject *GameObjectFactory::createObject(const QString &type, const QString &name)
{
    GameObjectProperty *property = repository_->createProperty(type, name);
    if (type == "ground") {
        return new GroundObject(name, property);
    }
    if (type == "static") {
        return new StaticObject(name, property);
    }
    if (type == "moving") {
        return new MovingObject(name, property);
    }
    Q_UNREACHABLE();
}
