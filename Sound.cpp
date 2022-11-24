#include "Sound.h"
Sound::Sound(const char* filepath, CoreStructures::GUPivotCamera* mainCamera, ALenum format)
{
	//SOUND SETUP//
	ALCdevice* alcDevice = alcOpenDevice(NULL);
	ALCcontext* alcContext = nullptr;

	alcContext = alcCreateContext(alcDevice, NULL);
	alcMakeContextCurrent(alcContext);

	ALuint soundBuffer;
	alGenBuffers(1, &soundBuffer);

	auto mySoundData = new GURiffModel(filepath);

	RiffChunk formatChunk = mySoundData->riffChunkForKey('tmf');
	RiffChunk dataChunk = mySoundData->riffChunkForKey('atad');

	WAVEFORMATEXTENSIBLE wv;
	memcpy_s(&wv, sizeof(WAVEFORMATEXTENSIBLE), formatChunk.data, formatChunk.chunkSize);

	alBufferData(
		soundBuffer,
		format,
		(ALvoid*)dataChunk.data,
		(ALsizei)dataChunk.chunkSize,
		(ALsizei)wv.Format.nSamplesPerSec
	);

	ALuint soundSource;
	alGenSources(1, &soundSource);

	//Attach soundbuffer to soundsource
	alSourcei(soundSource, AL_BUFFER, soundBuffer);
	//enables sound looping
	alSourcei(soundSource, AL_LOOPING, 1);

	alSource3f(soundSource, AL_POSITION, 10.0, 0.0, 0.0); //starting position of sound
	alSource3f(soundSource, AL_VELOCITY, 0.0, 0.0, 0.0); // speed at which the sound is moving
	alSource3f(soundSource, AL_DIRECTION, 0.0, 0.0, 0.0); // direction the sound is facing

	auto cameraLocation = mainCamera->cameraLocation();

	//listener values. 
	ALfloat listenerVel[] = { 0.0, 0.0, 0.0 }; 
	ALfloat listenerOri[] = { 0.0, 0.0, 0, 0.0, 0, 0.0 };

	alListenerfv(AL_POSITION, (ALfloat*)(&cameraLocation));
	alListenerfv(AL_VELOCITY,listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	alSourcePlay(soundSource);
}
