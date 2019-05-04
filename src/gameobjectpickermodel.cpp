#include <QModelIndex>
#include <QIcon>
#include <QtDebug>
#include <algorithm>
#include "gameobjectpickermodel.h"

GameObjectKey GameObjectPickerModel::indexToObjectKey(const QModelIndex &index) const
{
    int idx = index.row();
    Q_ASSERT(0 <= idx && idx < keys_.size());
    return keys_[idx];
}

QVariant GameObjectPickerModel::data(const QModelIndex &index, int role) const
{
    auto key = indexToObjectKey(index);
    auto value = repository_->getRenderInfo(key.type, key.name);
    auto info = repository_->getInfo(key.type, key.name);
    switch (role) {
    case Qt::DisplayRole: {
        return QVariant(QStringLiteral("%1\n(-%2 $)").arg(value->caption).arg(info->cost(), 0, 'f', 2));
    }
    case Qt::DecorationRole: {
        return QVariant(QIcon(textures_->getTexture(key.name)->pixmap));
    }
    default: {
        return QVariant();
    }
    }
}

Qt::ItemFlags GameObjectPickerModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int GameObjectPickerModel::columnCount(const QModelIndex &) const
{
    return 1;
}

int GameObjectPickerModel::rowCount(const QModelIndex &) const
{
    return keys_.size();
}

QModelIndex GameObjectPickerModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

GameObjectPickerModel::GameObjectPickerModel(GameObjectRenderRepository *repository, GameTextureRepository *textures,
        QObject *parent)
    : QAbstractListModel(parent), repository_(repository), textures_(textures)
{
    keys_ = repository_->keys();
    std::sort(keys_.begin(), keys_.end(), [](const GameObjectKey &key1, const GameObjectKey &key2) {
        return std::make_pair(key1.type, key1.name) < std::make_pair(key2.type, key2.name);
    });
}

void GameObjectSelectionModel::changeSelection(const QItemSelection &selected, const QItemSelection &)
{
    auto picker_model = qobject_cast<GameObjectPickerModel *>(model());
    if (!picker_model) {
        return;
    }
    const auto &indexes = selected.indexes();
    for (const auto &index : indexes) {
        GameObjectKey key = picker_model->indexToObjectKey(index);
        auto object = field_->add(key.type, key.name);
        
        connect(object, &GameObject::placed, this, [this, index]() {
            this->select(index, SelectionFlag::Deselect);
            this->select(index, SelectionFlag::Select);
        });
        connect(object, &GameObject::declined, this, [this, index]() {
            this->select(index, SelectionFlag::Deselect);
        });
    }
}

GameObjectSelectionModel::GameObjectSelectionModel(GameField *field, QObject *parent)
    : QItemSelectionModel(nullptr, parent), field_(field)
{
    connect(this, &GameObjectSelectionModel::selectionChanged, this, &GameObjectSelectionModel::changeSelection);
}
