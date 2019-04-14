#ifndef GAMEOBJECTRENDERREPOSITORY_H
#define GAMEOBJECTRENDERREPOSITORY_H

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "../objects/gameobjectrepository.h"

class GameObjectRenderRepository : public GameObjectRepository
{
    Q_OBJECT
public:
    struct RenderInfo {
        QVector<QString> textures;
        qreal priority;
        QString caption;
    };

    GameObjectRenderRepository(QObject *parent = nullptr);
    void addRenderInfo(const QString &type, const QString &name, const RenderInfo &info);
    const RenderInfo *getRenderInfo(const QString &type, const QString &name) const;
    const RenderInfo *getRenderInfo(GameObject *object) const;
protected:
    void doLoadObject(const QString &type, const QString &name, const QJsonObject &json) override;
private:
    QHash<QString, QSharedPointer<RenderInfo>> render_info_;
    QHash<QString, qreal> type_priorities_;
};

#endif // GAMEOBJECTRENDERREPOSITORY_H
