#include "APP.hpp"

// The plane data on the CCR frame
#define PLANE_FLIGHT_SPEED 654.0 / 50

// CCR refresh interval
#define CCR_INTERVAL 250

class CCR {
    private:
        vector<APP*> airports;
        thread* ccr_thread;

        /* ----------------------------- Private methods ---------------------------- */

        /**
         * @brief Get the APP id
         * @param app The APP to search
         * @return size_t The APP id
         */
        size_t getAppId(APP* app) const;
        
    public:
        CCR();
        ~CCR();

        /* --------------------------------- Getters -------------------------------- */

        vector<APP*> getAirports() { return this->airports; }
        void setThread(bool& stop_prgm);

        /* ----------------------------- Public methods ----------------------------- */

        void moveToRandomAirport(Plane* p, APP* from);

        /* ---------------------- Static attribute and methods ---------------------- */

        /**
         * @brief The CCR thread method
         * @param ccr The CCR to update
         * @param stop_prgm When stop the simulation
         */
        static void threadCCR(CCR& ccr, bool& stop_prgm);
};