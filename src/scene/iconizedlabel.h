#ifndef ICONIZEDLABEL_H
#define ICONIZEDLABEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class IconizedLabel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
    Q_PROPERTY(QString text READ text WRITE setText)
public:
    IconizedLabel();
    const QPixmap *pixmap() const;
    void setPixmap(const QPixmap &pixmap);
    QString text() const;
    void setText(const QString &text);
    
    /* By some reason, I need to do this manually... */
    void setPalette(const QPalette &palette);
private:
    QHBoxLayout *layout_;
    QLabel *icon_label_;
    QLabel *text_label_;
};

#endif // ICONIZEDLABEL_H
