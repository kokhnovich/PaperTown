#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QGraphicsBlurEffect>
#include <QLayout>
#include "gamefieldviews.h"
#include "gamescenegeometry.h"

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
    item->setPos(texture->offset + geometry_->coordinateToTopLeft(c));
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

QGraphicsItem *GameTextureRenderer::drawSelectionRect(GameObject *object)
{
    QColor brush_color, pen_color;
    if (object->canApplyMovingPosition()) {
        brush_color = QColor(0, 192, 0, 160);
        pen_color = QColor(0, 128, 0, 160);
    } else {
        brush_color = QColor(255, 0, 0, 160);
        pen_color = QColor(192, 0, 0, 160);
    }

    QPen pen(pen_color);
    pen.setWidth(2);

    QPolygonF poly;
    for (Coordinate delta : object->cellsRelative()) {
        poly = poly.united(geometry_->selectionPolygon().translated(geometry_->offset(delta)));
    }

    return scene_->addPolygon(poly, pen, QBrush(brush_color));
}

QGraphicsItem *GameTextureRenderer::drawMoving(GameObject *object)
{
    const GameTexture *texture = textures_->getTexture(object->name());

    QGraphicsPixmapItem *pixmap_item = scene_->addPixmap(texture->pixmap);
    pixmap_item->setOffset(texture->offset);
    pixmap_item->setZValue(1.0);

    QList<QGraphicsItem *> group_items;
    group_items.append(pixmap_item);
    group_items.append(drawSelectionRect(object));

    QGraphicsItemGroup *group = scene_->createItemGroup(group_items);
    group->setPos(geometry_->coordinateToTopLeft(object->movingPosition()));
    group->setZValue(4e6);

    return group;
}

QGraphicsWidget *GameTextureRenderer::drawControlButtons(const GameObject *object)
{
    QFont font;
    font.setPixelSize(2 * geometry_->cellSize());

    auto parent_widget = new QWidget();
    parent_widget->move({0, 0});
    parent_widget->setWindowFlags(Qt::FramelessWindowHint);
    parent_widget->setAttribute(Qt::WA_NoSystemBackground);
    parent_widget->setAttribute(Qt::WA_TranslucentBackground);
    parent_widget->setStyleSheet(QStringLiteral(R"CSS(
        QPushButton {
            border: 5px groove #878AB5;
            border-radius: 12px;
            padding: 4px;
            background-color: rgb(242, 246, 248);
        }
        
        QPushButton:pressed {
            border-style: ridge;
            background-color: rgb(224, 228, 232);
        }
        
        QPushButton:focus {
            border-color: #5E6596;
        }
        
        QPushButton#delete-btn {
            border-color: #A58A87;
            background-color: rgb(248, 232, 228);
        }
        
        QPushButton#delete-btn:pressed {
            background-color: #FF6F70;
        }
        
        QPushButton#delete-btn:pressed:focus {
            border-color: #872525;
        }
        
        QPushButton#delete-btn:focus {
            border-color: #C14848;
        }
    )CSS"));

    auto layout = new QVBoxLayout(parent_widget);

    auto move_btn = new QPushButton("Move", parent_widget);
    move_btn->setFont(font);
    move_btn->setVisible(object->canMove());
    layout->addWidget(move_btn);
    connect(move_btn, &QPushButton::released, object, &GameObject::startMoving);

    auto delete_btn = new QPushButton("Delete", parent_widget);
    delete_btn->setFont(font);
    delete_btn->setObjectName(QStringLiteral("delete-btn"));
    layout->addWidget(delete_btn);
    connect(delete_btn, &QPushButton::clicked, object, &GameObject::removeSelf);

    auto widget_proxy = scene_->addWidget(parent_widget);

    Rect bound_rect = boundingRect(object->cells());
    QPointF top_left = geometry_->coordinateToRect({bound_rect.top, bound_rect.left}).topLeft();
    QPointF bottom_right = geometry_->coordinateToRect({bound_rect.bottom, bound_rect.right}).bottomRight();
    QPointF widget_middle(parent_widget->size().width() / 2, parent_widget->size().height());

    widget_proxy->setZValue(3e6);
    widget_proxy->setPos((top_left + bottom_right) / 2 - widget_middle);

    return widget_proxy;
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

qreal GameFieldView::getStateZDelta(SelectionState state) const
{
    return (state == SelectionState::Selected) ? 1e6 : 0;
}

void GameFieldView::changeObjectSelectionState(GameObject *object, SelectionState state)
{
    iterateTextures(object, [ = ](const TextureInfo & info) {
        qreal opacity = 1.0;
        switch (state) {
        case SelectionState::None: {
            opacity = 1.0;
            break;
        }
        case SelectionState::Moving: {
            opacity = 0.5;
            break;
        }
        case SelectionState::Selected: {
            opacity = 0.75;
            break;
        }
        default: {
            Q_UNREACHABLE();
        }
        }
        info.item->setOpacity(opacity);

        double zdelta = getStateZDelta(state) - getStateZDelta(last_state_);
        if (!qFuzzyIsNull(zdelta)) {
            info.item->setZValue(info.item->zValue() + zdelta);
        }
    });

    if (state == SelectionState::Selected && object->active()) {
        control_buttons_ = renderer_->drawControlButtons(object);
    } else if (control_buttons_ != nullptr) {
        control_buttons_->deleteLater();
        control_buttons_ = nullptr;
    }

    if (state == SelectionState::Moving) {
        moving_item_ = renderer_->drawMoving(object);
    } else if (moving_item_ != nullptr) {
        scene_->removeItem(moving_item_);
        delete moving_item_;
        moving_item_ = nullptr;
    }

    last_state_ = state;
}

void GameFieldView::movingPositionChanged(const Coordinate &, const Coordinate &)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    Q_CHECK_PTR(moving_item_);
    scene_->removeItem(moving_item_);
    delete moving_item_;
    moving_item_ = renderer_->drawMoving(object);
}

void GameFieldView::selectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::Selected);
}

void GameFieldView::unselectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::None);
}

void GameFieldView::startMovingObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::Moving);

}

void GameFieldView::endMovingObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::Selected);
}

void GameFieldView::addObject(GameObject *object)
{
    connect(object, &GameObject::placed, this, &GameFieldView::placeObject);
    connect(object, &GameObject::moved, this, &GameFieldView::moveObject);
    connect(object, &GameObject::updated, this, &GameFieldView::updateObject);
    connect(object, &GameObject::selected, this, &GameFieldView::selectObject);
    connect(object, &GameObject::unselected, this, &GameFieldView::unselectObject);
    connect(object, &GameObject::startedMoving, this, &GameFieldView::startMovingObject);
    connect(object, &GameObject::endedMoving, this, &GameFieldView::endMovingObject);
    connect(object, &GameObject::movingPositionChanged, this, &GameFieldView::movingPositionChanged);

    putObject(object);
}

void GameFieldView::putObject(GameObject *object)
{
    if (object->active()) {
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
    if (object->isSelected()) {
        changeObjectSelectionState(object, object->isMoving() ? SelectionState::Moving : SelectionState::Selected);
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
      repository_(repository),
      objects_(),
      moving_item_(nullptr),
      control_buttons_(nullptr),
      last_state_(SelectionState::None)
{
    renderer_->setupScene();
}

void GameFieldView::unputObject(GameObject *object)
{
    if (object->isSelected()) {
        changeObjectSelectionState(object, SelectionState::None);
    }
    if (objects_.contains(object)) {
        iterateTextures(object, [&](const TextureInfo & info) {
            scene_->removeItem(info.item);
            delete info.item;
        });
        objects_.remove(object);
    }
}

void GameFieldView::removeObject(GameObject *object)
{
    disconnect(object, &GameObject::placed, this, &GameFieldView::placeObject);
    disconnect(object, &GameObject::moved, this, &GameFieldView::moveObject);
    disconnect(object, &GameObject::updated, this, &GameFieldView::updateObject);
    disconnect(object, &GameObject::selected, this, &GameFieldView::selectObject);
    disconnect(object, &GameObject::unselected, this, &GameFieldView::unselectObject);
    disconnect(object, &GameObject::startedMoving, this, &GameFieldView::startMovingObject);
    disconnect(object, &GameObject::endedMoving, this, &GameFieldView::endMovingObject);
    disconnect(object, &GameObject::movingPositionChanged, this, &GameFieldView::movingPositionChanged);

    unputObject(object);
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
    return render_info_[fullName(type, name)].data();
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


