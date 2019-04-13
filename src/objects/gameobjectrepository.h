#ifndef GAMEOBJECTREPOSITORY_H
#define GAMEOBJECTREPOSITORY_H

#include <QHash>
#include <QJsonDocument>
#include "core/gameobjects.h"

class GameObjectRepository : public GameObjectRepositoryBase
{
    Q_OBJECT
public:
    explicit GameObjectRepository(QObject *parent = nullptr);
    void loadFromJson(const QJsonDocument &document);
    void loadFromFile(const QString &file_name);
    void hideKey(const QString &type, const QString &name);
    void unhideKey(const QString &type, const QString &name);
    QVector<GameObjectKey> keys(bool show_hidden = false);
    GameObjectProperty *createProperty(const QString &type, const QString &name) const override;
    void addPropertyByName(const QString &type, const QString &name, const QString &prop_name);
protected:
    virtual void doLoadObject(const QString &type, const QString &name, const QJsonObject &json);
    bool isKeyHidden(const GameObjectKey &key);
private:
    QHash<QString, bool> is_hidden_;
    QHash<QString, QVector<QString>> properties_;
};

#endif // GAMEOBJECTREPOSITORY_H
