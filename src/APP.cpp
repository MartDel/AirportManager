#include "APP.hpp"

/* -------------------------------------------------------------------------- */
/*                                APP functions                               */
/* -------------------------------------------------------------------------- */

APP::APP(const json &data, const ReferenceFrame &_ref = ReferenceFrame::NOT_PRINTED)
: landing_plane(NULL), parking_plane(NULL), exiting_plane(NULL), ref_frame(_ref), background(NULL), airport_thread(NULL) {
    this->name = data["name"];
    this->trigramme = data["trigramme"];

    // Set global location
    this->global_location = Location(data["global_location"]["x"], data["global_location"]["y"], PLANE_FLIGHT_ALTITUDE);
    this->global_location.setRefFrame(ReferenceFrame::CCR);
    this->global_point = CircleShape(AIRPORTS_RADIUS);
    this->global_point.setFillColor(AIRPORTS_COLOR_DEFAULT);
    this->global_point.setPosition(this->global_location.toVector());

    // Init waiting and coming planes
    vector<Plane *> tmp1, tmp2;
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
    new_point.setRefFrame(this->ref_frame);
    for (size_t i = 0; i < CIRCULAR_TRAJ_NB_POINTS; i++) {
        theta = mapValue(i, 0, CIRCULAR_TRAJ_NB_POINTS, 0, 2 * M_PI);
        new_point.setX(center_x + (float(data["circular_traj_radius"]) * cos(theta)));
        new_point.setY(center_y + (float(data["circular_traj_radius"]) * sin(theta)));
        traj.push_back(new_point);
        #ifdef DEBUG
        if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
            this->important_points.push_back(new_point);
        #endif
    }
    traj.push_back(traj.at(0));
    this->circular_traj = Trajectory(traj);
    
    vector<Location> tmp_park;
    for (size_t i = 0; i < data["parking"].size(); i++) {
        Location tmp_spot(data["parking"][i]["x"], data["parking"][i]["y"]);
        tmp_spot.setRefFrame(this->ref_frame);
        tmp_park.push_back(tmp_spot);
        #ifdef DEBUG
        if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
            this->important_points.push_back(tmp_spot);
        #endif
    }

    Location tmp_entrance(data["perimeter_entrance"]["x"], data["perimeter_entrance"]["y"], data["perimeter_entrance"]["z"]);
    tmp_entrance.setRefFrame(this->ref_frame);
    #ifdef DEBUG
    if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
        this->important_points.push_back(tmp_entrance);
    #endif

    Location runway_entrance(data["runway_start"]["x"], data["runway_start"]["y"], data["runway_start"]["z"]);
    runway_entrance.setRefFrame(this->ref_frame);
    #ifdef DEBUG
    if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
        this->important_points.push_back(runway_entrance);
    #endif

    Location runway_exit(data["runway_end"]["x"], data["runway_end"]["y"], data["runway_end"]["z"]);
    runway_exit.setRefFrame(this->ref_frame);
    #ifdef DEBUG
    if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
        this->important_points.push_back(runway_exit);
    #endif

    Location parking_entrance(data["parking_entrance"]["x"], data["parking_entrance"]["y"], data["parking_entrance"]["z"]);
    parking_entrance.setRefFrame(this->ref_frame);
    #ifdef DEBUG
    if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
        this->important_points.push_back(parking_entrance);
    #endif

    this->perimeter_entrance = Location(data["perimeter_entrance"]["x"], data["perimeter_entrance"]["y"], data["perimeter_entrance"]["z"]);
    this->perimeter_entrance.setRefFrame(this->ref_frame);

    Location perimeter_end(data["perimeter_exit"]["x"], data["perimeter_exit"]["y"], data["perimeter_exit"]["z"]);
    perimeter_end.setRefFrame(this->ref_frame);
    #ifdef DEBUG
    if (this->ref_frame != ReferenceFrame::NOT_PRINTED)
        this->important_points.push_back(perimeter_end);
    #endif

    linked_twr = new TWR(tmp_park, runway_entrance, runway_exit, parking_entrance, perimeter_end, this->trigramme);
    updateLogs("Generating TWR " + this->name);
}

APP::~APP() {
    if (this->airport_thread != NULL) {
        if (this->airport_thread->joinable())
            this->airport_thread->join();
        delete this->airport_thread;
    }

    delete this->linked_twr;
    delete this->background;
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

size_t APP::getWaitingPlanePos(Plane *waiting_plane) const {
    size_t i(0);
    Plane* current;
    do {
        current = this->waiting_planes.at(i);
        ++i;
    } while(current != waiting_plane && i < this->waiting_planes.size());
    return --i;
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
    Plane* p = this->linked_twr->spawnPlane();
    if (p != NULL) p->getLocation().setRefFrame(this->ref_frame);
    return p;
}

void APP::askPlaneToWait(Plane* p) {
    updateLogs("[" + this->trigramme + "] Move " + p->getName() + " to circular trajectory");

    // Remove the plane from the coming planes list
    if (p->getLocation().getRefFrame() == ReferenceFrame::CCR)
        p->setLocation(this->perimeter_entrance);
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

    // Choose the stopping point before landing
    Location runway_start = this->linked_twr->getLandingTrajectory().getPointAt(0);
    size_t dest_pos = this->circular_traj.getNearPointPos(runway_start);
    
    // Update the landing plane
    to_land->stopAt(dest_pos);
    this->landing_plane = to_land;
    this->linked_twr->toggleIsRunwayUsed();

    updateLogs("[" + this->trigramme + "] Land plane " + to_land->getName() + ". (runway is " + (this->linked_twr->isRunwayUsed() ? "used)" : "free)"));
}

void APP::startLanding() {
    updateLogs("[" + this->trigramme + "] " + this->landing_plane->getName() + " is reaching the runway.");
    this->removePlaneFrom(this->landing_plane, waiting_planes);

    // Update other planes altitude
    for (Plane *current_waiting_p : this->waiting_planes) {
        size_t pos = this->getWaitingPlanePos(current_waiting_p);
        float new_alt = CIRCULAR_TRAJ_ALTITUDE_MIN + (CIRCULAR_TRAJ_ALTITUDE_STEP * pos);
        current_waiting_p->setTrajectoryAltitude(new_alt);
        updateLogs(current_waiting_p->getName() + " (" + to_string(pos) + ") -> " + to_string(new_alt));
    }

    // Remove the landing plane from the waiting planes list
    this->linked_twr->landPlane(this->landing_plane);
    this->parking_plane = this->landing_plane;
    this->landing_plane = NULL;
}

void APP::endLanding(Plane* p) {
    this->linked_twr->endLanding(p);
    this->parking_plane = NULL;
    updateLogs("[" + this->trigramme + "] " + p->getName() + " is landed and parked ! (runway is " + (this->linked_twr->isRunwayUsed() ? "used)" : "free)"));
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
                Plane* taking_off_plane = twr->takeOffPlane();
                app.setExitingPlane(taking_off_plane);
                Plane::cout_lock.unlock();
            }

        } else {
            // The runway is used
            Plane *landing_plane = app.getLandingPlane();
            Plane *parking_plane = app.getParkingPlane();

            if (landing_plane != NULL && landing_plane->isDestinationReached()) {
                // The landing plane reached the runway start
                Plane::cout_lock.lock();
                app.startLanding();
                Plane::cout_lock.unlock();
            } else if (parking_plane != NULL && parking_plane->isDestinationReached()) {
                // The plane is landed and park
                Plane::cout_lock.lock();
                app.endLanding(parking_plane);
                Plane::cout_lock.unlock();
            }

        }

        // Add 0.25s timeout
        this_thread::sleep_for(interval);
    }

    Plane::cout_lock.lock();
    cout << "The airport " << app.getName() << " stopped" << endl;
    updateLogs("The airport " + app.getName() + " stopped");
    Plane::cout_lock.unlock();
}