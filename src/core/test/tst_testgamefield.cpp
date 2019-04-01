#include <QtTest>

// add necessary includes here
#include "../gamefield.h"
#include "../gameobjects.h"

class TestGameField : public QObject
{
    Q_OBJECT

public:
    TestGameField();
    ~TestGameField();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void basicFeatures();
    void humans();
    void placement();
private:
    GameObjectRepository repository;
};

TestGameField::TestGameField()
{
    repository.addObject("ground", "sand", {{0, 0}});
    repository.addObject("static", "one-cell", {{0, 0}});
    repository.addObject("static", "square", {{0, 0}, {0, 1}, {1, 0}, {1, 1}});
    repository.addObject("static", "horz-line", {{-1, 0}, {0, 0}, {1, 0}});
    repository.addObject("moving", "human", {{0, 0}});
    repository.addObject("moving", "lying-human1", {{0, 0}, {1, 0}});
    repository.addObject("moving", "lying-human2", {{0, 0}, {0, 1}});
}

TestGameField::~TestGameField()
{
}

void TestGameField::initTestCase()
{
}

void TestGameField::cleanupTestCase()
{
}

void TestGameField::basicFeatures()
{
    GameField field(nullptr, &repository, 30, 30);
    // add obj1
    GameObject *obj1 = new StaticObject("square");
    QVERIFY(field.canPlace(obj1, Coordinate {2, 2}));
    field.add(obj1);
    obj1->setPosition(Coordinate {2, 2});
    QCOMPARE(field.getCell({2, 2}), QVector<GameObject *>{obj1});
    QCOMPARE(field.getCell({3, 3}), QVector<GameObject *>{obj1});
    QCOMPARE(field.getCell({1, 1}), QVector<GameObject *>{});
    // add obj2
    GameObject *obj2 = new StaticObject("horz-line");
    field.add(obj2);
    QVERIFY(!field.canPlace(obj2, Coordinate {2, 2}));
    QVERIFY(!obj2->canSetPosition(Coordinate {2, 2}));
    QVERIFY(obj2->canSetPosition(Coordinate {1, 1}));
    obj2->setPosition(Coordinate {1, 1});
    QCOMPARE(field.getCell({1, 1}), QVector<GameObject *>{obj2});
    QCOMPARE(field.getCell({2, 1}), QVector<GameObject *>{obj2});
    QCOMPARE(field.getCell({3, 1}), QVector<GameObject *>{});
    // remove obj1
    field.remove(obj1);
    QCOMPARE(field.getCell({2, 2}), QVector<GameObject *>{});
    // try to move obj2
    QVERIFY(obj2->canSetPosition({2, 1}));
    obj2->setPosition({2, 1});
    QCOMPARE(field.getCell({0, 1}), QVector<GameObject *>{});
    QCOMPARE(field.getCell({1, 1}), QVector<GameObject *>{obj2});
    QCOMPARE(field.getCell({2, 1}), QVector<GameObject *>{obj2});
}

void TestGameField::humans()
{
    GameField field(nullptr, &repository, 30, 30);
    // add ground and building
    GameObject *ground = field.add(new GroundObject("sand"));
    GameObject *building = field.add(new StaticObject("square"));
    ground->setPosition({1, 1});
    building->setPosition({2, 2});
    // add humans
    GameObject *human1 = field.add(new MovingObject("human"));
    GameObject *human2 = field.add(new MovingObject("human"));
    QVERIFY(human1->canSetPosition({0, 0}));
    QVERIFY(human1->canSetPosition({1, 1}));
    QVERIFY(!human1->canSetPosition({2, 2}));
    human1->setPosition({0, 0});
    QVERIFY(human2->canSetPosition({0, 0}));
    QVERIFY(human2->canSetPosition({1, 1}));
    QVERIFY(!human2->canSetPosition({2, 2}));
    human2->setPosition({0, 0});
    QCOMPARE(field.getCell({0, 0}), (QVector<GameObject *>{human1, human2}));
    QCOMPARE(field.getCell({1, 1}), QVector<GameObject *>{ground});
    QCOMPARE(field.getCell({2, 2}), QVector<GameObject *>{building});
    // move people
    QVERIFY(human1->canSetPosition({1, 1}));
    human1->setPosition({1, 1});
    QVERIFY(human2->canSetPosition({1, 1}));
    human2->setPosition({1, 1});
    QCOMPARE(field.getCell({0, 0}), QVector<GameObject *>{});
    QCOMPARE(field.getCell({1, 1}), (QVector<GameObject *>{ground, human1, human2}));
}

void TestGameField::placement()
{
    GameField field(nullptr, &repository, 10, 10);
    auto ground = field.add(new GroundObject("sand"));
    auto building = field.add(new StaticObject("square"));
    auto human1 = field.add(new MovingObject("lying-human1"));
    auto human2 = field.add(new MovingObject("human"));
    QVERIFY(!ground->canSetPosition({-1, -1}));
    QVERIFY(!ground->canSetPosition({-1, 8}));
    QVERIFY(!ground->canSetPosition({10, 10}));
    ground->setPosition({5, 5});
    QVERIFY(building->canSetPosition({5, 5}));
    building->setPosition({5, 5});
    QVERIFY(!human1->canSetPosition({4, 5}));
    QVERIFY(!human1->canSetPosition({5, 5}));
    QVERIFY(!human1->canSetPosition({9, 5}));
    QVERIFY(human1->canSetPosition({7, 5}));
    building->setPosition({6, 5});
    QVERIFY(!human1->canSetPosition({5, 5}));
    QVERIFY(human1->canSetPosition({4, 5}));
    human1->setPosition({4, 5});
    QVERIFY(!building->canSetPosition({5, 5}));
    QVERIFY(human2->canSetPosition({5, 5}));
    human2->setPosition({5, 5});
}

QTEST_APPLESS_MAIN(TestGameField)

#include "tst_testgamefield.moc"
