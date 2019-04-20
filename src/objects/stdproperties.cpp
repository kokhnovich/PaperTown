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
}

GameEvent::EventState HumanEvent::activate()
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

void GameProperty_human::setDirection(Util::Direction direction)
{
    direction_ = direction;
}

void GameProperty_human::ensureEvent()
{
    if (field() == nullptr || !gameObject()->active() || !isActive()) {
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
    direction_ = Util::Direction(rnd() % 4);
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
        Coordinate new_pos = gameObject()->position().applyDirection(direction_);
        if (gameObject()->canSetPosition(new_pos)) {
            gameObject()->setPosition(new_pos);
        } else {
            // TODO : remove this code
            direction_ = Util::Direction((direction_ + 2) % 4);
            Coordinate new_pos = gameObject()->position().applyDirection(direction_);
            if (gameObject()->canSetPosition(new_pos)) {
            gameObject()->setPosition(new_pos);
            } else {
                stop();
            }
            // TODO : remove this code
            //stop();
        }
    }
    if (!is_active_) {
        if (stage_ != 0) {
            stage_ = 0;
            emit updated();
        }
        return;
    }
    stage_ = 1;
    emit updated();
}

Util::Bool3 GameProperty_human::canMove() const
{
    return Util::False;
}

GameProperty_human::GameProperty_human()
    : direction_(Util::Down), stage_(0), is_active_(false), event_(nullptr)
{}

GAME_PROPERTY_REGISTER("house", GameProperty_house)
GAME_PROPERTY_REGISTER("human", GameProperty_human)
