#include "gameobjectfactory.h"
#include "gameobjecttypes.h"

GameObjectFactory::GameObjectFactory(GameFieldBase *field, QObject *parent)
    : QObject(parent), field_(field)
{}

GameObject *GameObjectFactory::createObject(const QString &type, const QString &name)
{
    GameObject *object = nullptr;
    GameObjectProperty *property = field_->repository()->createProperty(type, name);
    if (type == "ground") {
        object = new GroundObject(name);
    } else if (type == "static") {
        object = new StaticObject(name);
    } else if (type == "moving") {
        object = new MovingObject(name);
    } else {
        Q_UNREACHABLE();
    }
    object->initialize(field_, property);
    return object;
}
