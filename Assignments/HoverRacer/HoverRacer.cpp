// Szymon Janusz G20792986

#include <vector> // Vector class
#include <string> // String class
//#include <thread> // Used for multi-threading
#include <cmath> // Maths library for c++
//#include <chrono> // Timing, thread sleeping
#include <iostream> // Console output
//#include <cstdio> // File input and output
#include <fstream> // File input and output
//#include <future> // Possibly using async functions along with thread
//#include <algorithm>
#include <limits> // maximum data type values
#include <TL-Engine.h>	// TL-Engine include file and namespace

// Ignore warnings about enum class, invalid pointers, marking as not_null, gsl::at()
#pragma warning(disable : 26812 26486 26429 26446)

using namespace tle;

// Function prototypes
float HalfOf(const float& kF) noexcept;
// Return a random number in the range between rangeMin and rangeMax inclusive
// range_min <= random number <= range_max
float GetRandomFloat(const int& kRangeMin, const int& kRangeMax) noexcept;

// Constant declaration
constexpr unsigned int kGridSize = 50; // How big each grid square is. x * x dimensions.
// Check this many squares in the x and z axis relative to the current grid.
// EG when kGridVicinity = 1, check the current grid, and +-1 on x and +-1 on z (9 in total)
constexpr int kGridVicinity = 1;
constexpr int kArrayOffset = 1; // 0th item = 1st index for humans.
constexpr float kGameCountdownTimer = 3.0f; // Count down for 3 seconds before the game starts.
constexpr float kGameGoTimer = 1.0f; // Show "Go!" for x seconds when the race is starting
constexpr float kGameStageTimer = 1.0f; // How long to show "Stage X complete!" for.
constexpr float kScale = 1.0f / 6.0f;
constexpr float kSpeedConversion = 3.6f; // 1 Metre/s = 3.6 Kilomteres /h
constexpr float kCollisionDelay = 0.2f; // Health can only decrease every x seconds.
const string kCheckpointObject = "Checkpoint";
const string kIsleStraightObject = "Isle";
const string kWallObject = "Wall";
const string kWaypointObject = "Waypoint";
const string kWaterTankObject = "WaterTank";
constexpr float kGravity = -2.35f;
constexpr float kMinHeight = 0.0f;
constexpr unsigned int kLaps = 2;

// Control Scheme
const EKeyCode EGamePause = EKeyCode::Key_P;
const EKeyCode EGameExit = EKeyCode::Key_Escape;
const EKeyCode ECameraForward = EKeyCode::Key_Up;
const EKeyCode ECameraBackward = EKeyCode::Key_Down;
const EKeyCode ECameraRight = EKeyCode::Key_Right;
const EKeyCode ECameraLeft = EKeyCode::Key_Left;
const EKeyCode ECameraReset = EKeyCode::Key_1;
const EKeyCode ECameraFirstPerson = EKeyCode::Key_2;
const EKeyCode EPlayerIncreaseForwardThrust = EKeyCode::Key_W;
const EKeyCode EPlayerIncreaseBackwardThrust = EKeyCode::Key_S;
const EKeyCode EPlayerRotateLeft = EKeyCode::Key_A;
const EKeyCode EPlayerRotateRight = EKeyCode::Key_D;
const EKeyCode EGameStartKey = EKeyCode::Key_Space;
const EKeyCode EGameToggleMouseCapture = EKeyCode::Key_Tab;
const EKeyCode EPlayerBoostKey = EKeyCode::Key_Space;
const EKeyCode EGameResetKey = EKeyCode::Key_R;

// Enums

// The possible states for the game to be in.
enum EGameStates
{
	starting,
	playing,
	paused,
	over,
	finished,

	gameStatesTotal
};

// What items is the level file made from
enum EGameFileIndexes
{
	objectIndex,
	xPosIndex,
	yPosIndex,
	zPosIndex,
	globalXRotationIndex,
	globalYRotationIndex,
	globalZRotationIndex,
	localXRotationIndex,
	localYRotationIndex,
	localZRotationIndex,
	scaleIndex,

	fileIndexesTotal
};

// Components of a 2D Vector
enum EVector2D
{
	x2D,
	z2D,

	vector2DTotal
};

// Components of a 3D Vector
enum EVector3D
{
	x3D,
	y3D,
	z3D,

	vector3DTotal
};

// Which axis an object collided with
enum ECollisionAxis
{
	xAxis,
	zAxis,
	none,

	collisionTotal
};

// When dealing with a 3D model matrix, these are the indexes.
enum EModelMatrix
{
	localX,
	localY,
	localZ,
	position,

	matrixTotal
};

// Possible return codes used when returning from a function
enum EReturnCodes
{
	CodeSuccess = 0,
	CodeGameInitFail = 700,
	CodeSaveFileFail = 701,
	CodeEngineInitFail = 702
};

// When dealing with grids on the map
enum EGridVicinity
{
	sameGrid,
	closeBy, // currently not in use
	notInVicinity,

	gridVicinityTotal
};

// Structs

// Store the x and y coordinates of a Heads Up Display element. Used when drawing items on screen.
struct SHUDInfo
{
	int x; // The x component of the current hud element
	int y; // The y component of the current hud element
};

// A 2D vector struct.
struct SVector2D
{
	float x; // The x component of the current vector
	float z; // The y component of the current vector
};

// Multiply a 2D vector by a scalar
SVector2D ScalarMulti(const float& kS, const SVector2D& kV) noexcept;

// Classes

class CGameObject // Standard class for every interactable object in the game.
{
private: // Set to known bad values.
	IModel* model_ = nullptr;
	string type_ = "";
	int gridX_ = numeric_limits<int>::min();
	int gridZ_ = numeric_limits<int>::min();
	float radius_ = -numeric_limits<float>::max();
	float width_ = -numeric_limits<float>::max();
	float length_ = -numeric_limits<float>::max();

public:
	// Returns the object's model. Can return nullptr if no model was set.
	IModel* GetModel() const noexcept
	{
		return model_;
	}
	// Set the object's model to parameter.
	void SetModel(IModel* model) noexcept
	{
		model_ = model;
	}
	// Return the object type. Returns placeholder value if no type was set.
	string GetType() const
	{
		return type_;
	}
	// Set the object type to parameter
	void SetType(const string kType)
	{
		type_ = kType;
	}
	// Automatically set the grid X and grid Z based on model position.
	void UpdateGrid()
	{
		// do the X coordinate for the grid
		const float kModelX = model_->GetX();
		// Always get the positive number.
		float tempX = fabsf(kModelX);
		tempX /= kGridSize;
		int tempIntX = static_cast<int>(round(tempX));
		// if model pos is -ve, set to -ve
		if (kModelX < 0.0f)
		{
			tempIntX = -tempIntX;
		}
		gridX_ = tempIntX;	

		const float kModelZ = model_->GetZ();
		float tempZ = fabsf(kModelZ);
		tempZ /= kGridSize;
		int tempIntZ = static_cast<int>(round(tempZ));
		if (kModelZ < 0.0f)
		{
			tempIntZ = -tempIntZ;
		}
		gridZ_ = tempIntZ;
	}
	// Get the x component of the grid
	int GetGridX() const noexcept
	{
		return gridX_;
	}
	// Get the z component of the grid
	int GetGridZ() const noexcept
	{
		return gridZ_;
	}
	float GetRadius() const noexcept
	{
		return radius_;
	}
	void SetRadius(const float& kRadius) noexcept
	{
		radius_ = kRadius;
	}
	float GetWidth() const noexcept
	{
		return width_;
	}
	void SetWidth(const float& kWidth) noexcept
	{
		width_ = kWidth;
	}
	float GetLength() const noexcept
	{
		return length_;
	}
	void SetLength(const float& kLength) noexcept
	{
		length_ = kLength;
	}
};

class CCheckpoint : public CGameObject
{
private:
	vector<CGameObject> struts_;
	unsigned int stage_ = numeric_limits<unsigned int>::max();
	float strutRadius_ = 1.0f;
	float strutDiameter_ = 2.0f * strutRadius_;	
	const float kLifetimeMax_ = 1.0f;
	float currentLifetime_ = -1.0f;

public:
	unsigned int GetStage() const noexcept
	{
		return stage_;
	}
	void SetStage(const unsigned int& kStage) noexcept
	{
		stage_ = kStage;
	}
	float GetStrutRadius() const noexcept
	{
		return strutRadius_;
	}
	const vector<CGameObject> GetStrutVector() const
	{
		return struts_;
	}
	void SetStrutVector(const vector<CGameObject> kStrutVector)
	{
		struts_ = kStrutVector;
	}
	void UpdateCross(IModel* cross, const float& kFrametime, const float& kGameSpeed)
	{
		// move the cross to the checkpoint
		if (currentLifetime_ > 0.0f)
		{
			cross->AttachToParent(GetModel());
			cross->SetLocalY(5.0f);
			currentLifetime_ -= kFrametime * kGameSpeed;
			if (currentLifetime_ < 0.0f)
			{
				cross->DetachFromParent();
				cross->MoveLocalY(-1000.0f);
			}
		}
	}
	void SetCrossLifeTime()
	{
		currentLifetime_ = kLifetimeMax_;
	}
};

class CHoverCar : public CGameObject // Standard class used by all hover cars
{
protected:
	SVector2D momentum_{ 0.0f, 0.0f }; // Current momentum vector
	SVector2D thrust_{ 0.0f, 0.0f }; // Current thrust vector
	SVector2D drag_{ 0.0f, 0.0f }; // Current drag vector
	SVector2D facing_{ 0.0f, 0.0f }; // Current facing vector
	float modelMatrix_[EModelMatrix::matrixTotal][EModelMatrix::matrixTotal]{ 0.0f };
	float previousX_ = 0.0f; // The x position of the hover car in the previous frame
	float previousZ_ = 0.0f; // The z position of the hover car in the previous frame
	float thrustMultiplier_ = 60.0f; // Thrust multiplier. Increasing this increases the maximum speed and acceleration of the hover car.
	float dragMultiplier_ = -0.75f; // Drag value. Increasing this makes the car stop quicker, and reduces the arc size when turning corners.
	float rotationSpeed_ = 180.0f; // How much to rotate by per second, in degrees.
	float maxForwardThrustMulti_ = 1.0f; // Used when calculating max forward momentum of hover car
	float maxBackwardThrustMulti_ = HalfOf(maxForwardThrustMulti_); // The backwards thrust can reach a max of 0.5f * forward thrust.
	unsigned int currentStage_ = 0; // Which stage the hover car is at right now.
	float moveSpeed_ = 0.0f;
	int health_ = 100;
	const int kBoostThreshold_ = 30;
	float lastCollision_ = 0.0f; // When 0.0f, health can be taken away again
	float verticalVelocity_ = fabsf(kGravity);

public:
	SVector2D GetFacingVector() const noexcept
	{
		return facing_;
	}
	void SetFacingVector(const SVector2D& kV) noexcept
	{
		facing_ = kV;
	}
	float GetRotationSpeed() const noexcept
	{
		return rotationSpeed_;
	}
	// Set the rotation speed to the positive of parameter.
	void SetRotationSpeed(const float& kSpeed) noexcept
	{
		// Always get the positive value.
		rotationSpeed_ = fabsf(kSpeed);
	}
	void SetThrust(const SVector2D& kThrust) noexcept
	{
		thrust_ = kThrust;
	}
	SVector2D GetThrust() const noexcept
	{
		return thrust_;
	}
	float GetThrustMultiplier() const noexcept
	{
		return thrustMultiplier_;
	}
	float GetForwardThrustMulti() const noexcept
	{
		return maxForwardThrustMulti_;
	}
	float GetBackwardThrustMulti() const noexcept
	{
		return maxBackwardThrustMulti_;
	}
	void SetDrag(const SVector2D& kDrag) noexcept
	{
		drag_ = kDrag;
	}
	SVector2D GetDrag() const  noexcept
	{
		return drag_;
	}
	float GetDragMultiplier() const noexcept
	{
		return dragMultiplier_;
	}
	void SetDragMultiplier(const float& kDragMulti) noexcept
	{
		dragMultiplier_ = kDragMulti;
	}
	SVector2D GetMomentum() const noexcept
	{
		return momentum_;
	}
	void SetMomentum(const SVector2D& kMomentum) noexcept
	{
		momentum_ = kMomentum;
	}
	float GetModelMatrixComponent(const EModelMatrix& kRow, const EVector3D& kColumn) const noexcept
	{
		return modelMatrix_[kRow][kColumn];
	}
	float* GetModelMatrix() noexcept
	{
		return begin(*modelMatrix_);
	}
	float GetPreviousX() const noexcept
	{
		return previousX_;
	}
	void SetPreviousX(const float& kPreviousX) noexcept
	{
		previousX_ = kPreviousX;
	}
	float GetPreviousZ() const noexcept
	{
		return previousZ_;
	}
	void SetPreviousZ(const float& kPreviousZ) noexcept
	{
		previousZ_ = kPreviousZ;
	}
	void IncrementStage() noexcept
	{
		currentStage_++;
	}
	unsigned int GetCurrentStage() const noexcept
	{
		return currentStage_;
	}
	void SetCurrentStage(const unsigned int& kStage) noexcept
	{
		currentStage_ = kStage;
	}
	void UpdateMoveSpeed() noexcept
	{
		// Square, add, square root.
		moveSpeed_ = sqrtf(powf(momentum_.x, 2.0f) + powf(momentum_.z, 2.0f));			
	}
	float GetMoveSpeed() const noexcept
	{
		return moveSpeed_;
	}
	void PerformCollision() noexcept
	{
		if (lastCollision_ <= 0.0f)
		{
			health_ -= 1;
			lastCollision_ = kCollisionDelay;
		}
	}
	void UpdateCollisionDelay(const float& kFrametime) noexcept
	{
		if (lastCollision_ >= 0.0f)
		{
			lastCollision_ -= kFrametime;
		}
	}
	int GetHealth() const noexcept
	{
		return health_;
	}
	void Hover(const float& kFrametime, const float& kGameSpeed)
	{
		verticalVelocity_ += kGravity * kFrametime * kGameSpeed;
		GetModel()->MoveY(verticalVelocity_ * kFrametime * kGameSpeed);
		if (GetModel()->GetY() <= kMinHeight)
		{
			GetModel()->SetY(kMinHeight);
			verticalVelocity_ = fabsf(kGravity);
		}
	}
};

class CPlayer : public CHoverCar // Class used to create the player car
{
private:
	float boostTimer_ = 0.0f; // How long the current boost is being applied for
	const float kMaxBoostTime_ = 3.0f; // How long the player can boost for
	const float kBoostWarning_ = kMaxBoostTime_ - 1.0f; // When to display the warning message
	const float kBoostCooldown_ = 5.0f; // How long to cooldown the booster for when max boost time is reached
	bool usedBoost_ = false; // has the boost been used in the current frame
	bool overheated_ = false; // Is the boost overheated

public:
	bool CanUseBoost() const noexcept
	{
		return (health_ >= kBoostThreshold_ && !overheated_);
	}
	void Boost(const float& kFrametime) noexcept
	{
		// If the car can boost
		if (boostTimer_ < kMaxBoostTime_)
		{
			boostTimer_ += kFrametime;
			if (!usedBoost_)
			{
				thrustMultiplier_ *= 2.0f;
				usedBoost_ = true;
			}
		}
		else
		{
			BoostOverheat();
		}
	}
	bool DisplayBoostWarning() const noexcept
	{
		return (boostTimer_ >= kBoostWarning_ && !overheated_);
	}
	bool IsOverheated() const noexcept
	{
		return overheated_;
	}
	float GetBoostTime() const noexcept
	{
		return boostTimer_;
	}
	float GetBoostMaxTime() const noexcept
	{
		return kMaxBoostTime_;
	}
	void BoostOverheat() noexcept
	{
		overheated_ = true;
		boostTimer_ = kBoostCooldown_;
		dragMultiplier_ *= 2.0f;
		thrustMultiplier_ /= 2.0f;
	}
	void UpdateBoost(const float& kFrametime) noexcept
	{
		if (overheated_)
		{
			boostTimer_ -= kFrametime;
			if (boostTimer_ <= 0.0f)
			{
				overheated_ = false;
				dragMultiplier_ /= 2.0f;
			}
		}
		else
		{
			if (!usedBoost_ && boostTimer_ > 0.0f)
			{
				boostTimer_ -= kFrametime;
			}
			else if (usedBoost_)
			{
				thrustMultiplier_ /= 2.0f;
			}
		}
		usedBoost_ = false;
	}
};

float GetMagnitude(const SVector2D& v) noexcept
{
	constexpr float kSquared = 2.0f;
	return sqrtf(powf(v.x, kSquared) + powf(v.z, kSquared));
}

SVector2D GetNormalisedVector(const SVector2D& v) noexcept
{
	const float kLength = GetMagnitude(v);
	return { v.x / kLength, v.z / kLength };
}

float GetDotProduct(const SVector2D& v1, const SVector2D& v2) noexcept
{
	return ((v1.x * v2.x) + (v1.z * v2.z));
}

// Used when parsing the level file
void PrintErrorMessage(const unsigned int& kLineIndex, const unsigned int& kItemIndex, const string& kLevelFile, const exception* kException)
{
	if (kException != nullptr)
	{
		cout << "ERROR: " << kException->what() << ". Line " << kLineIndex + kArrayOffset << ", Index " << kItemIndex + kArrayOffset << "\n";
		cout << "Check the " << kLevelFile << " file. Aborting..." << endl;
	}
	else
	{
		cout << "ERROR: Something went wrong. Line " << kLineIndex + kArrayOffset << ", Index " << kItemIndex + kArrayOffset << "\n";
		cout << "Check the " << kLevelFile << " file. Aborting..." << endl;
	}
	char ch;
	cin >> ch;
}

// Load objects from a game level file
void LoadLevelFromFile(I3DEngine* myEngine, const string& kLevelFile, vector<CCheckpoint>& checkpoints, vector<CGameObject>& sphereObjects, vector<CGameObject>& boxObjects, vector<CGameObject>& waypoints)
{
	//	// Input file stream
	ifstream inputStream(kLevelFile);
	if (!inputStream)
	{
		cout << "Error:  File cannot be accessed/does not exist.\nFile: " << kLevelFile << endl;
		char ch;
		cin >> ch;
		exit(CodeSaveFileFail);
	}

	// Load all the meshes to create the objects later

	const string kCheckpointFile = "Checkpoint.x";
	IMesh* checkpointMesh = myEngine->LoadMesh(kCheckpointFile);
	constexpr float kCheckpointWidth = 19.0f;
	constexpr float kCheckpointLength = 2.0f;
	constexpr float kStrutRadius = 1.2f; // 1.25f
	// The checkpoint width includes the struct diameter * 2; struct radius * 4;
	constexpr float kCheckpointWidthNoStruts = kCheckpointWidth - (4.0f * kStrutRadius);

	const string kIsleStraightFile = "IsleStraight.x";
	IMesh* isleStraightMesh = myEngine->LoadMesh(kIsleStraightFile);
	constexpr float kIsleStraightLength = 7.0f;
	constexpr float kIsleStraightWidth = 4.5f; // 5.0f

	const string kWallFile = "Wall.x";
	IMesh* wallMesh = myEngine->LoadMesh(kWallFile);
	constexpr float kWallLength = 10.0f;
	constexpr float kWallWidth = 4.5f; // 1.5f

	const string kWaterTankFile = "TankSmall1.x";
	IMesh* waterTankMesh = myEngine->LoadMesh(kWaterTankFile);
	constexpr float kTankRadius = 4.5f;

	const string kDummyFile = "Dummy.x";
	IMesh* dummyMesh = myEngine->LoadMesh(kDummyFile);
	vector<CGameObject> struts;

	const string kWaypointFile = "Dummy.x";
	IMesh* waypointMesh = dummyMesh;

	IMesh* currentMesh = nullptr;
	
	CCheckpoint object;
	CGameObject strut;

	string currentItem;
	float length = -numeric_limits<float>::max();
	float width = -numeric_limits<float>::max();
	float radius = -numeric_limits<float>::max();
	int itemIndex = 0;
	int lineIndex = 0;
	constexpr unsigned int kItemsPerLine = EGameFileIndexes::fileIndexesTotal;

	// Iterate over the level file
	while (!inputStream.eof())
	{
		inputStream >> currentItem;
		switch (itemIndex)
		{
		case EGameFileIndexes::objectIndex:
		{
			// Reset the measurements for the current object.
			length = -numeric_limits<float>::max();
			width = -numeric_limits<float>::max();
			radius = -numeric_limits<float>::max(); 

			if (currentItem == kCheckpointObject)
			{
				// Set the current mesh depending on the object type.
				currentMesh = checkpointMesh;
				length = kCheckpointLength;
				width = kCheckpointWidthNoStruts;
			}
			else if (currentItem == kWaterTankObject)
			{
				currentMesh = waterTankMesh;
				radius = kTankRadius;
			}
			else if (currentItem == kIsleStraightObject)
			{
				currentMesh = isleStraightMesh;
				width = kIsleStraightWidth;
				length = kIsleStraightLength;
			}
			else if (currentItem == kWallObject)
			{
				currentMesh = wallMesh;
				width = kWallWidth;
				length = kWallLength;
			}
			else if (currentItem == kWaypointObject)
			{
				currentMesh = waypointMesh;
			}
			else // The object type is not recognised.
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, nullptr);
				exit(EReturnCodes::CodeSaveFileFail);
			}

			object.SetModel(currentMesh->CreateModel());
			object.SetLength(length);
			object.SetWidth(width);
			object.SetType(currentItem);
			object.SetRadius(radius);

			break;
		}
		case EGameFileIndexes::xPosIndex:
		{
			// Try get float from string
			try
			{
				object.GetModel()->SetX(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::yPosIndex:
		{
			// Try get float from string
			try
			{
				object.GetModel()->SetY(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::zPosIndex:
		{
			// Try get float from string
			try
			{
				object.GetModel()->SetZ(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::globalXRotationIndex:
		{
			try
			{
				object.GetModel()->RotateX(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::globalYRotationIndex:
		{
			try
			{
				// Try get float from string
				const float kRotation = static_cast<float>(stoi(currentItem));
				constexpr int kRightAngle = 90;
				constexpr int kCircle = 360;
				// If the object is rotated by a right angle, rotate the bounding box with it
				// Only support rigth angle rotation to make collision resolution easier.
				if (static_cast<int>(kRotation) == kRightAngle || static_cast<int>(kRotation) == (kCircle - kRightAngle))
				{
					object.GetModel()->RotateY(static_cast<float>(stoi(currentItem)));
					const float kLength = object.GetLength();
					const float kWidth = object.GetWidth();
					object.SetWidth(kLength);
					object.SetLength(kWidth);
				}
				else if (static_cast<int>(kRotation) != 0 && static_cast<int>(kRotation) != kCircle)
				{
					PrintErrorMessage(lineIndex, itemIndex, kLevelFile, nullptr);
					exit(EReturnCodes::CodeSaveFileFail);
				}
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::globalZRotationIndex:
		{
			try
			{
				object.GetModel()->RotateZ(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::localXRotationIndex:
		{
			try
			{
				// Try get float from string
				object.GetModel()->RotateLocalX(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::localYRotationIndex:
		{
			try
			{
				// Try get float from string
				object.GetModel()->RotateLocalY(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::localZRotationIndex:
		{
			try
			{
				// Try get float from string
				object.GetModel()->RotateLocalZ(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		case EGameFileIndexes::scaleIndex:
		{
			try
			{
				object.GetModel()->Scale(static_cast<float>(stoi(currentItem)));
			}
			catch (const exception* e)
			{
				PrintErrorMessage(lineIndex, itemIndex, kLevelFile, e);
				exit(EReturnCodes::CodeSaveFileFail);
			}
			break;
		}
		default:
		{
			PrintErrorMessage(lineIndex, itemIndex, kLevelFile, nullptr);
			exit(EReturnCodes::CodeSaveFileFail);
		}
		}
		itemIndex++;
		// If the end of the line is reached
		if (itemIndex > kItemsPerLine - kArrayOffset)
		{
			lineIndex++;
			itemIndex = 0;
			// Push the object to scenery or checkpoint vector
			object.UpdateGrid();
			if (object.GetType() == kCheckpointObject)
			{
				// Create the struts.
				// Clear local struts vector
				// Push local struts into local struts vector
				// Get the reference to object struts vector
				// Assign the reference to local struts vector
				// Check checkpoint rotation
				struts.clear();
				if (object.GetLength() > object.GetWidth())
				{
					strut.SetModel(dummyMesh->CreateModel(object.GetModel()->GetX(), object.GetModel()->GetY(), object.GetModel()->GetZ() + HalfOf(kCheckpointWidthNoStruts) + object.GetStrutRadius()));
					struts.push_back(strut);
					strut.SetModel(dummyMesh->CreateModel(object.GetModel()->GetX(), object.GetModel()->GetY(), object.GetModel()->GetZ() - HalfOf(kCheckpointWidthNoStruts) - object.GetStrutRadius()));
					struts.push_back(strut);
				}
				else
				{
					strut.SetModel(dummyMesh->CreateModel(object.GetModel()->GetX() - HalfOf(kCheckpointWidthNoStruts) - object.GetStrutRadius(), object.GetModel()->GetY(), object.GetModel()->GetZ()));
					struts.push_back(strut);
					strut.SetModel(dummyMesh->CreateModel(object.GetModel()->GetX() + HalfOf(kCheckpointWidthNoStruts) + object.GetStrutRadius(), object.GetModel()->GetY(), object.GetModel()->GetZ()));
					struts.push_back(strut);
				}
				object.SetStage(checkpoints.size());
				object.SetStrutVector(struts);
				checkpoints.push_back(object); // Create a copy of the item rather than emplacing			
			}
			else if (object.GetType() == kIsleStraightObject || object.GetType() == kWallObject)
			{
				boxObjects.push_back(object);
			}
			else if (object.GetType() == kWaterTankObject)
			{
				sphereObjects.push_back(object);
			}
			else if (object.GetType() == kWaypointObject)
			{
				waypoints.push_back(object);
			}
		}
	}	
	if (inputStream.eof())
	{
		cout << "Finished reading from file: " << kLevelFile << endl;
	}
}

// Create the skybox object to give the impression of clouds
void CreateSkybox(I3DEngine* myEngine, IModel* skybox)
{
	const float kSkyboxPos[]{ 0.0f, -960.0f, 0.0f };
	const string kSkyboxFile = "Skybox 07.x";
	IMesh* skyboxMesh = myEngine->LoadMesh(kSkyboxFile);
	skybox = skyboxMesh->CreateModel(kSkyboxPos[EVector3D::x3D], kSkyboxPos[EVector3D::y3D], kSkyboxPos[EVector3D::z3D]); // The skybox model used to give impression of clouds.
}

// Create the ground object for cars to hover over
void CreateGround(I3DEngine* myEngine, IModel* ground)
{
	const string kGroundFile = "ground.x";
	IMesh* groundMesh = myEngine->LoadMesh(kGroundFile);
	ground = groundMesh->CreateModel();
}

// Create the player object.
void CreatePlayer(I3DEngine* myEngine, CPlayer& player)
{
	const float kPlayerInitialPos[]{ -100.0f, 0.0f, -73.0f };
	const string kHoverCarFile = "race2.x";
	IMesh* hoverCarMesh = myEngine->LoadMesh(kHoverCarFile);
	player.SetModel(hoverCarMesh->CreateModel(kPlayerInitialPos[EVector3D::x3D], kPlayerInitialPos[EVector3D::y3D], kPlayerInitialPos[EVector3D::z3D]));
	constexpr float kLength = 12.0f; // 12.92f
	player.SetLength(kLength);
	constexpr float kWidth = 4.0f; // 4.46f
	player.SetWidth(kWidth);
	constexpr float kRadius = 4.0f; // 5.0f
	player.SetRadius(kRadius);
	player.UpdateGrid();
}

// Create an enemy
void CreateEnemy(I3DEngine* myEngine, CHoverCar& enemy)
{
	const float kPosition[]{ -100.0f, 0.0f, -87.0f };
	const string kEnemyFile = "race2.x";
	IMesh* enemyMesh = myEngine->LoadMesh(kEnemyFile);
	enemy.SetModel(enemyMesh->CreateModel(kPosition[EVector3D::x3D], kPosition[EVector3D::y3D], kPosition[EVector3D::z3D]));
	const string kSkin = "sp01.jpg";
	enemy.GetModel()->SetSkin(kSkin);
	constexpr float kLength = 12.0f; // 12.92f
	enemy.SetLength(kLength);
	constexpr float kWidth = 4.0f; // 4.46f
	enemy.SetWidth(kWidth);
	constexpr float kRadius = 4.0f; // 5.0f
	enemy.SetRadius(kRadius);
	enemy.UpdateGrid();
}

// Returns a half of a float
float HalfOf(const float& kF) noexcept
{
	return kF / 2.0f;
}

float GetRandomFloat(const int& kRangeMin, const int& kRangeMax) noexcept
{
	float result = static_cast<float>(rand()) / static_cast<float>(RAND_MAX + 1);
	result *= static_cast<float>(kRangeMax - kRangeMin);
	result += kRangeMin;

	return result;
}

// Add three 2D vectors together
SVector2D Sum3(const SVector2D& kV1, const SVector2D& kV2, const SVector2D& kV3) noexcept
{
	return{ kV1.x + kV2.x + kV3.x, kV1.z + kV2.z + kV3.z };
}

// Multiply a 2D vector by a scalar
SVector2D ScalarMulti(const float& kS, const SVector2D& kV) noexcept
{
	return{ kS * kV.x, kS * kV.z };
}

// Check if two objects are in the same grid or close by
EGridVicinity AreGridsClose(const CGameObject& kObject1, const CGameObject& kObject2) noexcept
{

	// Check if the grids are the same
	if (kObject1.GetGridX() == kObject2.GetGridX() && kObject1.GetGridZ() == kObject2.GetGridZ())
	{
		return EGridVicinity::sameGrid;
	}
	// Only need to check diagonal corners eg. bottom left and top right
	else if ((kObject2.GetGridZ() - kGridVicinity <= kObject1.GetGridZ() && kObject2.GetGridZ() + kGridVicinity >= kObject1.GetGridX()) 
		&& (kObject2.GetGridZ() + kGridVicinity >= kObject1.GetGridZ() && kObject2.GetGridX() - kGridVicinity <= kObject1.GetGridX()))
	{
		return EGridVicinity::closeBy;
	}
	else
	{
		return EGridVicinity::notInVicinity;
	}
}

// Check sphere-sphere collision between two objects
bool IsSphereSphereCollided(const IModel* kSphere1, const float& kSphere1Radius, const IModel* kSphere2, const float& kSphere2Radius)
{
	// Don't need to check Y Coordinates
	constexpr float kSquared = 2.0f; // Power of 2

	const float kSphere1X = kSphere1->GetX();
	const float kSphere1Z = kSphere1->GetZ();

	const float kSphere2X = kSphere2->GetX();
	const float kSphere2Z = kSphere2->GetZ();
	
	return (pow((kSphere2X - kSphere1X), kSquared) + pow((kSphere2Z - kSphere1Z), kSquared) < powf(kSphere1Radius + kSphere2Radius, kSquared));
}

// Check if there is a collision between two models
ECollisionAxis IsSphereBoxCollided(const IModel* kSphere, const float& kSpherePrevX, const float& kSpherePrevZ, const float& kSphereRadius, const IModel* kBox, const float& kBoxRadiusX, const float& kBoxRadiusZ)
{
	// Slightly inaccurate around corners.

	const float kBoxX = kBox->GetX();
	const float kBoxMaxX = kBoxX + kBoxRadiusX + kSphereRadius;
	const float kBoxMinX = kBoxX - kBoxRadiusX - kSphereRadius;
	const float kBoxZ = kBox->GetZ();
	const float kBoxMaxZ = kBoxZ + kBoxRadiusZ + kSphereRadius;
	const float kBoxMinZ = kBoxZ - kBoxRadiusZ - kSphereRadius;

	const float kSphereX = kSphere->GetX();
	const float kSphereZ = kSphere->GetZ();

	if (kSphereX < kBoxMaxX && kSphereX > kBoxMinX && kSphereZ < kBoxMaxZ && kSphereZ > kBoxMinZ)
	{
		// Check collision axis
		if (kSpherePrevX < kBoxMinX || kSpherePrevX > kBoxMaxX)
		{
			// Colliding parallel to the x axis
			return ECollisionAxis::xAxis;
		}
		else
		{
			// Colliding parallel to the z axis
			return ECollisionAxis::zAxis;
		}
	}
	else
	{
		return ECollisionAxis::none;
	}
}

// Check point to box collision between two models
bool IsPointBoxCollided(const IModel* kPoint, const IModel* kBox, const float& kBoxRadiusX, const float& kBoxRadiusZ)
{
	const float kPointX = kPoint->GetX();
	const float kPointZ = kPoint->GetZ();

	const float kBoxX = kBox->GetX();
	const float kBoxMaxX = kBoxX + kBoxRadiusX;
	const float kBoxMinX = kBoxX - kBoxRadiusX;
	const float kBoxZ = kBox->GetZ();
	const float kBoxMaxZ = kBoxZ + kBoxRadiusZ;
	const float kBoxMinZ = kBoxZ - kBoxRadiusZ;

	return (kPointZ > kBoxMinZ && kPointZ < kBoxMaxZ&& kPointX > kBoxMinX && kPointX < kBoxMaxX);
}

int main()
{
	// The engine type used.
	const EEngineType kEngineType = EEngineType::kTLX;
	// The 3D Engine used for the game.
	I3DEngine* myEngine = New3DEngine(kEngineType);
	if (myEngine == nullptr)
	{
		cout << "Engine Initialise Error.\nExiting." << endl;
		char ch;
		cin >> ch;
		return CodeEngineInitFail;
	}
	myEngine->StartWindowed();
	// Does the engine capture the mouse currently.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();
	float gameSpeed = 1.0f; // The speed at which the game runs. Used for slow-motion effects.

	// Add default folder for meshes and other media
	const string kMedia = "./media";
	myEngine->AddMediaFolder(kMedia);

	// Skybox used to give impression of clouds
	IModel* skybox = nullptr;
	CreateSkybox(myEngine, skybox);

	// The ground model the hover cars will hover above
	IModel* ground = nullptr;
	CreateGround(myEngine, ground);
	
	// List of all levels in the game
	vector<string> levels { "./media/level1.glf" };
	unsigned int levelIndex = 0;
	// All the scenery objects in the current level
	vector<CGameObject> sceneryBoxObjects;
	vector<CGameObject> scenerySphereObjects;
	vector<CGameObject> waypoints;

	// All the checkpoints in the current level
	vector<CCheckpoint> checkpoints;
	// Attempt to load the current level.
	LoadLevelFromFile(myEngine, levels.at(levelIndex), checkpoints, scenerySphereObjects, sceneryBoxObjects, waypoints);

	CPlayer player; // The player-controlled hover car.
	CreatePlayer(myEngine, player);
	CHoverCar enemy;
	CreateEnemy(myEngine, enemy);

	// The position of the camera relative to the player
	constexpr float kCameraPos[]{ 0.0f, 25.0f, -55.0f };
	constexpr float kCameraSpeed = 50.0f; // The camera will move at this amount per second.
	constexpr float kCameraRotationMax = 90.0f; // Max 90 degrees to left/right, and straight forward or straight down.
	float cameraRotationY = 0.0f; // The current camera rotation on the y axis.
	float mouseMovementX = 0.0f; // The difference in mouse movement between current and last frame.
	float cameraRotationX = 0.0f; // The current camera rotation on the x axis.
	float mouseMovementY = 0.0f; // The difference in mouse movement between current and last frame.
	const ECameraType kCameraType = ECameraType::kManual;
	// Main player-controlled camera
	ICamera* myCamera = myEngine->CreateCamera(kCameraType);
	myCamera->AttachToParent(player.GetModel());
	myCamera->SetLocalPosition(kCameraPos[EVector3D::x3D], kCameraPos[EVector3D::y3D], kCameraPos[EVector3D::z3D]);
	
	// The current state the game is in
	EGameStates gameState = EGameStates::starting;

	// Set up HUD Elements
	const SHUDInfo kHUDGameState = { 0, 0 }; // The position of where to draw the game state on screen
	const SHUDInfo kHUDUI = { 0, 0 };
	const SHUDInfo kHUDInstruction = { 0, 0 }; // The position of instructions shown on screen.
	const SHUDInfo kHUDCountdown = { 0, 0 };
	const SHUDInfo kHUDGo = { 0,0 }; // Where to draw the go text instruction
	const SHUDInfo kHUDSpeedKMH = { 240, 20 }; // Position of where to draw the player's speed
	const SHUDInfo kHUDSpeedMS = { 240, 40 }; // Position of where to draw the player's speed
	const SHUDInfo kHUDCurrentStage = { 0, 20 };
	const SHUDInfo kHUDGameOver = { 0, 60 };
	const SHUDInfo kHUDPlayerHealth = { 240, 0 };
	const SHUDInfo kHUDStageComplete = { 480, 0 };
	const SHUDInfo kHUDCurrentLap = { 0, 40 };
	const SHUDInfo kHUDBoostWarning = { 480, 20 };

	// Create UI Backdrop
	const string kUIBackdropFile = "ui_backdrop.jpg";
	const ISprite* uiBackdrop = myEngine->CreateSprite(kUIBackdropFile, static_cast<float>(kHUDUI.x), static_cast<float>(kHUDUI.y));

	// Create font
	const string kFontName = "Arial";
	IFont* myFont = myEngine->LoadFont(kFontName); // Font used to draw HUD elements on screen.
	const string kStartInstruction = "Hit Space to Start.";
	const string kGoInstruction = "Go!";
	bool drawCountdownText = false; // Draw the countdown before the game starts up?
	bool drawGoText = false;
	float countdownTimer = kGameCountdownTimer;
	float goTimer = kGameGoTimer;
	bool drawStageText = false;
	float stageTimer = 0.0f;

	// Enemy variables
	unsigned int enemyWaypointIndex = 0;
	constexpr float kEnemySpeed = 20.0f;

	// Player rotation when accelerating and turning
	bool playerRotated = false; // Has the player rotated left or right in the current frame
	float playerSidewaysRotation = 0.0f;
	constexpr float kPlayerMaxSidewaysRotation = 30.0f;
	bool playerAccelerated = false;
	float playerAccelerationRotation = 0.0f;
	constexpr float kPlayerMaxAccelerationRotation = 10.0f;

	// Player's current lap
	unsigned int currentLap = 0;

	// Checkpoint cross
	const string kCheckpointCross = "Cross.x";
	IMesh* crossMesh = myEngine->LoadMesh(kCheckpointCross);
	IModel* cross = crossMesh->CreateModel(0.0f, -1000.0f, 0.0f);

	// Prevent the mouse inputs from before the game loaded, to turn the camera
	myEngine->GetMouseMovementX();
	myEngine->GetMouseMovementY();

	// How long it took to render the last frame.
	float frametime = myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		// Update frametime
		frametime = myEngine->Timer();

		switch (gameState)
		{
		case EGameStates::starting:
		{
			myFont->Draw(kStartInstruction, kHUDInstruction.x, kHUDInstruction.y);

			if (myEngine->KeyHit(EGameStartKey))
			{
				gameState = EGameStates::playing;
				drawCountdownText = true;
			}
			break;
		}
		case EGameStates::playing:
		{
			if (drawCountdownText)
			{
				myFont->Draw(to_string(static_cast<int>(ceilf(countdownTimer))), kHUDCountdown.x, kHUDCountdown.y);
				countdownTimer -= (frametime * gameSpeed);
				if (countdownTimer < 0.0f)
				{
					drawCountdownText = false;
					drawGoText = true;
				}
				break;
			}
			else if (drawGoText)
			{
				myFont->Draw(kGoInstruction, kHUDGo.x, kHUDGo.y);
				goTimer -= (frametime * gameSpeed);
				if (goTimer < 0.0f)
				{
					drawGoText = false;
				}
			}
			else if (drawStageText)
			{
				if (player.GetCurrentStage() == 0)
				{
					myFont->Draw("Stage " + to_string(checkpoints.size() - 1) + " Complete!", kHUDStageComplete.x, kHUDStageComplete.y);
				}
				else
				{
					myFont->Draw("Stage " + to_string(player.GetCurrentStage() - 1) + " Complete!", kHUDStageComplete.x, kHUDStageComplete.y);
				}				
				stageTimer -= (frametime * gameSpeed);
				if (stageTimer < 0.0f)
				{
					drawStageText = false;
				}
			}

			// Get the facing vector of player
			player.GetModel()->GetMatrix(player.GetModelMatrix());
			player.SetFacingVector({ player.GetModelMatrixComponent(EModelMatrix::localZ, EVector3D::x3D), player.GetModelMatrixComponent(EModelMatrix::localZ, EVector3D::z3D) });

			// Rotation
			if (myEngine->KeyHeld(EPlayerRotateRight))
			{
				player.GetModel()->RotateY(player.GetRotationSpeed() * frametime * gameSpeed);
				if (playerSidewaysRotation > -kPlayerMaxSidewaysRotation)
				{
					player.GetModel()->RotateLocalZ(-frametime * gameSpeed * HalfOf(player.GetRotationSpeed()));
					playerSidewaysRotation += -frametime * gameSpeed * HalfOf(player.GetRotationSpeed());
				}
				playerRotated = true;
			}
			else if (myEngine->KeyHeld(EPlayerRotateLeft))
			{
				player.GetModel()->RotateY(-player.GetRotationSpeed() * frametime * gameSpeed);
				if (playerSidewaysRotation < kPlayerMaxSidewaysRotation)
				{
					player.GetModel()->RotateLocalZ(frametime * gameSpeed * HalfOf(player.GetRotationSpeed()));
					playerSidewaysRotation += frametime * gameSpeed * HalfOf(player.GetRotationSpeed());
				}
				playerRotated = true;
			}

			// Calculate thrust based on input
			if (myEngine->KeyHeld(EPlayerIncreaseForwardThrust))
			{
				player.SetThrust(ScalarMulti(player.GetThrustMultiplier() * frametime * gameSpeed * player.GetForwardThrustMulti(), player.GetFacingVector()));
				if (playerAccelerationRotation > -kPlayerMaxAccelerationRotation)
				{
					player.GetModel()->RotateLocalX(-frametime * gameSpeed * HalfOf(player.GetRotationSpeed()));
					playerAccelerationRotation += -frametime * gameSpeed * HalfOf(player.GetRotationSpeed());
				}
				playerAccelerated = true;
			}
			else if (myEngine->KeyHeld(EPlayerIncreaseBackwardThrust))
			{
				player.SetThrust(ScalarMulti(-player.GetThrustMultiplier() * frametime * gameSpeed * player.GetBackwardThrustMulti(), player.GetFacingVector()));
			}
			else
			{
				player.SetThrust({ 0.0f, 0.0f });
			}

			// Calculate the drag based on previous momentum
			player.SetDrag(ScalarMulti(player.GetDragMultiplier() * frametime * gameSpeed, player.GetMomentum()));

			// Calculate the momentum
			player.SetMomentum(Sum3(player.GetMomentum(), player.GetThrust(), player.GetDrag()));

			// Move the enemy
			enemy.GetModel()->LookAt(waypoints.at(enemyWaypointIndex).GetModel());
			enemy.GetModel()->MoveLocalZ(frametime * gameSpeed * kEnemySpeed);
			// Then check for collisions with the waypoint
			if (IsSphereBoxCollided(enemy.GetModel(), 0.0f, 0.0f, enemy.GetRadius(), waypoints.at(enemyWaypointIndex).GetModel(), 1.0f, 1.0f) != ECollisionAxis::none)
			{
				enemyWaypointIndex++;
				if (enemyWaypointIndex == waypoints.size())
				{
					enemyWaypointIndex = 0;
				}
			}

			// Check for collisions against box scenery objects
			for (const CGameObject& kObject : sceneryBoxObjects)
			{
				const EGridVicinity kGridVic = AreGridsClose(player, kObject);
				if (kGridVic == EGridVicinity::sameGrid || kGridVic == EGridVicinity::closeBy)
				{
					const ECollisionAxis kCollisionAxis = IsSphereBoxCollided(player.GetModel(), player.GetPreviousX(), player.GetPreviousZ(), player.GetRadius(), kObject.GetModel(), HalfOf(kObject.GetWidth()), HalfOf(kObject.GetLength()));
					switch (kCollisionAxis)
					{
					case ECollisionAxis::xAxis:
					{
						player.SetMomentum( {-HalfOf(player.GetMomentum().x), player.GetMomentum().z} );
						player.PerformCollision();
						player.GetModel()->SetX(player.GetPreviousX());
						player.GetModel()->SetZ(player.GetPreviousZ());
						break;
					}
					case ECollisionAxis::zAxis:
					{
						player.SetMomentum( {player.GetMomentum().x, -HalfOf(player.GetMomentum().z)} );
						player.PerformCollision();
						player.GetModel()->SetX(player.GetPreviousX());
						player.GetModel()->SetZ(player.GetPreviousZ());
						break;
					}
					default:
					{
						break;
					}
					}
				}
			} // End box scenery object collision checking

			// Check for collisions against sphere scenery objects.
			for (const CGameObject& kObject : scenerySphereObjects)
			{
				const EGridVicinity kGridVic = AreGridsClose(player, kObject);
				if (kGridVic == EGridVicinity::sameGrid || kGridVic == EGridVicinity::closeBy)
				{
					if (IsSphereSphereCollided(player.GetModel(), player.GetRadius(), kObject.GetModel(), kObject.GetRadius()))
					{
						player.SetMomentum( {-HalfOf(player.GetMomentum().x),  -HalfOf(player.GetMomentum().z)} );
						
						player.GetModel()->SetX(player.GetPreviousX());
						player.GetModel()->SetZ(player.GetPreviousZ());

						player.PerformCollision();
					}
				}
			} // End sphere scenery object collision checking

			// Check for collisions against checkpoints and struts
			for (CCheckpoint& checkpoint : checkpoints)
			{
				const EGridVicinity kGridVic = AreGridsClose(player, checkpoint);
				if (kGridVic == EGridVicinity::sameGrid || kGridVic == EGridVicinity::closeBy)
				{
					// Check current stage against index of checkpoints
					if (checkpoint.GetStage() == player.GetCurrentStage() && IsPointBoxCollided(player.GetModel(), checkpoint.GetModel(), HalfOf(checkpoint.GetWidth()), HalfOf(checkpoint.GetLength())))
					{
						if (player.GetCurrentStage() == 0)
						{
							currentLap++;
							if (currentLap > kLaps)
							{
								gameState = EGameStates::finished;
								break;
							}
						}
						player.IncrementStage();
						if (player.GetCurrentStage() >= checkpoints.size())
						{
							player.SetCurrentStage(0);
						}
						checkpoint.SetCrossLifeTime();						
						drawStageText = true;
						stageTimer = kGameStageTimer;
					}

					// Check strut collisions
					// Being const correct by using a const reference to a vector
					for (const CGameObject& kStrut : checkpoint.GetStrutVector())
					{
						if (IsSphereSphereCollided(player.GetModel(), player.GetRadius(), kStrut.GetModel(), checkpoint.GetStrutRadius()))
						{
							player.SetMomentum( {-HalfOf(player.GetMomentum().x), -HalfOf(player.GetMomentum().z)} );
							player.GetModel()->SetX(player.GetPreviousX());
							player.GetModel()->SetZ(player.GetPreviousZ());
							player.PerformCollision();
						}
					}
				}
				checkpoint.UpdateCross(cross, frametime, gameSpeed);
			} // End checkpoint and struts collision checking

			// Check collisions with the enemy
			if (IsSphereSphereCollided(player.GetModel(), player.GetRadius(), enemy.GetModel(), enemy.GetRadius()))
			{
				player.PerformCollision();
				player.SetMomentum({ -HalfOf(player.GetMomentum().x), -HalfOf(player.GetMomentum().z) });
				player.GetModel()->SetX(player.GetPreviousX());
				player.GetModel()->SetZ(player.GetPreviousZ());
			}

			if (!drawGoText)
			{
				myFont->Draw("Game Playing.", kHUDGameState.x, kHUDGameState.y);
			}			
			myFont->Draw("Stage: " + to_string(player.GetCurrentStage()), kHUDCurrentStage.x, kHUDCurrentStage.y);
			player.UpdateMoveSpeed();
			myFont->Draw("Speed: " + to_string(static_cast<int>(player.GetMoveSpeed() * kScale * kSpeedConversion)) + " KM/h", kHUDSpeedKMH.x, kHUDSpeedKMH.y);
			myFont->Draw("Speed: " + to_string(static_cast<int>(player.GetMoveSpeed() * kScale)) + " m/s", kHUDSpeedMS.x, kHUDSpeedMS.y);
			myFont->Draw("Health: " + to_string(player.GetHealth()), kHUDPlayerHealth.x, kHUDPlayerHealth.y);
			myFont->Draw("Lap: " + to_string(currentLap) + "/" + to_string(kLaps), kHUDCurrentLap.x, kHUDCurrentLap.y);

			if (player.DisplayBoostWarning())
			{
				myFont->Draw("Boost Warning!!!", kHUDBoostWarning.x, kHUDBoostWarning.y);
			}
			else if (player.IsOverheated())
			{
				myFont->Draw("Boost Overheated!!!", kHUDBoostWarning.x, kHUDBoostWarning.y);
			}

			// Set the previous model positions
			player.SetPreviousX(player.GetModel()->GetX());
			player.SetPreviousZ(player.GetModel()->GetZ());

			// Then move the car after checking collisions
			player.GetModel()->Move(player.GetMomentum().x * frametime * gameSpeed, 0.0f, player.GetMomentum().z * gameSpeed * frametime);
			player.UpdateGrid();
			player.UpdateCollisionDelay(frametime);
			player.Hover(frametime, gameSpeed);

			// Check the player's boost
			// Only apply boost if the player is going forward
			// Only apply boost if the player is holding down forward key
			// Not sure which approach is the best
			if (myEngine->KeyHeld(EPlayerBoostKey) && player.CanUseBoost() && myEngine->KeyHeld(EPlayerIncreaseForwardThrust))
			{
				player.Boost(frametime);
				if (player.GetBoostTime() >= player.GetBoostMaxTime())
				{
					player.BoostOverheat();
				}
			}
			else
			{
				player.UpdateBoost(frametime);
			}

			// Check if the game should end as the player's health is 0.
			if (player.GetHealth() <= 0)
			{
				gameState = EGameStates::over;
			}

			// If the player didn't rotate this frame, move the car to the middle
			if (!playerRotated)
			{
				// Set to some threshold else the camera moves back and forth
				if (static_cast<int>(playerSidewaysRotation) > 0)
				{
					player.GetModel()->RotateLocalZ(-frametime * gameSpeed * player.GetRotationSpeed());
					playerSidewaysRotation += -frametime * gameSpeed * player.GetRotationSpeed();
				}
				else if (static_cast<int>(playerSidewaysRotation) < 0)
				{
					player.GetModel()->RotateLocalZ(frametime * gameSpeed * player.GetRotationSpeed());
					playerSidewaysRotation += frametime * gameSpeed * player.GetRotationSpeed();
				}
			}

			if (!playerAccelerated)
			{
				if (static_cast<int>(playerAccelerationRotation) < 0)
				{
					player.GetModel()->RotateLocalX(frametime * gameSpeed * player.GetRotationSpeed());
					playerAccelerationRotation += frametime * gameSpeed * player.GetRotationSpeed();
				}
			}

			playerRotated = false;
			playerAccelerated = false;

			if (myEngine->KeyHit(EGamePause))
			{
				gameState = EGameStates::paused;
			}

			break;
		}
		case EGameStates::over:
		{
			myFont->Draw("Game Over.", kHUDGameState.x, kHUDGameState.y);
			myFont->Draw("You lost.", kHUDGameOver.x, kHUDGameOver.y);
			myFont->Draw("Health: " + to_string(player.GetHealth()), kHUDPlayerHealth.x, kHUDPlayerHealth.y);
			break;
		}
		case EGameStates::paused:
		{
			myFont->Draw("Paused.", kHUDCurrentStage.x, kHUDCurrentStage.y);
			if (myEngine->KeyHit(EGamePause))
			{
				gameState = EGameStates::playing;
			}
			break;
		}
		case EGameStates::finished:
		{
			myFont->Draw("You have finished the race.", kHUDCurrentStage.x, kHUDCurrentStage.y);
			break;
		}
		default:
		{
			break;
		}
		}

		// Camera controls

		// The camera can't move forward beyond half of the player's length in the z axis
		if (myEngine->KeyHeld(ECameraForward) && myCamera->GetLocalZ() < HalfOf(player.GetLength()))
		{
			myCamera->MoveLocalZ(frametime * kCameraSpeed * gameSpeed);
		}
		// The camera can't go further behind that it's initial position
		else if (myEngine->KeyHeld(ECameraBackward) && myCamera->GetLocalZ() > kCameraPos[z3D])
		{
			myCamera->MoveZ(-frametime * kCameraSpeed * gameSpeed);
		}
		// The camera can't go sideways more than half of the initial z position.
		else if (myEngine->KeyHeld(ECameraLeft) && myCamera->GetLocalX() > HalfOf(kCameraPos[z3D]))
		{
			myCamera->MoveLocalX(-frametime * kCameraSpeed * gameSpeed);
		}
		else if (myEngine->KeyHeld(ECameraRight) && myCamera->GetLocalX() < HalfOf(-kCameraPos[z3D]))
		{
			myCamera->MoveLocalX(frametime * kCameraSpeed * gameSpeed);
		}
		else if (myEngine->KeyHit(ECameraReset))
		{
			cameraRotationX = 0.0f;
			cameraRotationY = 0.0f;
			myCamera->SetLocalPosition(kCameraPos[x3D], kCameraPos[y3D], kCameraPos[z3D]);
			myCamera->ResetOrientation();
		}
		else if (myEngine->KeyHit(ECameraFirstPerson))
		{
			cameraRotationX = 0.0f;
			cameraRotationY = 0.0f;
			constexpr float kX = 0.0f;
			constexpr float kY = 2.5f; // Height of first person camera
			const float kZ = HalfOf(player.GetLength()) - 4.0f;
			myCamera->SetLocalPosition(kX, kY, kZ);
			myCamera->ResetOrientation();
		}

		// Mouse control for camera on the rotation on the y axis. Camera looks sideways.
		// +ve to the right
		// -ve to the left
		mouseMovementX = static_cast<float>(myEngine->GetMouseMovementX());
		if (mouseMovementX > FLT_EPSILON && cameraRotationY + FLT_EPSILON < kCameraRotationMax)
		{
			cameraRotationY += (frametime * kCameraSpeed * gameSpeed * mouseMovementX);
			myCamera->RotateY(frametime * kCameraSpeed * gameSpeed * mouseMovementX);
		}
		else if (mouseMovementX < -FLT_EPSILON && cameraRotationY - FLT_EPSILON > -kCameraRotationMax)
		{
			cameraRotationY += (frametime * kCameraSpeed * gameSpeed * mouseMovementX);
			myCamera->RotateY(frametime * kCameraSpeed * gameSpeed * mouseMovementX);
		}

		// Mouse control for camera on the rotation on the x axis. Camera looks up and down.
		mouseMovementY = static_cast<float>(myEngine->GetMouseMovementY());
		if (mouseMovementY > FLT_EPSILON && cameraRotationX + FLT_EPSILON < kCameraRotationMax)
		{
			cameraRotationX += (frametime * kCameraSpeed * gameSpeed * mouseMovementY);
			myCamera->RotateLocalX(frametime * kCameraSpeed * gameSpeed * mouseMovementY);
		}
		else if (mouseMovementY < -FLT_EPSILON && cameraRotationX - FLT_EPSILON > -kCameraRotationMax)
		{
			cameraRotationX += (frametime * kCameraSpeed * gameSpeed * mouseMovementY);
			myCamera->RotateLocalX(frametime * kCameraSpeed * gameSpeed * mouseMovementY);
		}
		
		// Controls and toggles
		if (myEngine->KeyHit(EGameExit))
		{
			myEngine->Stop();
		}
		if (myEngine->KeyHit(EGameToggleMouseCapture))
		{
			(isMouseCaptured) ? myEngine->StopMouseCapture() : myEngine->StartMouseCapture();
			isMouseCaptured = !isMouseCaptured;
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
	return CodeSuccess;
}