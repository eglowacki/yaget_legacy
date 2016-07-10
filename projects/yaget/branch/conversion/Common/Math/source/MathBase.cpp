#include "Precompiled.h"
#include "Math\MathBase.h"
#include <stdlib.h>


namespace eg {

//! This will return random number from 0 to 1
float GetRandom()
{
	return static_cast<float>(rand()) / RAND_MAX;
}


float GetRandomRange(float lowValue, float hiValue)
{
    float t = GetRandom();
	return (hiValue - lowValue) * t + lowValue;
}


//! Return random value from 0 to NumberOfSides. Simulate dice roll
int GetDice(int numberSides)
{
	float value = GetRandom();
	// and now scale to fit within MinRange and MaxRange
	float newResult = (numberSides-1) * value;
	// we need to round it to the nearest integer
	return static_cast<int>(newResult + 0.5f);
}


} // namespace eg
