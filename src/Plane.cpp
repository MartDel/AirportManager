#include "Plane.hpp"

void Location::setLocation(const float &_x, const float &_y, const float &_z) {
    this->x = _x;
    this->y = _y;
    this->z = _z;
}

float Location::getThetaTo(const Location &l) const {
    float
        x_diff = abs(l.getX() - this->x),
        y_diff = abs(l.getY() - this->y),
        theta = 0;
    if (l.getX() < this->x && l.getY() <= this->y) {
        // pi <= theta < 3pi/2
        // cout << "pi <= theta < 3pi/2" << endl;
        theta = atan(y_diff / x_diff) + M_PI;
    } else if (l.getX() >= this->x && l.getY() < this->y) {
        // 3pi/2 <= theta < 2pi
        // cout << "3pi/2 <= theta < 2pi" << endl;
        theta = atan(x_diff / y_diff) + ((3 * M_PI) / 2);
    } else if (l.getX() > this->x && l.getY() >= this->y) {
        // 0 <= theta < pi/2
        // cout << "0 <= theta < pi/2" << endl;
        theta = atan(y_diff / x_diff);
    } else if (l.getX() <= this->x && l.getY() > this->y) {
        // pi/2 <= theta < pi
        // cout << "pi/2 <= theta < pi" << endl;
        theta = atan(x_diff / y_diff) + (M_PI / 2);
    }
    return theta;
}

float Location::getDistanceTo(const Location &l) const {
    return sqrt(pow(l.getX() - this->x, 2) + pow(l.getY() - this->y, 2) + pow(l.getZ() - this->z, 2));
}

bool Location::operator==(const Location &l) const {
    return this->x == l.getX() && this->y == l.getY() && this->z == l.getZ();
}

ostream& operator<<(ostream& stream, const Location& l) {
    stream << "(" << l.getX() << ", " << l.getY() << ", " << l.getZ() << ")";
    return stream;
}

Location Trajectory::getLocationAt(const size_t &t) const {
    Location next;
    return next;
}

Location Trajectory::getLocationFrom(const Location &from, const float& speed) const {
    Location to = points.at(0);
    float theta = from.getThetaTo(to);
    float distance_between = from.getDistanceTo(to);
    float step = distance_between > speed ? speed : distance_between;
    Location next(from.getX() + (step * cos(theta)), from.getY() + (step * sin(theta)), from.getZ());
    cout << next << " theta = " << theta << " step = " << step << endl;
    return next;
}

Plane::Plane(const string& _name, const Location& _spawn, const Trajectory& _traj)
: name(_name), location(_spawn), trajectory(_traj), speed(0), fuel(MAX_FUEL), consumption(DEFAULT_CONSUMPTION) {}

void Plane::start() {
    this->destination = trajectory.getLastPoint();
    this->speed = 1;
    cout << "Start from : " << this->location << endl;
}

void Plane::updateLocation() {
    this->location = this->trajectory.getLocationFrom(this->location, this->speed);
    if (this->location == this->trajectory.getPoint(0))
        this->location = this->destination;
    cout << " -> " << this->location << endl;
}
