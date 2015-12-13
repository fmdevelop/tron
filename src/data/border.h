#ifndef BORDER_H
#define BORDER_H


#include "data.h"
#include <QColor>

namespace tron {
namespace data {

struct Border {
    QColor color;
    uint offset;
};

}
}

#endif // BORDER_H
