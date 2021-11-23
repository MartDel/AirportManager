#include "APP.hpp"
#include <cmath>

/* -------------------------------------------------------------------------- */
/*                                APP functions                               */
/* -------------------------------------------------------------------------- */

APP::APP(TWR *_twr, const Location &_perimeter_entrance, const Location& center)
: linked_twr(_twr), perimeter_entrance(_perimeter_entrance), airport_center(center), landing_plane(NULL) {
    // Init waiting and comming planes
    vector<Plane*> tmp1, tmp2;
    this->waiting_planes = tmp1;
    this->coming_planes = tmp2;

    // Build circular trajectory
    vector<Location> traj;
    float theta,
        center_x = this->airport_center.getX(),
        center_y = this->airport_center.getY();
    Location new_point;
    new_point.setZ(CIRCULAR_TRAJ_ALTITUDE_MIN);
    new_point.setSpeed(CIRCULAR_TRAJ_SPEED);
    cout << "Circular trajectory :" << endl;
    for (size_t i = 0; i < CIRCULAR_TRAJ_NB_POINTS; i++) {
        theta = mapValue(i, 0, CIRCULAR_TRAJ_NB_POINTS, 0, 2 * M_PI);
        cout << theta << endl;
        new_point.setX(center_x + (CIRCULAR_TRAJ_RADIUS * cos(theta)));
        new_point.setY(center_y + (CIRCULAR_TRAJ_RADIUS * sin(theta)));
        traj.push_back(new_point);
        cout << "   " << new_point << endl;
    }
    traj.push_back(traj.at(0));
    cout << endl;
    this->circular_traj = Trajectory(traj);
}

Plane* APP::spawnPlane(const string& name) {
    Plane* p = new Plane(name, this->perimeter_entrance);
    this->coming_planes.push_back(p);
    return p;
}

vector<Plane*> APP::getArrivedPlanes() const {
    vector<Plane*> arrived;
    for (const auto& comming_plane : this->coming_planes) {
        if (comming_plane->isDestinationReached())
            arrived.push_back(comming_plane);
    }
    return arrived;
}

void APP::askPlaneToWait(Plane* p) {
    // Remove the plane from the coming planes list
    vector<Plane *>::iterator it = this->coming_planes.begin();
    while (it != this->coming_planes.end()) {
        if (*it == p) break;
        else it++;
    }
    this->coming_planes.erase(it);

    // Add the plane to waiting planes
    Trajectory circular_t(this->circular_traj);
    circular_t.setAltitude(CIRCULAR_TRAJ_ALTITUDE_MIN + (CIRCULAR_TRAJ_ALTITUDE_STEP * this->waiting_planes.size()));
    this->waiting_planes.push_back(p);
    p->start(circular_t);
}

void APP::landPriorityPlane() {
    // Choose the plane to land
    Plane* to_land = this->waiting_planes.at(0);
    Location runway_start = this->linked_twr->getLandingTrajectory().getPointAt(0);
    Location dest = this->circular_traj.getPointAt(this->circular_traj.getNearPointPos(runway_start));
    to_land->setDestination(dest);
    this->landing_plane = to_land;
}