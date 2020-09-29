// Szymon Janusz

// Used to get constant Pi
#define _USE_MATH_DEFINES

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <string>
#include <cmath> // Using cmath for C++, rather than math for C.
#include <vector>
#include <iostream>

using namespace tle;

// Global constants
const float kGameSpeed = 10.0f; // How many units an object will move per second when multiplied by frametime.
const float kGameSeconds = 20.0f; // How many seconds the player has per frog.
const float kMapMinX = -50.0f; // The minimum (left hand side) coordinate of the map width.
const float kMapMaxX = 50.0f; // The maximum (right hand side) coordinate of the map width.
const float kMapMaxZ = 115.0f; // The maximum (forward) coordinate of the map length.
const float kMapMinZ = 15.0f; // The minimum (backward) coordinate of the map length.
const float kMapGroundLevel = 0.0f; // The Y coordinate of the ground.
const float kIslandOneMaxZ = 70.0f; // How far does island 1 reach
const float kTiresMaxZ = 110.0f; // How far do the tires extend to
const float kTyreSpeedMult = 1.0f; // Speed multiplier relative to kGameSpeed.
const float kFrogRadius = 4.95f; // Declare the radius of the frog to use for collision detection
const string kFrogHitSkin = "frog_red.jpg";	// Set skin for when frog is hit
const string kFrogSkinSafe = "frog_blue.jpg"; // Set skin for when frog is safe
const string kFrogDefaultSkin = "frog.jpg"; // Default skin of the frog.
const float kFrogDeadScale = 0.05f; // How much the frog gets flattened by when driven over by a vehicle
const float kFrogSpeedMult = 3.0f; // Speed relative to kGameSpeed
const float kFrogToTyreRatio = 0.1f; // How much smaller the frog is in comparison to tire scale
const float kBoxRadiusX = 6.0f;	// Declare the radius in the X axis of the box obstacles to use for collision detection.
const float kBoxRadiusZ = 2.5f;	// Declare the radius in the Z axis of the box obstacles to use for collision detection.
const float kBelowGroundY = -3000.0f; // Used to hide objects beneath the surface.
const float kHideTime = 2.0f; // Time in seconds to hide obstacles for when they reach edge of map.
const float kTyreRadius = 4.95f; // The radius of the tire.
const float kTyreScale = 1.0f / kFrogToTyreRatio; // Scale of the tires
const float kTyreYPos = -2.5f; // The Y cooridnate of the middle of the tyre
const EKeyCode GameExitKey = Key_Escape;
const EKeyCode GamePauseKey = Key_P;
const EKeyCode FrogMoveLeft = Key_Z;
const EKeyCode FrogMoveRight = Key_X;
const EKeyCode FrogMoveForward = static_cast<EKeyCode>(192); //  Character = `
const EKeyCode FrogMoveBackward = static_cast<EKeyCode>(191); // Character = /
const EKeyCode CameraRotateUp = Key_Up;
const EKeyCode CameraRotateDown = Key_Down;
const EKeyCode CameraRotateReset = Key_C;
const float kCameraMaxRotation = 90.0f; // Degrees relative from 0
const float kCameraMinRotation = 0.0f; // Looking straight ahead, along a flat horizontal plane
const float kCameraInitRotation = 20.0f; // Initial rotation of the camera
const float kCameraRotationSpeed = 45.0f; // How many degrees to rotate the camera by per second
const int k3DCoords = 3; // umber of coordinates in 3D-Space
const float kPlantRadius = 2.5;
const float kCameraInitPos[]{ 0.0f, 40.0f, -60.0f }; // Initial position of the camera. Used when resetting the camera position.
const float kFrogStartingZ = 15.0f;

// Enum Declarations

// Possible states for the game to be in
enum EGameStates
{
	playing,
	paused,
	over
};

// Possible states for frogs to be in
enum EFrogStates
{
	waiting,
	crossing,
	safe,
	dead
};

// Possible object rotation, relative from global Z axis.
enum ERotation
{
	rotationTop = 0,
	rotationLeft = 90,
	rotationBottom = 180,
	rotationRight = 270
};

// Possible states for Objects to be in
enum EObjectStates
{
	shown = true,
	hidden = false
};

// Possible ways for frog to die
enum EFrogDeathCause
{
	deathVehicle,
	deathWater,
	deathTime
};

// Struct Declarations

struct SObject
{
	IModel* model; // Object's model
	ERotation rotation; // Object's rotation/direction
	float reappearX; // X position of where the object will reappear when it goes out of bounds
	float reappearY; // Y position of where the object will reappear when it goes out of bounds
	float reappearZ; // Z position of where the object will reappear when it goes out of bounds
	float speedMult; // How fast the object is in relation to game speed multiplier

	// Change the rotation state and automatically rotate the model along the Y axis.
	void SetRotation(ERotation newRotation)
	{
		if (rotation != newRotation)
		{
			model->RotateY(rotation - newRotation);
			rotation = newRotation;
		}
	}

	// Reset the model position to the reappear value.
	void ResetPosition()
	{
		model->SetPosition(reappearX, reappearY, reappearZ);
	}
};

struct SVehicle : SObject
{
	EObjectStates state; // Object's state
	float timeToHide = 0.0f; // How much longer to hide the object for.

	// Move the obstacle, depending on the direction
	void Move(const float& frametime)
	{
		// Check if object is hidden first.
		if (state == shown)
		{
			// Move the object if it's shown and within bounds of map
			model->MoveLocalZ(speedMult * kGameSpeed * frametime);
			// If object is facing left, need to check the minimum map x coord.
			if (rotation == rotationLeft && model->GetX() < kMapMinX)
			{
				state = hidden;
				model->MoveY(kBelowGroundY);
				model->MoveZ(-kBelowGroundY); // Move the obstacle away from the spot where it disappears, to prevent collision with frog in x and z axis (2d plane)
				timeToHide = kHideTime;
			}
			// Check obstacle direction and whether or not it's out of bounds of map.
			else if (rotation == rotationRight && model->GetX() > kMapMaxX)
			{
				state = hidden;
				model->MoveY(kBelowGroundY);
				model->MoveZ(-kBelowGroundY); // Move the obstacle away from the spot where it disappears, to prevent collision with frog in x and z axis (2d plane)
				timeToHide = kHideTime;
			}
		}
		else if (state == hidden)
		{
			timeToHide -= frametime;
			if (timeToHide < 0.0f)
			{
				timeToHide = 0.0f;
				state = shown;
				ResetPosition();
			}
		}
	}
};

struct STyre : SObject
{
	EObjectStates state; // Object's state
	int index; // The tyre's index in the current lane, from left to right. 0 == upmost left tyre.
	float timeToHide = 0.0f; // How much longer to hide the object for.

	// Move the tyre from side to side.
	void Move(const float& frametime)
	{
		// As there are 3 tyres in a row
		const int kTyresInRow = 3;
		const float kTyreDiameter = kTyreRadius * 2.0f;
		if (rotation == rotationRight)
		{
			model->MoveX(frametime * kGameSpeed * kTyreSpeedMult);
			// The smaller the index, the closer to X = 0 the tyre will return
			if (model->GetX() > kMapMaxX - (((kTyresInRow - 1) - index) * kTyreDiameter))
			{
				rotation = rotationLeft;
			}
		}
		else if (rotation == rotationLeft)
		{
			model->MoveX(-frametime * kGameSpeed * kTyreSpeedMult);
			// The bigger the index, the closer to X = 0 the tyre will return.
			if (model->GetX() < kMapMinX + (index * kTyreDiameter))
			{
				rotation = rotationRight;
			}
		}
	}
};

// Frog struct that the player will control
struct SFrog : SObject
{
	int index;
	bool isJumping;
	EFrogStates state;
	float currentJumpLength = 0.0f;
	float maxJumpLength = 10.0f;
	float jumpHeightMultiplier = 10.0f;
	float maxDist = 0.0f; // How far along the map did the frog go.
	float secondsLeftAlive = kGameSeconds; // How much longer the frog will be alive for
	bool isAttached = false;

	// Make the frog jump in the Z and X axis in a certain direction
	void BeginJump(ERotation direction)
	{
		// Check if the frog can move in the specified direction and is currently not moving.
		if (IsInBounds(direction) && !isJumping)
		{
			// Make the frog aligned to the direction user wants to go.
			SetRotation(direction);
			isJumping = true;
			isAttached = false;
		}
	}

	// Check if the frog is in bounds of the map and can move in the specified direction.
	bool IsInBounds(ERotation direction)
	{
		bool inBounds = false;
		switch (direction)
		{
		case rotationTop:
			inBounds = (model->GetZ() + kFrogRadius < kMapMaxZ) ? true : false;
			break;
		case rotationLeft:
			inBounds = (model->GetX() > kMapMinX + kFrogRadius) ? true : false;
			break;
		case rotationBottom:
			inBounds = (model->GetZ() > kMapMinZ + kFrogRadius) ? true : false;
			break;
		case rotationRight:
			inBounds = (model->GetX() + kFrogRadius < kMapMaxX) ? true : false;
			break;
		default:
			break;
		}
		return inBounds;
	}

	// The current frog should die
	void Kill(const EFrogDeathCause kCauseOfDeath)
	{
		model->SetSkin(kFrogHitSkin);
		if (kCauseOfDeath == deathVehicle)
			model->ScaleY(kFrogDeadScale);
		model->SetY(kMapGroundLevel);
		state = dead;
	}

	// The current frog is now safesni
	void ReachedSafety()
	{
		state = safe;
		model->SetSkin(kFrogSkinSafe);
	}

	// Update the frog position whilst it is jumping
	void UpdateJump(const float& frametime)
	{
		// Move the frog forward on the local Z axis, until the sin wave <= 0.0f.
		float moveDistance = frametime * kGameSpeed * speedMult;
		model->MoveLocalZ(moveDistance);
		currentJumpLength += moveDistance;
		const float kDistanceY = sinf((currentJumpLength / maxJumpLength) * M_PI) * jumpHeightMultiplier;
		model->SetY(kDistanceY);

		// sine wave = 0 when radians = 0, radians = pi, and radians = 2pi.
		if (kDistanceY <= 0.0f)
		{
			isJumping = !isJumping;
			currentJumpLength = 0.0f;
		}
	}
};

// Function Declarations

IFont* CreateCustomFont(I3DEngine& myEngine);
const IModel* CreateSkybox(I3DEngine& myEngine);
const IModel* CreateWaterSurface(I3DEngine& myEngine);
const IModel* CreateIsland(I3DEngine& myEngine, const int kIslandNum);
ICamera* CreateCustomCamera(I3DEngine& myEngine);
bool IsSphereBoxCollided(const SFrog& sphere, const SVehicle& box);
void EndGame(EGameStates& GameState) noexcept;
bool TryIncrementCurrentFrog(unsigned int& CurrentFrogIndex, vector<SFrog>& Frogs, SFrog& CurrentFrog, IModel* dummy);
vector<SFrog> CreateFrogs(I3DEngine& myEngine);
vector<STyre> CreateTires(I3DEngine& myEngine);
void CreateVehicles(vector<SVehicle>& vehicles, I3DEngine& myEngine, const int& kAmount, const float(&kStartPos)[k3DCoords], const float& kOffset, const string& kMeshFile, const float& kSpeedMult, const ERotation& kRotation);
bool IsSphereSphereCollided(const IModel* sphere1, const IModel* sphere2, const float& kSphere1Radius, const float& kSphere2Radius);
void AttachDummyToTyre(IModel* dummy, const STyre& tyre);
void DetachDummyFromTyre(IModel* dummy);
void CreatePlants(vector<SObject>& plants, I3DEngine& myEngine);
void RotateCameraUp(const float& frametime, float& currentCameraRotation, IModel* dummy);
void RotateCameraDown(const float& frametime, float& currentCameraRotation, IModel* dummy);

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add media folder
	const string kMediaFolder = "./media";
	myEngine->AddMediaFolder(kMediaFolder);

	// The current state of the game
	EGameStates GameState = playing;

	// Set up the scene
	IFont* myFont = CreateCustomFont(*myEngine); // Import font to draw HUD later
	const IModel* kSkybox = CreateSkybox(*myEngine); // Create skybox
	const IModel* kWaterSurface = CreateWaterSurface(*myEngine); // Create a surface of water
	int islandNum = 1; // Current island being generated.
	const IModel* const kIslands[]{ CreateIsland(*myEngine, islandNum++), CreateIsland(*myEngine, islandNum) }; // Islands for frogs to walk on
	ICamera* myCamera = CreateCustomCamera(*myEngine); // Create a manual camera to later control manually
	float currentCameraRotation = kCameraInitRotation; // What is the current angle of the camera.

	// Create the frogs
	vector<SFrog> Frogs = CreateFrogs(*myEngine);
	// The current frog from the SFrog vector.
	// Starts at the 0th item.
	SFrog CurrentFrog = Frogs.front();
	unsigned int CurrentFrogIndex = 0;
	CurrentFrog.model->SetSkin(kFrogHitSkin, 0);
	CurrentFrog.model->SetSkin(kFrogSkinSafe, 1);
	CurrentFrog.model->SetSkin(kFrogDefaultSkin, 0);
	CurrentFrog.model->SetSkin(kFrogDefaultSkin, 1);

	// Store all the moving obstacles such as vehicles in lanes in one vector
	vector<SVehicle> Vehicles;

	// Define transit constants
	const int kTransitNum = 4;
	const string kTransitMeshFilename = "transit.x";
	const float kTransitStartPos[]{ 50.0f, 0.0f, 35.0f };
	const float kTransitXOffset = -25.0f; // Value added to kTransitStartX for each transit.
	const float kTransitSpeedMult = 1.0f;
	const ERotation kTransitDirection = rotationLeft;
	// Create transit vehicles
	CreateVehicles(Vehicles, *myEngine, kTransitNum, kTransitStartPos, kTransitXOffset, kTransitMeshFilename, kTransitSpeedMult, kTransitDirection);

	// Define Rover constants
	const int kRoverNum = 2;
	const string kRoverMeshFilename = "rover.x";
	const float kRoverStartPos[]{ -50.0f, 0.0f, 45.0f };
	const float kRoverXOffset = 25.0f; // Value added to kRoverStartX for each rover.
	const float kRoverSpeedMult = kTransitSpeedMult * 1.5f;
	const ERotation kRoverDirection = rotationRight;
	// Create rover vehicles
	CreateVehicles(Vehicles, *myEngine, kRoverNum, kRoverStartPos, kRoverXOffset, kRoverMeshFilename, kRoverSpeedMult, kRoverDirection);

	// Define jeep constants
	const int kJeepNum = 2;
	const string kJeepMeshFilename = "rangeRover.x";
	const float kJeepXOffset = 25.0f; // Value added to kJeepStartX for each jeep.
	const float kJeepStartPos[]{ -50.0f, 0.0f, 25.0f };
	const ERotation kJeepDirection = rotationRight;
	const float kJeepSpeedMult = kTransitSpeedMult / 2.0f;
	// Create Jeep vehicles.
	CreateVehicles(Vehicles, *myEngine, kJeepNum, kJeepStartPos, kJeepXOffset, kJeepMeshFilename, kJeepSpeedMult, kJeepDirection);

	// Define pickup constants
	const int kPickupNum = 3;
	const string kPickupMeshFilename = "pickUp.x";
	const float kPickupXOffset = -30.0f;
	const float kPickupStartPos[]{ 50.0f, 0.0f, 55.0f };
	const ERotation kPickupDirection = rotationLeft;
	const float kPickupSpeedMult = 0.75f;
	CreateVehicles(Vehicles, *myEngine, kPickupNum, kPickupStartPos, kPickupXOffset, kPickupMeshFilename, kPickupSpeedMult, kPickupDirection);

	// Create plants on the end island.
	vector<SObject> plants;
	CreatePlants(plants, *myEngine);

	// Create a dummy for the frog and camera;
	const string kDummyMeshFile = "dummy.x";
	IMesh* dummyMesh = myEngine->LoadMesh(kDummyMeshFile);
	IModel* dummyModel = dummyMesh->CreateModel();

	IModel* cameraDummy = dummyMesh->CreateModel();
	cameraDummy->AttachToParent(dummyModel);

	CurrentFrog.model->AttachToParent(dummyModel);
	myCamera->AttachToParent(cameraDummy);

	// Create tires joining two islands for frogs to walk on
	vector<STyre> tires = CreateTires(*myEngine);

	bool IsDummyAttachedToTyre = false;

	// The player's current score.
	float currentScore = 0.0f;

	// Time between each frame.
	// Used to ensure consistent object movement.
	float frametime = 0.0f;

	// Call this once before the main loop to
	// prevent loading times from subtracting time from secondsLeft.
	myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the frame no matter the game state
		myEngine->DrawScene();

		// Update the frametime every time function is true to eliminate stuttering vehicles when switching game states.
		frametime = myEngine->Timer();

		if (GameState == playing)
		{
			// Move current frog only
			if (!CurrentFrog.isJumping)
			{
				if (myEngine->KeyHit(FrogMoveForward))
				{
					CurrentFrog.BeginJump(rotationTop);
					if (IsDummyAttachedToTyre)
					{
						DetachDummyFromTyre(dummyModel);
						IsDummyAttachedToTyre = false;
					}					
				}
				else if (myEngine->KeyHit(FrogMoveLeft))
				{
					CurrentFrog.BeginJump(rotationLeft);
					if (IsDummyAttachedToTyre)
					{
						DetachDummyFromTyre(dummyModel);
						IsDummyAttachedToTyre = false;
					}
				}
				else if (myEngine->KeyHit(FrogMoveBackward))
				{
					CurrentFrog.BeginJump(rotationBottom);
					if (IsDummyAttachedToTyre)
					{
						DetachDummyFromTyre(dummyModel);
						IsDummyAttachedToTyre = false;
					}
				}
				else if (myEngine->KeyHit(FrogMoveRight))
				{
					CurrentFrog.BeginJump(rotationRight);
					if (IsDummyAttachedToTyre)
					{
						DetachDummyFromTyre(dummyModel);
						IsDummyAttachedToTyre = false;
					}
				}
			}
			else
			{
				// Update the frog jump.
				CurrentFrog.UpdateJump(frametime);
				// Update score every frame whilst jumping forwards
				if (CurrentFrog.model->GetZ() > CurrentFrog.maxDist)
				{
					currentScore += (CurrentFrog.model->GetZ() - CurrentFrog.maxDist);
					CurrentFrog.maxDist = CurrentFrog.model->GetZ();
				}
			}

			const float kFrogX = CurrentFrog.model->GetX();
			const float kFrogY = CurrentFrog.model->GetY();
			const float kFrogZ = CurrentFrog.model->GetZ();
			dummyModel->SetPosition(kFrogX, kFrogY, kFrogZ);
			CurrentFrog.model->SetLocalPosition(0.0f, 0.0f, 0.0f);

			// Check if current frog reached the end of the map.
			if (CurrentFrog.model->GetZ() >= kMapMaxZ)
			{
				CurrentFrog.ReachedSafety();
				if (TryIncrementCurrentFrog(CurrentFrogIndex, Frogs, CurrentFrog, dummyModel));
				else
					EndGame(GameState);
			}

			// Move Obstacles no matter where frog is positioned.
			for (SVehicle& vehicle : Vehicles)
			{
				vehicle.Move(frametime);
				// Only check collisions with current frog it the frog is on the island
				if (CurrentFrog.model->GetZ() < kIslandOneMaxZ)
				{
					if (IsSphereBoxCollided(CurrentFrog, vehicle))
					{
						CurrentFrog.Kill(deathVehicle);
						if (TryIncrementCurrentFrog(CurrentFrogIndex, Frogs, CurrentFrog, dummyModel));
						else
							EndGame(GameState);
					}
				}
			}

			// Check if frog is in collision with tires
			if (!CurrentFrog.isJumping && CurrentFrog.model->GetZ() > kIslandOneMaxZ&& CurrentFrog.model->GetZ() < kMapMaxZ - kFrogRadius)
			{
				// Tripwire algorithm.
				// Only need to know if frog is colliding with one tire to know that it didn't die
				bool isCollided = false;
				for (const STyre& tyre : tires)
				{
					if (IsSphereSphereCollided(CurrentFrog.model, tyre.model, kFrogRadius, kTyreRadius))
					{
						isCollided = true;
						if (!CurrentFrog.isAttached && !IsDummyAttachedToTyre)
						{
							AttachDummyToTyre(dummyModel, tyre);
							IsDummyAttachedToTyre = true;
							CurrentFrog.isAttached = true;
							CurrentFrog.model->ResetScale();
						}
						break;
					}
				}
				// Prevents the frog from dying mid-air before even touching the water
				if (!isCollided)
				{
					CurrentFrog.Kill(deathWater);
					if (TryIncrementCurrentFrog(CurrentFrogIndex, Frogs, CurrentFrog, dummyModel));
					else
						EndGame(GameState);
				}
			}

			// Move the tires from left to right, depending on their lane.
			for (STyre& tyre : tires)
			{
				tyre.Move(frametime);
			}

			// Check plant frog collision only after frog crossed some line
			if (CurrentFrog.model->GetZ() > kMapMaxZ - (kPlantRadius * 2.0f))
			{
				for (SObject plant : plants)
				{
					if (IsSphereSphereCollided(CurrentFrog.model, plant.model, kFrogRadius, kPlantRadius))
					{
						CurrentFrog.model->MoveZ(-frametime * kFrogSpeedMult * kGameSpeed);
					}
				}
			}
			

			// Update the time left for the current frog.
			CurrentFrog.secondsLeftAlive -= frametime;
			if (CurrentFrog.secondsLeftAlive <= 0.0f)
			{
				CurrentFrog.secondsLeftAlive = kGameSeconds;
				CurrentFrog.Kill(deathTime);
				if (TryIncrementCurrentFrog(CurrentFrogIndex, Frogs, CurrentFrog, dummyModel));
				else
					EndGame(GameState);
			}

			// Draw updated time left on HUD.
			myFont->Draw("Time left: " + to_string(static_cast<int>(roundf(CurrentFrog.secondsLeftAlive))), 0, 0);
			// Draw score on the HUD
			myFont->Draw("Score: " + to_string(static_cast<int>(roundf(currentScore))), 800, 0);

			// Controls
			// Toggle Pause
			if (myEngine->KeyHit(GamePauseKey))
			{
				GameState = paused;
			}
			// Change camera rotation
			if (myEngine->KeyHeld(CameraRotateUp))
			{
				if (currentCameraRotation < kCameraMaxRotation)
				{
					RotateCameraUp(frametime, currentCameraRotation, cameraDummy);
				}
			}
			if (myEngine->KeyHeld(CameraRotateDown))
			{
				if (currentCameraRotation > kCameraMinRotation)
				{
					RotateCameraDown(frametime, currentCameraRotation, cameraDummy);
				}
			}
			if (myEngine->KeyHit(CameraRotateReset))
			{
				myCamera->SetPosition(kCameraInitPos[0] + CurrentFrog.model->GetX() , kCameraInitPos[1] + CurrentFrog.model->GetY(), kCameraInitPos[2] + (CurrentFrog.model->GetZ() - kFrogStartingZ));
			}
		}
		else if (GameState == paused)
		{
			// Display Paused message
			myFont->Draw("Time left: " + to_string(static_cast<int>(roundf(CurrentFrog.secondsLeftAlive))), 0, 0);
			myFont->Draw("Game Paused.", 400, 0);
			myFont->Draw("Score: " + to_string(static_cast<int>(roundf(currentScore))), 800, 0);

			// Controls
			// Toggle Pause
			if (myEngine->KeyHit(GamePauseKey))
			{
				GameState = playing;
			}
		}
		else
		{
			// Show game over message
			myFont->Draw("Game Over", 0, 0);
			myFont->Draw("Score: " + to_string(static_cast<int>(roundf(currentScore))), 800, 0);
		}
		// The player can exit game from any state.
		if (myEngine->KeyHit(GameExitKey))
		{
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

// Check if there is a collision between two models
bool IsSphereBoxCollided(const SFrog& sphere, const SVehicle& box)
{
	// This is a sphere to box collision model.
	// Slightly inaccurate around corners.

	const float boxX = box.model->GetX();
	const float boxMaxX = boxX + kBoxRadiusX + kFrogRadius;
	const float boxMinX = boxX - kBoxRadiusX - kFrogRadius;
	const float boxZ = box.model->GetZ();
	const float boxMaxZ = boxZ + kBoxRadiusZ + kFrogRadius;
	const float boxMinZ = boxZ - kBoxRadiusZ - kFrogRadius;

	const float sphereX = sphere.model->GetX();
	const float sphereZ = sphere.model->GetZ();

	if (sphereX < boxMaxX && sphereX > boxMinX&& sphereZ < boxMaxZ && sphereZ > boxMinZ)
		return true;
	else
		return false;
}

// End the game
void EndGame(EGameStates& GameState) noexcept
{
	GameState = over;
}

// Try to increment the current frog. Returns true if operation was successful.
bool TryIncrementCurrentFrog(unsigned int& CurrentFrogIndex, vector<SFrog>& Frogs, SFrog& CurrentFrog, IModel* dummy)
{
	dummy->DetachFromParent();
	const float kX1 = CurrentFrog.model->GetX();
	const float kY1 = CurrentFrog.model->GetY();
	const float kZ1 = CurrentFrog.model->GetZ();
	CurrentFrog.model->DetachFromParent();
	CurrentFrog.model->SetPosition(kX1, kY1, kZ1);
	CurrentFrogIndex++;
	if (CurrentFrogIndex < Frogs.size())
	{
		CurrentFrog = Frogs.at(CurrentFrogIndex);
		const float kX = CurrentFrog.model->GetX();
		const float kY = CurrentFrog.model->GetY();
		const float kZ = CurrentFrog.model->GetZ();
		dummy->SetPosition(kX, kY, kZ);
		CurrentFrog.model->AttachToParent(dummy);
		CurrentFrog.model->SetLocalPosition(0.0f, 0.0f, 0.0f);
		return true;
	}
	else
	{
		return false;
	}
}

// Create a uniform font that will be used thorughout the game.
IFont* CreateCustomFont(I3DEngine& myEngine)
{
	const string kFontName = "Arial";
	const unsigned int kFontSize = 32;
	IFont* myFont = myEngine.LoadFont(kFontName, kFontSize);
	return myFont;
}

// Create a skybox in the scene.
const IModel* CreateSkybox(I3DEngine& myEngine)
{
	const string kSkyboxFileName = "skybox.x";
	const float kSkyboxPos[]{ 0.0f, -1000.0f, 0.0f };
	IMesh* skyboxMesh = myEngine.LoadMesh(kSkyboxFileName);
	const IModel* kSkybox = skyboxMesh->CreateModel(kSkyboxPos[0], kSkyboxPos[1], kSkyboxPos[2]);
	return kSkybox;
}

// Create a water surface in the scene
const IModel* CreateWaterSurface(I3DEngine& myEngine)
{
	const string kSurfaceFileName = "surface.x";
	const float kSurfacePos[]{ 0.0f, -2.5f, 0.0f };
	IMesh* surfaceMesh = myEngine.LoadMesh(kSurfaceFileName);
	const IModel* kSurface = surfaceMesh->CreateModel(kSurfacePos[0], kSurfacePos[1], kSurfacePos[2]);
	return kSurface;
}

// Create an island in the scene.
const IModel* CreateIsland(I3DEngine& myEngine, const int kIslandNum)
{
	string kIslandFileName;
	float islandPos[]{ 0.0f, -5.0f, 0.0f };
	if (kIslandNum == 1)
	{
		kIslandFileName = "island1.x";
		islandPos[2] = 40.0f;

	}
	else if (kIslandNum == 2)
	{
		kIslandFileName = "island2.x";
		islandPos[2] = 115.0f;
	}
	IMesh* islandMesh = myEngine.LoadMesh(kIslandFileName);
	const IModel* kIsland = islandMesh->CreateModel(islandPos[0], islandPos[1], islandPos[2]);
	return kIsland;
}

// Create a custom camera that will be used throughout the project.
ICamera* CreateCustomCamera(I3DEngine& myEngine)
{
	ICamera* myCamera = myEngine.CreateCamera(kManual);
	myCamera->SetPosition(kCameraInitPos[0], kCameraInitPos[1], kCameraInitPos[2]);

	myCamera->RotateLocalX(kCameraInitRotation);
	return myCamera;
}

// Create frogs with default configuration
vector<SFrog> CreateFrogs(I3DEngine& myEngine)
{
	vector<SFrog> frogs;
	// The game wil always start with 3 frogs
	const int kFrogNum = 3;
	const string kFrogMeshFilename = "frog.x";
	IMesh* frogMesh = myEngine.LoadMesh(kFrogMeshFilename);
	const float kFrogStartX = -10.0f;
	const float kFrogXOffset = 10.0f; // Value added to kFrogStartX for each frog.
	const float kFrogStartY = 0.0f;
	const float kFrogStartZ = 15.0f;
	for (size_t i = 0; i < kFrogNum; i++)
	{
		SFrog frog;
		frog.model = frogMesh->CreateModel(kFrogStartX + (kFrogXOffset * i), kFrogStartY, kFrogStartZ);
		frog.index = i;
		frog.isJumping = false;
		frog.state = waiting;
		frog.rotation = rotationTop;
		frog.speedMult = kFrogSpeedMult;
		frog.maxDist = kFrogStartZ;
		// Change the state for first frog only. This is the frog that the player will control first.
		if (i == 0)
			frog.state = crossing;
		frogs.push_back(frog);
	}
	return frogs;
}

// Create tire models which the frog can walk on between two islands.
vector<STyre> CreateTires(I3DEngine& myEngine)
{
	vector<STyre> tires;
	const string kTyreMeshFileName = "tyre.x";
	IMesh* tyreMesh = myEngine.LoadMesh(kTyreMeshFileName);
	const float kTyreX = -10.0f;
	const float kTyreZ = 75.0f;
	const float kTyreZOffset = 10.0f;
	const float kTyreXOffset = 10.0f;
	const float kScaleFactor = 10.0f;
	const int kTireLanes = 4;
	const int kTiresPerLane = 3;
	int index = 0;
	ERotation rotation;
	STyre Tyre;
	// Create the tyres from front to back, from left to right.
	for (int length = 0; length < kTireLanes; length++)
	{
		// Each tyre lane will go in opposite directions.
		if (length % 2 == 0)
			rotation = rotationLeft;
		else
			rotation = rotationRight;

		for (size_t width = 0; width < kTiresPerLane; width++)
		{
			Tyre.model = tyreMesh->CreateModel(kTyreX + (width * kTyreXOffset), kTyreYPos, kTyreZ + (length * kTyreZOffset));
			Tyre.model->Scale(kScaleFactor);
			Tyre.rotation = rotation;
			Tyre.index = index;
			index++;
			tires.push_back(Tyre);
		}
		index = 0;
	}
	return tires;
}

// Check sphere-sphere collision between two objects
bool IsSphereSphereCollided(const IModel* sphere1, const IModel* sphere2, const float& kSphere1Radius, const float& kSphere2Radius)
{
	// Don't need to check Y Coordinates

	float totalDistance = 0.0f;
	const float kSphere1X = sphere1->GetX();
	const float kSphere1Z = sphere1->GetZ();

	const float kSphere2X = sphere2->GetX();
	const float kSphere2Z = sphere2->GetZ();

	const float kSquared = 2.0f; // Power of 2
	totalDistance = pow((kSphere2X - kSphere1X), kSquared) + pow((kSphere2Z - kSphere1Z), kSquared);

	if (totalDistance < powf(kSphere2Radius + kSphere1Radius, kSquared))
		return true;
	else
		return false;
}

// Attach the dummy to a tyre, and move it onto the tyre.
void AttachDummyToTyre(IModel* dummy, const STyre& tyre)
{
	dummy->AttachToParent(tyre.model);
	dummy->Scale(kFrogToTyreRatio);
	dummy->SetX(tyre.model->GetX());
	dummy->SetY(-kTyreYPos * kFrogToTyreRatio);
	dummy->SetZ(tyre.model->GetZ());
}

// Detach the dummy from tyre if it is attached, and maintain position.
void DetachDummyFromTyre(IModel* dummy)
{
	const float kX = dummy->GetX();
	const float kY = dummy->GetY();
	const float kZ = dummy->GetZ();
	dummy->DetachFromParent();
	dummy->ResetScale();
	dummy->SetPosition(kX, kY, kZ);
}

// Create Vehicles
void CreateVehicles(vector<SVehicle>& vVehicles, I3DEngine& myEngine, const int& kAmount, const float(&kStartPos)[k3DCoords], const float& kOffset, const string& kMeshFile, const float& kSpeedMult, const ERotation& kRotation)
{
	// Mesh used to create vehicle models.
	IMesh* vehicleMesh = myEngine.LoadMesh(kMeshFile);
	for (size_t i = 0; i < size_t(kAmount); i++)
	{
		SVehicle vehicle;
		vehicle.model = vehicleMesh->CreateModel(kStartPos[0] + (kOffset * i), kStartPos[1], kStartPos[2]);
		vehicle.reappearX = kStartPos[0];
		vehicle.reappearY = kStartPos[1];
		vehicle.reappearZ = kStartPos[2];
		vehicle.rotation = rotationTop;
		vehicle.SetRotation(kRotation);
		vehicle.speedMult = kSpeedMult;
		vehicle.state = shown;
		vVehicles.push_back(vehicle);
	}
}

// Create default plants on the play field.
void CreatePlants(vector<SObject>& plants, I3DEngine& myEngine)
{
	const string kPlantMeshFile = "plant.x";
	IMesh* plantMesh = myEngine.LoadMesh(kPlantMeshFile);
	
	const float kFirstPos[]{ -50.0f, 0.0f, 115.0f };
	const float kOffsetX = 20.0f;
	const int kAmount = 6;

	for (size_t i = 0; i < kAmount; i++)
	{
		SObject plant;
		plant.model = plantMesh->CreateModel(kFirstPos[0] + (i * kOffsetX), kFirstPos[1], kFirstPos[2]);
		plants.push_back(plant);
	}
}

void RotateCameraDown(const float& frametime, float& currentCameraRotation, IModel* dummy)
{
	dummy->RotateX(-frametime * kCameraRotationSpeed * kGameSpeed);
	currentCameraRotation -= (frametime * kCameraRotationSpeed * kGameSpeed);	
}

void RotateCameraUp(const float& frametime, float& currentCameraRotation, IModel* dummy)
{
	dummy->RotateX(frametime * kCameraRotationSpeed * kGameSpeed);
	currentCameraRotation += (frametime * kCameraRotationSpeed * kGameSpeed);
}