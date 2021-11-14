#include "Plane.hpp"

int main(void) {
    Location spawn(0, 0, 0), start(3, 3, 0), destination(10, 10, 0);

    vector<Location> traj_vector;
    traj_vector.push_back(start);
    traj_vector.push_back(destination);
    Trajectory traj(traj_vector);

    Plane p1("test", spawn, traj);
    p1.start();

    while (!p1.isDestinationReached()) {
        p1.updateLocation();
    }

    // cout << spawn.getThetaTo(start) << endl;

    return 0;
}
