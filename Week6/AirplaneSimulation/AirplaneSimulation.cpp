// AirplaneSimulation.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#include <math.h> // Used to round numbers up for FPS limiting.
#include <Windows.h> // Used to get device refresh rate.
using namespace tle; // TLEngine namespace

// START OF FPS LIMITER CONSTANTS
const double desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // Get the screen's refresh rate. Using nullptr gets the refreshrate of the main display.
const double milliseconds = 1000.0;
const double frameTime = milliseconds / desiredFPS;
// END OF FPS LIMITER CONSTANTS

// Create some constants
const float gravity = 9.81; // Used when calculating speed of descent.
const float moveSpeed = 0.15; // Used when rotating or moving objects.
const float acceleration = 0.005; // Used when accelerating or decelerating
const int initialSpeedMultiplier = 240;
const float maxSpeed = 10.0; // Declare plane's max speed
const float takeoffSpeed = 5.0; // Declare at what speed the plane will take off.
const float speedMultiplier = initialSpeedMultiplier / desiredFPS; // To keep the movement consistent across machines, get the multiplier based on device refresh rate.

// Create global variable for plane speed.
float planeSpeed = 0.0;

// Function to move object with WASDQEZX keys.
void ControlObject(I3DEngine* myEngine, IModel* object, ISceneNode* propeller)
{
	// The movement is relative.
	if (myEngine->KeyHeld(Key_W))
	{
		if (planeSpeed < maxSpeed)
		{
			// Increase object speed.
			planeSpeed += acceleration * speedMultiplier;
		}		
	}
	if (myEngine->KeyHeld(Key_S))
	{
		// Slow object down if speed > 0
		if (planeSpeed > 0)
		{
			planeSpeed -= acceleration * speedMultiplier;
		}
		else
		{
			planeSpeed = 0;
		}
	}
	if (myEngine->KeyHeld(Key_Left))
	{
		// Rotate object left.
		object->RotateLocalZ(moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_Right))
	{
		// Rotate object right.
		object->RotateLocalZ(-moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_Up))
	{
		// Rotate object up.
		object->RotateLocalX(-moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_Down))
	{
		// Rotate object down.
		object->RotateLocalX(moveSpeed * speedMultiplier);
	}
	// Move the object forward if it is faster than takeoffSpeed, else go in a straight line.
	if (planeSpeed >= takeoffSpeed)
	{
		// Move object forward (relative).
		object->MoveLocalZ(planeSpeed * (speedMultiplier / desiredFPS * 1.25));
	}
	else
	{
		// Move object forward (relative), and if plane is above the ground, descend based on current speed. 
		object->MoveLocalZ(planeSpeed * (speedMultiplier / desiredFPS * 1.25));
		if (object->GetY() > 0)
		{
			object->MoveY(-gravity * 1/(planeSpeed * speedMultiplier * speedMultiplier));
		}
	}

	// Add colliision to ground level.
	if (object->GetY() < 0)
	{
		object->SetY(0);
	}

	// Rotate propeller based on plane speed, with a minimum rotation speed.
	if (planeSpeed > 0)
	{
		propeller->RotateLocalZ(moveSpeed * planeSpeed * speedMultiplier * speedMultiplier + planeSpeed);
	}
}

// Entry point of the program.
void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );
	// Add relative directory resources
	myEngine->AddMediaFolder("resources");

	/**** Set up your scene here ****/

	// Load "Comic Sans MS" font at 36 points
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);
	
	// Create mesh and model objects
	IMesh* airplaneMesh = myEngine->LoadMesh("sopwith-camel.x");
	IMesh* floorMesh = myEngine->LoadMesh("Floor.x");
	IMesh* skyboxMesh = myEngine->LoadMesh("Skybox 01.x");

	// Create model from mesh
	const float groundOffset = -1.05;
	IModel* airplaneModel = airplaneMesh->CreateModel();
	IModel* floorModel = floorMesh->CreateModel(0, groundOffset, 0);
	const int rotationOffset = -15;
	airplaneModel->RotateLocalX(rotationOffset);
	const int skyboxOffset = -940;
	const IModel* skyboxModel = skyboxMesh->CreateModel(0, skyboxOffset, 0);

	// Get propeller node from plane.
	ISceneNode* propellerNode = airplaneModel->GetNode(4);

	// Change ground texture.
	floorModel->SetSkin("ground_01.jpg");

	// Create camera and attach to plane.
	ICamera* myCamera = myEngine->CreateCamera(kManual);
	myCamera->AttachToParent(airplaneModel);
	const float cameraPlaneOffset[] = { 0.0, -8.0, 14.0 };
	myCamera->MoveLocal(cameraPlaneOffset[0], cameraPlaneOffset[1], cameraPlaneOffset[2]);

	// Define is the game paused.
	bool isPaused = false;

	// Define current camera view
	string currentCameraView = "plane";

	// Define is mouse captured by engine and capture the mouse immediately.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();

	// Define amount of frames since engine started.
	int totalFrames = 0;

	// Create variables used to limit FPS.
	std::chrono::system_clock::time_point timePointA = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point timePointB = std::chrono::system_clock::now();

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

		if (!isPaused)
		{
			// Draw the scene
			myEngine->DrawScene();

			// Increment totalFrames
			totalFrames++;
			//Print totalFrames on screen.
			myFont->Draw("Frames:", 0, 0);
			myFont->Draw(to_string(totalFrames), 110, 0);
			// Print the plane's speed on screen.
			myFont->Draw("Speed:", 0, 50);
			myFont->Draw(to_string(planeSpeed).substr(0, 4), 110, 50); // Limit the displayed value to 1 decimal place.

			/**** Update your scene each frame here ****/

			// Control keyboard input.
			// Exit game.
			if (myEngine->KeyHit(Key_Escape))
			{
				myEngine->Stop();
			}
			// Toggle isPaused.
			if (myEngine->KeyHit(Key_P))
			{
				isPaused = !isPaused;
			}
			// Toggle mouse capture.
			if (myEngine->KeyHit(Key_Tab))
			{
				if (isMouseCaptured)
				{
					myEngine->StopMouseCapture();
				}
				else
				{
					myEngine->StartMouseCapture();
				}
				isMouseCaptured = !isMouseCaptured;
			}
			// Move the object with keyboard.
			ControlObject(myEngine, airplaneModel, propellerNode);
		}
		else
		{
			// Toggle isPaused.
			if (myEngine->KeyHit(Key_P))
			{
				// To prevent the camera from moving when resuming game, call GetMouseMovementX/Y()
				myEngine->GetMouseMovementX();
				myEngine->GetMouseMovementY();
				isPaused = !isPaused;
			}
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
