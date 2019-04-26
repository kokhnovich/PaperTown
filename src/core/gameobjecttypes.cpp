#include "gameobjecttypes.h"

GroundObject::GroundObject(const QString &name, GameObjectRepositoryBase *repository)
    : GameObject(name, repository)
{}

QString GroundObject::type() const
{
    return "ground";
}

bool GroundObject::internalCanMove() const
{
    return false;
}

MovingObject::MovingObject(const QString &name, GameObjectRepositoryBase *repository)
    : GameObject(name, repository)
{}

QString MovingObject::type() const
{
    return "moving";
}

bool StaticObject::internalCanMove() const
{
    return true;
}

StaticObject::StaticObject(const QString &name, GameObjectRepositoryBase *repository)
    : GameObject(name, repository)
{}

QString StaticObject::type() const
{
    return "static";
}

bool StaticObject::conflictsWith(const GameObject *object) const
{
    bool res = object->type() == "moving";
    if (property() != nullptr) {
        res = property()->conflictsWith(res, object);
    }
    return res;
}

