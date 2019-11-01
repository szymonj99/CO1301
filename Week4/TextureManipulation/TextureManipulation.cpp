// TextureManipulation.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

// Create some constants
const float kCameraSpeed = 0.05f;
const float kCameraRotation = 0.05f;
const int minX = -40;
const int maxX = 40;
const float moveSpeed = 0.05;
const float rotationSpeed = 0.05;
const int speedMultiplier = 4;

// Get the sphere texture based on whether or not it's cloudy.
string GetSphereTexture(bool isCloudy)
{
	// Set up cloudy and clear texture.
	string cloudy = "Clouds.jpg";
	string clear = "EarthPlain.jpg";

	if (isCloudy)
	{
		return cloudy;
	}
	else
	{
		return clear;
	}
}

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	// Load "Comic Sans MS" font at 36 points
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);
	
	/**** Set up your scene here ****/

	// Create mesh and model objects
	IMesh* sphereMesh;
	IModel* sphere;
	IMesh* gridMesh;
	IModel* grid;

	// Set meshes
	sphereMesh = myEngine->LoadMesh("Sphere.x");
	gridMesh = myEngine->LoadMesh("Grid.x");

	// Create models
	int sphereHeightPos = 10;
	sphere = sphereMesh->CreateModel(0, sphereHeightPos, 0);
	grid = gridMesh->CreateModel();

	// Create camera
	const ICamera* myCamera = myEngine->CreateCamera(kFPS);

	// Change sphere texture
	sphere->SetSkin("EarthPlain.jpg");

	// Set is sphere cloudy
	bool sphereCloudy = false;

	// Set direction of sphere
	static string sphereDirection = "right";

	// Define is the game paused.
	bool isPaused = false;

	// Define is mouse captured by engine and capture the mouse immediately.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();

	// Define amount of frames since engine started.
	int totalFrames = 0;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// As the computer we are running this on is too fast for TLEngine, we add delay.
		for (int delay = 0; delay < 1000000; delay++) { /* empty body */ }

		if (!isPaused)
		{
			// Draw the scene
			myEngine->DrawScene();

			// Increment totalFrames
			totalFrames++;
			//Print totalFrames on screen.
			myFont->Draw(to_string(totalFrames), 0, 0);

			/**** Update your scene each frame here ****/

			// Bounce sphere between X = minX and X = maxX, roll, and change texture;
			if (sphereDirection == "right")
			{
				// If the sphere is below the maxX coordinate
				if (sphere->GetX() < maxX)
				{
					// Rotate and move towards maxX.
					sphere->RotateZ(-moveSpeed * speedMultiplier);
					sphere->MoveX(moveSpeed);
				}
				else
				{
					// Toggle the state of sphereCloudy, change texture and reverse direction.
					sphereCloudy = !sphereCloudy;
					sphere->SetSkin(GetSphereTexture(sphereCloudy));
					sphereDirection = "left";
				}
			}
			else
			{
				if (sphere->GetX() > minX)
				{
					// Rotate and move towards minX.
					sphere->RotateZ(moveSpeed * speedMultiplier);
					sphere->MoveX(-moveSpeed);
				}
				else
				{
					// Toggle the state of sphereCloudy and change texture.
					sphereCloudy = !sphereCloudy;
					sphere->SetSkin(GetSphereTexture(sphereCloudy));
					sphereDirection = "right";
				}
			}

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
		else
		{
			// Toggle isPaused.
			if (myEngine->KeyHit(Key_P))
			{
				isPaused = !isPaused;
			}
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}