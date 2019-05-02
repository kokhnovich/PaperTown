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

class GameDynamicIndicator : public QObject
{
    Q_OBJECT
public:
    virtual bool canSetValue() const;
    virtual void setValue(qreal value);
    virtual void addToValue(qreal delta);
    virtual qreal value() const = 0;
    virtual QString name() const;
    GameDynamicIndicator(QString &name);
signals:
    void updated();
private:
    QString name_;
};

class GameIndicators : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Unassigned,
        Resource,
        Static,
        Dynamic
    };
    Q_ENUM(Type);
    
    struct Info {
        QString name;
        Type type;
        qreal value;
    };
    
    GameResources *resources() const;
    
    Type getType(const QString &name) const;
    bool contains(const QString &name) const;
    
    qreal get(const QString &name) const;
    void set(const QString &name, qreal value);
    void add(const QString &name, qreal delta);
    
    void addDynamic(GameDynamicIndicator *counter);
    
    QVector<QString> listNames() const;
    QVector<Info> list() const;
    
    explicit GameIndicators(QObject *parent = nullptr);
signals:
    void updated(Type type, const QString &name);
    void added(Type type, const QString &name);
private:
    GameResources *resources_;
    QHash<QString, qreal> static_indicators_;
    QHash<QString, GameDynamicIndicator *> dynamic_indicators_;
};

#endif // GAMERESOURCES_H
