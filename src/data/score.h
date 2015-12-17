#ifndef SCORE_H
#define SCORE_H

#include <QtGlobal>
#include "../constants.h"

namespace tron {
namespace data {

struct Score {
    uint numberOfCollissions;
    uint numberOfKills;
    uint numberOfEscapes;
    uint numberOfSurvivals;

    inline uint getScore() const {
        return numberOfCollissions * tron::SCORE_COLLISSION
                + numberOfKills * tron::SCORE_KILL
                + numberOfEscapes * tron::SCORE_ESCAPE
                + numberOfSurvivals * tron::SCORE_SURVIVAL;
    }
};

}
}

#endif // SCORE_H
