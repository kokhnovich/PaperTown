#include "../objects/stdproperties.h"
#include <QDateTime>
#include <random>

const int HUMAN_MAX_STAGE = 10;
const int HUMAN_ANIMATION_STEP = 80;

// FIXME : use better random generator!
std::mt19937 rnd(QDateTime::currentMSecsSinceEpoch());

void GameProperty_house::doInitialize()
{
    GameObjectProperty::doInitialize();
    QVariant population_data = objectInfo()->keys["population"];
    if (population_data.type() == QVariant::List) {
        int min_population = population_data.toList()[0].toInt();
        int max_population = population_data.toList()[1].toInt();
        population_ = rnd() % (max_population - min_population + 1) + min_population;
    } else {
        population_ = population_data.toInt();
    }
}

GameProperty_house::GameProperty_house()
    : GameObjectProperty()
{}

int GameProperty_house::population()
{
    return population_;
}

HumanEvent::HumanEvent(GameProperty_human *property)
    : property_(property)
{
    property_->setEvent(this);
    this->attach(property_);
}

GameEvent::State HumanEvent::activate()
{
    if (!property_->isMoving()) {
        return GameEvent::Finish;
    }
    property_->step();
    return GameEvent::Replay;
}

HumanEvent::~HumanEvent()
{
    property_->unsetEvent(this);
}

GameField *GameProperty_human::field() const
{
    return qobject_cast<GameField *>(gameObject()->field());
}

Util::Direction GameProperty_human::direction() const
{
    return direction_;
}

Util::Direction GameProperty_human::queuedDirection() const
{
    return queued_direction_;
}

void GameProperty_human::setDirection(Util::Direction direction)
{
    queued_direction_ = direction;
    if (stage_ == 0) {
        updateDirection();
        emit updated();
    }
}

void GameProperty_human::updateDirection()
{
    direction_ = queued_direction_;
}

void GameProperty_human::ensureEvent()
{
    if (field() == nullptr || !gameObject()->isPlaced() || !isActive()) {
        return;
    }
    if (event_ == nullptr) {
        field()->scheduler()->addEvent(new HumanEvent(this), 0, HUMAN_ANIMATION_STEP);
    }
}

void GameProperty_human::setEvent(HumanEvent *event)
{
    Q_ASSERT(event_ == nullptr);
    event_ = event;
}

void GameProperty_human::unsetEvent(HumanEvent *event)
{
    Q_ASSERT(event_ == event);
    event_ = nullptr;
}

void GameProperty_human::go()
{
    Q_ASSERT(!is_active_);
    is_active_ = true;
    ensureEvent();
}

int GameProperty_human::stage() const
{
    return stage_;
}

int GameProperty_human::maxStage() const
{
    return HUMAN_MAX_STAGE;
}

bool GameProperty_human::isActive() const
{
    return is_active_;
}

bool GameProperty_human::isMoving() const
{
    return is_active_ || stage_ != 0;
}

void GameProperty_human::stop()
{
    Q_ASSERT(is_active_);
    is_active_ = false;
}

void GameProperty_human::doInitialize()
{
    GameObjectProperty::doInitialize();
    setDirection(Util::Direction(rnd() % 4));
    connect(gameObject(), &GameObject::attached, this, &GameProperty_human::ensureEvent);
    connect(gameObject(), &GameObject::placed, this, &GameProperty_human::ensureEvent);
    go();
}

void GameProperty_human::step()
{
    if (stage_ != 0 && stage_ != maxStage()) {
        ++stage_;
        emit updated();
        return;
    }
    if (is_active_) {
        updateDirection();
        // TODO : remove this code
        Util::Direction dirs[4] = {
            direction_,
            Util::Direction((direction_ + 1) % 4),
            Util::Direction((direction_ + 2) % 4),
            Util::Direction((direction_ + 3) % 4),
        };
        if (rnd() % 20 == 0) {
            std::shuffle(dirs, dirs + 4, rnd);
        }
        for (int i = 0; i < 4; ++i) {
            Coordinate new_pos = gameObject()->position().applyDirection(dirs[i]);
            if (gameObject()->canSetPosition(new_pos)) {
                direction_ = dirs[i];
                break;
            }
        }
        queued_direction_ = direction_;
        // TODO : remove this code (end)
        Coordinate new_pos = gameObject()->position().applyDirection(direction_);
        if (gameObject()->canSetPosition(new_pos)) {
            gameObject()->setPosition(new_pos);
        } else {
            stop();
        }
    }
    if (!is_active_) {
        if (stage_ != 0) {
            stage_ = 0;
            updateDirection();
            emit updated();
        }
        return;
    }
    stage_ = 1;
    emit updated();
}

Util::Bool3 GameProperty_human::canMove() const
{
    //return Util::False;
    return Util::Dont_Care;
}

GameProperty_human::GameProperty_human()
    : direction_(Util::Down), queued_direction_(Util::Down), stage_(0), is_active_(false), event_(nullptr)
{}

GameProperty_passable::GameProperty_passable()
{}

Util::Bool3 GameProperty_passable::conflictsWith(const GameObject *object) const
{
    if (object->type() == "moving") {
        return Util::False;
    }
    return Util::Dont_Care;
}

GameField *GameProperty_building::field() const
{
    return qobject_cast<GameField *>(gameObject()->field());
}

Util::Bool3 GameProperty_building::canSetPosition(const Coordinate &) const
{
    if (!gameObject()->isPlaced()) {
        return canGetBuilders() ? Util::Dont_Care : Util::False;
    }
    return Util::Dont_Care;
}

bool GameProperty_building::canGetBuilders() const
{
    return gameObject()->resources()->canAcquire(GameResources::Builders, 1);
}

void GameProperty_building::getBuilders()
{
    bool res = gameObject()->resources()->acquire(GameResources::Builders, 1);
    Q_ASSERT(res);
}

void GameProperty_building::ungetBuilders()
{
    gameObject()->resources()->add(GameResources::Builders, 1);
}

void GameProperty_building::buildFinished()
{
    const auto cells = gameObject()->cells();
    for (const Coordinate &cell : cells) {
        const auto cell_objects = field()->getCell(cell);
        for (GameObject *object : cell_objects) {
            if (object->type() == "moving") {
                Q_ASSERT(object != gameObject());
                field()->remove(object);
            }
        }
    }
    setState(Normal);
}

double GameProperty_building::buildProgress() const
{
    if (state_ == Unprepared) {
        return 0.0;
    }
    if (!isBuildInProgress()) {
        return 1.0;
    }
    return 1.0 * elapsedBuildTime() / totalBuildTime();
}

Util::Bool3 GameProperty_building::canAutoEnable() const
{
    return Util::False;
}

Util::Bool3 GameProperty_building::canMove() const
{
    return Util::False;
}

bool GameProperty_building::canStartRepairing() const
{
    return !isBuildInProgress()
           && canGetBuilders()
           && health() < REPAIR_THRESHOLD
           && gameObject()->resources()->canAcquire(GameResources::Money, repairCost());
}

Util::Bool3 GameProperty_building::conflictsWith(const GameObject *object) const
{
    if (object->type() == "moving" && (state() == Unprepared || state() == UnderConstruction)) {
        return Util::False;
    }
    return Util::Dont_Care;
}

qreal GameProperty_building::repairCost() const
{
    return objectInfo()->keys["repair-cost"].toReal();
}

void GameProperty_building::doInitialize()
{
    Q_ASSERT(gameObject()->type() == "static");
    total_build_time_ = qMax(500, objectInfo()->keys["build-time"].toInt() * 1000);
    if (objectInfo()->keys.contains("repair-time")) {
        total_repair_time_ = qMax(500, objectInfo()->keys["repair-time"].toInt() * 1000);
    } else {
        total_repair_time_ = total_build_time_;
    }
    health_loss_ = objectInfo()->keys["health-loss"].toReal();
    connect(gameObject(), &GameObject::placed, this, &GameProperty_building::tryPrepare);
    connect(gameObject(), &GameObject::attached, this, &GameProperty_building::tryPrepare);
}

qint64 GameProperty_building::elapsedBuildTime() const
{
    return totalBuildTime() - remainingBuildTime();
}

GameProperty_building::GameProperty_building()
    : GameObjectProperty()
{}

void GameProperty_building::handleLoop()
{
    if (state_ != Normal) {
        return;
    }
    bool can_start_repairing = canStartRepairing();
    health_ -= health_loss_;
    if (health_ <= 1e-9) {
        health_ = 0.0;
        setState(Wrecked);
    } else {
        if (canStartRepairing() != can_start_repairing) {
            emit updated();
        }
    }
}

qreal GameProperty_building::health() const
{
    return health_;
}

bool GameProperty_building::isBuildInProgress() const
{
    return state_ == Unprepared || state_ == UnderConstruction || state_ == Repairing;
}

bool GameProperty_building::needsEnabled() const
{
    return state_ == Normal;
}

qint64 GameProperty_building::remainingBuildTime() const
{
    Q_ASSERT(isBuildInProgress());
    Q_CHECK_PTR(cur_event_);
    return cur_event_->timeBeforeActivate();
}

void GameProperty_building::repairFinished()
{
    health_ = 1.0;
    setState(Normal);
}

void GameProperty_building::setState(GameProperty_building::State new_state)
{
    if (state_ == new_state) {
        return;
    }
    Q_ASSERT(new_state != Unprepared);
    Q_ASSERT(field() != nullptr);
    bool was_enabled = needsEnabled();
    if (cur_event_ != nullptr) {
        cur_event_->finish();
        cur_event_ = nullptr;
    }
    if (isBuildInProgress() && state_ != Unprepared) {
        ungetBuilders();
    }
    switch (new_state) {
    case Normal: {
        cur_event_ = new GameSignalEvent(GameEvent::Replay);
        cur_event_->attach(this);
        connect(cur_event_, &GameSignalEvent::activated, this, &GameProperty_building::handleLoop);
        field()->scheduler()->addEvent(cur_event_, 1000, 1000);
        break;
    }
    case UnderConstruction: {
        cur_event_ = new GameSignalEvent(GameEvent::Finish);
        cur_event_->attach(this);
        connect(cur_event_, &GameSignalEvent::activated, this, &GameProperty_building::buildFinished);
        field()->scheduler()->addEvent(cur_event_, total_build_time_);
        break;
    }
    case Repairing: {
        cur_event_ = new GameSignalEvent(GameEvent::Finish);
        cur_event_->attach(this);
        connect(cur_event_, &GameSignalEvent::activated, this, &GameProperty_building::repairFinished);
        field()->scheduler()->addEvent(cur_event_, total_repair_time_);
        break;
    }
    default: {
        // just ignore
    }
    };
    state_ = new_state;
    if (isBuildInProgress()) {
        getBuilders();
    }
    bool new_enabled = needsEnabled();
    if (was_enabled != new_enabled) {
        if (new_enabled) {
            this->enableObject();
        } else {
            this->disableObject();
        }
    }
    emit updated();
}

bool GameProperty_building::startRepairing()
{
    if (!canStartRepairing()) {
        return false;
    }
    gameObject()->resources()->acquire(GameResources::Money, repairCost());
    setState(Repairing);
    return true;
}

GameProperty_building::State GameProperty_building::state() const
{
    return state_;
}

qint64 GameProperty_building::totalBuildTime() const
{
    if (state_ == UnderConstruction) {
        return total_build_time_;
    }
    if (state_ == Repairing) {
        return total_repair_time_;
    }
    Q_UNREACHABLE();
}

void GameProperty_building::tryPrepare()
{
    if (state_ != Unprepared || field() == nullptr || !gameObject()->isPlaced()) {
        return;
    }
    setState(UnderConstruction);
}

GameProperty_building::~GameProperty_building()
{
    if (isBuildInProgress() && state_ != Unprepared) {
        ungetBuilders();
    }
}

GAME_PROPERTY_REGISTER("house", GameProperty_house)
GAME_PROPERTY_REGISTER("human", GameProperty_human)
GAME_PROPERTY_REGISTER("passable", GameProperty_passable)
GAME_PROPERTY_REGISTER("building", GameProperty_building)
