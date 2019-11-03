// MatchboxRacer.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#include <math.h> // Used to round numbers up for FPS limiting.
#include <Windows.h> // Used to get device refresh rate.
using namespace tle;

// START OF FPS LIMITER CONSTANTS
const double desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // Get the screen's refresh rate. Using nullptr gets the refreshrate of the main display.
const double milliseconds = 1000.0;
const double frameTime = milliseconds / desiredFPS;
// END OF FPS LIMITER CONSTANTS

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	// Create variables used to limit FPS.
	std::chrono::system_clock::time_point timePointA = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point timePointB = std::chrono::system_clock::now();

	/**** Set up your scene here ****/

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// START OF FPS LIMITER CODE ///////////////////////////////////
		// Set the timePointA to the time right now
			timePointA = std::chrono::system_clock::now();
		// Create a work_time double of type duration, measured in milliseconds (double data type), and set it to the time difference between timePointA and timePointB.
		std::chrono::duration<double, std::milli> const work_time = timePointA - timePointB;

		if (work_time.count() < frameTime)
		{
			// Set the delta_ms duration
			std::chrono::duration<double, std::milli> const delta_ms(frameTime - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		timePointB = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> const sleep_time = timePointB - timePointA;
		// END OF FPS LIMITER CODE ///////////////////////////////////

		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
