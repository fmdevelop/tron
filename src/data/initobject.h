#ifndef INITOBJECT_H
#define INITOBJECT_H

#include "field.h"
#include <QSet>
#include <QSize>
#include <QMap>
#include <QMetaType>

namespace tron {
namespace data {

struct InitObject {
    QSize fieldSize;
    // id -> name
    QMap<uint, QString> snakeNames;
};

}
}

Q_DECLARE_METATYPE(tron::data::InitObject)

#endif // INITOBJECT_H
