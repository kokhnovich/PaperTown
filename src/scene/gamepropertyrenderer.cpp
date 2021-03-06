#include <QtDebug>
#include <QWidget>
#include <QVBoxLayout>
#include "gamepropertyrenderer.h"

GameTextureRendererBase::GameTextureRendererBase(QObject *parent, GameSceneGeometry *geometry,
        GameTextureRepository *textures, GameObjectRenderRepository *repository, QGraphicsScene *scene)
    : QObject(parent),
      geometry_(geometry),
      textures_(textures),
      repository_(repository),
      scene_(scene)
{}

GameAbstractPropertyRenderer::GameAbstractPropertyRenderer(GameTextureRendererBase *renderer, QObject *parent)
    : QObject(parent), renderer_(renderer)
{}

QList<QGraphicsItem *> GameAbstractPropertyRenderer::drawProperty(GameObjectProperty *property)
{
    auto result = doDrawProperty(property);
    for (QGraphicsItem *item : qAsConst(result)) {
        item->setData(DATA_KEY_PROPERTY, QVariant::fromValue(property));
    }
    return result;
}

void GameAbstractPropertyRenderer::updatePropertyItem(QGraphicsItem *, GameObjectProperty *)
{}

QList<QGraphicsItem *> GameAbstractPropertyRenderer::doDrawProperty(GameObjectProperty *)
{
    return {};
}

QWidget *GameAbstractPropertyRenderer::createControlWidget(GameObjectProperty *)
{
    return nullptr;
}

void GameAbstractPropertyRenderer::updateControlWidget(GameObjectProperty *, QWidget *)
{}

Util::Bool3 GameAbstractPropertyRenderer::canShowMainObject(GameObjectProperty *)
{
    return Util::Dont_Care;
}

GamePropertyRenderer::GamePropertyRenderer(GameTextureRendererBase *renderer, QObject *parent)
    : GameAbstractPropertyRenderer(renderer, parent)
{}

QWidget *GamePropertyRenderer::createControlWidget(GameObjectProperty *property)
{
    if (!property->inherits("GameObjectPropertyContainer")) {
        GameAbstractPropertyRenderer *renderer = acquireRenderer(property);
        if (renderer == nullptr) {
            return nullptr;
        }
        QWidget *widget = renderer->createControlWidget(property);
        if (widget != nullptr) {
            widget->setProperty("__GPR__renderer", QVariant::fromValue(renderer));
        }
        return widget;
    }

    GameObjectPropertyContainer *container = qobject_cast<GameObjectPropertyContainer *>(property);

    auto widget = new QWidget(nullptr);
    auto layout = new QVBoxLayout(widget);
    
    for (auto item : container->properties()) {
        auto item_widget = createControlWidget(item);
        if (item_widget == nullptr) {
            continue;
        }
        item_widget->setObjectName(mangleWidgetName(item));
        layout->addWidget(item_widget);
    }

    return widget;
}

void GamePropertyRenderer::updateControlWidget(GameObjectProperty *property, QWidget *widget)
{
    auto renderer = qvariant_cast<GameAbstractPropertyRenderer *>(widget->property("__GPR__renderer"));
    if (renderer != nullptr) {
        renderer->updateControlWidget(property, widget);
        return;
    }
    
    GameObjectPropertyContainer *container = qobject_cast<GameObjectPropertyContainer *>(property);
    Q_CHECK_PTR(container);
    
    for (auto item : container->properties()) {
        auto item_widget = widget->findChild<QWidget *>(mangleWidgetName(item), Qt::FindDirectChildrenOnly);
        if (item_widget == nullptr) {
            continue;
        }
        updateControlWidget(item, item_widget);
    }
}

Util::Bool3 GamePropertyRenderer::canShowMainObject(GameObjectProperty* property)
{
    if (!property->inherits("GameObjectPropertyContainer")) {
        GameAbstractPropertyRenderer *renderer = acquireRenderer(property);
        if (renderer == nullptr) {
            return Util::Dont_Care;
        }
        return renderer->canShowMainObject(property);
    }
    
    Util::Bool3 res = Util::Dont_Care;
    GameObjectPropertyContainer *container = qobject_cast<GameObjectPropertyContainer *>(property);
    for (auto property: container->properties()) {
        res = mergeBooleans(res, canShowMainObject(property));
    }
    return res;
}

GameAbstractPropertyRenderer *GamePropertyRenderer::acquireRenderer(GameObjectProperty *property)
{
    auto name = property->metaObject()->className();
    if (!renderers_.contains(name)) {
        qWarning() << "property type" << name << "has no renderer!";
        return nullptr;
    }
    return renderers_.value(name);
}

QString GamePropertyRenderer::mangleWidgetName(GameObjectProperty* property)
{
    return QStringLiteral("__GPR__prop-%1-%2")
        .arg(property->metaObject()->className())
        .arg(reinterpret_cast<qintptr>(property), 0, 16);
}

QList<QGraphicsItem *> GamePropertyRenderer::drawProperty(GameObjectProperty *property)
{
    if (!property->inherits("GameObjectPropertyContainer")) {
        GameAbstractPropertyRenderer *renderer = acquireRenderer(property);
        if (renderer == nullptr) {
            return {};
        }
        return renderer->drawProperty(property);
    }

    GameObjectPropertyContainer *container = qobject_cast<GameObjectPropertyContainer *>(property);
    QList<QGraphicsItem *> items;
    for (auto property : container->properties()) {
        auto add_items = drawProperty(property);
        items.append(add_items);
    }
    return items;
}

void GamePropertyRenderer::updatePropertyItem(QGraphicsItem *item, GameObjectProperty *property)
{
    auto name = property->metaObject()->className();
    Q_ASSERT(renderers_.contains(name));
    GameAbstractPropertyRenderer *renderer = renderers_.value(name);
    Q_CHECK_PTR(renderer);
    renderer->updatePropertyItem(item, property);
}

void GamePropertyRenderer::addRenderer(const QString &property_class, GameAbstractPropertyRenderer *renderer)
{
    if (renderer != nullptr) {
        renderer->setParent(this);
    }
    renderers_[property_class] = renderer;
}
