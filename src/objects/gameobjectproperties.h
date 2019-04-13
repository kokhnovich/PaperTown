#ifndef GAMEOBJECTPROPERTYCONTAINER_H
#define GAMEOBJECTPROPERTYCONTAINER_H

#include <QVector>
#include "../core/gameobjects.h"

class GameObjectPropertyContainer : public GameObjectProperty
{
    Q_OBJECT
public:
    bool canSelect(bool last_value) const override;
    bool canMove(bool last_value) const override;
    bool canSetPosition(bool last_value, const Coordinate &position) const override;

    void addProperty(GameObjectProperty *property);

    Q_INVOKABLE GameObjectPropertyContainer();
protected:
    GameObjectProperty *castTo(const QMetaObject *meta) override;
private slots:
    void setGameObjectToChildren();
private:
    QVector<GameObjectProperty *> properties_;
};

void registerProperty(const QString &name, const QMetaObject *meta);
GameObjectProperty *createProperty(const QString &name);

#define GAME_PROPERTY_REGISTER(name, type) \
    namespace type##__Registerer__ { \
        static bool registered = []() { \
            registerProperty(name, &type::staticMetaObject); \
            return true; \
        }(); \
    }

#endif // GAMEOBJECTPROPERTYCONTAINER_H
