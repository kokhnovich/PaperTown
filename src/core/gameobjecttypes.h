#ifndef GAMEOBJECTTYPES_H
#define GAMEOBJECTTYPES_H

#include <QObject>
#include "gameobjects.h"

class GroundObject : public GameObject
{
    Q_OBJECT
public:
    GroundObject(const QString &name, GameObjectRepositoryBase *repository);
    QString type() const override;
protected:
    bool internalCanMove() const override;
};

class StaticObject : public GameObject
{
    Q_OBJECT
public:
    StaticObject(const QString &name, GameObjectRepositoryBase *repository);
    QString type() const override;
protected:
    bool internalCanMove() const override;
};

class MovingObject : public GameObject
{
    Q_OBJECT
public:
    MovingObject(const QString &name, GameObjectRepositoryBase *repository);
    QString type() const override;
};


#endif // GAMEOBJECTTYPES_H
