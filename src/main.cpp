#include "TWR.hpp"

#define WORLD_INTERVAL 1000
#define AIRPORTS_INTERVAL 250

mutex cout_lock;

void world(vector<Plane*>& planes, bool& stop_prgm) {
    chrono::milliseconds interval(WORLD_INTERVAL);
    while (!stop_prgm) {
        for (auto& current_plane : planes) {
            if (!current_plane->isDestinationReached())
                current_plane->updateLocation();
            else {
                switch (current_plane->getState()) {
                    case 1:
                        // The plane has token off
                        stop_prgm = true;
                        current_plane->setState(2);
                        break;
                }
            }
            cout_lock.lock();
            cout << *current_plane << endl;
            cout_lock.unlock();
        }
        this_thread::sleep_for(interval);
    }
    cout_lock.lock();
    cout << "World stop" << endl;
    cout_lock.unlock();
}

void twr_control(TWR& twr, bool& stop_prgm) {
    chrono::milliseconds interval(AIRPORTS_INTERVAL);
    while (!stop_prgm) {
        if (!twr.isRunwayUsed() && !twr.isParkingEmpty()) {
            cout_lock.lock();
            twr.takeOffPlane();
            cout_lock.unlock();
        }
        this_thread::sleep_for(interval);
    }
    cout_lock.lock();
    cout << "Airport stop" << endl;
    cout_lock.unlock();
}

int main(void) {
    bool stop_prgm(false);

    // Create first TWR
    vector<Location> twr1_parking;
    for (size_t i = 0; i < NB_PARKING_SPOTS; i++)
        twr1_parking.push_back(Location(i * 10, 0, 0, 0));
    TWR twr1(
        twr1_parking,
        Location(-20, 10, 0, 5),
        Location(60, 70, 0, 100),
        Location(0, 10, 0, 5),
        Location(300, 300, 200, 250)
    );

    // Create planes
    vector<Plane*> planes;
    Plane* p = twr1.spawnPlane("test1");
    if (p == NULL) {
        cout << "No more parking spot in Airport1..." << endl;
        return -1;
    }
    planes.push_back(p);

    thread world_thread(world, ref(planes), ref(stop_prgm));
    thread twr_thread(twr_control, ref(twr1), ref(stop_prgm));

    char input;
    while (!stop_prgm) {
        cin >> input;
        if (input == 'q')
            stop_prgm = true;
    }

    if(world_thread.joinable()) world_thread.join();
    if(twr_thread.joinable()) twr_thread.join();

    return 0;
}
