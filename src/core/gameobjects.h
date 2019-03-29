#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QVector>
#include <QMap>

struct Coordinate {
    int x, y;

    Coordinate(int x = 0, int y = 0);
};

Coordinate operator+(Coordinate a, const Coordinate &b);
Coordinate operator-(Coordinate a, const Coordinate &b);
Coordinate &operator+=(Coordinate &a, const Coordinate &b);
Coordinate &operator-=(Coordinate &a, const Coordinate &b);

bool inBounds(int height, int width, const Coordinate &coord);

class GameObject;

class GameObjectRepository : public QObject
{
    Q_OBJECT
public:
    GameObjectRepository(QObject *parent);
    void addObject(const QString &type, const QString &name, const QVector<Coordinate> cells);
    QVector<Coordinate> getCells(const QString &type, const QString &name) const;
protected:
    static QString fullName(const QString &type, const QString &name);
private:
    QMap<QString, QVector<Coordinate>> cells_;
};

class GameFieldBase : public QObject
{
    Q_OBJECT
public:
    GameFieldBase(QObject *parent);
    virtual GameObjectRepository *repository() const = 0;
    virtual bool canPlace(GameObject *object, const Coordinate &pos) const = 0;
};

class GameObjectProperty : public QObject
{
    Q_OBJECT
private:
    GameObject *gameObject() const;
signals:
    void updated();
};

class GameObject : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(Coordinate position READ position WRITE setPosition)

    GameObject(const QString &name, GameObjectProperty *property = nullptr, GameFieldBase *field = nullptr);

    QString name() const;
    virtual QString type() const = 0;
    Coordinate position() const;
    virtual const QVector<Coordinate> cellsRelative() const;
    const QVector<Coordinate> cells() const;
    GameObjectProperty *property() const;

    int x() const;
    int y() const;

    bool active() const;

    GameFieldBase *field() const;

    bool canSetPosition(const Coordinate &pos);
    bool setPosition(const Coordinate &pos);
signals:
    void placed(const Coordinate &position);
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);
    void updated();
private:
    QString name_;
    bool active_;
    Coordinate position_;
    GameFieldBase *field_;
    GameObjectProperty *property_;
};

class GroundObject : public GameObject
{
    Q_OBJECT
public:
    virtual QString type() const override;
};

class StaticObject : public GameObject
{
    Q_OBJECT
public:
    virtual QString type() const override;
};

class MovingObject : public GameObject
{
    Q_OBJECT
public:
    virtual QString type() const override;
};

#endif // GAMEOBJECT_H
