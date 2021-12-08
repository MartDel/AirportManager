#include "Plane.hpp"

/* -------------------------------------------------------------------------- */
/*                             Location functions                             */
/* -------------------------------------------------------------------------- */

/* --------------------------- Getters and setters -------------------------- */

void Location::setLocation(const float &_x, const float &_y, const float &_z) {
    this->x = _x;
    this->y = _y;
    this->z = _z;
}

/* ----------------------------- Public methods ----------------------------- */

Vector2f Location::toVector() const {
    Vector2f v = getFrameStartPoint(this->ref_frame);

    if (this->ref_frame == ReferenceFrame::CCR) {
        v.x += WINDOW_WIDTH * (this->x / WINDOW_CCR_REAL_WIDTH);
        v.y += WINDOW_HEIGHT * (this->y / WINDOW_CCR_REAL_HEIGHT);
    } else {
        v.x += WINDOW_WIDTH * (this->x / WINDOW_REAL_WIDTH);
        v.y += WINDOW_HEIGHT * (this->y / WINDOW_REAL_HEIGHT);
    }

    return v;
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
        theta = atan(xy_diff / z_diff);
    } else if(l.getZ() > this->z) {
        // pi/2 < theta <= pi
        theta = atan(z_diff / xy_diff) + (M_PI / 2);
    } else {
        // theta = pi/2
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

/* -------------------------------- Operators ------------------------------- */

bool Location::operator==(const Location &l) const {
    // cout.precision(10);
    return this->x == l.getX() && this->y == l.getY() && round(this->z) == l.getZ();
}

bool Location::operator!=(const Location &l) const {
    return !(*this == l);
}

bool Location::operator<(const Location& l) const {
    Location o;
    return this->get3dDistanceTo(o) < l.get3dDistanceTo(o);
}

ostream& operator<<(ostream& stream, const Location& l) {
    stream << "(" << l.getX() << ", " << l.getY() << ", " << l.getZ() << ")";
    if (l.getSpeed() != -1)
        stream << " [" << l.getSpeed() << "m/s]";
    return stream;
}

/* -------------------------------------------------------------------------- */
/*                            Trajectory functions                            */
/* -------------------------------------------------------------------------- */

/* ----------------------------- Private methods ---------------------------- */

size_t Trajectory::getPointPos(const Location &l) const {
    size_t i(0);
    Location current;
    do {
        current = points.at(i);
        i++;
    } while(current != l && i < points.size());
    return --i;
}


/* --------------------------- Getters and setters -------------------------- */

bool Trajectory::isCyclic() const {
    if (this->points.size() < 2) return false;
    else return this->points.at(0) == this->getLastPoint();
}

void Trajectory::setAltitude(const float& altitude) {
    if (!this->isCyclic()) return;
    for (size_t i = 0; i < this->points.size(); i++) {
        this->points.at(i).setZ(altitude);
    }
}

size_t Trajectory::getNearPointPos(const Location& from) {
    map<float, Location> points_away;
    for (auto& current_point : this->points) {
        points_away.insert(pair<float, Location>(from.get3dDistanceTo(current_point), current_point));
    }
    return this->getPointPos(points_away.begin()->second);
}

/* ----------------------------- Public methods ----------------------------- */

Location Trajectory::getNextLocation(const Location &from, const float &speed, const bool& verbose) {
    if (verbose) cout << "-- Next location --" << endl;

    Location next, *to;
    size_t next_point_pos(0);

    // Get the next point to reach
    if (this->reached_point != NULL)
        next_point_pos = (this->getPointPos(*this->reached_point) + 1) % this->points.size();
    else if (this->isCyclic())
        next_point_pos = this->getNearPointPos(from);

    // Choose a direction
    to = &this->points.at(next_point_pos);
    if (verbose) {
        cout << "next point pos : " << next_point_pos << endl;
        cout << "next point : " << *to << endl;
    }

    // Checking the different cases
    float d_between_next = from.get3dDistanceTo(*to);
    if (d_between_next <= speed) {
        if (next_point_pos == this->points.size() - 1) {
            // Last trajectory point reached
            if (this->isCyclic()) {
                // Restart from the first point
                this->reached_point = &this->points.at(0);
                if (verbose) cout << endl << "Restart (reached_point : " << *this->reached_point << ")" << endl;
                next = this->getNextLocation(*this->reached_point, speed - d_between_next);
                next.setSpeed(this->reached_point->getSpeed() == -1 ? speed : this->reached_point->getSpeed());
            } else {
                // Stop here
                if (verbose) cout << endl << "Stop" << endl;
                this->reached_point = to;
                next = *to;
            }
        } else {
            // Reach the next point
            if (verbose) cout << endl << "New point : " << *to << endl;
            this->reached_point = to;
            next = this->getNextLocation(*this->reached_point, speed - d_between_next);
            next.setSpeed(to->getSpeed() == -1 ? speed : to->getSpeed());
        }
    } else {
        // Calcul the next location
        float
            phi = from.getPhiTo(*to),
            theta = from.getThetaTo(*to),
            theta_z = theta * (float(M_PI) / 180);
        if (verbose)
            cout << "phi = " << phi << " theta = " << theta << endl;
        next.setX(from.getX() + (speed * sin(theta) * cos(phi)));
        next.setY(from.getY() + (speed * sin(theta) * sin(phi)));
        next.setZ(from.getZ() + (speed * (roundWithPrecision(theta) == roundWithPrecision(float(M_PI)/2) ? 0.f : cos(theta))));
        next.setPhi(phi);
        next.setTheta(theta);
        next.setSpeed(speed);
    }

    if (this->cutting_pos >= 0) {
        this->cutTrajectory(this->cutting_pos);
    }

    if (verbose) {
        cout << next << endl;
        cout << "------" << endl;
    }

    next.setRefFrame(from.getRefFrame());
    return next;
}


void Trajectory::cutTrajectory(const size_t& pos) {
    // Checking pos
    this->setCuttingPos(pos);
    if (this->cutting_pos < 0 && this->cutting_pos >= this->points.size()) return;

    // Init the iterator
    vector<Location>::iterator it = this->points.begin() + this->cutting_pos;
    if (*it == *this->reached_point) return;

    // Erase reached points
    it++;
    while (it != this->points.end() && *it != *this->reached_point) {
        this->points.erase(it);
        if (this->getPointPos(*this->reached_point) >= this->cutting_pos) {
            if (it != this->points.end())
                this->reached_point--;
            else
                this->reached_point = &this->points.at(0);
        }
    }
}

/* -------------------------------- Operators ------------------------------- */

ostream& operator<<(ostream& stream, const Trajectory& traj) {
    vector<Location> points = traj.getPoints();
    Location* reached_point = traj.getReachedPoint();
    for (Location& point : points) {
        cout << " - " << point << (reached_point != NULL && point == *reached_point ? " <-" : "") << endl;
    }
    return stream;
}

/* -------------------------------------------------------------------------- */
/*                               Plane functions                              */
/* -------------------------------------------------------------------------- */

Plane::Plane(const Location& _spawn)
: location(_spawn), destination(_spawn), speed(0), fuel(100), consumption(DEFAULT_CONSUMPTION) {
    ifstream i(COMPAGNIES_FILE);
    if (i.is_open()) {
        json j;
        i >> j;
        int random_id = rand()%j.size();
        this->name = j[random_id]["IATA"] + to_string(rand()%1000);
        this->company = j[random_id]["name"];
        i.close();
    } else {
        this->name = "ER" + to_string(rand()%100);
        this->company = "Martin Airlines";
    }
    updateLogs("🚁 Plane "+this->name + " created");
    // Set graphical plane
    this->graphical_plane = CircleShape(PLANE_CIRCLE_RADIUS);
    this->graphical_plane.setFillColor(PLANE_COLOR_DEFAULT);

    // Set altitude and name label
    Text altitude_label, name_label;
    altitude_label.setFont(Plane::default_font);
    altitude_label.setCharacterSize(ALTITUDE_LABEL_SIZE);
    altitude_label.setFillColor(Color::Black);
    this->altitude_label = altitude_label;
    name_label.setFont(Plane::default_font);
    name_label.setCharacterSize(ALTITUDE_LABEL_SIZE);
    name_label.setFillColor(Color::Black);
    this->name_label = name_label;
}

/* --------------------------- Getters and setters -------------------------- */

Text Plane::getAltitudeLabel() {
    Vector2f location_v = this->location.toVector();
    this->altitude_label.setString(to_string(int(round(this->location.getZ()))) + "m");
    this->altitude_label.setPosition(Vector2f(location_v.x + ALTITUDE_LABEL_X, location_v.y - ALTITUDE_LABEL_Y));
    return this->altitude_label;
}

Text Plane::getNameLabel() {
    Vector2f location_v = this->location.toVector();
    this->name_label.setString(this->name);
    this->name_label.setPosition(Vector2f(location_v.x + NAME_LABEL_X, location_v.y - NAME_LABEL_Y));
    return this->name_label;
}

CircleShape Plane::toSFML() {
    this->graphical_plane.setPosition(this->location.toVector());
    return this->graphical_plane;
}

void Plane::setLocation(const Location& l) {
    this->location = l;

    // Update the plane speed
    if (l.getSpeed() != -1)
        this->speed = l.getSpeed();
}

/* ----------------------------- Public methods ----------------------------- */

void Plane::start(const Trajectory& traj) {
    this->trajectory = traj;
    this->destination = trajectory.getLastPoint();
    float new_speed = trajectory.getPointAt(0).getSpeed();
    if (new_speed != -1) this->speed = new_speed;
}

void Plane::updateLocation() {
    this->setLocation(this->trajectory.getNextLocation(this->location, this->speed));
    this->fuel -= this->consumption;
    this->destination = this->trajectory.getLastPoint();
}


void Plane::stopAt(const size_t& stop_pos) {
    this->trajectory.cutTrajectory(stop_pos);
}

/* -------------------------------- Operators ------------------------------- */

ostream &operator<<(ostream &stream, const Plane &plane) {
    stream << plane.getName() << " :" << endl;
    stream << "    " << plane.getLocation() << " -> " << plane.getDestination() << endl;
    stream << "    Speed: " << plane.getSpeed() << " m/s" << endl;
    stream << "    Fuel: " << plane.getFuel() << " % Consumption: " << plane.getConsumption() << " %/s" << endl;
    return stream;
}

/* ---------------------- Static attributes and methods --------------------- */

void Plane::world(vector<Plane *> &planes, bool &stop_prgm) {
    chrono::milliseconds interval(WORLD_INTERVAL);

    while (!stop_prgm) {
        for (auto &current_plane : planes) {
            if (!current_plane->isDestinationReached()) {
                // Update plane location
                Plane::cout_lock.lock();
                current_plane->updateLocation();
                Plane::cout_lock.unlock();
            }
        }

        // Add 1s timeout
        this_thread::sleep_for(interval);
    }
    Plane::cout_lock.lock();
    cout << "World stopped" << endl;
    updateLogs("World stopped");
    Plane::cout_lock.unlock();
}

mutex Plane::cout_lock;
Font Plane::default_font = Font();

bool Plane::operator==(const Plane& p) const {
    return this->getLocation() == p.getLocation();
}
