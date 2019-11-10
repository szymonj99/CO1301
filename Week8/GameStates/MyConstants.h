#pragma once
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#include <Windows.h> // Used to get device refresh rate.

// START OF FPS LIMITER CONSTANTS
const int desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // The device's refresh rate.
const float frameTime = 1000 / desiredFPS; // The amount of time in ms per frame.
const float speedMultiplier = 1 / desiredFPS; // To keep the movement consistent across machines, get the multiplier based on device refresh rate.
// END OF FPS LIMITER CONSTANTS