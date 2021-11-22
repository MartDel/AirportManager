#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <thread>
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
const string BACKGROUND_IMG = "airport.jpg";

// Fonts
const string FONTS_FOLDER = "../fonts/";
const string OPENSANS_FONT = "OpenSans.ttf";

/* ---------------------------------- Utils --------------------------------- */

float roundWithPrecision(const float& f);