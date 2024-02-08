#include "wall.h"
#include <cmath>

Wall::Wall(qreal x1pos, qreal y1pos, qreal x2pos, qreal y2pos) {
    x1 = x1pos;
    y1 = y1pos;
    x2 = x2pos;
    y2 = y2pos;
    angle = atan2(y2-y1, x2-x1); //Calculate angle in radians

}
