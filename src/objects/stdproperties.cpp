#include "../objects/stdproperties.h"
#include <QDateTime>
#include <random>

// FIXME : use better random generator!
std::mt19937 rnd(QDateTime::currentMSecsSinceEpoch());

void GameProperty_house::doInitialize()
{
    QVariant population_data = objectInfo()->keys["population"];
    if (population_data.type() == QVariant::List) {
        int min_population = population_data.toList()[0].toInt();
        int max_population = population_data.toList()[1].toInt();
        population_ = rnd() % (max_population - min_population + 1) + min_population;
    } else {
        population_ = population_data.toInt();
    }
}

GameProperty_house::GameProperty_house()
    : GameObjectProperty()
{}

int GameProperty_house::population()
{
    return population_;
}

GAME_PROPERTY_REGISTER("house", GameProperty_house)
