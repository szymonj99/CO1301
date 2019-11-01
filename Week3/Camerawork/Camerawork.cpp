// Camerawork.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

// Define constants.
const float kCameraSpeed = 0.05f;
const float kCameraRotation = 0.05f;
const float kMouseRotation = 0.05f;

void main()
{
	// Create a 3D Engine (using TLX engine)
	I3DEngine* myEngine = New3DEngine(kTLX);
	// Open a window for TLF Engine.
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	/**** Set up your scene here ****/

	// Capture mouse to window.
	bool isMouseCaptured;
	myEngine->StartMouseCapture();
	isMouseCaptured = true;

	// Create mesh and model objects.
	IMesh* gridMesh;
	IModel* grid;

	// Set meshes.
	gridMesh = myEngine->LoadMesh("Grid.x");

	// Create models.
	grid = gridMesh->CreateModel();

	// Initialise variables.
	int mouseMoveX;
	int mouseMoveY;

	//Set up a camera
	ICamera* myCamera = myEngine->CreateCamera(kManual);

	// Define is the game paused.
	bool isPaused = false;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		if (!isPaused)
		{
			// Draw the scene
			myEngine->DrawScene();

			/**** Update your scene each frame here ****/

			// As the computer we are running this on is too fast for TLEngine, we add delay.
			for (int delay = 0; delay < 1000000; delay++) { /* empty body */ }

			// Get mouse move variables from mouse input.
			mouseMoveX = myEngine->GetMouseMovementX();
			mouseMoveY = myEngine->GetMouseMovementY();

			// Rotate camera based on mouse input
			myCamera->RotateLocalY(mouseMoveX * kMouseRotation);
			myCamera->RotateLocalX(mouseMoveY * kMouseRotation);

			// Handle keyboard control.
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

			// Move the Camera with WASDQE keys. 
			// The movement is relative to camera.
			if (myEngine->KeyHeld(Key_W))
			{
				// Move camera into the screen.
				myCamera->MoveLocalZ(kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_S))
			{
				// Move camera away from the screen.
				myCamera->MoveLocalZ(-kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_A))
			{
				// Move camera left.
				myCamera->MoveLocalX(-kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_D))
			{
				// Move camera right.
				myCamera->MoveLocalX(kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_Q))
			{
				// Move camera up.
				myCamera->MoveLocalY(kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_E))
			{
				// Move camera down.
				myCamera->MoveLocalY(-kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_Z))
			{
				// Rotate camera left.
				myCamera->RotateLocalY(-kCameraSpeed);
			}
			if (myEngine->KeyHeld(Key_X))
			{
				// Rotate camera right.
				myCamera->RotateLocalY(kCameraSpeed);
			}
			if (myEngine->KeyHit(Key_Tab))
			{
				// Toggle mouse capture.
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
			// As the computer we are running this on is too fast for TLEngine, we add delay.
			for (int delay = 0; delay < 1000000; delay++) { /* empty body */ }

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