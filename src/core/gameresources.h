#ifndef GAMERESOURCES_H
#define GAMERESOURCES_H

#include <QObject>
#include <QHash>
#include <QVector>

class GameResources : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Unknown,
        Money,
        Builders,
        // add more resources here
        MaxType
    };
    Q_ENUM(Type);
    
    static Type nameToType(const QString &name);
    static QString typeToName(Type res_type);
    
    explicit GameResources(QObject *parent = nullptr);
    
    bool canAcquire(Type type, qreal amount) const;
    bool acquire(Type type, qreal amount);
    void set(Type type, qreal value);
    void add(Type type, qreal delta);
    qreal get(Type type) const;
    
    bool isInfiniteMode() const;
    void enableInfiniteMode();
    void disableInfiniteMode();
signals:
    void updated(Type res_type);
private:
    bool infinite_mode_ = false;
    qreal resources_[Type::MaxType] = {};
};

class GameDynamicCounter : public QObject
{
    Q_OBJECT
public:
    virtual bool canSetValue() const;
    virtual void setValue(qreal value);
    virtual void addToValue(qreal delta);
    virtual qreal value() const = 0;
    virtual QString name() const;
    GameDynamicCounter(QString &name);
signals:
    void updated();
private:
    QString name_;
};

class GameCounters : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Unassigned,
        Resource,
        StaticCounter,
        DynamicCounter
    };
    Q_ENUM(Type);
    
    struct CounterInfo {
        QString name;
        Type type;
        qreal value;
    };
    
    GameResources *resources() const;
    
    Type counterType(const QString &name) const;
    bool hasCounter(const QString &name) const;
    
    qreal getCounter(const QString &name) const;
    void setCounter(const QString &name, qreal value);
    void addToCounter(const QString &name, qreal delta);
    
    void addDynamicCounter(GameDynamicCounter *counter);
    
    QVector<QString> listCounterNames() const;
    QVector<CounterInfo> listCounters() const;
    
    explicit GameCounters(QObject *parent = nullptr);
signals:
    void updated(Type type, const QString &name);
    void added(Type type, const QString &name);
private:
    GameResources *resources_;
    QHash<QString, qreal> static_counters_;
    QHash<QString, GameDynamicCounter *> dynamic_counters_;
};

#endif // GAMERESOURCES_H
