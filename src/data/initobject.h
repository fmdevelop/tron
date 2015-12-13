#ifndef INITOBJECT_H
#define INITOBJECT_H

#include "field.h"
#include <QSet>

namespace tron {
namespace data {

struct InitObject {
    Field field;
    QSet<QString> snakeNames;
};

}
}


#endif // INITOBJECT_H
