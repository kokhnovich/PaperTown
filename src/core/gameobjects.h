#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QVector>
#include <QHash>
#include "../util/coordinates.h"
#include "../util/misc.h"
#include "gameindicators.h"

class GameObject;
class GameObjectProperty;

namespace Selection
{
    Q_NAMESPACE
    
    enum State {
        None,
        Selected,
        Moving
    };
    Q_ENUM_NS(State);
}
using SelectionState = Selection::State;

struct GameObjectKey {
    QString type, name;
};

struct GameObjectInfo {
    QVector<Coordinate> cells;
    QVariantMap keys;
    
    qreal cost() const;
};

class GameObjectRepositoryBase : public QObject
{
    Q_OBJECT
public:    
    explicit GameObjectRepositoryBase(QObject *parent = nullptr);
    void addObject(const QString &type, const QString &name, const GameObjectInfo &info);
    GameObjectInfo *getInfo(const QString &type, const QString &name) const;
    QVector<GameObjectKey> keys() const;
    virtual GameObjectProperty *createProperty(const QString &type, const QString &name) const;
protected:
    static GameObjectKey splitName(const QString &full_name);
    static QString fullName(const QString &type, const QString &name);
private:
    QHash<QString, QSharedPointer<GameObjectInfo>> info_;
};

class GameFieldBase : public QObject
{
    Q_OBJECT
public:
    explicit GameFieldBase(QObject *parent);
    virtual GameObjectRepositoryBase *repository() const = 0;
    virtual bool canPlace(const GameObject *object, const Coordinate &pos) const = 0;
    virtual GameObject *add(GameObject *object) = 0;
    virtual void remove(GameObject *object) = 0;
    virtual GameIndicators *indicators() const = 0;
    GameResources *resources() const;
protected:
    void attach(GameObject *object);
    void detach(GameObject *object);
    void startObjectRemoval(GameObject *object);
    void finishObjectRemoval(GameObject *object);
};

class GameObjectProperty : public QObject
{
    Q_OBJECT
public:
    virtual bool canSelect(bool last_value) const;
    virtual bool canMove(bool last_value) const;
    virtual bool canSetPosition(bool last_value, const Coordinate &position) const;
    virtual bool conflictsWith(bool last_value, const GameObject *object) const;
    virtual bool canAutoEnable(bool last_value) const;
    virtual bool canPlace(bool last_value) const;
    
    QString objectName() const;
    GameObject *gameObject() const;
    void initialize(GameObject *object);
    bool isInitialized() const;
    
    Q_INVOKABLE GameObjectProperty();

    virtual GameObjectProperty *castTo(const QMetaObject *meta);

    GameObjectInfo *objectInfo() const;
    GameObjectRepositoryBase *repository() const;
protected:
    virtual Util::Bool3 canSelect() const;
    virtual Util::Bool3 canMove() const;
    virtual Util::Bool3 conflictsWith(const GameObject *object) const;
    virtual Util::Bool3 canSetPosition(const Coordinate &position) const;
    virtual Util::Bool3 canAutoEnable() const;
    virtual Util::Bool3 canPlace() const;
    
    virtual void doInitialize();
    
    void enableObject();
    void disableObject();
signals:
    void updated();
    void initializing();
private:
    GameObject *game_object_;
    bool initialized_;
};

template<typename T>
inline T *gameProperty_cast(GameObjectProperty *property) {
    if (property == nullptr) {
        return nullptr;
    }
    return qobject_cast<T *>(property->castTo(&T::staticMetaObject));
}

class GameObject : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(Coordinate position READ position WRITE setPosition)

    GameObject(const QString &name, GameObjectRepositoryBase *repository, GameIndicators *indicators);
    
    void initProperty(GameObjectProperty *property);
    
    QString name() const;
    virtual QString type() const = 0;
    Coordinate position() const;
    virtual const QVector<Coordinate> cellsRelative() const;
    const QVector<Coordinate> cells() const;
    GameObjectProperty *property() const;
    
    virtual qreal cost() const;
    virtual qreal removalCost() const;

    int x() const;
    int y() const;

    bool isPlaced() const;
    bool isSelected() const;
    bool isMoving() const;
    bool isRemoving() const;
    bool isEnabled() const;

    SelectionState getSelectionState() const;
    
    virtual bool canSelect() const;
    virtual bool canMove() const;
    virtual bool canPlace() const;
    
    Coordinate movingPosition() const;
    void setMovingPosition(const Coordinate &c);
    bool canApplyMovingPosition() const;
    bool applyMovingPosition();

    GameFieldBase *field() const;
    GameObjectRepositoryBase *repository() const;

    bool canSetPosition(const Coordinate &pos) const;
    bool setPosition(const Coordinate &pos);
    
    bool place(const Coordinate &pos);
    
    virtual bool conflictsWith(const GameObject *object) const;
    
    GameObjectInfo *objectInfo() const;
    
    GameIndicators *indicators() const;
    GameResources *resources() const;
    
    friend class GameObjectProperty;
    friend class GameFieldBase;
public slots:
    void removeSelf();

    void select();
    void unselect();

    void startMoving();
    void endMoving();
signals:
    void placed(const Coordinate &position);
    void moved(const Coordinate &oldPosition, const Coordinate &newPosition);
    void movingPositionChanged(const Coordinate &oldPosition, const Coordinate &newPosition);
    void updated();
    void selecting();
    void selected();
    void unselected();
    void startedMoving();
    void endedMoving();
    void declined();
    void attached();
    void enabled();
    void disabled();
    void removed();
protected:
    virtual bool internalCanSelect() const;
    virtual bool internalCanMove() const;
    void emitRemoved();
protected slots:
    void enable();
    void disable();
private:
    void setField(GameFieldBase *field);
    void decline();
    bool canAutoEnable() const;
    
    QString name_;
    bool is_placed_;
    bool is_placing_;
    bool is_selected_;
    bool is_moving_;
    bool is_removing_;
    bool is_enabled_;
    Coordinate position_;
    Coordinate moving_position_;
    GameFieldBase *field_;
    GameObjectProperty *property_;
    GameObjectRepositoryBase *repository_;
    GameIndicators *indicators_;
};

bool objectsConflict(const GameObject *a, const GameObject *b);

#endif // GAMEOBJECT_H
