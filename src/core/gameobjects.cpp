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

void GameObjectRepository::addObject(const QString &type, const QString &name, const QVector<Coordinate> cells)
{
    QString full = fullName(type, name);
    Q_ASSERT(!cells_.contains(full));
    cells_[full] = cells;
}

QVector<Coordinate> GameObjectRepository::getCells(const QString &type, const QString &name) const
{
    QString full = fullName(type, name);
    Q_ASSERT(cells_.contains(full));
    return cells_[full];
}

QString GameObjectRepository::fullName(const QString& type, const QString& name) {
    return type + "::" + name;
}

QVector<Coordinate> GameObject::cells() const
{
    auto res = cellsRelative();
    for (Coordinate &coord : res) {
        coord += position();
    }
    return res;
}

QVector<Coordinate> GameObject::cellsRelative() const
{
    if (!field() || !field()->repository()) {
        return {};
    }
    return field()->repository()->getCells(type(), name());
}

bool GameObject::canSetPosition(const Coordinate& pos)
{
    if (!field()) {
        return true;
    } else {
        return field()->canMoveObject(this, pos);
    }
}

GameObject::GameObject(QObject *parent, const QString &name, GameFieldBase *field)
    : QObject(parent),
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

QString GroundObject::type() const
{
    return "ground";
}

QString MovingObject::type() const
{
    return "moving";
}

QString StaticObject::type() const
{
    return "static";
}
