#ifndef STDINDICATORS_H
#define STDINDICATORS_H

#include "../core/gamefield.h"
#include "../core/gameindicators.h"
#include "../core/eventscheduler.h"

class GameProductionIndicator : public GameDynamicIndicator
{
    Q_OBJECT
public:
    qreal value() const override;
    bool canSetValue() const override;
    void setValue(qreal value) override;
    GameProductionIndicator(GameField *field);
private:
    qreal value_;
    GameField *field_;
};

class GameEcologyIndicator : public GameDynamicIndicator
{
    Q_OBJECT
public:
    qreal value() const override;
    void addPlantPoints(qreal delta);
    void addTreePoints(qreal delta);
    qreal plantPoints() const;
    qreal treePoints() const;
    GameEcologyIndicator();
private:
    qreal plant_points_;
    qreal tree_points_;
    GameField *field_;
};

void addStdIndicators(GameFieldBase *field);

#endif // STDINDICATORS_H
