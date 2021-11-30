#include "APP.hpp"

/* -------------------------------------------------------------------------- */
/*                                APP functions                               */
/* -------------------------------------------------------------------------- */

APP::APP(const json& data) : landing_plane(NULL) {
    this->name = data["name"];
    this->trigramme = data["trigramme"];
    

    // Init waiting and comming planes
    vector<Plane*> tmp1, tmp2;
    this->waiting_planes = tmp1;
    this->coming_planes = tmp2;

    // Build circular trajectory
    vector<Location> traj;
    float theta,
        center_x = data["airport_center"]["x"],
        center_y = data["airport_center"]["y"];
    Location new_point;
    new_point.setZ(CIRCULAR_TRAJ_ALTITUDE_MIN);
    new_point.setSpeed(CIRCULAR_TRAJ_SPEED);
    for (size_t i = 0; i < CIRCULAR_TRAJ_NB_POINTS; i++) {
        theta = mapValue(i, 0, CIRCULAR_TRAJ_NB_POINTS, 0, 2 * M_PI);
        new_point.setX(center_x + (float(data["circular_traj_radius"]) * cos(theta)));
        new_point.setY(center_y + (float(data["circular_traj_radius"]) * sin(theta)));
        traj.push_back(new_point);
    }
    traj.push_back(traj.at(0));
    this->circular_traj = Trajectory(traj);
    
    vector<Location> tmp_park;
    for (size_t i = 0; i < data["parking"].size(); i++) {
        tmp_park.push_back(Location(data["parking"][i]["x"], data["parking"][i]["y"]));
    }

    Location tmp_entrance(data["perimeter_entrance"]["x"], data["perimeter_entrance"]["y"], data["perimeter_entrance"]["z"]);

    Location runway_entrance(data["runway_start"]["x"], data["runway_start"]["y"], data["runway_start"]["z"]);
    Location runway_exit(data["runway_end"]["x"], data["runway_end"]["y"], data["runway_end"]["z"]);
    Location parking_entrance(data["parking_entrance"]["x"], data["parking_entrance"]["y"], data["parking_entrance"]["z"]);
    Location perimeter_end(data["perimeter_exit"]["x"], data["perimeter_exit"]["y"], data["perimeter_exit"]["z"]);

    linked_twr = new TWR(tmp_park, runway_entrance, runway_exit, parking_entrance, perimeter_end);
    updateLogs("Generating TWR " + this->name);
}

APP::~APP() {
    if (this->airport_thread->joinable()) this->airport_thread->join();
    delete this->airport_thread;
    delete this->linked_twr;
}

/* ----------------------------- Private methods ---------------------------- */

void APP::removePlaneFrom(Plane* plane, vector<Plane*>& list) {
    vector<Plane *>::iterator it = list.begin();
    while (it != list.end()) {
        if (*it == plane) break;
        else it++;
    }
    list.erase(it);
}

/* --------------------------- Getters and setters -------------------------- */

void APP::setThread(bool &stop_prgm) {
    thread* tmp = new thread(APP::airportControl, ref(*this), ref(stop_prgm));
    this->airport_thread = tmp;
}

vector<Plane*> APP::getArrivedPlanes() const {
    vector<Plane*> arrived;
    for (const auto& comming_plane : this->coming_planes) {
        if (comming_plane->isDestinationReached())
            arrived.push_back(comming_plane);
    }
    return arrived;
}

/* ----------------------------- Public methods ----------------------------- */

Plane* APP::spawnPlane() {
    Plane* p = new Plane(this->perimeter_entrance);
    this->coming_planes.push_back(p);
    return p;
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
    cout << "🛬 -- Land plane " << to_land->getName() << " --" << endl << endl;
    updateLogs("🛬 -- Land plane " + to_land->getName() + " --" );

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
    // Update other planes altitude
    for (Plane* current_waiting_p : this->waiting_planes) {
        float new_alt = current_waiting_p->getLocation().getZ() - CIRCULAR_TRAJ_ALTITUDE_STEP;
        current_waiting_p->setTrajectoryAltitude(new_alt);
    }

    // Remove the landing plane from the waiting planes list
    this->removePlaneFrom(this->landing_plane, waiting_planes);
    this->linked_twr->landPlane(this->landing_plane);
    this->landing_plane = NULL;
}

/* ---------------------- Static attributes and methods --------------------- */

void APP::airportControl(APP &app, bool &stop_prgm) {
    chrono::milliseconds interval(AIRPORTS_INTERVAL);
    TWR *twr = app.getTWR();

    while (!stop_prgm) {
        // Check arrived planes
        vector<Plane *> arrived_planes = app.getArrivedPlanes();
        for (auto &arrived_plane : arrived_planes) {
            Plane::cout_lock.lock();
            cout << " -- Move " << arrived_plane->getName() << " to circular trajectory --" << endl << endl;
            app.askPlaneToWait(arrived_plane);
            Plane::cout_lock.unlock();
        }

        if (!twr->isRunwayUsed()) {
            // The runway is free

            if (!twr->isParkingFull() && app.isPlaneWaiting()) {
                // Land the most important plane
                Plane::cout_lock.lock();
                app.landPriorityPlane();
                Plane::cout_lock.unlock();
            } else if (!twr->isParkingEmpty()) {
                // Take off a plane
                Plane::cout_lock.lock();
                twr->takeOffPlane();
                Plane::cout_lock.unlock();
            }
        } else {
            // The runway is used
            Plane *landing_plane = app.getLandingPlane();
            Plane *plane_using_runway = twr->getPlaneInRunway();

            if (landing_plane != NULL && landing_plane->isDestinationReached()) {
                // The landing plane reached the runway start
                Plane::cout_lock.lock();
                cout << " -- " << landing_plane->getName() << " is reaching the runway --" << endl;
                updateLogs(" -- " + landing_plane->getName() + " is reaching the runway --");
                app.startLanding();
                Plane::cout_lock.unlock();
            } else if (plane_using_runway != NULL && plane_using_runway->isDestinationReached()) {
                // The plane which is landing or taking off has finished
                Plane::cout_lock.lock();
                cout << " -- " << plane_using_runway->getName() << " has finished its landing or taking off --" << endl;
                updateLogs(" -- " + plane_using_runway->getName() + " has finished its landing or taking off --");
                twr->toggleIsRunwayUsed();
                Plane::cout_lock.unlock();
            }
        }

        // Add 0.25s timeout
        this_thread::sleep_for(interval);
    }

    Plane::cout_lock.lock();
    cout << "The airport " << app.getName() << " stopped" << endl;
    Plane::cout_lock.unlock();
}