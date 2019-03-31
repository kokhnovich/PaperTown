#include "gameobjects.h"

Coordinate::Coordinate(int x, int y)
    : x(x), y(y)
{
}

Coordinate &operator+=(Coordinate &a, const Coordinate &b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

Coordinate &operator-=(Coordinate &a, const Coordinate &b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

Coordinate operator+(Coordinate a, const Coordinate &b)
{
    return a += b;
}

Coordinate operator-(Coordinate a, const Coordinate &b)
{
    return a -= b;
}

bool inBounds(int height, int width, const Coordinate& coord)
{
    return 0 <= coord.x && coord.x < height && 0 <= coord.y && coord.y < width;
}

GameObjectRepository::GameObjectRepository(QObject *parent) : QObject(parent)
{
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

QString GameObjectRepository::fullName(const QString &type, const QString &name)
{
    return type + "::" + name;
}

GameObject *GameObjectProperty::gameObject() const
{
    return qobject_cast<GameObject *>(parent());
}

void GameFieldBase::attach(GameObject* object)
{
    object->setField(this);
}

void GameFieldBase::detach(GameObject* object)
{
    object->setField(nullptr);
}

const QVector<Coordinate> GameObject::cells() const
{
    auto res = cellsRelative();
    for (Coordinate &coord : res) {
        coord += position();
    }
    return res;
}

const QVector<Coordinate> GameObject::cellsRelative() const
{
    if (!field() || !field()->repository()) {
        return {};
    }
    return field()->repository()->getCells(type(), name());
}

bool GameObject::canSetPosition(const Coordinate &pos)
{
    if (!field()) {
        return true;
    } else {
        return field()->canPlace(this, pos);
    }
}

GameObject::GameObject(const QString &name, GameObjectProperty *property)
    : QObject(nullptr),
      name_(name),
      active_(false),
      position_(),
      field_(nullptr),
      property_(property)
{
    connect(this, &GameObject::moved, [ = ]() {
        emit this->updated();
    });
    connect(this, &GameObject::placed, [ = ]() {
        emit this->updated();
    });
    if (property_) {
        connect(this, SIGNAL(updated()), property_, SIGNAL(updated));
        property_->setParent(this);
    }
}

GameObjectProperty *GameObject::property() const
{
    return property_;
}

QString GameObject::name() const
{
    return name_;
}

Coordinate GameObject::position() const
{
    Q_ASSERT(active_);
    return position_;
}

bool GameObject::setPosition(const Coordinate &pos)
{
    if (!canSetPosition(pos)) {
        return false;
    }
    bool wasActive = active_;
    Coordinate oldPosition = position_;
    active_ = true;
    position_ = pos;
    if (wasActive) {
        emit moved(oldPosition, position_);
    } else {
        emit placed(position_);
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

GameFieldBase *GameObject::field() const
{
    return field_;
}

void GameObject::setField(GameFieldBase* field)
{
    assert(field_ == nullptr || field == nullptr);
    field_ = field;
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

GameFieldBase::GameFieldBase(QObject *parent) : QObject(parent)
{
}
