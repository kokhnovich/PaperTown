#ifndef EVENTSCHEDULER_H
#define EVENTSCHEDULER_H

#include <QObject>
#include <QElapsedTimer>
#include <QPointer>
#include <queue>

/*
 * Important note: all the times here are measured in milliseconds!
 */

class GameEventScheduler;

class GameEvent : public QObject
{
    Q_OBJECT
public:
    enum State {
        Finish,
        Replay
    };
    Q_ENUM(State);
    
    virtual State activate();

    qint64 interval() const;
    void setInterval(qint64 interval);
    
    qint64 timeBeforeActivate() const;
    
    void attach(QObject *object);
    
    bool isFinished() const;
    
    friend class GameEventScheduler;
public slots:
    void finish();
private:
    void setParent(GameEventScheduler *parent);

    qint64 time_point_;
    qint64 interval_ = -1;
    GameEventScheduler *scheduler_ = nullptr;
    bool is_finished = false;
};

class GameSignalEvent : public GameEvent
{
    Q_OBJECT
public:
    State activate() override;
    GameSignalEvent(GameEvent::State type);
signals:
    void activated();
private:
    GameEvent::State type_;
};

struct GameEventContainer {
    qint64 time_point;
    QPointer<GameEvent> event;
};

bool operator<(const GameEventContainer &a, const GameEventContainer &b);

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
    
    qint64 timeBeforeActivate(const GameEvent *event) const;
signals:
    void eventActivated(GameEvent *event);
protected:
    void activateEvent(GameEvent *event);
private:
    qint64 realTimePoint() const;
    
    QElapsedTimer timer_;
    std::priority_queue<GameEventContainer> events_;
    bool active_;
    qint64 delta_;
    qint64 pause_start_;
};

#endif // EVENTSCHEDULER_H
