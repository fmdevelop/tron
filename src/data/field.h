#ifndef FIELD_H
#define FIELD_H

#include "data.h"
#include "border.h"

#include <QColor>
#include <QList>

namespace tron {
namespace data {

struct Field {
    int width;
    int height;
    QColor backgroundColor;
    QColor explosionColor;
    QList<Border> borders;
};

}
}

#endif // FIELD_H
