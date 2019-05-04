#include <QtDebug>
#include <QHash>
#include "gameobjectproperties.h"

bool GameObjectPropertyContainer::canSelect(bool last_value) const
{
    last_value = GameObjectProperty::canSelect(last_value);
    for (auto property : qAsConst(properties_)) {
        last_value = property->canSelect(last_value);
    }
    return last_value;
}

bool GameObjectPropertyContainer::canMove(bool last_value) const
{
    last_value = GameObjectProperty::canMove(last_value);
    for (auto property : qAsConst(properties_)) {
        last_value = property->canMove(last_value);
    }
    return last_value;
}

bool GameObjectPropertyContainer::canSetPosition(bool last_value, const Coordinate &position) const
{
    last_value = GameObjectProperty::canSetPosition(last_value, position);
    for (auto property : qAsConst(properties_)) {
        last_value = property->canSetPosition(last_value, position);
    }
    return last_value;
}

bool GameObjectPropertyContainer::canPlace(bool last_value) const
{
    last_value = GameObjectProperty::canPlace(last_value);
    for (auto property : qAsConst(properties_)) {
        last_value = property->canPlace(last_value);
    }
    return last_value;    
}

bool GameObjectPropertyContainer::conflictsWith(bool last_value, const GameObject *object) const
{
    last_value = GameObjectProperty::conflictsWith(last_value, object);
    for (auto property : qAsConst(properties_)) {
        last_value = property->conflictsWith(last_value, object);
    }
    return last_value;
}

bool GameObjectPropertyContainer::canAutoEnable(bool last_value) const
{
    last_value = GameObjectProperty::canAutoEnable(last_value);
    for (auto property : qAsConst(properties_)) {
        last_value = property->canAutoEnable(last_value);
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
{}

void GameObjectPropertyContainer::addProperty(GameObjectProperty *property)
{
    if (isInitialized()) {
        property->initialize(gameObject());
    }
    property->setParent(this);
    connect(property, &GameObjectProperty::updated, this, &GameObjectProperty::updated);
    properties_.append(property);
}

void GameObjectPropertyContainer::doInitialize()
{
    for (auto property : qAsConst(properties_)) {
        property->initialize(gameObject());
    }
}

using PropertiesHash = QHash<QString, const QMetaObject *>;

Q_GLOBAL_STATIC(PropertiesHash, g_properties)

GameObjectProperty *createProperty(const QString &name)
{
    if (!g_properties->contains(name)) {
        qCritical() << "property" << name << "not found";
        Q_ASSERT(false);
    }
    const QMetaObject *meta = g_properties->value(name);
    Q_CHECK_PTR(meta);
    GameObjectProperty *result = qobject_cast<GameObjectProperty *>(meta->newInstance());
    Q_CHECK_PTR(result);
    return result;
}

void registerProperty(const QString &name, const QMetaObject *meta)
{
    Q_ASSERT(meta->inherits(&GameObjectProperty::staticMetaObject));
    (*g_properties)[name] = meta;
    qDebug() << "registerProperty: property" << name << "with type" << meta->className() << "registered";
}

GAME_PROPERTY_REGISTER("empty", GameObjectProperty)
