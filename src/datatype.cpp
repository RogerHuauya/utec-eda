#include "datatype.h"

std::ostream &operator<<(std::ostream &os, const RelationType &type) {
    switch (type) {
        case COINCIDENT:
            os << "COINCIDENT";
            break;
        case IN_FRONT:
            os << "IN_FRONT";
            break;
        case BEHIND:
            os << "BEHIND";
            break;
        case SPLIT:
            os << "SPLIT";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}