#include "Plane.hpp"

void Location::setLocation(const float &_x, const float &_y, const float &_z) {
    this->x = _x;
    this->y = _y;
    this->z = _z;
}

float Location::getThetaTo(const Location &l) {
    float
        x_diff = abs(l.getX() - this->x),
        y_diff = abs(l.getY() - this->y),
        theta = 0;
    if (l.getX() < this->x && l.getY() <= this->y) {
        // pi <= theta < 3pi/2
        cout << "pi <= theta < 3pi/2" << endl;
        theta = atan(y_diff / x_diff) + M_PI;
    } else if (l.getX() >= this->x && l.getY() < this->y) {
        // 3pi/2 <= theta < 2pi
        cout << "3pi/2 <= theta < 2pi" << endl;
        theta = atan(x_diff / y_diff) + ((3 * M_PI) / 2);
    } else if (l.getX() > this->x && l.getY() >= this->y) {
        // 0 <= theta < pi/2
        cout << "0 <= theta < pi/2" << endl;
        theta = atan(y_diff / x_diff);
    } else if (l.getX() <= this->x && l.getY() > this->y) {
        // pi/2 <= theta < pi
        cout << "pi/2 <= theta < pi" << endl;
        theta = atan(x_diff / y_diff) + (M_PI / 2);
    }
    return theta;
}

Location Trajectory::getLocationAt(const size_t &t) {
    Location next;
    return next;
}

Location Trajectory::getLocationFrom(const Location &from) {
    Location next;

    return next;
}

Plane::Plane(const string& _name, const Location& _spawn, const Trajectory& _traj)
: name(_name), location(_spawn), trajectory(_traj), speed(0), fuel(MAX_FUEL), consumption(DEFAULT_CONSUMPTION), reached_destination(true) {}

void Plane::start() {
    this->reached_destination = false;
    this->reached_first_traj_point = false;
    this->speed = 1;
}

void Plane::updateLocation() {

}
