#include "TWR.hpp"

// The circular trajectory config
#define CIRCULAR_TRAJ_RADIUS 800
#define CIRCULAR_TRAJ_NB_POINTS 10
#define CIRCULAR_TRAJ_SPEED 100
#define CIRCULAR_TRAJ_ALTITUDE_MIN 500
#define CIRCULAR_TRAJ_ALTITUDE_STEP 500

/**
 * @brief A APP manage airport perimeter and landing.
 * It manage all of waiting planes for landing.
 */
class APP {
    private:
        vector<Plane *> waiting_planes;
        vector<Plane *> coming_planes;
        TWR* linked_twr;
        Location perimeter_entrance, airport_center;
        Trajectory circular_traj; // Circular trajectory to lead waiting planes

    public:
        APP(TWR* _twr, const Location& _perimeter_entrance, const Location& center);
        TWR* getTWR() const { return this->linked_twr; }
        Trajectory getCircularTrajectory() const { return this->circular_traj; }

        /**
         * @brief Spawn a plane to the perimeter entrance.
         * @param name The plane name
         * @return Plane* The spawned plane
         */
        Plane* spawnPlane(const string& name);

        /**
         * @brief Get all arrived planes in the coming planes array
         * @return vector<Plane*> Arrived planes
         */
        vector<Plane*> getArrivedPlanes() const;
};