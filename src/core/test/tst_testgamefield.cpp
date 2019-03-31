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
    void addRemove();
private:
    GameObjectRepository repository;
};

TestGameField::TestGameField()
{
    repository.addObject("static", "one-cell", {{0, 0}});
    repository.addObject("static", "square", {{0, 0}, {0, 1}, {1, 0}, {1, 1}});
    repository.addObject("static", "horz-line", {{-1, 0}, {0, 0}, {1, 0}});
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

void TestGameField::addRemove()
{
    GameField field(nullptr, 30, 30);
    // TODO : write the test
}

QTEST_APPLESS_MAIN(TestGameField)

#include "tst_testgamefield.moc"
