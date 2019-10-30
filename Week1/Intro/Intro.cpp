// Intro.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Intro.h"
using namespace tle;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\Programs\\TL-Engine\\Media");

	/**** Set up your scene here ****/

	// Create mesh and model objects.
	IMesh* cubeMesh;
	IModel* cube;
	IMesh* sphereMesh;
	IModel* sphere;

	// Set meshes.
	cubeMesh = myEngine->LoadMesh("Cube.x");
	sphereMesh = myEngine->LoadMesh("Sphere.x");

	// Create models.
	cube = cubeMesh->CreateModel();
	sphere = sphereMesh->CreateModel();

	// Create FPS-Style camera.
	ICamera* myCamera;
	myCamera = myEngine->CreateCamera(kFPS);

	// Define variables
	const double rotationSpeed = 0.05; // How quickly to rotate object.
	const double maxHeight = 30.0; // Max height object can reach before moving down to 0.
	const double moveSpeed = 0.05; // How fast object moves up.
	const double maxX = 40.0; // Max positive X sphere can reach.
	const double minX = -40.0; // Max negative X sphere can reach.

	// Define direction for sphere.
	string moveDirection = "right";
	

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/
		// As the computer we are running this on is too fast for TLEngine, we add delay.
		for (int delay = 0; delay < 1000000; delay++) { /* empty body */ }

		// Rotate cube.
		cube->RotateY(rotationSpeed);
		cube->RotateX(rotationSpeed);
		cube->RotateZ(rotationSpeed);

		// Move cube towards Y = maxPos, then back to initialPos.
		cube->MoveY(moveSpeed);
		if (cube->GetY() > maxHeight)
		{
			// Reset cube Y position.
			cube->SetY(0);
		}

		// Move sphere in the sphereDirection
		if (moveDirection == "right")
		{
			sphere->MoveX(moveSpeed);
		}
		else
		{
			sphere->MoveX(-moveSpeed);
		}

		// Bounce sphere between X = maxX and X = minX.
		if (sphere->GetX() > maxX)
		{
			moveDirection = "left";
		}
		else if (sphere->GetX() < minX)
		{
			moveDirection = "right";
		}

		// Rotate sphere.
		const int speedMultiplier = 4;
		sphere->RotateY(rotationSpeed * speedMultiplier);
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}