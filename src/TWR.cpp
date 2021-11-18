#include "TWR.hpp"

/* -------------------------------------------------------------------------- */
/*                                TWR functions                               */
/* -------------------------------------------------------------------------- */

TWR::TWR(
    const vector<Location>& _parking_spots,
    const Location& runway_start,
    const Location& runway_end,
    const Location& parking_entrance,
    const Location& perimeter_limit
) : parking_spots(_parking_spots), is_runway_used(false) {
    // Create take off trajectory
    vector<Location> takeoff_vector;
    takeoff_vector.push_back(parking_entrance);
    takeoff_vector.push_back(runway_start);
    takeoff_vector.push_back(runway_end);
    takeoff_vector.push_back(perimeter_limit);
    this->takeoff = Trajectory(takeoff_vector);

    // Create landing trajectory
    vector<Location> landing_vector;
    landing_vector.push_back(perimeter_limit);
    landing_vector.push_back(runway_end);
    landing_vector.push_back(runway_start);
    landing_vector.push_back(parking_entrance);
    this->landing = Trajectory(landing_vector);
}

void TWR::takeOffPlane() {
    Plane* plane = this->parking.front();
    cout << " -- Starting take off for " << plane->getName() << " --" << endl;
    plane->setState(1);
    plane->start(this->takeoff);
    this->parking.pop();
}

Plane* TWR::spawnPlane(const string& name){
    if (this->parking.size() < NB_PARKING_SPOTS) {
        size_t parking_spot_id = this->parking.size();
        Plane* p = new Plane(name, parking_spots.at(parking_spot_id), parking_spot_id);
        this->parking.push(p);
        return p;
    } else return NULL;
}
