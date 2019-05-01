#include <QPainter>
#include "progressbargraphicsitem.h"

ProgressBarGraphicsItem::ProgressBarGraphicsItem(const QPixmap &progress_bar_image)
    : progress_bar_image_(progress_bar_image)
{}

void ProgressBarGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int img_width = progress_bar_image_.width();
    int img_height = progress_bar_image_.height() / 2;
    int filled_with = qBound(0, qRound(img_width * progress_), img_width);
    if (filled_with != 0) {
        painter->drawPixmap(0, 0, progress_bar_image_, 0, img_height, filled_with, img_height);
    }
    if (filled_with != img_height) {
        painter->drawPixmap(filled_with, 0, progress_bar_image_, filled_with, 0, img_width - filled_with, img_height);
    }
}

QRectF ProgressBarGraphicsItem::boundingRect() const
{
    int img_width = progress_bar_image_.width();
    int img_height = progress_bar_image_.height() / 2;
    return QRectF(0, 0, img_width, img_height);
}

qreal ProgressBarGraphicsItem::progress() const
{
    return progress_;
}

void ProgressBarGraphicsItem::setProgress(qreal progress)
{
    if (qFuzzyCompare(progress, progress_)) {
        return;
    }
    progress_ = progress;
    update();
}

