#include <QtDebug>
#include "gameobjects.h"

qreal GameObjectInfo::cost() const
{
    return keys["cost"].toReal();
}

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

bool GameObjectProperty::canSetPosition(bool last_value, const Coordinate &position) const
{
    return mergeBooleans(last_value, canSetPosition(position));
}

Util::Bool3 GameObjectProperty::canSetPosition(const Coordinate &) const
{
    return Util::Dont_Care;
}

bool GameObjectProperty::conflictsWith(bool last_value, const GameObject *object) const
{
    return mergeBooleans(last_value, conflictsWith(object));
}

Util::Bool3 GameObjectProperty::conflictsWith(const GameObject *) const
{
    return Util::Dont_Care;
}

bool GameObjectProperty::canAutoEnable(bool last_value) const
{
    return mergeBooleans(last_value, canAutoEnable());
}

Util::Bool3 GameObjectProperty::canAutoEnable() const
{
    return Util::Dont_Care;
}

bool GameObjectProperty::canPlace(bool last_value) const
{
    return mergeBooleans(last_value, canPlace());
}

Util::Bool3 GameObjectProperty::canPlace() const
{
    return Util::Dont_Care;
}

void GameObjectProperty::enableObject()
{
    Q_CHECK_PTR(game_object_);
    game_object_->enable();
}

void GameObjectProperty::disableObject()
{
    Q_CHECK_PTR(game_object_);
    game_object_->disable();
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
    object->attach(this);
}

void GameFieldBase::startObjectRemoval(GameObject *object)
{
    object->is_removing_ = true;
}

void GameFieldBase::finishObjectRemoval(GameObject *object)
{
    object->emitRemoved();
}

GameResources *GameFieldBase::resources() const
{
    return indicators()->resources();
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
    return repository()->getInfo(type(), name())->cells;
}

bool GameObject::canAutoEnable() const
{
    bool res = true;
    if (property_ != nullptr) {
        res = property_->canAutoEnable(res);
    }
    return res;
}

void GameObject::disable()
{
    Q_ASSERT(is_enabled_);
    is_enabled_ = false;
    emit disabled();
}

void GameObject::enable()
{
    Q_ASSERT(!is_enabled_);
    is_enabled_ = true;
    emit enabled();
}

bool GameObject::isEnabled() const
{
    return is_enabled_;
}

qreal GameObject::removalCost() const
{
    bool ok = false;
    qreal value = objectInfo()->keys["remove-cost"].toReal(&ok);
    if (ok) {
        return value;
    }
    return 0.75 * cost();
}

void GameObject::emitRemoved()
{
    if (isPlaced()) {
        resources()->add(GameResources::Money, removalCost());
        emit soundEvent(Util::Sound::Removing);
    }
    emit removed();
}

bool GameObject::canSetPosition(const Coordinate &pos) const
{
    bool res = true;
    if (field()) {
        res = field()->canPutObject(this, pos);
    }
    if (property_) {
        res = property_->canSetPosition(res, pos);
    }
    return res;
}

GameObject::GameObject(const QString &name)
    : QObject(nullptr),
      name_(name),
      is_placed_(false),
      is_placing_(false),
      is_selected_(false),
      is_moving_(false),
      is_removing_(false),
      is_enabled_(false),
      is_attached_(false),
      position_(-65536, -65536),
      moving_position_(),
      field_(nullptr),
      property_(nullptr)
{
    select();
    startMoving();
}

bool GameObject::conflictsWith(const GameObject *object) const
{
    bool res = false;
    if (property_ != nullptr) {
        res = property_->conflictsWith(res, object);
    }
    return res;
}

void GameObject::initialize(GameFieldBase *field, GameObjectProperty *property)
{
    Q_ASSERT(property_ == nullptr);
    Q_ASSERT(field_ == nullptr);
    field_ = field;
    property_ = property;
    if (property_) {
        connect(property_, &GameObjectProperty::updated, this, &GameObject::updated);
        property_->setParent(this);
        property_->initialize(this);
        if (canAutoEnable() && !isEnabled()) {
            enable();
        }
    }
}

GameObjectInfo *GameObject::objectInfo() const
{
    return repository()->getInfo(type(), name_);
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
    if (field() != nullptr) {
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
    if (!canApplyMovingPosition()) {
        if (is_selected_) {
            unselect();
        }
        return false;
    }
    is_placing_ = true;
    if (is_selected_) {
        unselect();
    }
    is_placing_ = false;
    setPosition(pos);
    if (!is_placed_) {
        is_placed_ = true;
        resources()->acquire(GameResources::Money, cost());
        emit placed(position_);
        //emit soundEvent(Util::Sound::Building);
    }
    return true;
}

bool GameObject::place(const Coordinate &pos)
{
    Q_ASSERT(!is_placed_);
    Q_ASSERT(is_selected_ && is_moving_);
    if (!canPlace() || !canSetPosition(pos)) {
        return false;
    }
    setMovingPosition(pos);
    return applyMovingPosition();
}

bool GameObject::canApplyMovingPosition() const
{
    Q_ASSERT(is_moving_);
    if (!is_placed_ && !canPlace()) {
        return false;
    }
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

bool GameObject::canPlace() const
{
    if (!resources()->canAcquire(GameResources::Money, cost())) {
        return false;
    }
    bool res = true;
    if (property() != nullptr) {
        res = property()->canPlace(res);
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
    if (isPlaced() && !canMove()) {
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
    if (isPlaced() && !canSelect()) {
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
    if (!isPlaced() && !is_placing_) {
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
    Q_ASSERT(is_placed_);
    return position_;
}

bool GameObject::setPosition(const Coordinate &pos)
{
    if (!canSetPosition(pos)) {
        return false;
    }
    Coordinate oldPosition = position_;
    position_ = pos;
    if (is_placed_) {
        emit moved(oldPosition, position_);
    }
    return true;
}

bool GameObject::isPlaced() const
{
    return is_placed_;
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

void GameObject::attach(GameFieldBase *field)
{
    Q_CHECK_PTR(field_);
    Q_ASSERT_X(field == field_, "GameObject::attach", "Attached field and parent field mismatch");
    Q_ASSERT(!is_attached_);
    is_attached_ = true;
    emit attached();
}

GameIndicators *GameObject::indicators() const
{
    return field_->indicators();
}

GameResources *GameObject::resources() const
{
    return field_->resources();
}

qreal GameObject::cost() const
{
    return objectInfo()->cost();
}

GameObjectRepositoryBase *GameObject::repository() const
{
    return field_->repository();
}

bool objectsConflict(const GameObject *a, const GameObject *b)
{
    Q_CHECK_PTR(a);
    Q_CHECK_PTR(b);
    if (a == b) {
        return false;
    } else {
        return a->conflictsWith(b) || b->conflictsWith(a);
    }
}

void GameObject::playSound(const Util::Sound &sound)
{
    emit soundEvent(sound);
}
