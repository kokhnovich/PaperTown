#include "gameobjecttypes.h"

GroundObject::GroundObject(const QString &name)
    : GameObject(name)
{}

QString GroundObject::type() const
{
    return "ground";
}

bool GroundObject::internalCanMove() const
{
    return false;
}

MovingObject::MovingObject(const QString &name)
    : GameObject(name)
{}

QString MovingObject::type() const
{
    return "moving";
}

bool StaticObject::internalCanMove() const
{
    return true;
}

StaticObject::StaticObject(const QString &name)
    : GameObject(name)
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

