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
    return M_PI - theta;
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

bool Location::operator!=(const Location &l) const {
    return !(*this == l);
}

ostream& operator<<(ostream& stream, const Location& l) {
    stream << "(" << l.getX() << ", " << l.getY() << ", " << l.getZ() << ")";
    return stream;
}

size_t Trajectory::getPointPos(const Location &l) const {
    size_t i(0);
    Location current;
    do {
        current = points.at(i);
        i++;
    } while(current != l && i < points.size());
    return --i;
}

Location Trajectory::getNextLocation(const Location &from, const float &speed, const bool& verbose) {
    Location next, *to;
    size_t next_point_pos(0);

    if (this->reached_point != NULL)
        next_point_pos = (this->getPointPos(*this->reached_point) + 1) % this->points.size();

    to = &this->points.at(next_point_pos);
    float d_between_next = from.get3dDistanceTo(*to);

    if (d_between_next <= speed) {
        // Reach the next point
        if (next_point_pos == this->points.size() - 1) {
            // Last trajectory point reached
            if (this->isCyclic()) {
                // Restart from the first point
                this->reached_point = &this->points.at(0);
                if (verbose) cout << endl << "Restart (reached_point : " << *this->reached_point << ")" << endl;
                next = this->getNextLocation(*this->reached_point, speed - d_between_next);
            } else {
                // Stop here
                if (verbose) cout << endl << "Stop" << endl;
                this->reached_point = to;
                next = *to;
            }
        } else {
            if (verbose) cout << endl << "New point : " << *to << endl;
            this->reached_point = to;
            next = this->getNextLocation(*this->reached_point, speed - d_between_next);
        }
    } else {
        // Calcul the next location
        float
            phi = from.getPhiTo(*to),
            theta = from.getThetaTo(*to);
        next.setX(from.getX() + (speed * sin(theta) * cos(phi)));
        next.setY(from.getY() + (speed * sin(theta) * sin(phi)));
        next.setZ(from.getZ() + (speed * (theta == float(M_PI)/2 ? 0.f : cos(theta))));
        if (verbose)
            cout << "phi = " << phi << " theta = " << theta << endl;
    }

    if (verbose) cout << next << endl;
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
    this->location = this->trajectory.getNextLocation(this->location, this->speed, true);
}
