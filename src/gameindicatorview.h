#ifndef GAMECOUNTERVIEW_H
#define GAMECOUNTERVIEW_H

#include <QWidget>
#include <QHash>
#include <QSharedPointer>
#include <QJsonDocument>
#include "core/gameindicators.h"
#include "scene/iconizedlabel.h"

class GameIndicatorRepository : public QWidget
{
    Q_OBJECT
public:
    struct Info {
        QColor text_color;
        QString stylesheet;
        QPixmap icon;
        QString format;
        QString text;
        bool is_graded_color;
    };
    
    void addInfo(const QString &name, const Info &info);
    const Info *getInfo(const QString &name) const;
    void loadFromJson(const QJsonDocument &document);
    void loadFromFile(const QString &file_name);
    explicit GameIndicatorRepository(QWidget *parent = nullptr);
private:
    QHash<QString, QSharedPointer<Info>> data_;
};

class GameIndicatorView : public QFrame
{
    Q_OBJECT
public:
    explicit GameIndicatorView(QWidget *parent = nullptr);
    void initialize(GameIndicators *indicators, GameIndicatorRepository *info);
protected:
    void addIndicator(const QString &name);
    void updateIndicator(const QString &name);
    
    void paintEvent(QPaintEvent *event) override;
private:
    GameIndicators *indicators_;
    GameIndicatorRepository *indicator_info_;
    QHash<QString, IconizedLabel *> widgets_;
};

#endif // GAMECOUNTERVIEW_H
