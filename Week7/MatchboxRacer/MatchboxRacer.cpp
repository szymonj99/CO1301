// MatchboxRacer.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#define _USE_MATH_DEFINES // Used to get constant pi.
#include <math.h> // Used to round numbers up for FPS limiting.
#include <Windows.h> // Used to get device refresh rate.
using namespace tle;

// START OF FPS LIMITER CONSTANTS
const double desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // Get the screen's refresh rate. Using nullptr gets the refreshrate of the main display.
const double milliseconds = 1000.0;
const double frameTime = milliseconds / desiredFPS;
// END OF FPS LIMITER CONSTANTS

// The constant that ensures proper physics functions on all refresh rates.
const float speedMultiplier = 1 / desiredFPS;

// Create a 3D engine (using TLX engine here).
I3DEngine* myEngine = New3DEngine(kTLX);

// Create enum that stores the axle that is currently being created in the car object.
const enum currentAxle {front, back};

// Class storing all of the matchbox car's components
class MatchboxCar
{
public:
	// The car's body.
	IModel* mainBody;
	// The car's front axle.
	IModel* frontAxle;
	// The car's back axle.
	IModel* backAxle;
	// The car's front right wheel.
	IModel* frontRightWheel;
	// The car's front left wheel.
	IModel* frontLeftWheel;
	// The car's back right wheel.
	IModel* backRightWheel;
	// The car's back left wheel.
	IModel* backLeftWheel;
	// The car's steering wheel rotation in the Y axis.
	float steeringPosition;
	// The car's maximum steering wheel rotation.
	float steeringPositionMax;
	// The car's acceleration ms^2.
	float acceleration;
	// The multiplier by which to multiply velocity to make the car go quicker.
	float velocityMultiplier;
	// The car's current forward velocity.
	float velocity;
	// The car's maximum forward velocity.
	float velocityMax;
	// How much to rotate the car's components each frame.
	float rotation;

	// Create the car's main body.
	void CreateBody(IMesh* matchboxMesh)
	{
		// The offset in the Y axis allowing for wheels to properly touch the ground when created.
		int carHeightOffset = 13;
		mainBody = matchboxMesh->CreateModel(0, carHeightOffset, 0);
	}

	// Create the car's axle, along with wheels that are attached to main body (parent).
	void CreateAxleAndWheels(IMesh* axleMesh, IMesh* wheelMesh, currentAxle currentAxle)
	{
		// The axle's offset from the middle of the car to where it should be placed.
		const int axleOffset[3] = { 0, 0, 20 };
		// The wheel's offset from the middle of the car to where it should be placed.
		const int wheelOffset[3] = { 23, 0, axleOffset[2] };
		if (currentAxle == front)
		{
			// Create the axle and attach to parent.
			frontAxle = axleMesh->CreateModel(axleOffset[0], axleOffset[1], axleOffset[2]);
			frontAxle->AttachToParent(mainBody);

			// Create the wheels and attach to parent.
			frontRightWheel = wheelMesh->CreateModel(wheelOffset[0], wheelOffset[1], 0);
			frontRightWheel->AttachToParent(frontAxle);
			frontLeftWheel = wheelMesh->CreateModel(-wheelOffset[0], wheelOffset[1], 0);
			frontLeftWheel->AttachToParent(frontAxle);
		}
		else
		{
			// Create the axle and attach to parent.
			backAxle = axleMesh->CreateModel(axleOffset[0], axleOffset[1], -axleOffset[2]);
			backAxle->AttachToParent(mainBody);

			// Create the wheels and attach to parent.
			backRightWheel = wheelMesh->CreateModel(wheelOffset[0], wheelOffset[1], 0);
			backRightWheel->AttachToParent(backAxle);
			backLeftWheel = wheelMesh->CreateModel(-wheelOffset[0], wheelOffset[1], 0);
			backLeftWheel->AttachToParent(backAxle);
		}
	}

	// Change the car's velocity by value.
	void ChangeVelocity(float velocityToChange)
	{
		velocity += velocityToChange;
	}

	// Change the car's steering.
	void ChangeSteering(float steeringToChange)
	{
		steeringPosition += steeringToChange;
		UpdateAxleRotation(steeringToChange);
	}

	// Update the car's front axle to reflect steering.
	void UpdateAxleRotation(float rotationChange)
	{
		frontAxle->RotateY(rotationChange);
	}

	// Control the car's variables with keyboard input.
	void ControlCar(I3DEngine* engine = myEngine)
	{
		// The movement is relative.
		if (engine->KeyHeld(Key_W))
		{
			if (velocity < velocityMax)
			{
				// Increase velocity.
				ChangeVelocity(acceleration * speedMultiplier);
			}
		}
		if (myEngine->KeyHeld(Key_S))
		{
			if (velocity > -velocityMax)
			{
				// Accelerate in opposite direction.
				ChangeVelocity(-acceleration * speedMultiplier);
			}
		}
		if (myEngine->KeyHeld(Key_A))
		{
			if (steeringPosition > -steeringPositionMax)
			{
				// Increase steering left.
				ChangeSteering(-acceleration * speedMultiplier);
			}
		}
		if (myEngine->KeyHeld(Key_D))
		{
			if (steeringPosition < steeringPositionMax)
			{
				// Increase steering right.
				ChangeSteering(acceleration * speedMultiplier);
			}
		}
		if (velocity != 0)
		{
			// Rotate the axles forwards.
			frontAxle->RotateLocalX(velocity * rotation);
			backAxle->RotateLocalX(velocity * rotation);
			// Move the car after all calculations are done.
			mainBody->MoveLocalZ(velocity * velocityMultiplier * speedMultiplier);
			if (steeringPosition != 0)
			{
				mainBody->RotateY(velocity * rotation * sin(steeringPosition * M_PI / 180));
			}
		}
	}

	// Create a camera that will follow the car.
	void AttachCamera(ICamera* myCamera)
	{
		// Attach camera to parent, move by an offset, and rotate.
		myCamera->AttachToParent(mainBody);
		myCamera->MoveLocal(0, 60, -60);
		myCamera->RotateLocalX(25);
	}
};

void main()
{
	// Open a new window for the 3D engine.
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	// Create variables used to limit FPS.
	std::chrono::system_clock::time_point timePointA = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point timePointB = std::chrono::system_clock::now();

	/**** Set up your scene here ****/

	// Load "Comic Sans MS" font at 36 points
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);

	// Create the matchbox car object.
	MatchboxCar* matchboxCar = new MatchboxCar();

	// Create the car body.
	matchboxCar->CreateBody(myEngine->LoadMesh("Matchbox.x")); // The matchbox mesh will only be used once. No point in storing it in memory as a variable.
	
	// Create the front and back axle and wheels of the car, attached to parent.
	IMesh* carAxle = myEngine->LoadMesh("Match.x");
	IMesh* carWheel = myEngine->LoadMesh("TwoPence.x");
	matchboxCar->CreateAxleAndWheels(carAxle, carWheel, front);
	matchboxCar->CreateAxleAndWheels(carAxle, carWheel, back);	

	// Define the car's variables.
	matchboxCar->steeringPositionMax = 15.0;
	matchboxCar->velocityMax = 15.0;
	matchboxCar->acceleration = 5.0;
	matchboxCar->velocityMultiplier = 7.5;
	matchboxCar->rotation = speedMultiplier * matchboxCar->velocityMultiplier * M_PI;

	// Create the floor.
	IMesh* floorMesh = myEngine->LoadMesh("Floor.x");
	const IModel* floorModel = floorMesh->CreateModel();

	// Create camera and attatch to parent
	matchboxCar->AttachCamera(myEngine->CreateCamera(kManual));

	// Define is the game paused
	bool isPaused = false;

	// Define is mouse captured by engine and capture the mouse immediately.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();

	// Define amount of frames since engine started.
	int totalFrames = 0;

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

			if (myEngine->AnyKeyHit())
			{
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
			}
			// Control the matchbox car here.
			matchboxCar->ControlCar();


			// Increment totalFrames
			totalFrames++;
			// Print info on screen.
			myFont->Draw("Total Frames: " + to_string(totalFrames), 0, 0);
			myFont->Draw("Velocity: " + to_string(matchboxCar->velocity), 0, 50);
			myFont->Draw("Steering: " + to_string(matchboxCar->steeringPosition), 0, 100);
		}
		else
		{
			if (myEngine->AnyKeyHit())
			{
				// Toggle isPaused.
				if (myEngine->KeyHit(Key_P))
				{
					isPaused = !isPaused;
				}
				// Exit game.
				if (myEngine->KeyHit(Key_Escape))
				{
					myEngine->Stop();
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
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
