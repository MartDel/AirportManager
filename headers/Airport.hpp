#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <thread>
#include <fstream>
#include <mutex>
#include <chrono>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using json = nlohmann::json;

/* ------------------------------ SFML defines ------------------------------ */

// Window size
#define WINDOW_WIDTH 800.0
#define WINDOW_REAL_WIDTH 3000.0
#define WINDOW_HEIGHT 494.0
#define WINDOW_REAL_HEIGHT (WINDOW_REAL_WIDTH * (WINDOW_HEIGHT / WINDOW_WIDTH))

// Images
const string IMG_FOLDER = "../img/";

// Fonts
const string FONTS_FOLDER = "../fonts/";
const string OPENSANS_FONT = "OpenSans.ttf";

/* ------------------------------ Config Files ------------------------------ */

#define AIRPORTS_FILE "../config/airports.json"
#define COMPAGNIES_FILE "../config/compagnies.json"
#define LOGS_FILE "../config/logs.log"

/* ---------------------------------- Utils --------------------------------- */

/**
 * @brief Round a float with a given precision 
 * 
 * @param f 
 * @return float 
 */
float roundWithPrecision(const float& f);
/**
 * @brief 
 * 
 * @param x 
 * @param in_min 
 * @param in_max 
 * @param out_min 
 * @param out_max 
 * @return float 
 */
float mapValue(const float &x, const float &in_min, const float &in_max, const float &out_min, const float &out_max);


/**
 * @brief Update the logs file
 * 
 * @param logs  
 */
void updateLogs(const string& logs);