#include "utilities.h"
#include <QtGlobal>

int tron::generateRandomIntInRange(int from, int to)
{
    Q_ASSERT_X(to >= from, "generateRandomIntInRange", "invalid range");
    uint random = (uint) qrand() / (((uint) RAND_MAX + 1) / (uint)(to-from+1));
    return from + random;
}

bool tron::generateRandomBool()
{
    return qrand() < RAND_MAX/2;
}
