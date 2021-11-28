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
    for (size_t i = 0; i < CIRCULAR_TRAJ_NB_POINTS; i++) {
        theta = mapValue(i, 0, CIRCULAR_TRAJ_NB_POINTS, 0, 2 * M_PI);
        new_point.setX(center_x + (CIRCULAR_TRAJ_RADIUS * cos(theta)));
        new_point.setY(center_y + (CIRCULAR_TRAJ_RADIUS * sin(theta)));
        traj.push_back(new_point);
    }
    traj.push_back(traj.at(0));
    this->circular_traj = Trajectory(traj);
}

void APP::removePlaneFrom(Plane* plane, vector<Plane*>& list) {
    vector<Plane *>::iterator it = list.begin();
    while (it != list.end()) {
        if (*it == plane) break;
        else it++;
    }
    list.erase(it);
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
    this->removePlaneFrom(p, this->coming_planes);

    // Add the plane to waiting planes
    Trajectory circular_t(this->circular_traj);
    circular_t.setAltitude(CIRCULAR_TRAJ_ALTITUDE_MIN + (CIRCULAR_TRAJ_ALTITUDE_STEP * this->waiting_planes.size()));
    this->waiting_planes.push_back(p);
    p->start(circular_t);
}

void APP::landPriorityPlane() {
    // Choose the plane to land
    Plane* to_land = this->waiting_planes.at(0);

    // Check if the plane started the circular trajectory
    if (!to_land->isTrajectoryStarted()) return;
    cout << " -- Land plane " << to_land->getName() << " --" << endl << endl;

    // Choose the stoping point before landing
    Location runway_start = this->linked_twr->getLandingTrajectory().getPointAt(0);
    size_t dest_pos = this->circular_traj.getNearPointPos(runway_start);
    
    // Update the landing plane
    to_land->stopAt(dest_pos);
    this->landing_plane = to_land;
    this->linked_twr->toggleIsRunwayUsed();
    this->linked_twr->setPlaneInRunway(this->landing_plane);
}

void APP::startLanding() {
    // Remove the landing plane from the waiting planes list
    this->removePlaneFrom(this->landing_plane, waiting_planes);
    this->linked_twr->landPlane(this->landing_plane);
    this->landing_plane = NULL;
}