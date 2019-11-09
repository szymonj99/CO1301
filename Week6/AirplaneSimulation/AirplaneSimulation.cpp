#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "IAirplane.h" // Used to create IAirplane object.
#include "MyConstants.h" // Used to get my constants.
using namespace tle; // TLEngine namespace

// Create a 3D engine (using TLX engine here) and open a window for it
I3DEngine* myEngine = New3DEngine(kTLX);

// Function to move object with WASD keys.
void ControlAirplane(IAirplane* airplane, I3DEngine* engine = myEngine)
{
	// The movement is relative.
	if (myEngine->KeyHeld(Key_W))
	{
		if (airplane->GetPropellerCurrentRPM() < airplane->GetPropellerMaxRPM())
		{
			// Increase propeller revolutions.
			airplane->ChangePropellerCurrentRPM(acceleration * speedMultiplier);
		}		
	}
	if (myEngine->KeyHeld(Key_S))
	{
		// Slow RPM down if RPM > 0
		if (airplane->GetPropellerCurrentRPM() > 0)
		{
			// Decrease propeller revolutions.
			airplane->ChangePropellerCurrentRPM(-acceleration * speedMultiplier);
		}
		else
		{
			airplane->ChangePropellerCurrentRPM(-airplane->GetPropellerCurrentRPM());
			if (airplane->GetVelocity() > 0)
			{
				airplane->ChangeVelocity(-acceleration / speedMultiplier);
			}
			else
			{
				airplane->ChangeVelocity(-airplane->GetVelocity());
			}
		}
	}
	if (myEngine->KeyHeld(Key_Left))
	{
		// Rotate airplane left.
		if (airplane->GetRotationZ() < 90)
		{
			airplane->ChangeRotationZ(moveSpeed * speedMultiplier);
			airplane->GetModel()->RotateLocalZ(moveSpeed * speedMultiplier);
		}
		if (airplane->GetRotationZ() > 90)
		{
			airplane->ChangeRotationZ(90 - airplane->GetRotationZ());
		}
	}
	if (myEngine->KeyHeld(Key_Right))
	{
		// Rotate airplane right.
		if (airplane->GetRotationZ() > -90)
		{
			airplane->ChangeRotationZ(-moveSpeed * speedMultiplier);
			airplane->GetModel()->RotateLocalZ(-moveSpeed * speedMultiplier);
		}
		if (airplane->GetRotationZ() < -90)
		{
			airplane->ChangeRotationZ(-90 - airplane->GetRotationZ());
		}
		
	}
	if (myEngine->KeyHeld(Key_Down))
	{
		// Rotate airplane down.
		if (airplane->GetRotationX() < 90)
		{
			airplane->ChangeRotationX(moveSpeed * speedMultiplier * cos(airplane->GetRotationZ() * M_PI / 180));
			airplane->GetModel()->RotateLocalX(moveSpeed * speedMultiplier);
		}
		if (airplane->GetRotationX() > 90)
		{
			airplane->ChangeRotationX(90 - airplane->GetRotationX());
		}
		
	}
	if (myEngine->KeyHeld(Key_Up))
	{
		// Rotate airplane up.
		if (airplane->GetRotationX() > -90)
		{
			airplane->ChangeRotationX(-moveSpeed * speedMultiplier * cos(airplane->GetRotationZ() * M_PI / 180));
			airplane->GetModel()->RotateLocalX(-moveSpeed * speedMultiplier);
		}
		if (airplane->GetRotationX() < -90)
		{
			airplane->ChangeRotationX(-90 - airplane->GetRotationX());
		}
		
	}
	if (myEngine->KeyHeld(Key_D))
	{
		// Roll the airplane to the right.
		if (airplane->GetRotationY() < 0)
		{
			airplane->ChangeRotationY(360);
		}
		airplane->ChangeRotationY(moveSpeed * speedMultiplier);
		//airplane->ChangeRotationX(moveSpeed * speedMultiplier * sin(airplane->GetRotationY() * M_PI / 180));
		airplane->GetModel()->RotateLocalY(moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_A))
	{
		// Roll the airplane to the left.
		if (airplane->GetRotationY() < 0)
		{
			airplane->ChangeRotationY(360);
		}
		airplane->ChangeRotationY(-moveSpeed * speedMultiplier);
		airplane->GetModel()->RotateLocalY(-moveSpeed * speedMultiplier);
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

	// Add colliision to ground level.
	if (airplane->GetModel()->GetY() < 0)
	{
		airplane->GetModel()->SetY(0);
	}

	// Rotate propeller based on plane speed, with a minimum rotation speed.
	if (airplane->GetVelocity() > 0)
	{
		//airplane->model->GetNode(4)->RotateLocalZ(moveSpeed * airplane->velocity * speedMultiplier + airplane->velocity);
		airplane->GetModel()->GetNode(4)->RotateLocalZ(airplane->GetPropellerCurrentRPM() / speedMultiplier);
	}
}

// Update the Heads Up Display elements with passed in font.
void UpdateHUD(IFont* myFont, IAirplane* airplane, int totalFrames)
{
	// Redo the displayed value limitation - round up before turning to string.
	myFont->Draw("Frames: " + to_string(totalFrames), 0, 0);
	myFont->Draw("Forward Velocity: " + to_string(airplane->GetVelocity()).substr(0, 4), 0, 40); // Limit display to 4 characters, including decimal point.
	myFont->Draw("Air Resistance: " + to_string(static_cast<int>(airplane->GetAirResistance())), 0, 80);
	myFont->Draw("RPM: " + to_string(static_cast<int>(airplane->GetPropellerCurrentRPM())), 0, 120);
	myFont->Draw("Max RPM: " + to_string(airplane->GetPropellerMaxRPM()), 0, 160);
	myFont->Draw("Pitch: " + to_string(static_cast<int>(airplane->GetRotationX()) % 360), 0, 200);
	myFont->Draw("Yaw: " + to_string(static_cast<int>(airplane->GetRotationY()) % 360), 0, 240);
	myFont->Draw("Roll: " + to_string(static_cast<int>(airplane->GetRotationZ()) % 360), 0, 280);
	myFont->Draw("Thrust: " + to_string(static_cast<int>(airplane->GetPropellerThrust())), 0, 320);
	myFont->Draw("Lift Force: " + to_string(static_cast<int>(airplane->GetUpwardsLift())), 0, 360);
	myFont->Draw("Down Force: " + to_string(static_cast<int>(airplane->GetDownwardsLift())), 0, 400);
	myFont->Draw("Resultant Lift: " + to_string(static_cast<int>(airplane->GetResultantLift())), 0, 440);
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

	// Airplane object with starting values.
	IAirplane* airplane = new IAirplane();
	airplane->SetModel(myEngine->LoadMesh("sopwith-camel.x")->CreateModel());
	// The values start at 0.
	airplane->ChangeMass(250.0);
	airplane->ChangeWeight(airplane->GetMass() * gravity);
	airplane->ChangeDownwardsLift(airplane->GetWeight());
	airplane->ChangeDragCoefficient(0.025);
	airplane->ChangeArea(30.0);
	airplane->ChangeWingArea(60.0);
	airplane->ChangePropellerDiameter(2.0);
	airplane->ChangePropellerMaxRPM(600);

	// Offset the X rotation of the plane by this value to make sure front and back wheels are touching the ground.
	constexpr int rotationOffset = -15;
	airplane->GetModel()->RotateLocalX(rotationOffset);
	airplane->ChangeRotationX(rotationOffset);

	// Offset the ground by this value for the top of the ground to be at Y = 0;
	constexpr float groundOffset = -1.05;
	// Floor model based on floor mesh.
	IModel* floorModel = myEngine->LoadMesh("Floor.x")->CreateModel(0, groundOffset, 0);
	// Change ground texture.
	floorModel->SetSkin("ground_01.jpg");
	
	// Offset the skybox by this value.
	constexpr int skyboxOffset = -940;
	const IModel* skyboxModel = myEngine->LoadMesh("Skybox 01.x")->CreateModel(0, skyboxOffset, 0);

	// kManual camera attached to parent airplane.
	ICamera* myCamera = myEngine->CreateCamera(kManual);
	myCamera->AttachToParent(airplane->GetModel());
	// The position of the camera relative to the plane model.
	const float cameraPlaneOffset[] = { 0.0, -8.0, 14.0 };
	myCamera->MoveLocal(cameraPlaneOffset[0], cameraPlaneOffset[1], cameraPlaneOffset[2]);

	// Define is the game paused.
	bool isPaused = false;

	// Define is mouse captured by engine.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();

	// The amount of frames since engine started.
	int totalFrames = 0;

	// Create variables used to limit FPS.
	std::chrono::system_clock::time_point timePointA = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point timePointB = std::chrono::system_clock::now();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		if (true)
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
		}	

		if (!isPaused)
		{
			// Draw the scene
			myEngine->DrawScene();

			/**** Update your scene each frame here ****/

			// Control keyboard input.
			if (myEngine->AnyKeyHit())
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

			// Move the object with keyboard.
			ControlAirplane(airplane);
			totalFrames++;
			airplane->UpdateAirDensityAroundPlane();
			airplane->UpdateAirResistance();
			airplane->UpdatePropellerThrust();
			airplane->UpdatePlaneVelocity();
			airplane->UpdatePlaneUpwardsLift();
			//airplane->UpdatePlaneDownwardsLift();
			airplane->UpdatePlaneResultantLift();
			airplane->MovePlane();
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
