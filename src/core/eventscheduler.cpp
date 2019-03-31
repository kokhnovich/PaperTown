#include "eventscheduler.h"

void GameAbstractEvent::activate()
{
}

bool GameEventScheduler::active() const
{
    return active_;
}

void GameEventScheduler::addEvent(GameAbstractEvent* event, qint64 delay)
{
    event->setParent(this);
    event->time_point_ = realTimePoint() + delay;
    events_.push(event);
}

GameEventScheduler::GameEventScheduler(QObject* parent, bool active)
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
        GameAbstractEvent *passed_event = events_.top();
        events_.pop();
        passed_event->activate();
        emit eventActivated(passed_event);
        delete passed_event;
    }
}

void GameEventScheduler::activateEvent(GameAbstractEvent* event)
{
    event->activate();
}
