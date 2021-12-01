#include "APP.hpp"

#define PLANE_FLIGHT_SPEED 654

class CCR
{
    private:
        vector<APP*> airports;
        
    public:
        CCR();
        ~CCR();

        /* --------------------------------- Getters -------------------------------- */

        vector<APP*> getAirports() { return this->airports; }

};