#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#include <Windows.h> // Used to get device refresh rate.
#include "MyConstants.h" // My own constants.
using namespace tle;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	// Load "Comic Sans MS" font at 36 points
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);

	// Create variables used to limit FPS.
	std::chrono::system_clock::time_point timePointA = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point timePointB = std::chrono::system_clock::now();

	/**** Set up your scene here ****/


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		// START OF FPS LIMITER CODE ///////////////////////////////////
		// Set the timePointA to the time right now
		timePointA = std::chrono::system_clock::now();
		// Create a work_time double of type duration, measured in milliseconds (double data type), and set it to the time difference between timePointA and timePointB.
		std::chrono::duration<double, std::milli> const work_time = timePointA - timePointB;

		if (work_time.count() < 1000 / GetDeviceCaps(GetDC(nullptr), VREFRESH))
		{
			// Set the delta_ms duration
			std::chrono::duration<double, std::milli> const delta_ms(frameTime - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		timePointB = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> const sleep_time = timePointB - timePointA;
		// END OF FPS LIMITER CODE ///////////////////////////////////

		myFont->Draw("Desired FPS: " + to_string(desiredFPS), 0, 0);
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
