#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QVector>
#include <QHash>
#include "../util/coordinates.h"
#include "../util/misc.h"

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
protected:
    void attach(GameObject *object);
    void detach(GameObject *object);
    void startObjectRemoval(GameObject *object);
};

class GameObjectProperty : public QObject
{
    Q_OBJECT
public:
    virtual bool canSelect(bool last_value) const;
    virtual bool canMove(bool last_value) const;
    virtual bool canSetPosition(bool last_value, const Coordinate &position) const;
    
    QString objectName() const;
    GameObject *gameObject() const;
    void initialize(GameObject *object);
    bool isInitialized() const;
    
    Q_INVOKABLE GameObjectProperty();

    virtual GameObjectProperty *castTo(const QMetaObject *meta);
    
    template<typename T>
    inline friend T *gameProperty_cast(GameObjectProperty *property) {
        if (!property) {
            return nullptr;
        }
        return qobject_cast<T *>(property->castTo(&T::staticMetaObject));
    }

    GameObjectInfo *objectInfo() const;
    
    GameObjectRepositoryBase *repository() const;
protected:
    virtual Util::Bool3 canSelect() const;
    virtual Util::Bool3 canMove() const;
    
    virtual void doInitialize();
signals:
    void updated();
    void initializing();
private:
    GameObject *game_object_;
    bool initialized_;
};

class GameObject : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(Coordinate position READ position WRITE setPosition)

    GameObject(const QString &name, GameObjectRepositoryBase *repository);
    void initProperty(GameObjectProperty *property);
    
    QString name() const;
    virtual QString type() const = 0;
    Coordinate position() const;
    virtual const QVector<Coordinate> cellsRelative() const;
    const QVector<Coordinate> cells() const;
    GameObjectProperty *property() const;

    int x() const;
    int y() const;

    bool active() const;
    bool isSelected() const;
    bool isMoving() const;
    bool isRemoving() const;

    SelectionState getSelectionState() const;
    
    virtual bool canSelect() const;
    virtual bool canMove() const;

    Coordinate movingPosition() const;
    void setMovingPosition(const Coordinate &c);
    bool canApplyMovingPosition() const;
    bool applyMovingPosition();

    GameFieldBase *field() const;
    GameObjectRepositoryBase *repository() const;

    bool canSetPosition(const Coordinate &pos) const;
    bool setPosition(const Coordinate &pos);

    GameObjectInfo *objectInfo() const;
    
    friend class GameFieldBase;
protected:
    virtual bool internalCanSelect() const;
    virtual bool internalCanMove() const;
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
public slots:
    void removeSelf();

    void select();
    void unselect();

    void startMoving();
    void endMoving();
protected:
    void decline();
private:
    void setField(GameFieldBase *field);

    QString name_;
    bool active_;
    bool activating_;
    bool is_selected_;
    bool is_moving_;
    bool is_removing_;
    Coordinate position_;
    Coordinate moving_position_;
    GameFieldBase *field_;
    GameObjectProperty *property_;
    GameObjectRepositoryBase *repository_;
};

#endif // GAMEOBJECT_H
