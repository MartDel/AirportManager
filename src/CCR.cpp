#include "CCR.hpp"

/* -------------------------------------------------------------------------- */
/*                                CCR functions                               */
/* -------------------------------------------------------------------------- */

CCR::CCR() : ccr_thread(NULL) {
    vector<APP*> tmp_airports;

    ifstream file_in(AIRPORTS_FILE);
    if (!file_in.is_open()) {
        cerr << "Cannot open file : " << AIRPORTS_FILE << endl;
        throw runtime_error("Cannot open file");
    } else {
        json j;
        file_in >> j;
        for (size_t i = 0; i < j.size(); i++) {
            cout << "Generating the airport : " << j[i]["name"] << endl;
            tmp_airports.push_back(new APP(j[i], getFrameFromId(i)));
        }
    }

    this->airports = tmp_airports;
}

CCR::~CCR() {
    if (this->ccr_thread != NULL) {
        if (this->ccr_thread->joinable())
            this->ccr_thread->join();
        delete this->ccr_thread;
    }

    for (auto& airport : this->airports)
        delete airport;
}

/* ----------------------------- Private methods ---------------------------- */

size_t CCR::getAppId(APP* app) const {
    size_t i(0);
    APP* current;
    do {
        current = this->airports.at(i);
        i++;
    } while (current != app && i < this->airports.size());
    return --i;
}

/* --------------------------- Getters and setters -------------------------- */

void CCR::setThread(bool &stop_prgm) {
    thread *tmp = new thread(CCR::threadCCR, ref(*this), ref(stop_prgm));
    this->ccr_thread = tmp;
}

/* ----------------------------- Public methods ----------------------------- */

void CCR::moveToRandomAirport(Plane *p, APP* from) {
    // Choose a random airport
    size_t app_id, from_id = this->getAppId(from);
    do {
        app_id = rand() % this->airports.size();
    } while (app_id == from_id);
    APP* to = this->airports.at(app_id);

    // Update the travelling plane
    cout << to->getName() << endl;
    updateLogs(p->getName() + " is going to " + to->getName());
}

/* ---------------------- Static attributes and methods --------------------- */

void CCR::threadCCR(CCR &ccr, bool &stop_prgm) {
    chrono::milliseconds interval(CCR_INTERVAL);
    vector<APP*> airports = ccr.getAirports();

    while (!stop_prgm) {

        // Check all airports
        for (auto& app : airports) {
            Plane* exiting_plane = app->getExitingPlane();

            if (exiting_plane != NULL && exiting_plane->isDestinationReached()) {
                // A plane exited its airport
                ccr.moveToRandomAirport(exiting_plane, app);
                app->setExitingPlane(NULL);
            }

        }

        // Add 0.25s timeout
        this_thread::sleep_for(interval);
    }

    Plane::cout_lock.lock();
    cout << "The CCR stopped" << endl;
    updateLogs("The CCR stopped");
    Plane::cout_lock.unlock();
}