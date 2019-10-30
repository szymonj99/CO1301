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
	IMesh* arrowMesh;
	IModel* arrow;

	// Set meshes.
	arrowMesh = myEngine->LoadMesh("Arrow.x");

	// Create models.
	arrow = arrowMesh->CreateModel();

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
		// Move the arrow with WASDQE keys. 
		// The movement is relative to arrow.
		if (myEngine->KeyHeld(Key_W))
		{
			// Move arrow into the screen.
			arrow->MoveLocalZ(moveSpeed);
		}
		if (myEngine->KeyHeld(Key_S))
		{
			// Move arrow away from the screen.
			arrow->MoveLocalZ(-moveSpeed);
		}
		if (myEngine->KeyHeld(Key_A))
		{
			// Move arrow left.
			arrow->MoveLocalX(-moveSpeed);
		}
		if (myEngine->KeyHeld(Key_D))
		{
			// Move arrow right.
			arrow->MoveLocalX(moveSpeed);
		}
		if (myEngine->KeyHeld(Key_Q))
		{
			// Move arrow up.
			arrow->MoveLocalY(moveSpeed);
		}
		if (myEngine->KeyHeld(Key_E))
		{
			// Move arrow down.
			arrow->MoveLocalY(-moveSpeed);
		}
		if (myEngine->KeyHeld(Key_Z))
		{
			// Rotate arrow left.
			arrow->RotateLocalY(-moveSpeed);
		}
		if (myEngine->KeyHeld(Key_X))
		{
			// Rotate arrow right.
			arrow->RotateLocalY(moveSpeed);
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
