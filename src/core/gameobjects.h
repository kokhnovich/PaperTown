#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QVector>
#include <QHash>

struct Coordinate {
    int x, y;

    Coordinate(int x = 0, int y = 0);
};

struct Rect {
    int top, bottom, left, right;
    
    Rect(int top, int bottom, int left, int right);
    Rect(Coordinate a, Coordinate b);
};

Rect boundingRect(const QVector<Coordinate> &points);

Coordinate operator+(Coordinate a, const Coordinate &b);
Coordinate operator-(Coordinate a, const Coordinate &b);
Coordinate &operator+=(Coordinate &a, const Coordinate &b);
Coordinate &operator-=(Coordinate &a, const Coordinate &b);

bool inBounds(int height, int width, const Coordinate &coord);

class GameObject;

class GameObjectRepositoryBase : public QObject
{
    Q_OBJECT
public:
    explicit GameObjectRepositoryBase(QObject *parent = nullptr);
    void addObject(const QString &type, const QString &name, const QVector<Coordinate> cells);
    QVector<Coordinate> getCells(const QString &type, const QString &name) const;
protected:
    static QString fullName(const QString &type, const QString &name);
private:
    QHash<QString, QVector<Coordinate>> cells_;
};

class GameFieldBase : public QObject
{
    Q_OBJECT
public:
    explicit GameFieldBase(QObject *parent);
    virtual GameObjectRepositoryBase *repository() const = 0;
    virtual bool canPlace(const GameObject *object, const Coordinate &pos) const = 0;
protected:
    void attach(GameObject *object);
    void detach(GameObject *object);
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

    GameObject(const QString &name, GameObjectProperty *property = nullptr);

    QString name() const;
    virtual QString type() const = 0;
    Coordinate position() const;
    virtual const QVector<Coordinate> cellsRelative() const;
    const QVector<Coordinate> cells() const;
    GameObjectProperty *property() const;

    int x() const;
    int y() const;

    bool active() const;
    bool isSelected() const;
    
    virtual bool canSelect() const;
    void select();
    void unselect();

    Coordinate selectPosition() const;
    void setSelectPosition(const Coordinate &c);
    bool canApplySelectPosition() const;
    bool applySelectPosition();
    
    GameFieldBase *field() const;

    bool canSetPosition(const Coordinate &pos) const;
    bool setPosition(const Coordinate &pos);
    
    friend class GameFieldBase;
signals:
    void placed(const Coordinate &position);
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);
    void selectMoved(const Coordinate &oldPosition, const Coordinate &newPosition);
    void updated();
    void selecting();
    void selected();
    void unselected();
private:
    void setField(GameFieldBase *field);
    
    QString name_;
    bool active_;
    bool is_selected_;
    Coordinate position_;
    Coordinate select_position_;
    GameFieldBase *field_;
    GameObjectProperty *property_;
};

class GroundObject : public GameObject
{
    Q_OBJECT
public:
    GroundObject(const QString &name, GameObjectProperty *property = nullptr);
    virtual QString type() const override;
};

class StaticObject : public GameObject
{
    Q_OBJECT
public:
    StaticObject(const QString &name, GameObjectProperty *property = nullptr);
    virtual QString type() const override;
};

class MovingObject : public GameObject
{
    Q_OBJECT
public:
    MovingObject(const QString &name, GameObjectProperty *property = nullptr);
    virtual QString type() const override;
};

#endif // GAMEOBJECT_H
