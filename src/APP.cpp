#include "APP.hpp"
#include <cmath>

/* -------------------------------------------------------------------------- */
/*                                APP functions                               */
/* -------------------------------------------------------------------------- */

APP::APP(TWR *_twr, const Location &_perimeter_entrance, const Location& center)
: linked_twr(_twr), perimeter_entrance(_perimeter_entrance), airport_center(center) {
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
    for (size_t i = 0; i < CIRCULAR_TRAJ_NB_POINTS; i++) {
        theta = mapValue(i, 0, 20, 0, (5 * M_PI) / 2);
        new_point.setX(center_x + (CIRCULAR_TRAJ_RADIUS * cos(theta)));
        new_point.setY(center_y + (CIRCULAR_TRAJ_RADIUS * sin(theta)));
        traj.push_back(new_point);
    }
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