#include "Movement/PMAcceleration.h"

void FPMAcceleration::UpdateAcceleration()
{
	m_acceleration = m_acceleration.GetClampedToMaxSize(MaxAcceleration);
}