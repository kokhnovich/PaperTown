#include "gameobjectfactory.h"
#include "gameobjecttypes.h"

GameObjectFactory::GameObjectFactory(GameObjectRepositoryBase *repository, GameIndicators *indicators, QObject *parent)
    : QObject(parent), repository_(repository), indicators_(indicators)
{}

GameObject *GameObjectFactory::createObject(const QString &type, const QString &name)
{
    GameObject *object = nullptr;
    GameObjectProperty *property = repository_->createProperty(type, name);
    if (type == "ground") {
        object = new GroundObject(name, repository_, indicators_);
    } else if (type == "static") {
        object = new StaticObject(name, repository_, indicators_);
    } else if (type == "moving") {
        object = new MovingObject(name, repository_, indicators_);
    } else {
        Q_UNREACHABLE();
    }
    object->initProperty(property);
    return object;
}
