#ifndef STDPROPERTIES_H
#define STDPROPERTIES_H

#include <QObject>
#include "gameobjectproperties.h"
#include "../core/gameobjects.h"

class GameProperty_house : public GameObjectProperty
{
    Q_OBJECT
public:
    int population();
    
    Q_INVOKABLE GameProperty_house();
protected:
    void doInitialize() override;
private:
    int population_;
};

#endif // STDPROPERTIES_H
