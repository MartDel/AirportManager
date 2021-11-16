#include "Plane.hpp"

int main(void) {
    Location spawn(0, 0, 0), p1(10, 0, 0), p2(0, 10, 0), p3(0, 0, 10);

    vector<Location> traj_vector;
    traj_vector.push_back(p1);
    traj_vector.push_back(p2);
    traj_vector.push_back(p3);
    traj_vector.push_back(p1);
    Trajectory traj(traj_vector);

    Plane plane1("test", spawn, traj);
    plane1.start();

    size_t i(0);
    while (!plane1.isDestinationReached() && i < 100) {
        cout << " -- " << i << " -- " << endl;
        plane1.updateLocation();
        i++;
    }

    // cout << spawn.getPhiTo(start) << endl;
    // cout << spawn.getThetaTo(start) << endl;

    return 0;
}
