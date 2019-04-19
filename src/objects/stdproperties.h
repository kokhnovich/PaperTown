#ifndef STDPROPERTIES_H
#define STDPROPERTIES_H

#include <QObject>
#include "gameobjectproperties.h"
#include "../core/gameobjects.h"
#include "../core/gamefield.h"
#include "../core/eventscheduler.h"

class GameProperty_house : public GameObjectProperty
{
    Q_OBJECT
public:
    int population();
    
    Q_INVOKABLE GameProperty_house();
protected:
    void doInitialize() override;
private:
    int population_;
};

class GameProperty_human;

class HumanEvent : public GameEvent
{
    Q_OBJECT
public:
    GameEvent::EventState activate() override;
protected:
    friend class GameProperty_human;
    
    HumanEvent(GameProperty_human *property);
    ~HumanEvent() override;
private:
    GameProperty_human *property_;
};

class GameProperty_human : public GameObjectProperty
{
    Q_OBJECT
public:
    GameField *field() const;
    
    /*
     * When idle:
     *   - isMoving() == false
     *   - isActive() == false
     * 
     * When stopping:
     *   - isMoving() == true
     *   - isActive() == false
     * 
     * When moving:
     *   - isMoving() == true
     *   - isActive() == true
     */
    bool isMoving() const;
    bool isActive() const;
    
    void go();
    void stop();
    Util::Direction direction() const;
    void setDirection(Util::Direction direction);
    
    /*
     * Stages are numbered from 0 to maxStage()
     * Zero means idle position
     */
    int stage() const;
    int maxStage() const;
    
    void step();
    
    Q_INVOKABLE GameProperty_human();
protected:
    void doInitialize() override;
    void setEvent(HumanEvent *event);
    void unsetEvent(HumanEvent *event);
    
    Util::Bool3 canMove() const override;
protected slots:
    void ensureEvent();
private:
    friend class HumanEvent;
    
    Util::Direction direction_;
    int stage_;
    bool is_active_;
    HumanEvent *event_;
};

#endif // STDPROPERTIES_H