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
    return result;
}

QList<QGraphicsItem *> GameAbstractPropertyRenderer::doDrawProperty(GameObjectProperty *)
{
    return {};
}

QWidget *GameAbstractPropertyRenderer::drawControlWidget(GameObjectProperty *)
{
    return nullptr;
}

GamePropertyRenderer::GamePropertyRenderer(GameTextureRendererBase *renderer, QObject *parent)
    : GameAbstractPropertyRenderer(renderer, parent)
{}

QWidget *GamePropertyRenderer::drawControlWidget(GameObjectProperty *property)
{
    if (!property->inherits("GameObjectPropertyContainer")) {
        auto name = property->metaObject()->className();
        if (!renderers_.contains(name)) {
            qWarning() << "property type" << name << "has no renderer!";
            return nullptr;
        }
        return renderers_.value(name)->drawControlWidget(property);
    }

    GameObjectPropertyContainer *container = qobject_cast<GameObjectPropertyContainer *>(property);

    auto widget = new QWidget(nullptr);
    auto layout = new QVBoxLayout(widget);

    for (auto item : container->properties()) {
        auto item_widget = drawControlWidget(item);
        if (item_widget == nullptr) {
            continue;
        }
        layout->addWidget(item_widget);
    }

    return widget;
}

QList<QGraphicsItem *> GamePropertyRenderer::doDrawProperty(GameObjectProperty *property)
{
    if (!property->inherits("GameObjectPropertyContainer")) {
        auto name = property->metaObject()->className();
        if (!renderers_.contains(name)) {
            qWarning() << "property type" << name << "has no renderer!";
            return {};
        }
        return renderers_.value(name)->drawProperty(property);
    }

    GameObjectPropertyContainer *container = qobject_cast<GameObjectPropertyContainer *>(property);
    QList<QGraphicsItem *> items;
    for (auto property : container->properties()) {
        auto add_items = doDrawProperty(property);
        items.append(add_items);
    }
    return items;
}

void GamePropertyRenderer::addRenderer(const QString &property_class, GameAbstractPropertyRenderer *renderer)
{
    renderer->setParent(this);
    renderers_[property_class] = renderer;
}
