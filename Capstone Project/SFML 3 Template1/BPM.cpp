#include "BPM.h"

BPM::BPM(float smoothing)
	: source(nullptr), tempo(nullptr), buffer(nullptr),
	  bpm(0.0f), smoothedBPM(0.0f), smoothingFactor(smoothing)
{
}

BPM::~BPM()
{
}

bool BPM::loadFile(const std::string& filePath)
{
	return false;
}

void BPM::update()
{
}

float BPM::getBPM() const
{
	return 0.0f;
}

std::string BPM::getBPMState() const
{
	return std::string();
}

float BPM::getIntensityMultiplier() const
{
	return 0.0f;
}

void BPM::smoothBPM(float newBPM)
{

}
