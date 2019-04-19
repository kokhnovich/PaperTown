#include <QDebug>
#include "eventscheduler.h"

GameEvent::EventState GameEvent::activate()
{
    return GameEvent::Finish;
}

qint64 GameEvent::interval() const
{
    return interval_;
}

void GameEvent::setInterval(qint64 interval)
{
    interval_ = interval;
}

bool GameEventScheduler::active() const
{
    return active_;
}

void GameEventScheduler::addEvent(GameEvent *event, qint64 delay, qint64 interval)
{
    event->setParent(this);
    event->setInterval(interval);
    event->time_point_ = realTimePoint() + delay;
    events_.push(event);
}

GameEventScheduler::GameEventScheduler(QObject *parent, bool active)
    : QObject(parent), timer_(), events_(), active_(true), delta_(0)
{
    timer_.start();
    if (active) {
        start();
    }
}

void GameEventScheduler::pause()
{
    if (!active_) {
        return;
    }
    active_ = false;
    pause_start_ = timer_.elapsed();
}

qint64 GameEventScheduler::realTimePoint() const
{
    if (!active_) {
        return pause_start_ - delta_;
    }
    return timer_.elapsed() - delta_;
}

void GameEventScheduler::start()
{
    if (active_) {
        return;
    }
    delta_ += timer_.elapsed() - pause_start_;
    active_ = true;
}

void GameEventScheduler::update()
{
    qint64 cur_time = realTimePoint();
    while (!events_.empty() && events_.top()->time_point_ <= cur_time) {
        GameEvent *passed_event = events_.top();
        events_.pop();
        activateEvent(passed_event);
    }
}

void GameEventScheduler::activateEvent(GameEvent *event)
{
    auto state = event->activate();
    emit eventActivated(event);
    if (event->interval() < 0) {
        state = GameEvent::Finish;
    }
    switch (state) {
    case GameEvent::Finish: {
        delete event;
        break;
    }
    case GameEvent::Replay: {
        event->time_point_ += event->interval_;
        events_.push(event);
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    }
}

