#include "gameobjecttypes.h"

GroundObject::GroundObject(const QString &name, GameObjectProperty *property, GameObjectRepositoryBase *repository)
    : GameObject(name, property, repository)
{}

QString GroundObject::type() const
{
    return "ground";
}

bool GroundObject::internalCanMove() const
{
    return false;
}

MovingObject::MovingObject(const QString &name, GameObjectProperty *property, GameObjectRepositoryBase *repository)
    : GameObject(name, property, repository)
{
}

QString MovingObject::type() const
{
    return "moving";
}

bool StaticObject::internalCanMove() const
{
    return true;
}

StaticObject::StaticObject(const QString &name, GameObjectProperty *property, GameObjectRepositoryBase *repository)
    : GameObject(name, property, repository)
{}

QString StaticObject::type() const
{
    return "static";
}
