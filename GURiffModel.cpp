
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include "GURiffModel.h"

using namespace std;


void GURiffModel::setupRiffModelFromFile(const char *filepath) {

	FILE		*fp = nullptr;

	// little-endian encoding of RIFF chunk IDs
	const DWORD RIFF_HEADER = 'FFIR'; // RIFF
	const DWORD RIFF_WAVE = 'EVAW'; // WAVE

	DWORD		header, size;

	try
	{
		if (!filepath)
			throw("no RIFF file specified");

		fopen_s(&fp, filepath, "rb");

		if (!fp)
			throw("cannot open RIFF file");

		struct stat fileStatus;
		int file_error = stat(filepath, &fileStatus);

		if (file_error != 0)
			throw("cannot create file info structure");

		_off_t fileSize = fileStatus.st_size;



		// read header ('RIFF')
		fread_s(&header, sizeof(DWORD), sizeof(DWORD), 1, fp);

		if (header != RIFF_HEADER)
			throw("expecting a RIFF file header - not found");

		// read file size (excludes 'RIFF' header + size DWORD)->only chunk data (file contents)
		fread_s(&riffFileSize, sizeof(DWORD), sizeof(DWORD), 1, fp);
		// read type
		fread_s(&riffType, sizeof(DWORD), sizeof(DWORD), 1, fp);

		// validate type
		if (riffType != RIFF_WAVE)
			throw("expecting WAVE sub-type");

		// initialise total byte read to RIFF chunk header + type info
		_off_t numBytesRead = 12;

		// read each chunk and store in the RIFF model dictionary
		while (numBytesRead < fileSize) {

			// read header
			fread_s(&header, sizeof(DWORD), sizeof(DWORD), 1, fp);

			// read chunk size
			fread_s(&size, sizeof(DWORD), sizeof(DWORD), 1, fp);

			gu_byte* buffer = (gu_byte*)malloc(size);

			if (!buffer)
				throw("cannot create buffer for chunk");

			// read chunk data
			fread_s(buffer, size, size, 1, fp);

			numBytesRead += (sizeof(DWORD) << 1) + size;

			// setup new RiffChunk
			RiffChunk		C;

			C.chunkSize = size;
			C.data = buffer;

			// store in map (assumes ownership of buffer)
			riffModel[header] = C;
		}

		// close file
		fclose(fp);
	}
	catch (char *err)
	{
		cout << err << endl;

		// cleanup local resources

		// close file
		if (fp)
			fclose(fp);

		throw; // rethrow error
	}
}

GURiffModel::GURiffModel(const char* filepath) {

	riffModel = map<DWORD, RiffChunk>();

	setupRiffModelFromFile(filepath);
}


GURiffModel::~GURiffModel() {
}


RiffChunk GURiffModel::riffChunkForKey(DWORD key) {

	return riffModel[key];
}
