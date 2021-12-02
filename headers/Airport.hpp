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
#include <SFML/System/Vector2.hpp>

using namespace std;
using namespace sf;
using json = nlohmann::json;

/* ------------------------------ SFML defines ------------------------------ */

// Debug mode : don't generate planes and print important points
#define DEBUG

// Window size
#define WINDOW_WIDTH 800.0
#define WINDOW_REAL_WIDTH 3000.0
#define WINDOW_CCR_REAL_WIDTH 2000.0
#define WINDOW_HEIGHT 450.0
#define WINDOW_REAL_HEIGHT (WINDOW_REAL_WIDTH * (WINDOW_HEIGHT / WINDOW_WIDTH))
#define WINDOW_CCR_REAL_HEIGHT (WINDOW_CCR_REAL_WIDTH * (WINDOW_HEIGHT / WINDOW_WIDTH))

// Images
const string IMG_FOLDER = "../img/";
const string IMG_EXTENSION = ".jpg";

// Fonts
const string FONTS_FOLDER = "../fonts/";
const string OPENSANS_FONT = "OpenSans.ttf";

/* ------------------------------ Config Files ------------------------------ */

#define AIRPORTS_FILE "../config/airports.json"
#define COMPAGNIES_FILE "../config/compagnies.json"
#define LOGS_FILE "../config/logs.log"

/* ---------------------------------- Utils --------------------------------- */

/**
 * @brief Represent the frame where the plane will be printed
 */
enum class ReferenceFrame { CCR=0, APP1=1, APP2=2, APP3=3 };

/**
 * @brief Get the frame start point
 * @param ref The frame
 * @return Vector2f The start point
 */
Vector2f getFrameStartPoint(const ReferenceFrame& ref);

/**
 * @brief Get frame from its id
 * @param id The frame id
 * @return ReferenceFrame The frame
 */
ReferenceFrame getFrameFromId(const size_t& id);

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

/**
 * @brief Get the full image name
 * @param trigramme The airport trigramme
 * @return string The full image path
 */
string imgAirport(const string &trigramme);

#ifdef DEBUG
/**
 * @brief Generate a point to debug a position
 * @param pos The circle position
 * @return CircleShape The generated point
 */
CircleShape generateDebugCircle(const Vector2f& pos);
#endif