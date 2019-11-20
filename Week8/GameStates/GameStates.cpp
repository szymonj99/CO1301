#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

// How quickly should the particle turn in degrees per second
constexpr float speed = 0.05;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media");

	/**** Set up your scene here ****/
	myEngine->CreateCamera(kManual, 0, 0, -40);

	// Create font
	IFont* myFont = myEngine->LoadFont("Arial", 36);

	// Particle mesh, to be used as stars.
	IMesh* particleMesh = myEngine->LoadMesh("Particle.x");

	// Create particle model.
	IModel* particleModel = particleMesh->CreateModel();

	// Set up total frame counter.
	int totalFrames = 0;

	// Define is the game paused.
	bool isPaused = false;

	// Define is mouse captured by engine.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();

	// Define is the particle shown
	enum possibleParticleStates { shown, hidden };
	possibleParticleStates particleState = shown;

	// Define array storing possible textures
	string possibleTextures[7];
	possibleTextures[0] = "006_tlxadd.tga";
	possibleTextures[1] = "007_tlxadd.tga";
	possibleTextures[2] = "008_tlxadd.tga";
	possibleTextures[3] = "009_tlxadd.tga";
	possibleTextures[4] = "010_tlxadd.tga";
	possibleTextures[5] = "011_tlxadd.tga";
	possibleTextures[6] = "012_tlxadd.tga";

	// Declare what index is used to get texture.
	int currentTextureIndex = 0;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		if (!isPaused)
		{
			// Draw the scene
			myEngine->DrawScene();

			/**** Update your scene each frame here ****/

			// Increment totalFrames.
			totalFrames++;

			// Draw HUD elements.
			myFont->Draw("Frames: " + to_string(totalFrames), 0, 0);

			// Keyboard control.
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
				// Toggle particle state.
				if (myEngine->KeyHit(Key_Return))
				{
					if (particleState == shown)
					{
						particleState = hidden;
						particleMesh->RemoveModel(particleModel);
					}
					else
					{
						particleState = shown;
						particleModel = particleMesh->CreateModel();
						currentTextureIndex++;
						if (currentTextureIndex > possibleTextures->capacity() - 2)
						{
							currentTextureIndex = 0;
						}
						particleModel->SetSkin(possibleTextures[currentTextureIndex]);
					}
				}
			}
			if (particleState == shown)
			{
				// Rotate particleModel along Z axis.
				particleModel->RotateLocalZ(speed);
			}
		}
		else
		{
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
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}