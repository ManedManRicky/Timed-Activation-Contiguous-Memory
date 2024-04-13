/*****************************************************************//**
 * \file   timedFuse.c
 * \brief  C file implementing the functions declared in 'timedFuse.h'.
 * 
 * \author oldte
 * \date   April 2024
 *********************************************************************/
/*------------------------------------------------------*/
#pragma region Preprocessor
/*Standard-Library Headers*/
#include <stdlib.h>
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

	fuseObj_t* curFuse = NULL;

	/*Scenario Switch*/
	if (fuses->fuses == NULL) { //If it's the first fuse
		/*Create New Fuse*/
		fuses->fuses = (fuseObj_t*)calloc(1, sizeof(fuseObj_t));
		if (fuses->fuses == NULL) { //If it failed to create a new instance
			return 1;
		}

		curFuse = fuses->fuses;
	}
	else { //If it's not the first fuse
		/*Itteration Loop*/
		fuseObj_t* nextFuse = fuses->fuses;
		while (nextFuse->nextFuse != NULL) { //While the current fuse is not the last
			nextFuse = nextFuse->nextFuse;
		}

		/*Create New Fuse*/
		nextFuse->nextFuse = (fuseObj_t*)calloc(1, sizeof(fuseObj_t));
		if (nextFuse->nextFuse == NULL) { //If it failed to create a new instance
			return 1;
		}

		/*Update Pointer*/
		curFuse = nextFuse->nextFuse;
	}

	/*Copy Data*/
	curFuse->fuseData = fuseData;
	curFuse->startTime = time(NULL); //Current time
	curFuse->fuseTimeSec = fuseTimeSec;
	curFuse->fuseEndFunc = fuseEndFunc;
	fuses->numFuses++; //Increment the fuse counter

	/*Success*/
	return 0;
}

static int removeStatic(fuses_t* fuses, size_t idx) {
	/*Error Correction*/
	if (!((fuses == NULL) || !((0 <= idx) && (idx <= (fuses->numFuses - 1))))) { //If good parameters were inputted
		/*Itterate Fuse List*/
		size_t fuseIdx = 0;
		fuseObj_t* curFuse = fuses->fuses;
		fuseObj_t* prevFuse = NULL;
		while (fuseIdx != idx) { //While the current fuse index is not the requested fuse index
			prevFuse = curFuse;
			curFuse = curFuse->nextFuse;

			fuseIdx++;
		}

		/*Remove Fuse*/
		fuseObj_t* nextFuse = curFuse->nextFuse;
		free(curFuse);
		if (prevFuse == NULL) { //If the furse that was removed was not the head
			fuses->fuses = nextFuse;
		}
		else { //If the fuse that was removed was the head
			prevFuse->nextFuse = nextFuse;
		}

		/*Decrement Number Of Fuses*/
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
		/*Itterate Fuse List*/
		size_t fuseIdx = 0;
		fuseObj_t* curFuse = fuses->fuses;
		while (fuseIdx != idx) { //While the current fuse index is not the requested fuse index 
			curFuse = curFuse->nextFuse;

			fuseIdx++;
		}

		/*Check Status*/
		if ((unsigned)time(NULL) > (curFuse->startTime + curFuse->fuseTimeSec)) { //If the delay has ended
			/*Execute End Function*/
			(*curFuse->fuseEndFunc)(curFuse->fuseData); 

			/*Remove Spent Fuse*/
			removeStatic(fuses, fuseIdx);

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
		/*Itterate Fuse List*/
		fuseObj_t* prevFuse = NULL;
		fuseObj_t* curFuse = fuses->fuses;
		fuseObj_t* nextFuse = NULL;
		while (curFuse != NULL) { //While the current fuse isn't the last
			/*Check Status*/
			if ((unsigned)time(NULL) > (curFuse->startTime + curFuse->fuseTimeSec)) { //If the delay has ended | Stitch & Advance
				/*Execute End Function*/
				(*curFuse->fuseEndFunc)(curFuse->fuseData);
				nextFuse = curFuse->nextFuse;

				/*Remove Fuse*/
				free(curFuse);
				if (prevFuse == NULL) { //If the removed fuse was the head
					fuses->fuses = nextFuse;
				}
				else { //If the removed fuse wasn't the head
					prevFuse->nextFuse = nextFuse;
				}
				curFuse = nextFuse;

				/*Decrement Fuse Count*/
				fuses->numFuses--;
			}
			else { //The delay hasn't ended | Advance
				/*Update Pointers*/
				prevFuse = curFuse;
				curFuse = curFuse->nextFuse;
				nextFuse = ((curFuse == NULL) ? (NULL) : (curFuse->nextFuse));
			}
		} 
	}
}

static int resetStatic(fuses_t* fuses, size_t idx) {
	/*Error Correction*/
	if (!((fuses == NULL) || !((0 <= idx) && (idx <= (fuses->numFuses - 1))))) { //If good parameters were inputted
		/*Itterate Fuse List*/
		size_t fuseIdx = 0;
		fuseObj_t* curFuse = fuses->fuses;
		while (fuseIdx != idx) { //While the index isn't the requested index
			curFuse = curFuse->nextFuse;

			fuseIdx++;
		}

		/*Reset Fuse Timer*/
		curFuse->startTime = time(NULL);

		/*Success*/
		return 0;
	}

	/*Failure*/
	return 1;
}

static void resetAllStatic(fuses_t* fuses) {
	/*Error Correction*/
	if (fuses != NULL) {
		/*Itterate Fuse List*/
		fuseObj_t* curFuse = fuses->fuses;
		for (size_t idx = 0; idx < fuses->numFuses; idx++) {
			/*Reset Fuse Timer*/
			curFuse->startTime = time(NULL);
			curFuse = curFuse->nextFuse;
		}
	}
}

static void clearStatic(fuses_t* fuses) {
	/*Error Correction*/
	if (fuses != NULL) {
		/*Itterate Fuse List*/
		fuseObj_t* curFuse = fuses->fuses;
		fuseObj_t* nextFuse = NULL;
		while (curFuse != NULL) {//Do while there is a fuse after this
			/*Save The Next Fuse*/
			nextFuse = curFuse->nextFuse;

			/*Free Fuse*/
			free(curFuse);
			fuses->numFuses--;

			curFuse = nextFuse;
		} 

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
