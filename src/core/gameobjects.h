#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QVector>

struct Coordinate {
    int x, y;
    
    Coordinate(int x = 0, int y = 0);
};

Coordinate operator+(Coordinate a, const Coordinate &b);
Coordinate operator-(Coordinate a, const Coordinate &b);
Coordinate &operator+=(Coordinate &a, const Coordinate &b);
Coordinate &operator-=(Coordinate &a, const Coordinate &b);

class GameObject;

class GameFieldBase : public QObject {
    Q_OBJECT
public:
    virtual bool canMoveObject(GameObject *object, const Coordinate &pos) const = 0;
};

class GameObject : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(Coordinate position READ position WRITE setPosition)
    
    QString name() const;
    virtual QString type() const = 0;
    Coordinate position() const;
    virtual QVector<Coordinate> cellsRelative();
    QVector<Coordinate> cells();
    
    int x() const;
    int y() const;
    
    bool active() const;
    
    GameFieldBase *field() const;
    
    bool canSetPosition(const Coordinate &pos);
    bool setPosition(const Coordinate &pos);
signals:
    void move(const Coordinate &oldPosition, const Coordinate &newPosition);
    void update();
protected:
    QVector<Coordinate> cells_;
    
    GameObject(QObject *parent, const QString &name, GameFieldBase *field = nullptr);
private:
    QString name_;
    bool active_;
    Coordinate position_;
    GameFieldBase *field_;
};

#endif // GAMEOBJECT_H
