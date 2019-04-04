#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>
#include "gamefieldviews.h"

void GameTextureRenderer::setupScene()
{
    scene_->setSceneRect(geometry_->fieldRect());

    QBrush light_brush(QPixmap(":/img/cell.png"));
    QBrush dark_brush(QPixmap(":/img/cell-dark.png"));
    
    QPen border_pen(QColor(255, 0, 0, 128));
    border_pen.setWidth(6.0);
    
    scene_->setBackgroundBrush(dark_brush);
    scene_->addPolygon(geometry_->fieldActivePolygon(), border_pen, light_brush)->setZValue(-1e9);

    for (int i = 0; i < geometry_->fieldHeight(); ++i) {
        for (int j = 0; j < geometry_->fieldWidth(); ++j) {
            QBrush brush(QColor(255.0 / geometry_->fieldHeight() * i, 255.0 / geometry_->fieldWidth() * j, 0, 0));
            QPen pen(QColor(0, 0, 0, 32));
            pen.setWidth(1.0);
            scene_->addPolygon(geometry_->coordinateToPoly({i, j}), pen, brush)->setZValue(-1e9);
        }
    }
}

QGraphicsItem *GameTextureRenderer::drawTexture(const QString &name, const Coordinate &c, qreal priority)
{
    const GameTexture *texture = textures_->getTexture(name);
    Q_CHECK_PTR(texture);
    QGraphicsPixmapItem *item = scene_->addPixmap(texture->pixmap);
    item->setOffset(texture->offset + geometry_->coordinateToTopLeft(c));
    item->setZValue(zOrder(c + texture->z_offset, priority));
    return item;
}

QGraphicsItem *GameTextureRenderer::moveTexture(QGraphicsItem *item, const QString &name, const Coordinate &c, qreal priority)
{
    const GameTexture *texture = textures_->getTexture(name);
    Q_CHECK_PTR(texture);
    item->setPos(texture->offset + geometry_->coordinateToTopLeft(c));
    item->setZValue(zOrder(c + texture->z_offset, priority));
    return item;
}

QGraphicsScene *GameTextureRenderer::scene()
{
    return scene_;
}

GameTextureRenderer::GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
        GameTextureRepository *textures, QGraphicsScene *scene)
    : QObject(parent), geometry_(geometry), textures_(textures), scene_(scene)
{}

qreal GameTextureRenderer::zOrder(const Coordinate &c, qreal priority) const
{
    return (c.x + 1) * geometry_->fieldWidth() - c.y + priority;
}

void GameFieldView::changeObjectSelectionState(GameObject *object, bool selected)
{
    iterateTextures(object, [ = ](const TextureInfo & info) {
        info.item->setOpacity(selected ? 0.6 : 1.0);
    });
}

void GameFieldView::selectionMoved(const Coordinate &, const Coordinate &newPosition)
{
    // TODO
}

void GameFieldView::selectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, true);
    // TODO
}

void GameFieldView::unselectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, false);
    // TODO
}

void GameFieldView::addObject(GameObject *object)
{
    connect(object, &GameObject::placed, this, &GameFieldView::placeObject);
    connect(object, &GameObject::moved, this, &GameFieldView::moveObject);
    connect(object, &GameObject::updated, this, &GameFieldView::updateObject);
    connect(object, &GameObject::selected, this, &GameFieldView::selectObject);
    connect(object, &GameObject::unselected, this, &GameFieldView::unselectObject);
    connect(object, &GameObject::selectMoved, this, &GameFieldView::selectionMoved);
}

void GameFieldView::putObject(GameObject *object)
{
    auto info = repository_->getRenderInfo(object);
    for (const QString &texture_name : info->textures) {
        QGraphicsItem *item = renderer_->drawTexture(texture_name, object->position(), info->priority);
        item->setData(DATA_KEY_GAMEOBJECT, QVariant::fromValue(object));
        objects_.insert(object, {
            texture_name,
            item,
            info->priority
        });
    }
}

void GameFieldView::moveObject(const Coordinate &, const Coordinate &newPosition)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    iterateTextures(object, [ = ](const TextureInfo & info) {
        renderer_->moveTexture(info.item, info.name, newPosition);
    });
}

void GameFieldView::placeObject(const Coordinate &)
{
    putObject(qobject_cast<GameObject *>(sender()));
}

void GameFieldView::updateObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    if (objects_.contains(object)) {
        unputObject(object);
        putObject(object);
    }
}

GameFieldView::GameFieldView(QObject *parent, GameTextureRenderer *renderer, GameObjectRepository *repository)
    : QObject(parent),
    renderer_(renderer),
    scene_(renderer_->scene()),
    repository_(repository), objects_()
{
    renderer_->setupScene();
}

void GameFieldView::unputObject(GameObject *object)
{
    Q_ASSERT(objects_.contains(object));
    iterateTextures(object, [&](const TextureInfo & info) {
        scene_->removeItem(info.item);
        delete info.item;
    });
    objects_.remove(object);
}

void GameFieldView::removeObject(GameObject *object)
{
    disconnect(object, &GameObject::placed, this, &GameFieldView::placeObject);
    disconnect(object, &GameObject::moved, this, &GameFieldView::moveObject);
    disconnect(object, &GameObject::updated, this, &GameFieldView::updateObject);
    disconnect(object, &GameObject::selected, this, &GameFieldView::selectObject);
    disconnect(object, &GameObject::unselected, this, &GameFieldView::unselectObject);
    disconnect(object, &GameObject::selectMoved, this, &GameFieldView::selectionMoved);

    if (objects_.contains(object)) {
        unputObject(object);
    }
}

void GameObjectRepository::addRenderInfo(const QString &type, const QString &name,
        const GameObjectRepository::RenderInfo &info)
{
    QString full_name = fullName(type, name);
    render_info_[full_name] = QSharedPointer<RenderInfo>(new RenderInfo(info));
}

void GameObjectRepository::loadFromFile(const QString &file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    qDebug() << "GameObjectRepository: json parsed, error =" << error.errorString();
    Q_ASSERT(error.error == QJsonParseError::NoError);
    return loadFromJson(document);
}

void GameObjectRepository::loadFromJson(const QJsonDocument &document)
{
    Q_ASSERT(document.isObject());
    auto root_object = document.object();
    for (auto it = root_object.begin(); it != root_object.end(); ++it) {
        Q_ASSERT(it->isObject());
        auto local_object = it->toObject();
        RenderInfo info;
        QString type = local_object.value("type").toString();
        auto texture_arr = local_object.value("textures").toArray();
        info.textures.resize(texture_arr.size());
        for (int i = 0; i < texture_arr.size(); ++i) {
            info.textures[i] = texture_arr[i].toString();
        }
        auto cell_arr = local_object.value("cells").toArray();
        QVector<Coordinate> cells(cell_arr.size());
        for (int i = 0; i < cell_arr.size(); ++i) {
            cells[i].x = cell_arr.at(i).toArray().at(0).toInt();
            cells[i].y = cell_arr.at(i).toArray().at(1).toInt();
        }
        info.priority = local_object.value("priority").toDouble(type_priorities_[type]);
        addRenderInfo(type, it.key(), info);
        addObject(type, it.key(), cells);
    }
}

const GameObjectRepository::RenderInfo *GameObjectRepository::getRenderInfo(const QString &type, const QString &name) const
{
    return render_info_[fullName(type, name)].get();
}

const GameObjectRepository::RenderInfo *GameObjectRepository::getRenderInfo(GameObject *object) const
{
    return getRenderInfo(object->type(), object->name());
}

GameObjectRepository::GameObjectRepository(QObject *parent)
    : GameObjectRepositoryBase(parent)
{
    type_priorities_["ground"] = 0;
    type_priorities_["static"] = 0.33;
    type_priorities_["moving"] = 0.67;
}

