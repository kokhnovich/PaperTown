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

void GameEvent::attach(QObject *object)
{
    connect(object, &QObject::destroyed, this, [ = ]() {
        delete this;
    });
}

qint64 GameEvent::timeBeforeActivate() const
{
    Q_CHECK_PTR(scheduler_);
    return scheduler_->timeBeforeActivate(this);
}

void GameEvent::setParent(GameEventScheduler *parent)
{
    QObject::setParent(parent);
    scheduler_ = parent;
}

bool operator<(const GameEventContainer &a, const GameEventContainer &b)
{
    return a.time_point > b.time_point;
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
    events_.push({event->time_point_, event});
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

qint64 GameEventScheduler::timeBeforeActivate(const GameEvent *event) const
{
    return qMax(Q_INT64_C(0), event->time_point_ - realTimePoint());
}

void GameEventScheduler::update()
{
    qint64 cur_time = realTimePoint();
    while (!events_.empty() && events_.top().time_point <= cur_time) {
        GameEvent *passed_event = events_.top().event;
        events_.pop();
        if (passed_event != nullptr) {
            activateEvent(passed_event);
        }
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
        events_.push({event->time_point_, event});
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    }
}

