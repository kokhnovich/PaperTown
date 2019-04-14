#include <QtDebug>
#include "gameobjects.h"

GameObjectRepositoryBase::GameObjectRepositoryBase(QObject *parent) : QObject(parent)
{}

GameObjectRepositoryBase::GameObjectInfo *GameObjectRepositoryBase::getInfo(const QString &type, const QString &name) const
{
    QString full = fullName(type, name);
    Q_ASSERT(info_.contains(full));
    return info_[full].data();
}

void GameObjectRepositoryBase::addObject(const QString &type, const QString &name,
        const GameObjectRepositoryBase::GameObjectInfo &info)
{
    QString full = fullName(type, name);
    Q_ASSERT(!info_.contains(full));
    info_[full] = QSharedPointer<GameObjectInfo>::create(info);
}

QString GameObjectRepositoryBase::fullName(const QString &type, const QString &name)
{
    return type + "::" + name;
}

QVector<GameObjectKey> GameObjectRepositoryBase::keys() const
{
    QList<QString> keys = info_.keys();
    QVector<GameObjectKey> obj_keys(keys.size());
    for (int i = 0; i < keys.size(); ++i) {
        obj_keys[i] = splitName(keys[i]);
    }
    return obj_keys;
}

GameObjectProperty *GameObjectRepositoryBase::createProperty(const QString &, const QString &) const
{
    return nullptr;
}

GameObjectKey GameObjectRepositoryBase::splitName(const QString &full_name)
{
    auto list = full_name.split("::");
    Q_ASSERT(list.size() == 2);
    return {list[0], list[1]};
}

GameObject *GameObjectProperty::gameObject() const
{
    return game_object_;
}

void GameObjectProperty::setGameObject(GameObject *object)
{
    Q_ASSERT(!game_object_);
    game_object_ = object;
    emit gameObjectSet();
}

bool GameObjectProperty::canMove(bool last_value) const
{
    return mergeBooleans(last_value, canMove());
}

Util::Bool3 GameObjectProperty::canMove() const
{
    return Util::Dont_Care;
}

bool GameObjectProperty::canSelect(bool last_value) const
{
    return mergeBooleans(last_value, canSelect());
}

Util::Bool3 GameObjectProperty::canSelect() const
{
    return Util::Dont_Care;
}

bool GameObjectProperty::canSetPosition(bool last_value, const Coordinate &) const
{
    return last_value;
}

GameObjectProperty::GameObjectProperty()
    : QObject(nullptr), game_object_(nullptr)
{}

QString GameObjectProperty::objectName() const
{
    return gameObject()->name();
}

GameObjectProperty *GameObjectProperty::castTo(const QMetaObject *meta)
{
    if (meta->inherits(metaObject()) || metaObject()->inherits(meta)) {
        return this;
    }
    return nullptr;
}

GameObjectRepositoryBase *GameObjectProperty::repository() const
{
    return game_object_->repository();
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

GameFieldBase::GameFieldBase(QObject *parent) : QObject(parent)
{}

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
    return repository_->getInfo(type(), name())->cells;
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

GameObject::GameObject(const QString &name, GameObjectProperty *property, GameObjectRepositoryBase *repository)
    : QObject(nullptr),
      name_(name),
      active_(false),
      activating_(false),
      is_selected_(false),
      is_moving_(false),
      is_removing_(false),
      position_(-65536, -65536),
      moving_position_(),
      field_(nullptr),
      property_(property),
      repository_(repository)
{
    if (property_) {
        connect(this, &GameObject::updated, property_, &GameObjectProperty::updated);
        property_->setParent(this);
        property_->setGameObject(this);
    }
    select();
    startMoving();
}

SelectionState GameObject::getSelectionState() const
{
    if (isSelected()) {
        return isMoving() ? Selection::Moving : Selection::Selected;
    }
    return Selection::None;
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
    Q_ASSERT_X(field->repository() == repository_, "GameObject::setField", "field and object repository must be the same");
    field_ = field;
}

GameObjectRepositoryBase *GameObject::repository() const
{
    return repository_;
}
