// AirplaneSimulation.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#define _USE_MATH_DEFINES // Used to get constant pi.
#include <math.h> // Used to round numbers up for FPS limiting.
#include <Windows.h> // Used to get device refresh rate.
using namespace tle; // TLEngine namespace

// START OF FPS LIMITER CONSTANTS
const double desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // Get the screen's refresh rate. Using nullptr gets the refresh rate of the main display.
const double milliseconds = 1000.0;
const double frameTime = milliseconds / desiredFPS;
// END OF FPS LIMITER CONSTANTS

// Create a 3D engine (using TLX engine here) and open a window for it
I3DEngine* myEngine = New3DEngine(kTLX);

// Create some constants
const float groundFrictionCoefficient = 0.175; // Used when calculating friction against ground.
const float airDensity = 1.225; // Air density around the plane, based on height. Used when calculating air resistance.
const float gravity = 9.81; // Used when calculating speed of descent.
const float moveSpeed = 0.15; // Used when rotating or moving objects.
const float acceleration = 0.1; // Used when accelerating or decelerating
const int initialSpeedMultiplier = 240; // Used when calculating coefficient to have consistent physics between refresh rates.
const float maxSpeed = 10.0; // Declare plane's max speed
const float takeoffSpeed = 5.0; // Declare at what speed the plane will take off.
const float speedMultiplier = initialSpeedMultiplier / desiredFPS; // To keep the movement consistent across machines, get the multiplier based on device refresh rate.

// Class used to create a plane object.
class IAirplane
{
public:
	// The model used by IAirplane.
	IModel* model;
	// The airplane's mass in kilograms.
	float mass;
	// The airplane's weight in Newtons.
	float weight;
	// The plane's velocity in m/s.
	float velocity;
	// The plane's front area in m^2.
	float area;
	// The plane's air resistance in Newtons.
	float airResistance;
	// The air density around the plane. This changes with height.
	float airDensityAroundPlane;	
	// The plane's drag coefficient.
	float dragCoefficient;
	// The plane's upwards lift in Newtons.
	float upwardsLift;
	// The plane's downards lift in Newtons.
	float downwardsLift;
	// The propeller's diameter in m.
	float propellerDiameter;
	// The propeller's thrust in Newtons.
	float propellerThrust;
	// The plane's maximum forward velocity in m/s.
	float maxVelocity;
	// The plane's minimum velocity before it lifts off the ground in m/s.
	float takeoffVelocity;
	// The propeller's max revolutions per minute.
	int propellerMaxRPM;
	// The propeller's current revolutions per minute.
	float propellerCurrentRPM;
	// The plane's rotation along its X axis in degrees.
	float rotationX;
	// The plane's rotation along its Y axis in degrees.
	float rotationY;
	// The plane's rotation along its Z axis in degrees.
	float rotationZ;

	// Change the airplane's velocity by value in m/s.
	void ChangeRPM(float rpmChange)
	{
		propellerCurrentRPM += rpmChange;
	}
	// Change the plane's current rotation.
	void ChangeRotation(float rotationChange, string axis)
	{
		if (axis == "X")
		{
			rotationX += rotationChange;
			model->RotateLocalX(rotationChange);
		}
		else if (axis == "Y")
		{
			rotationY += rotationChange;
			model->RotateLocalY(rotationChange);
		}
		else
		{
			rotationZ += rotationChange;
			model->RotateLocalZ(rotationChange);
		}
	}
	// Update the air density around the plane.
	void UpdateAirDensityAroundPlane()
	{
		airDensityAroundPlane = airDensity - (model->GetY() / 1000);
	}
	// Update the plane's current air resistance.
	void UpdateAirResistance()
	{
		if (model->GetY() > 1)
		{
			if (airDensityAroundPlane <= 0.0)
			{
				airDensityAroundPlane = 0.01;
			}
		}
		airResistance = roundf((airDensityAroundPlane * dragCoefficient * area) / 2 * pow(1.75, velocity) * 100) / 100; // In real life, the velocity is squared. This results in crazy numbers however.
	}
	// Update the propeller's thrust.
	void UpdatePropellerThrust()
	{
		propellerThrust = roundf((airDensityAroundPlane * propellerCurrentRPM * M_PI * pow(2, propellerDiameter / 2)) * 100) / 100;
	}
	// Update the plane's velocity
	void UpdatePlaneVelocity()
	{
		velocity = (propellerThrust / gravity - airResistance / gravity) / gravity;
		if (velocity < 0)
		{
			velocity = 0;
		}
	}
	// Move the plane forwards, down/up.
	void MovePlane()
	{
		model->MoveLocalZ(velocity * speedMultiplier);
	}
};

// Function to move object with WASD keys.
void ControlAirplane(IAirplane* airplane, I3DEngine* engine = myEngine)
{
	// The movement is relative.
	if (myEngine->KeyHeld(Key_W))
	{
		if (airplane->propellerCurrentRPM < airplane->propellerMaxRPM)
		{
			// Increase propeller revolutions.
			airplane->ChangeRPM(acceleration * speedMultiplier);
		}		
	}
	if (myEngine->KeyHeld(Key_S))
	{
		// Slow RPM down if RPM > 0
		if (airplane->propellerCurrentRPM > 0)
		{
			// Decrease propeller revolutions.
			airplane->ChangeRPM(-acceleration * speedMultiplier);
		}
		else
		{
			airplane->propellerCurrentRPM = 0;
		}
	}
	if (myEngine->KeyHeld(Key_Left))
	{
		// Rotate airplane left.
		airplane->ChangeRotation((moveSpeed * speedMultiplier), "Z");
	}
	if (myEngine->KeyHeld(Key_Right))
	{
		// Rotate airplane right.
		airplane->ChangeRotation((-moveSpeed * speedMultiplier), "Z");
	}
	if (myEngine->KeyHeld(Key_Up))
	{
		// Rotate airplane up.
		airplane->ChangeRotation((-moveSpeed * speedMultiplier), "X");
	}
	if (myEngine->KeyHeld(Key_Down))
	{
		// Rotate airplane down.
		airplane->ChangeRotation((moveSpeed * speedMultiplier), "X");
	}
	if (myEngine->KeyHeld(Key_D))
	{
		// Roll the airplane to the right.
		airplane->ChangeRotation((moveSpeed * speedMultiplier), "Y");
	}
	if (myEngine->KeyHeld(Key_A))
	{
		// Roll the airplane to the left.
		airplane->ChangeRotation((-moveSpeed * speedMultiplier), "Y");
	}
	//// Move the airplane forward if it is faster than takeoffSpeed, else go in a straight line.
	//if (airplane->velocity >= airplane->takeoffVelocity)
	//{
	//	// Move object forward (relative).
	//	airplane->model->MoveLocalZ(airplane->velocity * (speedMultiplier / desiredFPS * 1.25));
	//}
	//else
	//{
	//	// Move object forward (relative), and if plane is above the ground, descend based on current speed. 
	//	airplane->model->MoveLocalZ(airplane->velocity * (speedMultiplier / desiredFPS * 1.25));
	//	if (airplane->model->GetY() > 0)
	//	{
	//		airplane->model->MoveY(-gravity * 1/(airplane->velocity * speedMultiplier));
	//	}
	//}

	//// Add colliision to ground level.
	//if (airplane->model->GetY() < 0)
	//{
	//	airplane->model->SetY(0);
	//}

	// Rotate propeller based on plane speed, with a minimum rotation speed.
	if (airplane->velocity > 0)
	{
		//airplane->model->GetNode(4)->RotateLocalZ(moveSpeed * airplane->velocity * speedMultiplier + airplane->velocity);
		airplane->model->GetNode(4)->RotateLocalZ(airplane->propellerCurrentRPM / speedMultiplier);
	}
}

// Update the Heads Up Display elements with passed in font.
void UpdateHUD(IFont* myFont, IAirplane* airplane, int totalFrames)
{
	// Redo the displayed value limitation - round up before turning to string.
	// Print totalFrames on screen.
	myFont->Draw("Frames: " + to_string(totalFrames), 0, 0);
	// Print the plane's forward velocity on screen.
	myFont->Draw("Forward Velocity: " + to_string(airplane->velocity).substr(0, 4), 0, 40); // Limit the displayed value to 1 decimal place.
	// Print the plane's air resistance on screen.
	myFont->Draw("Air Resistance: " + to_string(airplane->airResistance), 0, 80); // Limit the displayed value to 1 decimal place.
	// Print the plane's propeller current RPM on screen.
	myFont->Draw("RPM: " + to_string(airplane->propellerCurrentRPM), 0, 120); // Limit the displayed value to 1 decimal place.
	// Print the plane's propeller max RPM on screen.
	myFont->Draw("Max RPM: " + to_string(airplane->propellerMaxRPM), 0, 160);
	// Print the plane's Pitch on screen.
	myFont->Draw("Pitch: " + to_string(airplane->rotationX).substr(0, 5), 0, 200);
	// Print the plane's Yaw on screen.
	myFont->Draw("Yaw: " + to_string(airplane->rotationY), 0, 240);
	// Print the plane's Roll on screen.
	myFont->Draw("Roll: " + to_string(airplane->rotationZ), 0, 280);
	// Print the plane's propeller thrust on screen.
	myFont->Draw("Thrust: " + to_string(airplane->propellerThrust), 0, 320); // Limit the displayed value to 1 decimal place.
}

// Entry point of the program.
void main()
{
	// Create window for myEngine.
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );
	// Add relative directory resources
	myEngine->AddMediaFolder("resources");

	/**** Set up your scene here ****/

	// Load "Comic Sans MS" font at 36 points
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);
	
	// Create mesh and model objects

	// Mesh used to create airplane model.
	IMesh* airplaneMesh = myEngine->LoadMesh("sopwith-camel.x");
	// Mesh used to create floor model.
	IMesh* floorMesh = myEngine->LoadMesh("Floor.x");
	// Mesh used to create skybox model.
	IMesh* skyboxMesh = myEngine->LoadMesh("Skybox 01.x");

	// Airplane object with default values.
	IAirplane* airplane = new IAirplane();
	airplane->model = airplaneMesh->CreateModel();
	airplane->mass = 300.0;
	airplane->weight = airplane->mass * gravity;
	airplane->downwardsLift = airplane->weight;
	airplane->maxVelocity = 20.0;
	airplane->dragCoefficient = 0.025;
	airplane->area = 30.0;
	airplane->propellerDiameter = 1.0;
	airplane->propellerMaxRPM = 100;
	airplane->rotationX = 0;
	airplane->rotationY = 0;
	airplane->rotationZ = 0;

	// Offset the X rotation of the plane by this value to make sure front and back wheels are touching the ground.
	const int rotationOffset = -15;
	airplane->model->RotateLocalX(rotationOffset);
	airplane->rotationX = rotationOffset;

	// Offset the ground by this value for the top of the ground to be at Y = 0;
	const float groundOffset = -1.05;
	// Floor model based on floor mesh.
	IModel* floorModel = floorMesh->CreateModel(0, groundOffset, 0);
	// Change ground texture.
	floorModel->SetSkin("ground_01.jpg");
	
	// Offset the skybox by this value.
	const int skyboxOffset = -940;
	const IModel* skyboxModel = skyboxMesh->CreateModel(0, skyboxOffset, 0);

	// kManual camera attached to parent airplane.
	ICamera* myCamera = myEngine->CreateCamera(kManual);
	myCamera->AttachToParent(airplane->model);
	// The position of the camera relative to the plane model.
	const float cameraPlaneOffset[] = { 0.0, -8.0, 14.0 };
	myCamera->MoveLocal(cameraPlaneOffset[0], cameraPlaneOffset[1], cameraPlaneOffset[2]);

	// Define is the game paused.
	bool isPaused = false;

	// Define is mouse captured by engine.
	// And capture the mouse.
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
			ControlAirplane(airplane);

			// Increment totalFrames
			totalFrames++;
			// Update the plane's air density.
			airplane->UpdateAirDensityAroundPlane();
			// Update the plane's air resistance.
			airplane->UpdateAirResistance();
			// Update the plane's propeller thrust.
			airplane->UpdatePropellerThrust();
			// Update the plane's velocity.
			airplane->UpdatePlaneVelocity();
			// Update HUD elements.
			UpdateHUD(myFont, airplane, totalFrames);
		}
		else
		{
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
			
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
