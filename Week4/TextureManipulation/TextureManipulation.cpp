// TextureManipulation.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

// Create some variables
const float kCameraSpeed = 0.05f;
const float kCameraRotation = 0.05f;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );

	/**** Set up your scene here ****/

	// Create mesh and model objects
	IMesh* sphereMesh;
	IModel* sphere;

	// Change mesh
	sphereMesh = myEngine->LoadMesh("Sphere.x");

	// Create model
	sphere = sphereMesh->CreateModel();

	// Create camera
	const ICamera* myCamera = myEngine->CreateCamera(kFPS);

	// Change sphere texture
	sphere->SetSkin("Jupiter.jpg");

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
