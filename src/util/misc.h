#ifndef MISC_H
#define MISC_H

#include <QtCore>


namespace Util
{
    Q_NAMESPACE
    
    enum Sound {
        Building,
        Removing,
        Wrecked
    };

    enum Bool3 {
        True,
        False,
        Dont_Care
    };
    Q_ENUM_NS(Bool3);
    
    enum Direction {
        Up,
        Left,
        Down,
        Right
    };
    Q_ENUM_NS(Direction);
}

bool mergeBooleans(bool a, Util::Bool3 b);
Util::Bool3 mergeBooleans(Util::Bool3 a, Util::Bool3 b);

#endif // MISC_H
