#include <QMetaEnum>
#include "gameindicators.h"

const qreal EPS = 1e-9;

void makePositive(double &value) {
    if (value < 0.0) {
        value = 0.0;
    }
}

GameResources *GameIndicators::resources() const
{
    return resources_;
}

void GameIndicators::addDynamic(GameDynamicIndicator *counter)
{
    QString name = counter->name();
    Q_ASSERT(!contains(name));
    counter->setParent(this);
    connect(counter, &GameDynamicIndicator::updated, this, [ = ]() {
        emit updated(Dynamic, name);
    });
    dynamic_indicators_[name] = counter;
    emit added(Dynamic, name);
}

void GameIndicators::add(const QString &name, qreal delta)
{
    Type type = getType(name);
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
    case Static: {
        static_indicators_[name] += delta;
        emit updated(Static, name);
        break;
    }
    case Dynamic: {
        dynamic_indicators_[name]->addToValue(delta);
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    };
}

GameIndicators::Type GameIndicators::getType(const QString &name) const
{
    if (GameResources::nameToType(name) != GameResources::Unknown) {
        return Resource;
    }
    if (static_indicators_.contains(name)) {
        return Static;
    }
    if (dynamic_indicators_.contains(name)) {
        return Dynamic;
    }
    return Unassigned;
}

GameIndicators::GameIndicators(QObject *parent)
    : QObject(parent), resources_(new GameResources(this))
{
    connect(resources_, &GameResources::updated, this, [ = ](GameResources::Type type) {
        emit updated(Resource, GameResources::typeToName(type));
    });
}

qreal GameIndicators::get(const QString &name) const
{
    Type type = getType(name);
    if (type == Unassigned) {
        Q_ASSERT_X(false, "GameCounters::getCounter", "trying to get unassigned Counter");
        return 0.0;
    }
    if (type == Resource) {
        return resources_->get(GameResources::nameToType(name));
    }
    if (type == Static) {
        return static_indicators_[name];
    }
    if (type == Dynamic) {
        return dynamic_indicators_[name]->value();
    }
    Q_UNREACHABLE();
}

bool GameIndicators::contains(const QString &name) const
{
    return getType(name) != Unassigned;
}

QVector<QString> GameIndicators::listNames() const
{
    QVector<QString> res;
    const auto counters = list();
    res.reserve(counters.size());
    for (const Info &info : counters) {
        res.append(info.name);
    }
    return res;
}

QVector<GameIndicators::Info> GameIndicators::list() const
{
    QVector<GameIndicators::Info> counters;
    for (int i = 1; i < GameResources::MaxType; ++i) {
        auto type = static_cast<GameResources::Type>(i);
        counters.append({GameResources::typeToName(type), Resource, resources_->get(type)});
    }
    for (auto iter = static_indicators_.cbegin(); iter != static_indicators_.cend(); ++iter) {
        counters.append({iter.key(), Static, iter.value()});
    }
    for (auto iter = dynamic_indicators_.cbegin(); iter != dynamic_indicators_.cend(); ++iter) {
        counters.append({iter.key(), Dynamic, iter.value()->value()});
    }
    return counters;
}

void GameIndicators::set(const QString &name, qreal value)
{
    Type type = getType(name);
    switch (type) {
    case Unassigned: {
        static_indicators_[name] = value;
        emit added(Static, name);
        break;
    }
    case Resource: {
        resources()->set(GameResources::nameToType(name), value);
        break;
    }
    case Static: {
        static_indicators_[name] = value;
        emit updated(Static, name);
        break;
    }
    case Dynamic: {
        dynamic_indicators_[name]->setValue(value);
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    };
}

void GameDynamicIndicator::addToValue(qreal delta)
{
    setValue(value() + delta);
}

bool GameDynamicIndicator::canSetValue() const
{
    return false;
}

GameDynamicIndicator::GameDynamicIndicator(QString &name)
    : name_(name)
{}

QString GameDynamicIndicator::name() const
{
    return name_;
}

void GameDynamicIndicator::setValue(qreal)
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
{
    resources_[Money] = 10000.0;
    resources_[Builders] = 3.0;
}

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
