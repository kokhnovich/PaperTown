#ifndef EVENTSCHEDULER_H
#define EVENTSCHEDULER_H

#include <QObject>
#include <QElapsedTimer>
#include <queue>

class GameEvent : public QObject
{
    Q_OBJECT
public:
    enum EventState {
        Finish,
        Replay
    };
    Q_ENUM(EventState);
    
    virtual EventState activate();

    qint64 interval() const;
    void setInterval(qint64 interval);
    
    friend class GameEventScheduler;
private:
    qint64 time_point_;
    qint64 interval_ = -1;
    
    struct Compare {
        inline bool operator()(GameEvent *a, GameEvent *b) {
            return a->time_point_ > b->time_point_;
        }
    };
    
    friend struct Compare;
};

class GameEventScheduler : public QObject
{
    Q_OBJECT
public:
    GameEventScheduler(QObject *parent = nullptr, bool active = true);
    void addEvent(GameEvent *event, qint64 delay, qint64 interval = -1);
    bool active() const;
    void start();
    void pause();
    void update();
signals:
    void eventActivated(GameEvent *event);
protected:
    void activateEvent(GameEvent *event);
private:
    qint64 realTimePoint() const;
    
    QElapsedTimer timer_;
    std::priority_queue<GameEvent *, std::vector<GameEvent *>, GameEvent::Compare> events_; 
    bool active_;
    qint64 delta_;
    qint64 pause_start_;
};

#endif // EVENTSCHEDULER_H
