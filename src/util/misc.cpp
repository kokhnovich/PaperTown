#include "misc.h"

bool mergeBooleans(bool a, Util::Bool3 b)
{
    if (b == Util::Dont_Care) {
        return a;
    }
    return b == Util::True;
}

Util::Bool3 mergeBooleans(Util::Bool3 a, Util::Bool3 b)
{
    if (b == Util::Dont_Care) {
        return a;
    }
    return b;
}
