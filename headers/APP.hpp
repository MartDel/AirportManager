#include "TWR.hpp"

// The circular trajectory config
#define CIRCULAR_TRAJ_NB_POINTS 20
#define CIRCULAR_TRAJ_SPEED 100
#define CIRCULAR_TRAJ_ALTITUDE_MIN 500
#define CIRCULAR_TRAJ_ALTITUDE_STEP 350

// Airport refresh interval
#define AIRPORTS_INTERVAL 250

/**
 * @brief A APP manage airport perimeter and landing.
 * It manage all of waiting planes for landing.
 */
class APP {
    private:
        string name;
        string trigramme;
        vector<Plane *> waiting_planes;
        vector<Plane *> coming_planes;
        TWR* linked_twr;
        Location perimeter_entrance, airport_center;
        Trajectory circular_traj; // Circular trajectory to lead waiting planes
        Plane* landing_plane;
        thread* airport_thread;

        /**
         * @brief Remove a plane from a vector
         * @param plane The plane to remove
         * @param list The list to remove the plane from
         */
        void removePlaneFrom(Plane* plane, vector<Plane*>& list);

    public:
        APP(const json& data);
        ~APP();

        /* --------------------------- Getters and setters -------------------------- */

        string getName() const { return this->name; }
        string getTrigramme() const { return this->trigramme; }
        TWR* getTWR() const { return this->linked_twr; }
        Trajectory getCircularTrajectory() const { return this->circular_traj; }
        bool isPlaneWaiting() const { return !this->waiting_planes.empty() && this->landing_plane == NULL; }
        Plane* getLandingPlane() const { return this->landing_plane; }
        void setThread(bool &stop_prgm);

        /**
         * @brief Get all arrived planes in the coming planes array
         * @return vector<Plane*> Arrived planes
         */
        vector<Plane*> getArrivedPlanes() const;

        /* ----------------------------- Public methods ----------------------------- */

        /**
         * @brief Spawn a plane to the perimeter entrance.
         * @return Plane* The spawned plane
         */
        Plane* spawnPlane();

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
        void landPriorityPlane();

        /**
         * @brief Start landing a plane to the runway.
         * Transfert the landing plane to the TWR.
         */
        void startLanding();

        /**
         * @brief Airport thread function.
         * Check if a plane can take off or land.
         * @param twr The TWR to manage
         * @param stop_prgm If the simulation must stoped
         */
        static void airportControl(APP &app, bool &stop_prgm);
};