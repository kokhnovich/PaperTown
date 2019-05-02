#include <QMetaEnum>
#include "gameresources.h"

const qreal EPS = 1e-9;

void makePositive(double &value) {
    if (value < 0.0) {
        value = 0.0;
    }
}

GameResources *GameCounters::resources() const
{
    return resources_;
}

void GameCounters::addDynamicCounter(GameDynamicCounter *counter)
{
    QString name = counter->name();
    Q_ASSERT(!hasCounter(name));
    counter->setParent(this);
    connect(counter, &GameDynamicCounter::updated, this, [ = ]() {
        emit updated(DynamicCounter, name);
    });
    dynamic_counters_[name] = counter;
    emit added(DynamicCounter, name);
}

void GameCounters::addToCounter(const QString &name, qreal delta)
{
    Type type = counterType(name);
    switch (type) {
    case Unassigned: {
        /*static_counters_[name] = delta;
        emit added(StaticCounter, name);*/
        Q_ASSERT_X(false, "GameCounters::addToCounter", "trying to add to an unassigned Counter");
        break;
    }
    case Resource: {
        resources()->add(GameResources::nameToType(name), delta);
        break;
    }
    case StaticCounter: {
        static_counters_[name] += delta;
        emit updated(StaticCounter, name);
        break;
    }
    case DynamicCounter: {
        dynamic_counters_[name]->addToValue(delta);
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    };
}

GameCounters::Type GameCounters::counterType(const QString &name) const
{
    if (GameResources::nameToType(name) != GameResources::Unknown) {
        return Resource;
    }
    if (static_counters_.contains(name)) {
        return StaticCounter;
    }
    if (dynamic_counters_.contains(name)) {
        return DynamicCounter;
    }
    return Unassigned;
}

GameCounters::GameCounters(QObject *parent)
    : QObject(parent), resources_(new GameResources(this))
{
    connect(resources_, &GameResources::updated, this, [ = ](GameResources::Type type) {
        emit updated(Resource, GameResources::typeToName(type));
    });
}

qreal GameCounters::getCounter(const QString &name) const
{
    Type type = counterType(name);
    if (type == Unassigned) {
        Q_ASSERT_X(false, "GameCounters::getCounter", "trying to get unassigned Counter");
        return 0.0;
    }
    if (type == Resource) {
        return resources_->get(GameResources::nameToType(name));
    }
    if (type == StaticCounter) {
        return static_counters_[name];
    }
    if (type == DynamicCounter) {
        return dynamic_counters_[name]->value();
    }
    Q_UNREACHABLE();
}

bool GameCounters::hasCounter(const QString &name) const
{
    return counterType(name) != Unassigned;
}

QVector<QString> GameCounters::listCounterNames() const
{
    QVector<QString> res;
    const auto counters = listCounters();
    res.reserve(counters.size());
    for (const CounterInfo &info : counters) {
        res.append(info.name);
    }
    return res;
}

QVector<GameCounters::CounterInfo> GameCounters::listCounters() const
{
    QVector<GameCounters::CounterInfo> counters;
    for (int i = 1; i < GameResources::MaxType; ++i) {
        auto type = static_cast<GameResources::Type>(i);
        counters.append({GameResources::typeToName(type), Resource, resources_->get(type)});
    }
    for (auto iter = static_counters_.cbegin(); iter != static_counters_.cend(); ++iter) {
        counters.append({iter.key(), StaticCounter, iter.value()});
    }
    for (auto iter = dynamic_counters_.cbegin(); iter != dynamic_counters_.cend(); ++iter) {
        counters.append({iter.key(), DynamicCounter, iter.value()->value()});
    }
    return counters;
}

void GameCounters::setCounter(const QString &name, qreal value)
{
    Type type = counterType(name);
    switch (type) {
    case Unassigned: {
        static_counters_[name] = value;
        emit added(StaticCounter, name);
        break;
    }
    case Resource: {
        resources()->set(GameResources::nameToType(name), value);
        break;
    }
    case StaticCounter: {
        static_counters_[name] = value;
        emit updated(StaticCounter, name);
        break;
    }
    case DynamicCounter: {
        dynamic_counters_[name]->setValue(value);
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    };
}

void GameDynamicCounter::addToValue(qreal delta)
{
    setValue(value() + delta);
}

bool GameDynamicCounter::canSetValue() const
{
    return false;
}

GameDynamicCounter::GameDynamicCounter(QString &name)
    : name_(name)
{}

QString GameDynamicCounter::name() const
{
    return name_;
}

void GameDynamicCounter::setValue(qreal)
{
    Q_UNREACHABLE();
}

bool GameResources::acquire(GameResources::Type type, qreal amount)
{
    if (!canAcquire(type, amount)) {
        return false;
    }
    if (!infinite_mode_) {
        resources_[type] -= amount;
        makePositive(resources_[type]);
        emit updated(type);
    }
    return true;
}

void GameResources::add(GameResources::Type type, qreal delta)
{
    resources_[type] += delta;
    Q_ASSERT(resources_[type] >= -EPS);
    makePositive(resources_[type]);
    emit updated(type);
}

bool GameResources::canAcquire(GameResources::Type type, qreal amount) const
{
    if (infinite_mode_) {
        return true;
    }
    return resources_[type] - amount >= -EPS;
}

void GameResources::disableInfiniteMode()
{
    Q_ASSERT(infinite_mode_);
    infinite_mode_ = false;
}

void GameResources::enableInfiniteMode()
{
    Q_ASSERT(!infinite_mode_);
    infinite_mode_ = true;
}

GameResources::GameResources(QObject *parent)
    : QObject(parent)
{}

qreal GameResources::get(GameResources::Type type) const
{
    return resources_[type];
}

GameResources::Type GameResources::nameToType(const QString &name)
{
    if (name == QStringLiteral("money")) {
        return Money;
    }
    if (name == QStringLiteral("builders")) {
        return Builders;
    }
    return Unknown;
}

bool GameResources::isInfiniteMode() const
{
    return infinite_mode_;
}

void GameResources::set(GameResources::Type type, qreal value)
{
    Q_ASSERT(value >= -EPS);
    makePositive(value);
    resources_[type] = value;
}

QString GameResources::typeToName(GameResources::Type res_type)
{
    static QMetaEnum meta = QMetaEnum::fromType<Type>();
    return QString(meta.valueToKey(res_type)).toLower();
}
