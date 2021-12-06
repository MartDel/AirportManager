#include "TWR.hpp"

/* -------------------------------------------------------------------------- */
/*                                TWR functions                               */
/* -------------------------------------------------------------------------- */

TWR::TWR(
    const vector<Location>& _parking_spots,
    Location& runway_start,
    Location& runway_end,
    Location& parking_entrance,
    Location& perimeter_limit,
    const string& trigramme
) : is_runway_used(false), app_trigramme(trigramme) {

    // Create take off trajectory
    vector<Location> takeoff_vector;
    parking_entrance.setSpeed(TAKEOFF_PARKING_ENTRANCE_SPEED);
    takeoff_vector.push_back(parking_entrance);
    runway_start.setSpeed(TAKEOFF_RUNWAY_START_SPEED);
    takeoff_vector.push_back(runway_start);
    runway_end.setSpeed(TAKEOFF_RUNWAY_END_SPEED);
    takeoff_vector.push_back(runway_end);
    takeoff_vector.push_back(perimeter_limit);
    this->takeoff = Trajectory(takeoff_vector);

    // Create landing trajectory
    vector<Location> landing_vector;
    runway_start.setSpeed(LANDING_RUNWAY_START_SPEED);
    landing_vector.push_back(runway_start);
    runway_end.setSpeed(LANDING_RUNWAY_END_SPEED);
    landing_vector.push_back(runway_end);
    parking_entrance.setSpeed(LANDING_PARKING_ENTRANCE_SPEED);
    landing_vector.push_back(parking_entrance);
    this->landing = Trajectory(landing_vector);

    // Create parking spots map
    map<Location, bool> tmp_spots;
    for (const Location& current_spot : _parking_spots) {
        tmp_spots.insert(pair<Location, bool>(current_spot, false));
    }
    this->parking_spots = tmp_spots;
}

/* ----------------------------- Private methods ---------------------------- */

std::_Rb_tree_const_iterator<std::pair<const Location, bool>> TWR::getFreeParkingSpot() const {
    auto spot = std::find_if(
        this->parking_spots.begin(),
        this->parking_spots.end(),
        [](const std::pair<Location, bool> &spot) -> bool
        { return !spot.second; }
    );
    return spot != end(this->parking_spots) ? (spot) : this->parking_spots.end();
}

/* ----------------------------- Public methods ----------------------------- */

void TWR::landPlane(Plane* plane) {
    // Choose the parking spot
    auto free_spot = this->getFreeParkingSpot();
    if (free_spot == this->parking_spots.end()) {
        cerr << "[" + this->app_trigramme + "] Trying to land plane but no free parking spot is available." << endl;
        updateLogs("[" + this->app_trigramme + "] Trying to land plane but no free parking spot is available.");
        return;
    }
    Location spot = (*free_spot).first;

    // Add the free spot to the landing trajectory
    Trajectory landing_copy(this->landing);
    landing_copy.addPoint(spot);
    plane->start(landing_copy);
}

void TWR::endLanding(Plane* p) {
    this->parking.push(p);
    this->parking_spots.at(p->getLocation()) = true;
    this->toggleIsRunwayUsed();
}

Plane* TWR::takeOffPlane() {
    Plane* plane = this->parking.front();
    this->parking_spots.at(plane->getLocation()) = false;
    plane->start(this->takeoff);
    this->parking.pop();
    this->toggleIsRunwayUsed();
    updateLogs("[" + this->app_trigramme + "] Starting take off for " + plane->getName() + " (runway is " + (this->is_runway_used ? "used)" : "free)"));
    return plane;
}

Plane* TWR::spawnPlane(){
    if (this->parking.size() < this->parking_spots.size()) {
        auto parking_spot_it = this->getFreeParkingSpot();
        if (parking_spot_it == this->parking_spots.end()) return NULL;
        Location parking_spot = (*parking_spot_it).first;
        this->parking_spots.at(parking_spot) = true;
        Plane* p = new Plane(parking_spot);
        this->parking.push(p);
        return p;
    } else return NULL;
}
