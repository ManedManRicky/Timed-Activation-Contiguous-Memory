/*****************************************************************//**
 * \file   timedFuse.c
 * \brief  C file implementing the functions declared in 'timedFuse.h'.
 * Contiguous memory version.
 * 
 * \author oldte
 * \date   April 2024
 *********************************************************************/
/*------------------------------------------------------*/
#pragma region Preprocessor
/*Standard-Library Headers*/
#include <stdlib.h>
#include <string.h>
/*User-Defined Headers*/
#include "timedFuse.h"
/*Definitions*/
/*Macros*/
#pragma endregion

/*------------------------------------------------------*/
#pragma region Static_Functions

static int addStatic(fuses_t* fuses, void* fuseData, size_t fuseTimeSec, onFuseEnd fuseEndFunc) {
	/*Error Correction*/
	if ((fuses == NULL) || (fuseData == NULL) || (fuseEndFunc == NULL)) { //If bad parameters were inputted
		return 1;
	}

	size_t idx = 0;

	/*First Node Check*/
	if (fuses->fuses == NULL) { //If this is the first node
		/*Allocate New Fuse*/
		fuses->fuses = (fuseObj_t*)calloc(1, sizeof(fuseObj_t));
		if (fuses->fuses == NULL) { //If it failed to allocate the memory
			return 1;
		}
	}
	else { //If this isn't the first node
		/*Allocate New Fuse*/
		fuseObj_t* temp = (fuseObj_t*)calloc((fuses->numFuses + 1), sizeof(fuseObj_t));
		if (temp == NULL) { //if it failed to allocate the memory
			return 1;
		}

		/*Copy Previous Data*/
		#ifdef _WIN32 //If this is being compiled for Windows platforms
			memcpy_s(temp, ((fuses->numFuses + 1) * sizeof(fuseObj_t)), fuses->fuses, (fuses->numFuses * sizeof(fuseObj_t)));
		#else //If this is not being compiled for Windows platforms
			memcpy(temp, fuses->fuses, (fuses->numFuses * sizeof(fuseObj_t)));
		#endif
		free(fuses->fuses);
		fuses->fuses = temp;

		/*Set Index*/
		idx = fuses->numFuses;
	}

	/*Copy Data*/
	fuses->fuses[idx].fuseData = fuseData;
	fuses->fuses[idx].startTime = time(NULL);
	fuses->fuses[idx].fuseTimeSec = fuseTimeSec;
	fuses->fuses[idx].fuseEndFunc = fuseEndFunc;

	/*Increment Fuse Count*/
	fuses->numFuses++;

	/*Success*/
	return 0;
}

static int removeStatic(fuses_t* fuses, size_t idx) {
	/*Error Correction*/
	if (!((fuses == NULL) || !((0 <= idx) && (idx <= (fuses->numFuses - 1))))) { //If good parameters were inputted
		/*Scenario Check*/
		fuseObj_t* stitchData = NULL;
		size_t advanceAmt = 0;
		if (idx == 0) { //Head is being removed | If of length one, then it's considered a head
			/*Size Switch*/
			switch (fuses->numFuses) {
				/*Single Fuse*/
				case 1:
					free(fuses->fuses);
					fuses->fuses = NULL;
					fuses->numFuses = 0;
					return 0;

				/*Mutliple Fuses*/
				default:
					stitchData = &fuses->fuses[1];
			}
		} else
		if ((0 <= idx) && (idx <= (fuses->numFuses - 1))) { //Middle section is being removed
			stitchData = &fuses->fuses[idx + 1];
			advanceAmt = idx;
		}

		/*Allocate Memory*/
		fuseObj_t* temp = (fuseObj_t*)calloc((fuses->numFuses - 1), sizeof(fuseObj_t));
		if (temp == NULL) { //If temp is NULL and this isn't the only fuse that's being removed
			return 1;
		}

		/*Data Copying*/
		#ifdef _WIN32 //If this is being compiled for Windows platforms
			memcpy_s(temp, ((fuses->numFuses - 1) * sizeof(fuseObj_t)), fuses->fuses, (idx * sizeof(fuseObj_t))); //Before removed section
			if (!(stitchData == NULL)) { //If stitchData isn't NULL
				memcpy_s(&temp[advanceAmt], (((fuses->numFuses - 1) - advanceAmt) * sizeof(fuseObj_t)), stitchData, (((fuses->numFuses - 1) - advanceAmt) * sizeof(fuseObj_t))); //After removed section
			}
		#else //If this is not being compiled for Windows platforms
			memcpy(temp, fuses->fuses, (idx * sizeof(fuseObj_t))); //Before removed section
			if (!(stitchData == NULL)) { //If stitchData isn't NULL
				memcpy(&temp[advanceAmt], stitchData, (((fuses->numFuses - 1) - advanceAmt) * sizeof(fuseObj_t))); //After removed section
			}
		#endif
		free(fuses->fuses);
		fuses->fuses = temp;
		fuses->numFuses--;

		/*Success*/
		return 0;
	}

	/*Failure*/
	return 1;
}

static int pollStatic(fuses_t* fuses, size_t idx) {
	/*Error Correction*/
	if (!((fuses == NULL) || !((0 <= idx) && (idx <= (fuses->numFuses - 1))))) { //If good parameters were inputted
		/*Fuse Timer Check*/
		if ((unsigned)time(NULL) > (fuses->fuses[idx].startTime + fuses->fuses[idx].fuseTimeSec)) { //If the timer has run up
			/*Execute Fuse Function*/
			(fuses->fuses[idx].fuseEndFunc)(fuses->fuses[idx].fuseData);

			/*Remove Fuse*/
			if (removeStatic(fuses, idx)) { //If there was a failure, for whatever reason
				/*Failure*/
				return 1;
			}

			/*Success*/
			return 0;
		}
	}

	/*Failure*/
	return 1;
}

static void pollAllStatic(fuses_t* fuses) {
	/*Error Correction*/
	if (fuses != NULL) {
		/*Polling Loop*/
		size_t lastSz = fuses->numFuses;
		for (size_t idx = 0; idx < fuses->numFuses; idx++) { //For every fuse
			pollStatic(fuses, idx);

			/*Fuse Count Change Check*/
			if (fuses->numFuses != lastSz) { //If the previous size differs from the current size/
				lastSz = fuses->numFuses;
				idx--;
			}
		}
	}
}

static int resetStatic(fuses_t* fuses, size_t idx) {
	/*Error Correction*/
	if (!((fuses == NULL) || !((0 <= idx) && (idx <= (fuses->numFuses - 1))))) { //If good parameters were inputted
		/*Reset Fuse*/
		fuses->fuses[idx].startTime = time(NULL);
	}

	/*Failure*/
	return 1;
}

static void resetAllStatic(fuses_t* fuses) {
	/*Error Correction*/
	if (fuses != NULL) {
		/*Reseting Loop*/
		for (size_t idx = 0; idx < fuses->numFuses; idx++) { //For every fuse
			fuses->fuses[idx].startTime = time(NULL);
		}
	}
}

static void clearStatic(fuses_t* fuses) {
	/*Error Correction*/
	if (fuses != NULL) {
		/*Clear Fuses*/
		free(fuses->fuses);
		fuses->numFuses = 0;
		fuses->fuses = NULL;
	}
}

static void closeStatic(fuses_t** fuses) {
	/*Error Correction*/
	if (!((fuses == NULL) || (*fuses == NULL))) { //If good parameters were inputted
		clearStatic(*fuses);

		free(*fuses);
		*fuses = NULL;
	}
}
#pragma endregion

/*------------------------------------------------------*/

/**
 * Method table.
 */
static fuseMethods_t methods = {
	.add = &addStatic,
	.remove = &removeStatic,
	.poll = &pollStatic,
	.pollAll = &pollAllStatic,
	.reset = &resetStatic,
	.resetAll = &resetAllStatic,
	.clear = &clearStatic,
	.close = &closeStatic
};

/*------------------------------------------------------*/
#pragma region Functions

fuses_t* newFuseContainer(void) {
	/*Instance Creation*/
	fuses_t* newFuses = (fuses_t*)calloc(1, sizeof(fuses_t));
	if (newFuses == NULL) {
		return NULL;
	}

	/*Setup Method Table*/
	newFuses->methods = &methods;

	/*Return New Instance*/
	return newFuses;
}
#pragma endregion
