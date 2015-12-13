#ifndef INITOBJECT_H
#define INITOBJECT_H

#include "field.h"
#include <QSet>
#include <QSize>

namespace tron {
namespace data {

struct InitObject {
    QSize fieldSize;
    QSet<QString> snakeNames;
};

}
}


#endif // INITOBJECT_H
