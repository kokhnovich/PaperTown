#ifndef EVENTSCHEDULER_H
#define EVENTSCHEDULER_H

#include <QObject>
#include <QElapsedTimer>
#include <queue>

class GameAbstractEvent : public QObject
{
    Q_OBJECT
public:
    virtual void activate();

    friend class GameEventScheduler;
private:
    qint64 time_point_;
    
    struct Compare {
        inline bool operator()(GameAbstractEvent *a, GameAbstractEvent *b) {
            return a->time_point_ < b->time_point_;
        }
    };
    
    friend struct Compare;
};

class GameEventScheduler : public QObject
{
    Q_OBJECT
public:
    GameEventScheduler(QObject *parent = nullptr, bool active = true);
    void addEvent(GameAbstractEvent *event, qint64 delay);
    bool active() const;
    void start();
    void pause();
    void update();
signals:
    void eventActivated(GameAbstractEvent *event);
protected:
    void activateEvent(GameAbstractEvent *event);
private:
    qint64 realTimePoint() const;
    
    QElapsedTimer timer_;
    std::priority_queue<GameAbstractEvent *, std::vector<GameAbstractEvent *>, GameAbstractEvent::Compare> events_; 
    bool active_;
    qint64 delta_;
    qint64 pause_start_;
};

#endif // EVENTSCHEDULER_H
