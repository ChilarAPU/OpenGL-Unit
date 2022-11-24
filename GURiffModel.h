
#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <CoreStructures\gu_math.h>
#include <CoreStructures\GUObject.h>

struct RiffChunk {

	DWORD			chunkSize;
	gu_byte			*data;

	RiffChunk() : chunkSize(0), data(nullptr) {}
};

class GURiffModel : public CoreStructures::GUObject {

	DWORD							riffFileSize;
	DWORD							riffType; // WAVE for example - see docs

											  // store RIFF blocks in a map (dictionary) - encode key as a 4 byte block ID and the actual data as a pointer to a buffer
	std::map<DWORD, RiffChunk>		riffModel;

	void setupRiffModelFromFile(const char* filepath);

public:

	// build a RIFF model from a given RIFF-based file (.wav for example)
	GURiffModel(const char* filepath);
	~GURiffModel();

	RiffChunk riffChunkForKey(DWORD key);
};


