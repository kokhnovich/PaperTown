#include <QtDebug>
#include "iconizedlabel.h"

const QPixmap *IconizedLabel::pixmap() const
{
    return icon_label_->pixmap();
}

IconizedLabel::IconizedLabel()
{
    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);

    icon_label_ = new QLabel;
    layout_->addWidget(icon_label_);

    text_label_ = new QLabel;
    layout_->addWidget(text_label_);
}

void IconizedLabel::setPixmap(const QPixmap &pixmap)
{
    icon_label_->setPixmap(pixmap);
}

void IconizedLabel::setText(const QString &text)
{
    text_label_->setText(text);
}

QString IconizedLabel::text() const
{
    return text_label_->text();
}

void IconizedLabel::setPalette(const QPalette &palette)
{
    QWidget::setPalette(palette);
    text_label_->setPalette(palette);
}
