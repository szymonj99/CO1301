// Szymon Janusz

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

constexpr float kGameSpeed = 0.1f; // Dictate the speed of the game and how quickly objects move.
constexpr float kSphereDiameter = 20.5f; // Dictates the sphere's diameter.
constexpr float kSphereRadius = kSphereDiameter / 2.0f; // Dictates the sphere's radius.
constexpr float kMaxHeightMovementUnits = 30.0f; // How many units the spheres can move vertically.
constexpr float kMaxHeight = kMaxHeightMovementUnits + kSphereRadius; // Dictates the maximum vertical height the balls can climb to.
constexpr float kPi = 3.1416f; // Dictates the constant pi.
constexpr float kVerticalMultiplier = 0.15f; // Declare the vertical speed multiplier for the spheres.
constexpr float kMaxSphereSpeedMultiplier = 5.0f; // Declare what to multiply the kGameSpeed by to get max possible sphere speed.
constexpr float kMaxSphereSpeed = kGameSpeed * kMaxSphereSpeedMultiplier; // Declare the maximum speed of spheres.
constexpr float kMinSphereSpeedMultiplier = 2.0f; // Declare what to divide kGameSpeed by to get minimum sphere speed.
constexpr float kMinSphereSpeed = kGameSpeed / kMinSphereSpeedMultiplier; // Declare the minimum speed of spheres.
constexpr float kSphereSpeedIncrementMultiplier = 10.0f; // What to multiply game speed by to get the speed increment.
constexpr float kSphereSpeedIncrement = kGameSpeed / kSphereSpeedIncrementMultiplier; // Declare the increments in which sphereSpeed will be changed.
constexpr int kCircleDegrees = 360; // Declare how many degrees there are in a circle.
constexpr int kHUDStartX = 0; // Declare the starting X position for first element in HUD
constexpr int kHUDStartY = 0; // Declare the starting Y position for first element in HUD
constexpr float kDegreesToRotateByOnCorner = 90.0f; // Declare how many degrees to rotate the balls by when they reach a corner.
constexpr float kInitialSphereSpeedMultplier = 2.0f; // What to multiply/divide the starting sphere speed by.
float sphereHorizontalSpeed = kGameSpeed / kInitialSphereSpeedMultplier; // !Global variable! Spec. only wanted one variable for all the spheres.
constexpr float kReverseDirectionDegrees = 180.0f; // How many degrees to rotate the balls by when reversing direction.
constexpr int kSkinsToMoveBy = 1; // How many skins to change the ball skin by.

// How many coordinates each corner will store, (X, Y, Z) = 3;
constexpr int kCoordNumber = 3;
// Declare the 4 corners where the balls will bounce. Use arrays as these will not change.
constexpr float kTopCornerCoords[kCoordNumber]{ 0.0f, 0.0f, 100.0f };
constexpr float kRightCornerCoords[kCoordNumber]{ 50.0f, 0.0f, 50.0f };
constexpr float kBottomCornerCoords[kCoordNumber]{ 0.0f, 0.0f, 0.0f };
constexpr float kLeftCornerCoords[kCoordNumber]{ -50.0f, 0.0f, 50.0f };

// Declare the index of x y z coordinates in corner arrays
constexpr int kX = 0;
constexpr int kY = 1;
constexpr int kZ = 2;

// enum storing the 2 possible pause states.
const enum class ePauseState
{
	paused = true,
	unpaused = false
};

// enum storing the 2 possible ball directions (clockwise/anticlockwise)
const enum class eBallDirection
{
	anticlockwise = false,
	clockwise = true
};

// Store all the currently used keys
EKeyCode UpKey = Key_Up,
DownKey = Key_Down,
ChangeDirectionKey = Key_R,
ChangeSkinsKey = Key_E,
PauseKey = Key_P,
ExitKey = Key_Escape;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it. Can be marked as not_null with the gsl library.
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add media folder to load assets.
	const string kMediaFolder = "./media";
	myEngine->AddMediaFolder(kMediaFolder);

	// Load a font to draw HUD elements. Can be marked as not_null with the gsl library.
	constexpr unsigned kFontSize = 36;
	const string kFontName = "Arial";
	IFont* myFont = myEngine->LoadFont(kFontName, kFontSize);

	// Floor object made from the floor mesh (loaded only once and not stored in memory).
	const string kFloorModelFilename = "floor.x";
	const IModel* kFloorModel = myEngine->LoadMesh(kFloorModelFilename)->CreateModel();

	// Store the camera's initial position on game start, used to create the camera.
	constexpr float kStartCameraPosition[kCoordNumber]{ 0.0f, 120.0f, -200.0f };
	constexpr float kStartCameraXRotation = 20.0f;

	// Create a Manual camera to look at the field.
	ICamera* myCamera = myEngine->CreateCamera(kManual, kStartCameraPosition[kX], kStartCameraPosition[kY], kStartCameraPosition[kZ]);
	myCamera->RotateLocalX(kStartCameraXRotation);

	// The ball mesh used to create Ball models.
	const string kBallMeshFilename = "ball.x";
	IMesh* ballMesh = myEngine->LoadMesh(kBallMeshFilename);

	// Declare how many balls/ball parents there are.
	constexpr int kTotalBalls = 4;
	// Declare what node is changed to turn ball parent invisible.
	constexpr int kBallSkinNode = 1;
	// Declare the needed scale to turn parent invisible.
	constexpr float kBallSkinScale = 0.0f;

	// Set the possible skins for the Balls.
	const string kBallSkinOne = "blue.png";
	const string kBallSkinTwo = "violet.png";
	const string kBallSkinThree = "fawn.png";
	const string kBallSkinFour = "indigo.png";
	const string kBallSkins[kTotalBalls]{ kBallSkinOne, kBallSkinTwo, kBallSkinThree, kBallSkinFour };

	// Create four parents, add them to an array and make them invisible.
	IModel* ball1Parent = ballMesh->CreateModel(kTopCornerCoords[kX], kTopCornerCoords[kY] + kSphereRadius, kTopCornerCoords[kZ]);
	IModel* ball2Parent = ballMesh->CreateModel(kRightCornerCoords[kX], kRightCornerCoords[kY] + kSphereRadius, kRightCornerCoords[kZ]);
	IModel* ball3Parent = ballMesh->CreateModel(kBottomCornerCoords[kX], kBottomCornerCoords[kY] + kSphereRadius, kBottomCornerCoords[kZ]);
	IModel* ball4Parent = ballMesh->CreateModel(kLeftCornerCoords[kX], kLeftCornerCoords[kY] + kSphereRadius, kLeftCornerCoords[kZ]);
	IModel* parentArray[kTotalBalls]{ ball1Parent, ball2Parent, ball3Parent, ball4Parent };

	// Set the ball parent skin node to be invisible.
	for (IModel* ballParent : parentArray)
	{
		ballParent->GetNode(kBallSkinNode)->Scale(kBallSkinScale);
	}

	// Create the array storing initial ball angles, starting with the ball1 angle placed at top corner and going clockwise.
	//constexpr float kInitialBallAngles[kTotalBalls]{ 135.0f, 225.0f, 315.0f, 45.0f };
	constexpr float kInitialBallAngles[kTotalBalls]{ 45.0f, 135.0f, 225.0f, 315.0f };

	// Create four balls, add them to an array and attach to parent.
	IModel* ball1 = ballMesh->CreateModel();
	IModel* ball2 = ballMesh->CreateModel();
	IModel* ball3 = ballMesh->CreateModel();
	IModel* ball4 = ballMesh->CreateModel();
	IModel* ballArray[kTotalBalls]{ ball1, ball2, ball3, ball4 };

	// Attach balls to parent, change the ball skin and rotate the parent.
	int currentPositionIndex = 0;
	for (IModel* ball : ballArray)
	{
		ball->AttachToParent(parentArray[currentPositionIndex]);
		ball->SetSkin(kBallSkins[currentPositionIndex]);
		parentArray[currentPositionIndex]->RotateY(kInitialBallAngles[currentPositionIndex]);
		currentPositionIndex++;
	}

	// The index of ball1's skin in kBallSkins, used when switching textures.
	int currentBall1SkinIndex = 0;

	// Use an enum to store ball direction
	eBallDirection ballDirection = eBallDirection::clockwise;

	// To be able to evaluate mouse wheel movement multiple times, save it to a variable.
	float mouseWheelMovement = 0.0f;

	// Is the game currently paused.
	ePauseState isPaused = ePauseState::unpaused;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		if (!static_cast<bool>(isPaused))
		{
			// Draw the scene
			myEngine->DrawScene();

			// Need to check all corners on all balls, else they will bounce out of sync.
			for (IModel* ballParent : parentArray)
			{
				// Check if the ball parent is touching or beyond any of the four corners, to then bounce.
				if (ballParent->GetZ() >= kTopCornerCoords[kZ] || ballParent->GetX() >= kRightCornerCoords[kX] || ballParent->GetZ() <= kBottomCornerCoords[kZ] || ballParent->GetX() <= kLeftCornerCoords[kX])
				{
					if (ballDirection == eBallDirection::clockwise)
					{
						ballParent->RotateY(kDegreesToRotateByOnCorner);
					}
					else
					{
						ballParent->RotateY(-kDegreesToRotateByOnCorner);
					}
				}
				ballParent->MoveLocalZ(kGameSpeed * sphereHorizontalSpeed);
			}
			// Keep on rotating the ball if it is on the ground.
			for (IModel* ball : ballArray)
			{
				if (ball->GetY() <= kSphereRadius)
				{
					ball->RotateLocalX((kGameSpeed * sphereHorizontalSpeed * kSphereDiameter) / kPi);
				}
			}

			// Compare the value from mouse wheel movement to decide if balls should speed up or slow down.
			mouseWheelMovement = myEngine->GetMouseWheelMovement();
			if (mouseWheelMovement != 0.0f)
			{
				// fabs method reference: Docs.microsoft.com. (2018). fabs, fabsf, fabsl. [online] Available at: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fabs-fabsf-fabsl?view=vs-2019 [Accessed 17 Nov. 2019].
				// Scroll wheel was scrolled up, so balls speed up.
				if (mouseWheelMovement > 0.0f)
				{
					// Use fabs to get approximation of floats
					if (fabs(sphereHorizontalSpeed - kMaxSphereSpeed) >= kSphereSpeedIncrement)
					{
						sphereHorizontalSpeed += kSphereSpeedIncrement;
					}
					else
					{
						sphereHorizontalSpeed = kMaxSphereSpeed;
					}
				}
				// Scroll wheel was scrolled down, so balls slow down.
				if (mouseWheelMovement < 0.0f)
				{
					// Use fabs to get approximation of floats
					if (fabs(sphereHorizontalSpeed - kMinSphereSpeed) >= kSphereSpeedIncrement)
					{
						sphereHorizontalSpeed -= kSphereSpeedIncrement;
					}
					else
					{
						sphereHorizontalSpeed = kMinSphereSpeed;
					}
				}
			}

			// Keyboard control - Key presses
			if (myEngine->AnyKeyHit())
			{
				// Toggle pause
				if (myEngine->KeyHit(PauseKey))
				{
					isPaused = ePauseState::paused;
				}
				// Quit game
				if (myEngine->KeyHit(ExitKey))
				{
					myEngine->Stop();
				}
				// Reverse direction of balls.
				if (myEngine->KeyHit(ChangeDirectionKey))
				{
					// Rotate the balls so that they're facing the opposite direction.
					for (IModel* ballParent : parentArray)
					{
						ballParent->RotateY(kReverseDirectionDegrees);
					}
					// Invert the clockwise/anticlockwise direction of ball parents.
					if (ballDirection == eBallDirection::clockwise)
					{
						ballDirection = eBallDirection::anticlockwise;
					}
					else
					{
						ballDirection = eBallDirection::clockwise;
					}
				}
				// Change the skin of the balls in a clockwise direction.
				if (myEngine->KeyHit(ChangeSkinsKey))
				{
					// The skinOffset is used to give the illusion of clockwise changing skins.
					constexpr int kSkinOffset = size(kBallSkins) - 1;
					// Declare the counter to count which skin index the ball should change to.
					int skinIndexCounter = 0;
					for (IModel* ball : ballArray)
					{
						// Switch the skins in a clockwise direction.
						ball->SetSkin(kBallSkins[(-currentBall1SkinIndex + skinIndexCounter + kSkinOffset) % size(kBallSkins)]);
						skinIndexCounter++;
					}
					// Prevent buffer overflow by limiting the values to the size of ball skin array.
					currentBall1SkinIndex = currentBall1SkinIndex++ % size(kBallSkins);
				}
			}

			// Keyboard control - Key held
			if (myEngine->AnyKeyHeld())
			{
				// Move the spheres up if up key is held and spheres are below height limit
				if (myEngine->KeyHeld(UpKey) && ball1->GetY() < kMaxHeight)
				{
					for (IModel* ball : ballArray)
					{
						ball->MoveY(kGameSpeed * kVerticalMultiplier);
					}
				}
				// Move the spheres down if down key is held and spheres are above ground
				else if (myEngine->KeyHeld(DownKey) && ball1->GetY() > kSphereRadius)
				{
					for (IModel* ball : ballArray)
					{
						ball->MoveY(-kGameSpeed * kVerticalMultiplier);
					}
				}
			}

			// Update HUD elements after every other action took place.
			myFont->Draw("Sphere speed: " + to_string(sphereHorizontalSpeed), kHUDStartX, kHUDStartY);
		}
		else
		{
			if (myEngine->AnyKeyHit())
			{
				// Toggle pause.
				if (myEngine->KeyHit(PauseKey))
				{
					isPaused = ePauseState::unpaused;
				}
				// Quit game
				if (myEngine->KeyHit(ExitKey))
				{
					myEngine->Stop();
				}
			}
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
