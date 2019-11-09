#pragma once
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#include <Windows.h> // Used to get device refresh rate.

// START OF FPS LIMITER CONSTANTS
const int desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // Get the screen's refresh rate. Using nullptr gets the refresh rate of the main display.
const int milliseconds = 1000;
const double frameTime = milliseconds / desiredFPS;
// END OF FPS LIMITER CONSTANTS

// Create some constants
const float groundFrictionCoefficient = 0.175; // Used when calculating friction against ground.
const float airDensity = 1.225; // Air density around the plane, based on height. Used when calculating air resistance.
const float gravity = 9.81; // Used when calculating speed of descent.
const float moveSpeed = 0.15; // Used when rotating or moving objects.
const float acceleration = 0.5; // Used when accelerating or decelerating
const int initialSpeedMultiplier = 240; // Used when calculating coefficient to have consistent physics between refresh rates.
const float speedMultiplier = initialSpeedMultiplier / desiredFPS; // To keep the movement consistent across machines, get the multiplier based on device refresh rate.