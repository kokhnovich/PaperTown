#ifndef GAMEOBJECTTYPES_H
#define GAMEOBJECTTYPES_H

#include <QObject>
#include "gameobjects.h"

class GroundObject : public GameObject
{
    Q_OBJECT
public:
    GroundObject(const QString &name);
    QString type() const override;
protected:
    bool internalCanMove() const override;
};

class StaticObject : public GameObject
{
    Q_OBJECT
public:
    StaticObject(const QString &name);
    QString type() const override;
    bool conflictsWith(const GameObject *object) const override;
protected:
    bool internalCanMove() const override;
};

class MovingObject : public GameObject
{
    Q_OBJECT
public:
    MovingObject(const QString &name);
    QString type() const override;
};


#endif // GAMEOBJECTTYPES_H
