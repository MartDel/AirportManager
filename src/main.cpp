#include "Plane.hpp"

int main(void) {
    Location spawn(0, 0, 0), start(3, 3, -2), destination(10, 10, 0);

    vector<Location> traj_vector;
    traj_vector.push_back(start);
    traj_vector.push_back(destination);
    Trajectory traj(traj_vector);

    Plane p1("test", spawn, traj);
    p1.start();

    size_t i(0);
    while (!p1.isDestinationReached() && i < 10) {
        p1.updateLocation();
        i++;
    }

    // cout << spawn.getThetaTo(start) << endl;

    return 0;
}
