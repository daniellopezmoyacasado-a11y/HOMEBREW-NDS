#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

inline bool moveTowards(int& x, int& y, int targetX, int targetY, int speed)
{
    if (x < targetX) {
        x += speed;
        if (x > targetX) x = targetX;
    } else if (x > targetX) {
        x -= speed;
        if (x < targetX) x = targetX;
    }

    if (y < targetY) {
        y += speed;
        if (y > targetY) y = targetY;
    } else if (y > targetY) {
        y -= speed;
        if (y < targetY) y = targetY;
    }

    return x == targetX && y == targetY;
}

#endif