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

GameObject::GameObject(QObject *parent, const QString &name, GameFieldBase *field)
    : QObject(parent),
      cells_(),
      name_(name),
      active_(false),
      position_(),
      field_(field)
{
    connect(this, &GameObject::move, [=]() { emit this->update(); });
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
    Coordinate oldPosition = position_;
    active_ = true;
    position_ = pos;
    emit move(oldPosition, position_);
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

