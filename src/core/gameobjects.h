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

class GameObject;

class GameObjectRepository : public QObject
{
    Q_OBJECT
public:
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
    virtual GameObjectRepository *repository() const = 0;
    virtual bool canMoveObject(GameObject *object, const Coordinate &pos) const = 0;
};

class GameObjectProperty : public QObject
{
    Q_OBJECT
private:
    GameObject *gameObject() const;
signals:
    void update();
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
    virtual QVector<Coordinate> cellsRelative() const;
    QVector<Coordinate> cells() const;
    GameObjectProperty *property() const;

    int x() const;
    int y() const;

    bool active() const;

    GameFieldBase *field() const;

    bool canSetPosition(const Coordinate &pos);
    bool setPosition(const Coordinate &pos);
signals:
    void move(const Coordinate &oldPosition, const Coordinate &newPosition);
    void update();
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
