#ifndef PROGRESSBARGRAPHICSITEM_H
#define PROGRESSBARGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPixmap>

class ProgressBarGraphicsItem : public QGraphicsItem
{
public:
    ProgressBarGraphicsItem(const QPixmap &progress_bar_image);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    qreal progress() const;
    void setProgress(qreal progress);
private:
    QPixmap progress_bar_image_;
    qreal progress_;
    
    int filledWidth() const;
    int imageWidth() const;
    int imageHeight() const;
};

#endif // PROGRESSBARGRAPHICSITEM_H
