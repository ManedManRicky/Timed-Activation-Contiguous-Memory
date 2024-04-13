/*****************************************************************//**
 * \file   timedFuse.h
 * \brief  Header file defining fuse class used for timed activation/
 * action on objects.
 * 
 * \author oldte
 * \date   April 2024
 *********************************************************************/
#ifndef TIMEDFUSE_INCLUDE
	/*------------------------------------------------------*/
	#pragma region Preprocessor
	/*Include Guard*/
	#define TIMEDFUSE_INCLUDE
	/*Standard-Library Headers*/
	#include <stdint.h>
	#include <time.h>
	/*User-Defined Headers*/
	/*Definitions*/
	/*Macros*/
	#define _add methods->add
	#define _remove methods->remove
	#define _poll methods->poll
	#define _pollAll methods->pollAll
	#define _reset methods->reset
	#define _resetAll methods->resetAll
	#define _clear methods->clear
	#define _close methods->close
	#pragma endregion

	/*------------------------------------------------------*/
	/*Global Objects/Variables*/

	/**
	 * Type definition for fuse function.
	 */
	typedef void (*onFuseEnd)(void* fuseData);

	/**
	 * Structure for an individual fuse.
	 */
	typedef struct fuseObj_t {
		/*Fuse Data*/
		void* fuseData;
		time_t startTime;
		size_t fuseTimeSec;

		/*Interface*/
		/**
		 * \brief Pointer to the function that'll be executed at the end of the fuse.
		 * 
		 * \param[in] void* fuseData
		 */
		onFuseEnd fuseEndFunc;
	} fuseObj_t;
	
	/**
	 * Compiler has a stroke if I don't add this.
	 */
	typedef struct fuses_t fuses_t;

	/**
	 * Method table for fuse container.
	 */
	typedef struct fuseMethods_t {
		/**
		 * \brief Add a new fuse to the fuse container.
		 * 
		 * \param[in] fuseData_t* fuses
		 * \param[in] void* fuseData
		 * \param[in] size_t fuseTimeMS
		 * \param[in] onFuseEnd fuseEndFunc
		 * 
		 * \returns Success flag
		 */
		int (*add)(fuses_t* fuses, void* fuseData, size_t fuseTimeSec, onFuseEnd fuseEndFunc);
		/**
		 * \brief Remove a fuse from the container at the index 'idx'.
		 *
		 * \param[in] fuses_t* fuses
		 * \param[in] size_t idx
		 * 
		 * \returns Success flag
		 */
		int (*remove)(fuses_t* fuses, size_t idx);
		/**
		 * \brief Poll the status of a fuse at the index 'idx'.
		 *
		 * \param[in] fuses_t* fuses
		 * \param[in] size_t idx
		 * 
		 * \returns Success flag
		 */
		int (*poll)(fuses_t* fuses, size_t idx);
		/**
		 * \brief Poll the status of all fuses in the fuse container.
		 * Tested to not cause memory leaks.
		 *
		 * \param[in] fuses_t* fuses
		 */
		void (*pollAll)(fuses_t* fuses);
		/**
		 * \brief Reset the fuse timer of the fuse at index 'idx'.
		 *
		 * \param[in] fuses_t* fuses
		 * \param[in] size_t idx
		 * 
		 * \returns Success flag
		 */
		int (*reset)(fuses_t* fuses, size_t idx);
		/**
		 * \brief Reset the user timer for all of the fuses.
		 *
		 * \param[in] fuses_t* fuses
		 */
		void (*resetAll)(fuses_t* fuses);
		/**
		 * \brief Clear all of the fuses from the fuse container.
		 * Tested to not cause memory leaks.
		 *
		 * \param[in] fuses_t* fuses
		 */
		void (*clear)(fuses_t* fuses);
		/**
		 * \brief Clear and close the fuse container instance.
		 * Tested to not cause memory leaks.
		 *
		 * \param[in,out] fuses_t** fuses
		 */
		void (*close)(fuses_t** fuses);
	} fuseMethods_t;

	/**
	 * Structure for a fuse container.
	 */
	typedef struct fuses_t {
		/*Fuses*/
		fuseObj_t* fuses;
		size_t numFuses;

		/*Method Table*/
		fuseMethods_t* methods;
	} fuses_t;

	/*------------------------------------------------------*/
	/*Function Prototypes*/

	/**
	 * \brief Create a new fuse container
	 *
	 * \returns Pointer to new fuse container
	 */
	fuses_t* newFuseContainer(void);
#endif
