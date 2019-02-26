#include "gameobjects.h"

Coordinate::Coordinate(int x, int y)
    : x(x), y(y)
{
}

Coordinate & operator+=(Coordinate& a, const Coordinate& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

Coordinate & operator-=(Coordinate& a, const Coordinate& b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

Coordinate operator+(Coordinate a, const Coordinate& b)
{
    return a += b;
}

Coordinate operator-(Coordinate a, const Coordinate& b)
{
    return a -= b;
}

QVector<Coordinate> GameObject::cells()
{
    auto res = cellsRelative();
    for (Coordinate &coord : res) {
        coord += position();
    }
    return res;
}

QVector<Coordinate> GameObject::cellsRelative()
{
    return cells_;
}

bool GameObject::canSetPosition(const Coordinate& pos)
{
    if (field() == nullptr) {
        return true;
    } else {
        return field()->canMoveObject(this, pos);
    }
}

GameObject::GameObject(const QString& name)
    : cells_(),
      name_(name),
      active_(false),
      position_()
{
    connect(this, &move, [=]() { emit this->update(); })
}

QString GameObject::name() const
{
    return name_;
}

Coordinate GameObject::position() const
{
    return position_;
}

bool GameObject::setPosition(const Coordinate& pos)
{
    if (!canSetPosition(pos)) {
        return false;
    }
    
    
    return true;
}

bool GameObject::active() const
{
    return active_;
}

int GameObject::x() const
{
    return position().x;
}

int GameObject::y() const
{
    return position().y;
}

GameFieldBase * GameObject::field() const
{
    return qobject_cast<GameFieldBase *>(this->parent());
}

