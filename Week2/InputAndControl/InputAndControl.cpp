// InputAndControl.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	/**** Set up your scene here ****/

	// Create mesh and model objects.
	IMesh* cubeMesh;
	IModel* cube;

	// Set meshes.
	cubeMesh = myEngine->LoadMesh("Cube.x");

	// Create models.
	cube = cubeMesh->CreateModel();

	// Create FPS-Style camera.
	ICamera* myCamera;
	myCamera = myEngine->CreateCamera(kFPS);

	// Define variables.
	const double moveSpeed = 0.05;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		// As the computer we are running this on is too fast for TLEngine, we add delay.
		for (int delay = 0; delay < 1000000; delay++) { /* empty body */ }

		// Keyboard control.
		// Exit game.
		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}
		// Move the cube with WASDQE keys. 
		// The movement is relative to the screen at 0,0,0 not the camera.
		if (myEngine->KeyHeld(Key_W))
		{
			// Move cube into the screen.
			cube->MoveZ(moveSpeed);
		}
		if (myEngine->KeyHeld(Key_S))
		{
			// Move cube away from the screen.
			cube->MoveZ(-moveSpeed);
		}
		if (myEngine->KeyHeld(Key_A))
		{
			// Move cube left.
			cube->MoveX(-moveSpeed);
		}
		if (myEngine->KeyHeld(Key_D))
		{
			// Move cube right.
			cube->MoveX(moveSpeed);
		}
		if (myEngine->KeyHeld(Key_Q))
		{
			// Move cube up.
			cube->MoveY(moveSpeed);
		}
		if (myEngine->KeyHeld(Key_E))
		{
			// Move cube down.
			cube->MoveY(-moveSpeed);
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
