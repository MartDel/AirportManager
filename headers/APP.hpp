#include "TWR.hpp"

// The plane data on the CCR frame
#define PLANE_FLIGHT_ALTITUDE 10000

// The circular trajectory config
#define CIRCULAR_TRAJ_NB_POINTS 20
#define CIRCULAR_TRAJ_SPEED 100
#define CIRCULAR_TRAJ_ALTITUDE_MIN 500
#define CIRCULAR_TRAJ_ALTITUDE_STEP 350

// Airport refresh interval
#define AIRPORTS_INTERVAL 250

// SFML defines
#define AIRPORT_COLOR_DEFAULT Color::Red
#define AIRPORT_COLOR_NOT_PRINTED Color(190, 190, 190)
#define AIRPORT_RADIUS_DEFAULT 8.f
#define AIRPORT_RADIUS_NOT_PRINTED 7.f
#define AIRPORT_NAME_LABEL_X 15
#define AIRPORT_NAME_LABEL_Y -15
#define AIRPORT_NAME_LABEL_SIZE 10

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
        Location perimeter_entrance, global_location;
        CircleShape global_point;
        Trajectory circular_traj; // Circular trajectory to lead waiting planes
        Plane* landing_plane; // The plane which is reaching the runway to land
        Plane *parking_plane; // The plane which is reaching the parking
        Plane *exiting_plane; // The plane which is reaching the perimeter limit
        thread* airport_thread;
        ReferenceFrame ref_frame;
        Sprite *background; // The airport background image
        Text name_label; // The airport name label to print on the CCR display

#ifdef DEBUG
        vector<Location> important_points;
        #endif

        /**
         * @brief Remove a plane from a vector
         * @param plane The plane to remove
         * @param list The list to remove the plane from
         */
        void removePlaneFrom(Plane* plane, vector<Plane*>& list);

        /**
         * @brief Get the plane position in the waiting_planes vector
         * @param waiting_plane The plane to search
         * @return The plane position
         */
        size_t getWaitingPlanePos(Plane* waiting_plane) const;

    public:
        APP(const json& data, const ReferenceFrame& ref);
        ~APP();

        /* --------------------------------- Getters -------------------------------- */

        string getName() const { return this->name; }
        string getTrigramme() const { return this->trigramme; }
        TWR* getTWR() const { return this->linked_twr; }
        Trajectory getCircularTrajectory() const { return this->circular_traj; }
        bool isPlaneWaiting() const { return !this->waiting_planes.empty() && this->landing_plane == NULL; }
        Plane *getLandingPlane() const { return this->landing_plane; }
        Plane *getParkingPlane() const { return this->parking_plane; }
        Plane* getExitingPlane() const { return this->exiting_plane; }
        Sprite *getBackground() const { return this->background; }
        Text getNameLabel() const { return this->name_label; }
        ReferenceFrame getReferenceFrame() const { return this->ref_frame; }
        Location getGlobalLocation() { return this->global_location; }
        Location getPerimeterEntrance() { return this->perimeter_entrance; }
        CircleShape getGlobalPoint() { return this->global_point; }

        /* --------------------------------- Setters -------------------------------- */

        void setExitingPlane(Plane *p) { this->exiting_plane = p; }
        void setBackground(Sprite* bg) { this->background = bg; }
        void setThread(bool &stop_prgm);
        void addComingPlane(Plane* p) { this->coming_planes.push_back(p); }

        /**
         * @brief Get all arrived planes in the coming planes array
         * @return vector<Plane*> Arrived planes
         */
        vector<Plane*> getArrivedPlanes() const;

        #ifdef DEBUG
        vector<Location> getImportantPoints() const { return this->important_points; }
        #endif

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
         * @brief End the plane landing
         * @param p The plane which has reached the parking
         */
        void endLanding(Plane* p);

        /**
         * @brief Airport thread function.
         * Check if a plane can take off or land.
         * @param twr The TWR to manage
         * @param stop_prgm If the simulation must stoped
         */
        static void airportControl(APP &app, bool &stop_prgm);
};