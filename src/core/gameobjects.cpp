#include <QtDebug>
#include "gameobjects.h"

GameObjectRepositoryBase::GameObjectRepositoryBase(QObject *parent) : QObject(parent)
{}

GameObjectInfo *GameObjectRepositoryBase::getInfo(const QString &type, const QString &name) const
{
    QString full = fullName(type, name);
    Q_ASSERT(info_.contains(full));
    return info_[full].data();
}

void GameObjectRepositoryBase::addObject(const QString &type, const QString &name,
        const GameObjectInfo &info)
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

void GameObjectProperty::initialize(GameObject *object)
{
    Q_ASSERT(object);
    Q_ASSERT(!game_object_ && !initialized_);
    game_object_ = object;
    doInitialize();
    emit initializing();
    initialized_ = true;
}

void GameObjectProperty::doInitialize()
{}

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

Util::Bool3 GameObjectProperty::conflitsWith(const GameObject *) const
{
    return Util::Dont_Care;
}

bool GameObjectProperty::conflitsWith(bool last_value, const GameObject* object) const
{
    return mergeBooleans(last_value, conflitsWith(object));
}

GameObjectProperty::GameObjectProperty()
    : QObject(nullptr), game_object_(nullptr), initialized_(false)
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

bool GameObjectProperty::isInitialized() const
{
    return initialized_;
}

GameObjectInfo *GameObjectProperty::objectInfo() const
{
    return gameObject()->objectInfo();
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

GameObject::GameObject(const QString &name, GameObjectRepositoryBase *repository)
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
      property_(nullptr),
      repository_(repository)
{
    select();
    startMoving();
}

bool GameObject::conflitsWith(const GameObject* object) const
{
    bool res = false;
    if (property_ != nullptr) {
        res = property_->conflitsWith(res, object);
    }
    return res;
}

void GameObject::initProperty(GameObjectProperty* property)
{
    Q_ASSERT(property_ == nullptr);
    property_ = property;
    if (property_) {
        connect(property_, &GameObjectProperty::updated, this, &GameObject::updated);
        property_->setParent(this);
        property_->initialize(this);
    }
}


GameObjectInfo *GameObject::objectInfo() const
{
    return repository_->getInfo(type(), name_);
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
    Coordinate pos = moving_position_;
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
    activating_ = false;
    setPosition(pos);
    if (!active_) {
        active_ = true;
        emit placed(position_);
    }
    return true;
}

bool GameObject::activate(const Coordinate &pos)
{
    Q_ASSERT(!active_);
    Q_ASSERT(is_selected_ && is_moving_);
    setMovingPosition(pos);
    return applyMovingPosition();
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
        return false;
    }
    Coordinate oldPosition = position_;
    position_ = pos;
    if (active_) {
        emit moved(oldPosition, position_);
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
    emit attached();
}

GameObjectRepositoryBase *GameObject::repository() const
{
    return repository_;
}

bool objectsConflict(const GameObject *a, const GameObject *b)
{
    Q_CHECK_PTR(a);
    Q_CHECK_PTR(b);
    if (a == b) {
        return false;
    } else {
        return a->conflitsWith(b) || b->conflitsWith(a);
    }
}

