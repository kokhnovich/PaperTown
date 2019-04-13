#include <QtDebug>
#include <QHash>
#include "gameobjectproperties.h"

bool GameObjectPropertyContainer::canSelect(bool last_value) const
{
    for (auto property : qAsConst(properties_)) {
        last_value = property->canSelect(last_value);
    }
    return last_value;
}

bool GameObjectPropertyContainer::canMove(bool last_value) const
{
    for (auto property : qAsConst(properties_)) {
        last_value = property->canMove(last_value);
    }
    return last_value;
}

bool GameObjectPropertyContainer::canSetPosition(bool last_value, const Coordinate &position) const
{
    for (auto property : qAsConst(properties_)) {
        last_value = property->canSetPosition(last_value, position);
    }
    return last_value;
}

GameObjectProperty *GameObjectPropertyContainer::castTo(const QMetaObject *meta)
{
    GameObjectProperty *result = GameObjectProperty::castTo(meta);
    if (result != nullptr) {
        return result;
    }
    for (auto property : qAsConst(properties_)) {
        result = property->castTo(meta);
        if (result != nullptr) {
            break;
        }
    }
    return result;
}

GameObjectPropertyContainer::GameObjectPropertyContainer()
    : GameObjectProperty()
{
    connect(this, &GameObjectProperty::gameObjectSet, this, &GameObjectPropertyContainer::setGameObjectToChildren);
}

void GameObjectPropertyContainer::addProperty(GameObjectProperty* property)
{
    if (gameObject() != nullptr) {
        property->setGameObject(gameObject());
    }
    property->setParent(this);
    connect(property, &GameObjectProperty::updated, this, &GameObjectProperty::updated);
    properties_.append(property);
}

void GameObjectPropertyContainer::setGameObjectToChildren()
{
    for (auto property : qAsConst(properties_)) {
        property->setGameObject(gameObject());
    }
}

static QHash<QString, const QMetaObject *> g_properties; // clazy:exclude=non-pod-global-static

GameObjectProperty *createProperty(const QString& name)
{
    const QMetaObject *meta = g_properties.value(name);
    Q_CHECK_PTR(meta);
    GameObjectProperty *result = qobject_cast<GameObjectProperty *>(meta->newInstance());
    Q_CHECK_PTR(result);
    return result;
}

void registerProperty(const QString& name, const QMetaObject* meta)
{
    Q_ASSERT(meta->inherits(&GameObjectProperty::staticMetaObject));
    g_properties[name] = meta;
    qDebug() << "registerProperty: property" << name << "with type" << meta->className() << "registered";
}

GAME_PROPERTY_REGISTER("empty", GameObjectProperty) // clazy:exclude=non-pod-global-static
