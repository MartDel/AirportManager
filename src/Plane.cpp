#include "Plane.hpp"

void Location::setLocation(const float &_x, const float &_y, const float &_z) {
    this->x = _x;
    this->y = _y;
    this->z = _z;
}

float Location::getPhiTo(const Location &l) const {
    float
        x_diff(abs(l.getX() - this->x)),
        y_diff(abs(l.getY() - this->y)),
        phi(0);
    if (l.getX() < this->x && l.getY() <= this->y) {
        // pi <= phi < 3pi/2
        phi = atan(y_diff / x_diff) + M_PI;
    } else if (l.getX() >= this->x && l.getY() < this->y) {
        // 3pi/2 <= phi < 2pi
        phi = atan(x_diff / y_diff) + ((3 * M_PI) / 2);
    } else if (l.getX() > this->x && l.getY() >= this->y) {
        // 0 <= phi < pi/2
        phi = atan(y_diff / x_diff);
    } else if (l.getX() <= this->x && l.getY() > this->y) {
        // pi/2 <= phi < pi
        phi = atan(x_diff / y_diff) + (M_PI / 2);
    }
    return phi;
}

float Location::getThetaTo(const Location &l) const {
    float
        z_diff = abs(l.getZ() - this->z),
        xy_diff = this->get2dDistanceTo(l),
        theta(0);
    if (l.getZ() < this->z) {
        // 0 <= theta < pi/2
        // cout << "0 <= theta < pi/2 " << z_diff << " " << xy_diff << endl;
        theta = atan(xy_diff / z_diff);
    } else if(l.getZ() > this->z) {
        // pi/2 < theta <= pi
        // cout << "pi/2 < theta <= pi" << endl;
        theta = atan(xy_diff / z_diff) + (M_PI / 2);
    } else {
        // theta = pi/2
        // cout << "theta = pi/2" << endl;
        theta = M_PI/2;
    }
    return theta;
}

float Location::get2dDistanceTo(const Location &l) const {
    return sqrt(pow(l.getX() - this->x, 2) + pow(l.getY() - this->y, 2));
}

float Location::get3dDistanceTo(const Location &l) const {
    return sqrt(pow(l.getX() - this->x, 2) + pow(l.getY() - this->y, 2) + pow(l.getZ() - this->z, 2));
}

bool Location::operator==(const Location &l) const {
    // cout.precision(10);
    return this->x == l.getX() && this->y == l.getY() && round(this->z) == l.getZ();
}

ostream& operator<<(ostream& stream, const Location& l) {
    stream << "(" << l.getX() << ", " << l.getY() << ", " << l.getZ() << ")";
    return stream;
}

Location Trajectory::getNextLocation(const Location &from, const float &speed) {
    Location next;
    if (reached_point == NULL) {
        // Try to reached the first point
        Location to = points.at(0);
        float phi = from.getPhiTo(to);
        float theta = M_PI - from.getThetaTo(to);
        float distance_between = from.get3dDistanceTo(to);
        float step = speed;
        if (distance_between <= speed) {
            step = distance_between;
            reached_point = &points.at(0);
        }
        next.setX(from.getX() + (step * sin(theta) * cos(phi)));
        next.setY(from.getY() + (step * sin(theta) * sin(phi)));
        next.setZ(from.getZ() + (step * (theta == float(M_PI)/2 ? 0.f : cos(theta))));
        cout << next << " phi = " << phi << " theta = " << theta << " step = " << step << endl;
    }
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
    this->location = this->trajectory.getNextLocation(this->location, this->speed);
    if (this->location == this->trajectory.getPoint(0))
        this->location = this->destination;
    cout << " -> " << this->location << endl;
}
