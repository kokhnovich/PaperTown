#include <QPainter>
#include "progressbargraphicsitem.h"

ProgressBarGraphicsItem::ProgressBarGraphicsItem(const QPixmap &progress_bar_image)
    : progress_bar_image_(progress_bar_image)
{}

void ProgressBarGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int img_width = imageWidth();
    int img_height = imageHeight();
    int filled_width = filledWidth();
    if (filled_width != 0) {
        painter->drawPixmap(0, 0, progress_bar_image_, 0, img_height, filled_width, img_height);
    }
    if (filled_width != img_height) {
        painter->drawPixmap(filled_width, 0, progress_bar_image_, filled_width, 0, img_width - filled_width, img_height);
    }
}

QRectF ProgressBarGraphicsItem::boundingRect() const
{
    return QRectF(0, 0, imageWidth(), imageHeight());
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
    int old_fill_width = filledWidth();
    progress_ = progress;
    int new_fill_width = filledWidth();
    if (old_fill_width > new_fill_width) {
        std::swap(old_fill_width, new_fill_width);
    }
    if (old_fill_width != new_fill_width) {
        update(QRect(old_fill_width, 0, new_fill_width - old_fill_width + 1, imageHeight()));
    }
}

int ProgressBarGraphicsItem::filledWidth() const
{
    int img_width = imageWidth();
    return qBound(0, qRound(img_width * progress_), img_width);
}

int ProgressBarGraphicsItem::imageHeight() const
{
    return progress_bar_image_.height() / 2;
}

int ProgressBarGraphicsItem::imageWidth() const
{
    return progress_bar_image_.width();
}

