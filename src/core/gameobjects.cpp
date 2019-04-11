#include <QtDebug>
#include "gameobjects.h"

GameObjectRepositoryBase::GameObjectRepositoryBase(QObject *parent) : QObject(parent)
{}

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

QVector<GameObjectKey> GameObjectRepositoryBase::keys() const
{
    QList<QString> keys = cells_.keys();
    QVector<GameObjectKey> obj_keys(keys.size());
    for (int i = 0; i < keys.size(); ++i) {
        obj_keys[i] = splitName(keys[i]);
    }
    return obj_keys;
}

GameObjectKey GameObjectRepositoryBase::splitName(const QString &full_name)
{
    auto list = full_name.split("::");
    Q_ASSERT(list.size() == 2);
    return {list[0], list[1]};
}

GameObject *GameObjectProperty::gameObject() const
{
    return qobject_cast<GameObject *>(parent());
}

bool GameObjectProperty::canMove(bool last_value) const
{
    return mergeBooleans(last_value, canMove());
}

bool GameObjectProperty::canSelect(bool last_value) const
{
    return mergeBooleans(last_value, canSelect());
}

bool GameObjectProperty::canSetPosition(bool last_value, const Coordinate &) const
{
    return last_value;
}

GameObjectProperty::GameObjectProperty(QObject *parent)
    : QObject(parent)
{}

QString GameObjectProperty::objectName()
{
    return gameObject()->name();
}

void GameFieldBase::attach(GameObject *object)
{
    object->setField(this);
}

void GameFieldBase::detach(GameObject *object)
{
    object->setField(nullptr);
}

void GameFieldBase::startObjectRemoval(GameObject *object)
{
    object->is_removing_ = true;
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
    bool res = true;
    if (field()) {
        res = field()->canPlace(this, pos);
    }
    if (property_) {
        res = property_->canSetPosition(res, pos);
    }
    return res;
}

GameObject::GameObject(const QString &name, GameObjectProperty *property)
    : QObject(nullptr),
      name_(name),
      active_(false),
      activating_(false),
      is_selected_(false),
      is_moving_(false),
      is_removing_(false),
      position_({-65536, -65536}),
moving_position_(),
field_(nullptr),
property_(property)
{
    if (property_) {
        connect(this, SIGNAL(updated()), property_, SIGNAL(updated));
        property_->setParent(this);
    }
    select();
    startMoving();
}

void GameObject::removeSelf()
{
    if (field()) {
        field()->remove(this);
    }
}

void GameObject::decline()
{
    emit declined();
    removeSelf();
}

bool GameObject::applyMovingPosition()
{
    Q_ASSERT(is_moving_);
    return setPosition(moving_position_);
}

bool GameObject::canApplyMovingPosition() const
{
    Q_ASSERT(is_moving_);
    return canSetPosition(moving_position_);
}

bool GameObject::canMove() const
{
    bool res = internalCanMove();
    if (property_ != nullptr) {
        res = property_->canMove(res);
    }
    return res;
}

bool GameObject::internalCanMove() const
{
    return true;
}

void GameObject::startMoving()
{
    Q_ASSERT(is_selected_ && !is_moving_);
    if (active() && !canMove()) {
        return;
    }
    is_moving_ = true;
    moving_position_ = position_;
    emit startedMoving();
}

void GameObject::endMoving()
{
    Q_ASSERT(is_selected_ && is_moving_);
    is_moving_ = false;
    emit endedMoving();
}

bool GameObject::isMoving() const
{
    return is_moving_;
}

bool GameObject::isRemoving() const
{
    return is_removing_;
}

bool GameObject::canSelect() const
{
    bool res = internalCanSelect();
    if (property_ != nullptr) {
        res = property_->canSelect(res);
    }
    return res;
}

bool GameObject::internalCanSelect() const
{
    return true;
}

void GameObject::select()
{
    Q_ASSERT(!is_selected_);
    if (active() && !canSelect()) {
        return;
    }
    emit selecting();
    is_selected_ = true;
    emit selected();
}

bool GameObject::isSelected() const
{
    return is_selected_;
}

Coordinate GameObject::movingPosition() const
{
    Q_ASSERT(is_moving_);
    return moving_position_;
}

void GameObject::setMovingPosition(const Coordinate &c)
{
    Q_ASSERT(is_moving_);
    Coordinate old_position = moving_position_;
    moving_position_ = c;
    emit movingPositionChanged(old_position, moving_position_);
}

void GameObject::unselect()
{
    Q_ASSERT(is_selected_);
    if (is_moving_) {
        endMoving();
    }
    is_selected_ = false;
    emit unselected();
    if (!active() && !activating_) {
        decline();
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
        if (is_selected_) {
            unselect();
        }
        return false;
    }
    activating_ = true;
    if (is_selected_) {
        unselect();
    }
    bool wasActive = active_;
    Coordinate oldPosition = position_;
    active_ = true;
    position_ = pos;
    activating_ = false;
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

void GameObject::setField(GameFieldBase *field)
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

bool GroundObject::internalCanMove() const
{
    return false;
}

MovingObject::MovingObject(const QString &name, GameObjectProperty *property)
    : GameObject(name, property)
{
}

QString MovingObject::type() const
{
    return "moving";
}

bool StaticObject::internalCanMove() const
{
    return false;
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
{}
