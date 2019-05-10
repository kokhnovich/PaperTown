#include <QtDebug>
#include "stdindicators.h"

bool GameProductionIndicator::canSetValue() const
{
    return true;
}

GameProductionIndicator::GameProductionIndicator(GameField *field)
    : GameDynamicIndicator("production"), value_(0.0), field_(field)
{
    GameSignalEvent *event = new GameSignalEvent(GameEvent::Replay);
    event->attach(this);
    connect(event, &GameSignalEvent::activated, this, [ = ]() {
        field->resources()->add(GameResources::Money, event->interval() / 60000.0 * value_);
    });
    field->scheduler()->addEvent(event, 100, 100);
}

void GameProductionIndicator::setValue(qreal value)
{
    value_ = value;
    emit updated();
}

qreal GameProductionIndicator::value() const
{
    return value_;
}

void GameEcologyIndicator::addPlantPoints(qreal delta)
{
    plant_points_ += delta;
    emit updated();
}

void GameEcologyIndicator::addTreePoints(qreal delta)
{
    tree_points_ += delta;
    emit updated();
}

qreal GameEcologyIndicator::plantPoints() const
{
    return plant_points_;
}

qreal GameEcologyIndicator::treePoints() const
{
    return tree_points_;
}

qreal GameEcologyIndicator::value() const
{
    qreal res = (tree_points_ - plant_points_) / qMax(50.0, qAbs(plant_points_) + qAbs(tree_points_));
    res = qBound(-1.0, res, 1.0);
    return (0.5 + res / 2) * 100.0;
}

GameEcologyIndicator::GameEcologyIndicator()
    : GameDynamicIndicator("ecology"), plant_points_(0), tree_points_(0)
{}

void addStdIndicators(GameFieldBase *a_field)
{
    if (!a_field->property("_INTERNAL_stdIndicators_isSet_").isNull()) {
        return;
    }
    auto field = qobject_cast<GameField *>(a_field);
    Q_CHECK_PTR(field);
    field->setProperty("_INTERNAL_stdIndicators_isSet_", QVariant::fromValue(true));    
    field->indicators()->addDynamic(new GameProductionIndicator(field));
    field->indicators()->addDynamic(new GameEcologyIndicator);
}
