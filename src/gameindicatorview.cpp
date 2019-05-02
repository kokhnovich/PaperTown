#include <QtDebug>
#include <QFile>
#include <QFont>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include "contrib/flowlayout/flowlayout.h"
#include "gameindicatorview.h"

const int ICON_SIZE = 48;

void GameIndicatorRepository::addInfo(const QString &name, const GameIndicatorRepository::Info &info)
{
    Q_ASSERT(!data_.contains(name));
    data_[name] = QSharedPointer<Info>(new Info(info));
}

GameIndicatorRepository::GameIndicatorRepository(QWidget *parent)
    : QWidget(parent)
{}

const GameIndicatorRepository::Info *GameIndicatorRepository::getInfo(const QString &name) const
{
    if (!data_.contains(name)) {
        qCritical() << "could not get indicator with name =" << name;
        Q_ASSERT(data_.contains(name));
    }
    return data_.value(name).data();
}

void GameIndicatorRepository::loadFromFile(const QString &file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    qDebug() << "GameIndicatorRepository: json parsed, error =" << error.errorString();
    Q_ASSERT(error.error == QJsonParseError::NoError);
    return loadFromJson(document);
}

void GameIndicatorRepository::loadFromJson(const QJsonDocument &document)
{
    Q_ASSERT(document.isObject());
    auto root_object = document.object();
    for (auto it = root_object.begin(); it != root_object.end(); ++it) {
        Q_ASSERT(it.value().isObject());
        auto inner_object = it.value().toObject();
        Info info;
        info.format = inner_object["format"].toString(QStringLiteral("%.2f"));
        QString icon_name = inner_object["icon"].toString();
        if (!icon_name.isEmpty()) {
            info.icon = QPixmap(QStringLiteral(":/img/icon-%1.png").arg(icon_name)).scaledToHeight(ICON_SIZE);
        }
        info.stylesheet = inner_object["stylesheet"].toString();
        info.text_color = QColor(inner_object["text-color"].toString(QStringLiteral("black")));
        info.text = inner_object["text"].toString();
        addInfo(it.key(), info);
    }
}

void GameIndicatorView::addIndicator(const QString &name)
{
    auto label = new IconizedLabel;
    auto info = indicator_info_->getInfo(name);

    layout()->addWidget(label);

    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, info->text_color);
    label->setPalette(palette);

    label->setPixmap(info->icon);
    label->setToolTip(info->text);

    if (!info->stylesheet.isEmpty()) {
        label->setStyleSheet(info->stylesheet);
    }

    widgets_[name] = label;

    updateIndicator(name);
}

void GameIndicatorView::updateIndicator(const QString &name)
{
    auto label = widgets_[name];
    auto info = indicator_info_->getInfo(name);
    label->setText(QString::asprintf(info->format.toUtf8().data(), indicators_->get(name)));
}

void GameIndicatorView::initialize(GameIndicators *indicators, GameIndicatorRepository *info)
{
    Q_ASSERT(indicators_ == nullptr);
    Q_ASSERT(indicator_info_ == nullptr);

    indicators_ = indicators;
    indicator_info_ = info;

    foreach (const QString &iter, indicators->listNames()) {
        addIndicator(iter);
    }

    auto update = [ = ](GameIndicators::Type, const QString & name) {
        updateIndicator(name);
    };

    connect(indicators_, &GameIndicators::added, this, update);
    connect(indicators_, &GameIndicators::updated, this, update);
}

void GameIndicatorView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->contentsRect(), palette().brush(QPalette::Background));
    QFrame::paintEvent(event);
}

GameIndicatorView::GameIndicatorView(QWidget *parent)
    : QFrame(parent), indicators_(nullptr), indicator_info_(nullptr)
{
    QFont font;
    font.setWeight(QFont::Bold);
    font.setPixelSize(qRound(0.75 * ICON_SIZE));
    setFont(font);

    QPixmap cells(QPixmap(":/img/cell.png"));
    cells = cells.scaledToHeight(cells.height() / 3);
    QBrush brush;
    brush.setTexture(cells);
    QPalette palette;
    palette.setBrush(QPalette::Background, brush);
    setPalette(palette);
    
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    
    setContentsMargins(2, 2, 2, 2);
    setLayout(new FlowLayout(-1, ICON_SIZE * 2 / 5, 0));
    setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
}
