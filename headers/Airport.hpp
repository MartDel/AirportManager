#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

/* ------------------------------ SFML defines ------------------------------ */

// Window size
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 494

// Images
const string IMG_FOLDER = "../img/";
const string BACKGROUND_IMG = "airport.jpg";

// Fonts
const string FONTS_FOLDER = "../fonts/";
const string OPENSANS_FONT = "OpenSans.ttf";