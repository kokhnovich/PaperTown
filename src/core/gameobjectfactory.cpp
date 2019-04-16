#include "gameobjectfactory.h"
#include "gameobjecttypes.h"

GameObjectFactory::GameObjectFactory(GameObjectRepositoryBase *repository, QObject *parent)
    : QObject(parent), repository_(repository)
{}

GameObject *GameObjectFactory::createObject(const QString &type, const QString &name)
{
    GameObject *object = nullptr;
    GameObjectProperty *property = repository_->createProperty(type, name);
    if (type == "ground") {
        object = new GroundObject(name, repository_);
    } else if (type == "static") {
        object = new StaticObject(name, repository_);
    } else if (type == "moving") {
        object = new MovingObject(name, repository_);
    } else {
        Q_UNREACHABLE();
    }
    object->initProperty(property);
    return object;
}
