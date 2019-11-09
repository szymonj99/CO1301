#pragma once
#include <TL-Engine.h>
#include "MyConstants.h"
#define _USE_MATH_DEFINES // Used to get constant pi.
#include <math.h> // Used to round numbers up for FPS limiting.

class IAirplane
{
private:
	// Private properties.

	tle::IModel* _model; // The model used by IAirplane.
	float _mass; // The airplane's mass in kilograms.
	float _weight; // The airplane's weight in Newtons.
	float _velocity; // The plane's velocity in m/s.
	float _area; // The plane's front area in m^2.
	float _airResistance; // The plane's air resistance in Newtons.
	float _airDensityAroundPlane; // The air density around the plane. This changes with height.
	float _dragCoefficient; // The plane's drag coefficient.
	float _upwardsLift; // The plane's upwards lift in Newtons.
	float _downwardsLift; // The plane's downards lift in Newtons.
	float _resultantLift; // The plane's resultant lift in Newtons.
	float _wingArea; // The plane's area of wings in m^2.
	float _propellerDiameter; // The propeller's diameter in m.
	int _propellerMaxRPM; // The propeller's max revolutions per minute.
	float _propellerCurrentRPM; // The propeller's current revolutions per minute.
	float _propellerThrust; // The propeller's thrust in Newtons.
	float _rotationX; // The plane's rotation along its X axis in degrees.
	float _rotationY; // The plane's rotation along its Y axis in degrees.
	float _rotationZ; // The plane's rotation along its Z axis in degrees.

public:
	// Getters, Setters and Changers.

	tle::IModel* GetModel()
	{
		return _model;
	}
	void SetModel(tle::IModel* model)
	{
		_model = model;
	}

	float GetMass()
	{
		return _mass;
	}
	void ChangeMass(float change)
	{
		_mass += change;
	}

	float GetWeight()
	{
		return _weight;
	}
	void ChangeWeight(float change)
	{
		_weight += change;
	}

	float GetVelocity()
	{
		return _velocity;
	}
	void ChangeVelocity(float change)
	{
		_velocity += change;
	}

	float GetArea()
	{
		return _area;
	}
	void ChangeArea(float change)
	{
		_area += change;
	}

	float GetAirResistance()
	{
		return _airResistance;
	}
	void ChangeAirResistance(float change)
	{
		_airResistance += change;
	}

	float GetAirDensityAroundPlane()
	{
		return _airDensityAroundPlane;
	}
	void ChangeAirDensityAroundPlane(float change)
	{
		_airDensityAroundPlane += change;
	}

	float GetDragCoefficient()
	{
		return _dragCoefficient;
	}
	void ChangeDragCoefficient(float change)
	{
		_dragCoefficient += change;
	}

	float GetUpwardsLift()
	{
		return _upwardsLift;
	}
	void ChangeUpwardsLift(float change)
	{
		_upwardsLift += change;
	}

	float GetDownwardsLift()
	{
		return _downwardsLift;
	}
	void ChangeDownwardsLift(float change)
	{
		_downwardsLift += change;
	}

	float GetResultantLift()
	{
		return _resultantLift;
	}
	void ChangeResultantLift(float change)
	{
		_resultantLift += change;
	}

	float GetWingArea()
	{
		return _wingArea;
	}
	void ChangeWingArea(float change)
	{
		_wingArea += change;
	}

	float GetPropellerDiameter()
	{
		return _propellerDiameter;
	}
	void ChangePropellerDiameter(float change)
	{
		_propellerDiameter += change;
	}

	float GetPropellerMaxRPM()
	{
		return _propellerMaxRPM;
	}
	void ChangePropellerMaxRPM(float change)
	{
		_propellerMaxRPM += change;
	}

	float GetPropellerCurrentRPM()
	{
		return _propellerCurrentRPM;
	}
	void ChangePropellerCurrentRPM(float change)
	{
		_propellerCurrentRPM += change;
	}
	
	float GetPropellerThrust()
	{
		return _propellerThrust;
	}
	void ChangePropellerThrust(float change)
	{
		_propellerThrust += change;
	}
	
	float GetRotationX()
	{
		return _rotationX;
	}
	void ChangeRotationX(float change)
	{
		_rotationX += change;
	}
	
	float GetRotationY()
	{
		return _rotationY;
	}
	void ChangeRotationY(float change)
	{
		_rotationY += change;
	}
	
	float GetRotationZ()
	{
		return _rotationZ;
	}
	void ChangeRotationZ(float change)
	{
		_rotationZ += change;
	}
	
	//Update the plane's current air densirty around plane.
	void UpdateAirDensityAroundPlane()
	{
		// Only update if the value actually changed.
		if (-GetAirDensityAroundPlane() != (airDensity - GetModel()->GetY() / 1000))
		{
			ChangeAirDensityAroundPlane(-GetAirDensityAroundPlane() + (airDensity - GetModel()->GetY() / 1000));
		}
	}
	
	// Update the plane's current air resistance.
	void UpdateAirResistance()
	{
		// Only update if the value actually changed.
		if ((GetAirDensityAroundPlane() * GetDragCoefficient() * GetArea() / 2) * pow(GetVelocity(), 3) != GetAirResistance())
		{
			if (GetAirDensityAroundPlane() < 0.1)
			{
				ChangeAirDensityAroundPlane(0.1 - GetAirDensityAroundPlane());
			}
			ChangeAirResistance((GetAirDensityAroundPlane() * GetDragCoefficient() * GetArea() / 2) * pow(GetVelocity(), 3) - GetAirResistance());
		}
	}
	
	// Update the propeller's thrust.
	void UpdatePropellerThrust()
	{
		// Only update if the value actually changed.
		if ((GetPropellerThrust() != (GetAirDensityAroundPlane() * GetPropellerCurrentRPM() * M_PI * pow(GetPropellerDiameter() / 2, 2))))
		{
			ChangePropellerThrust((GetAirDensityAroundPlane() * GetPropellerCurrentRPM() * M_PI * pow(GetPropellerDiameter() / 2, 2)) - GetPropellerThrust());
		}
	}
	
	// Update the plane's velocity
	void UpdatePlaneVelocity()
	{
		ChangeVelocity(((GetPropellerThrust() / gravity - GetAirResistance() / gravity) / pow(gravity, 2)));
	}
	
	// Update the plane's upwards force.
	void UpdatePlaneUpwardsLift()
	{
		// Only update if the value actually changed.
		if (GetUpwardsLift() != ((0.5 * GetAirDensityAroundPlane() * pow(GetVelocity(), 2) * GetWingArea()) / 2) * cos(GetRotationX() * M_PI / 180))
		{
			ChangeUpwardsLift(((0.5 * GetAirDensityAroundPlane() * pow(GetVelocity(), 2) * GetWingArea()) / 2) * cos(GetRotationX() * M_PI / 180) - GetUpwardsLift());
		}
	}

	// Update the plane's downward force.
	void UpdatePlaneDownwardsLift()
	{
		// Only update if the value actually changed.
		if (GetDownwardsLift() != (GetWeight() * sin(GetRotationX() * M_PI / 180)))
		{
			ChangeDownwardsLift((GetWeight() * sin(GetRotationX() * M_PI / 180)) - GetDownwardsLift() + GetWeight());
			cout << GetDownwardsLift() << endl;
		}
	}

	// Update the plane's resultant lift.
	void UpdatePlaneResultantLift()
	{
		if (GetResultantLift() != GetUpwardsLift() - GetDownwardsLift())
		{
			ChangeResultantLift(-GetResultantLift() + GetUpwardsLift() - GetDownwardsLift());
		}
	}

	// Move the plane forwards, down/up.
	void MovePlane()
	{
		if (GetVelocity() > 0)
		{
			GetModel()->MoveLocalZ(GetVelocity() / (speedMultiplier * 5));
		}
		/*if (GetResultantLift() > 0)
		{
			GetModel()->MoveLocalY((GetResultantLift() / pow(gravity, 2)) / (pow(gravity, 2) * speedMultiplier));
		}*/
		if (GetDownwardsLift() > GetResultantLift())
		{
			GetModel()->MoveY((GetResultantLift() / pow(gravity, 2)) / (pow(gravity, 2) * speedMultiplier));
		}
		//model->MoveLocalZ(velocity / speedMultiplier);
	}
};