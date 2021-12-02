#include "CCR.hpp"

/* -------------------------------------------------------------------------- */
/*                                CCR functions                               */
/* -------------------------------------------------------------------------- */

CCR::CCR() {
    vector<APP*> tmp_airports;

    ifstream file_in(AIRPORTS_FILE);
    if (!file_in.is_open()) {
        cerr << "Cannot open file : " << AIRPORTS_FILE << endl;
        throw runtime_error("Cannot open file");
    } else {
        json j;
        file_in >> j;
        for (size_t i = 0; i < j.size(); i++) {
            cout << "Generating the airport : " << j[i]["name"] << endl;
            tmp_airports.push_back(new APP(j[i], getFrameFromId(i)));
        }
    }

    this->airports = tmp_airports;
}

CCR::~CCR() {
    for (auto& airport : this->airports)
        delete(airport);
}
