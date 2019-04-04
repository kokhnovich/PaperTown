#include <QtDebug>
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

GameObjectRepositoryBase::GameObjectRepositoryBase(QObject *parent) : QObject(parent)
{
}

void GameObjectRepositoryBase::addObject(const QString &type, const QString &name, const QVector<Coordinate> cells)
{
    QString full = fullName(type, name);
    Q_ASSERT(!cells_.contains(full));
    cells_[full] = cells;
}

QVector<Coordinate> GameObjectRepositoryBase::getCells(const QString &type, const QString &name) const
{
    QString full = fullName(type, name);
    Q_ASSERT(cells_.contains(full));
    return cells_[full];
}

QString GameObjectRepositoryBase::fullName(const QString &type, const QString &name)
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

bool GameObject::canSetPosition(const Coordinate &pos) const
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
      is_selected_(false),
      position_(),
      select_position_(),
      field_(nullptr),
      property_(property)
{
    if (property_) {
        connect(this, SIGNAL(updated()), property_, SIGNAL(updated));
        property_->setParent(this);
    }
}

bool GameObject::applySelectPosition()
{
    Q_ASSERT(is_selected_);
    return setPosition(select_position_);
}

bool GameObject::canApplySelectPosition() const
{
    Q_ASSERT(is_selected_);
    return canSetPosition(select_position_);
}

bool GameObject::canSelect() const
{
    return true;
}

void GameObject::select()
{
    Q_ASSERT(!is_selected_);
    emit selecting();
    is_selected_ = true;
    select_position_ = position_;
    emit selected();
}

bool GameObject::isSelected() const
{
    return is_selected_;
}

Coordinate GameObject::selectPosition() const
{
    Q_ASSERT(is_selected_);
    return select_position_;
}

void GameObject::setSelectPosition(const Coordinate& c)
{
    Q_ASSERT(is_selected_);
    Coordinate old_position = select_position_;
    select_position_ = c;
    emit selectMoved(old_position, select_position_);
}

void GameObject::unselect()
{
    Q_ASSERT(is_selected_);
    is_selected_ = false;
    emit unselected();
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
    if (is_selected_) {
        unselect();
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
    Q_ASSERT(field_ == nullptr || field == nullptr);
    field_ = field;
}

GroundObject::GroundObject(const QString &name, GameObjectProperty *property)
    : GameObject(name, property)
{
}

QString GroundObject::type() const
{
    return "ground";
}

MovingObject::MovingObject(const QString &name, GameObjectProperty *property)
    : GameObject(name, property)
{
}

QString MovingObject::type() const
{
    return "moving";
}

StaticObject::StaticObject(const QString &name, GameObjectProperty *property)
    : GameObject(name, property)
{
}

QString StaticObject::type() const
{
    return "static";
}

GameFieldBase::GameFieldBase(QObject *parent) : QObject(parent)
{
}
