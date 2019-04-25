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
    bool conflitsWith(bool last_value, const GameObject *object) const override;
    bool canAutoEnable(bool last_value) const override;

    void addProperty(GameObjectProperty *property);

    inline const QVector<GameObjectProperty *> &properties() { return properties_; }

    Q_INVOKABLE GameObjectPropertyContainer();
protected:
    GameObjectProperty *castTo(const QMetaObject *meta) override;
    void doInitialize() override;
private:
    QVector<GameObjectProperty *> properties_;
};

void registerProperty(const QString &name, const QMetaObject *meta);
GameObjectProperty *createProperty(const QString &name);

#define GAME_PROPERTY_REGISTER(name, type) \
    namespace type##__Registerer__ { \
        struct Register \
        { \
            Register() { \
                registerProperty(name, &type::staticMetaObject); \
            } \
        }; \
        static Register reg_##type_; \
    }

#endif // GAMEOBJECTPROPERTYCONTAINER_H
