#pragma once
#include <CoreStructures/GUPivotCamera.h>
#include <CoreStructures\GUObject.h>
//sound related includes
#include "src/GURiffModel.h"
#include "al.h"
#include "alc.h"
#include "efx.h"
#include "EFX-Util.h"
#include "efx-creative.h"
#include <xram.h>
#include <mmreg.h>

class Sound : public CoreStructures::GUObject
{
private:

public:
	Sound(const char* filepath, CoreStructures::GUPivotCamera* mainCamera, ALenum format = AL_FORMAT_MONO16);
};

