#include "TWR.hpp"

// The circular trajectory config
#define CIRCULAR_TRAJ_RADIUS 800
#define CIRCULAR_TRAJ_NB_POINTS 20
#define CIRCULAR_TRAJ_SPEED 100
#define CIRCULAR_TRAJ_ALTITUDE_MIN 500
#define CIRCULAR_TRAJ_ALTITUDE_STEP 350

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
        Plane* landing_plane;

        /**
         * @brief Remove a plane from a vector
         * @param plane The plane to remove
         * @param list The list to remove the plane from
         */
        void removePlaneFrom(Plane* plane, vector<Plane*>& list);

    public:
        APP(TWR* _twr, const Location& _perimeter_entrance, const Location& center);
        TWR* getTWR() const { return this->linked_twr; }
        Trajectory getCircularTrajectory() const { return this->circular_traj; }
        bool isPlaneWaiting() const { return !this->waiting_planes.empty() && this->landing_plane == NULL; }
        Plane* getLandingPlane() const { return this->landing_plane; }

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

        /**
         * @brief Put a plane to the waiting mode.
         * The plane will turn around the airport with a altitude depending of number of planes in the waiting mode.
         * @param p The plane to update
         */
        void askPlaneToWait(Plane* p);

        /**
         * @brief Land the plane with the high priority.
         * A plane is pririty if it has trouble or if it's the lower plane in the air.
         * @param verbose Print some debug data or not
         */
        void landPriorityPlane(const bool& verbose = false);

        /**
         * @brief Start landing a plane to the runway.
         * Transfert the landing plane to the TWR.
         */
        void startLanding();
};