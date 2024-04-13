#include <stdio.h>
#include "timedFuse.h"

static void printThing(void* fuseData) {
	printf("%s", (char*)fuseData);
}

int main(void) {
	fuses_t* fuses = newFuseContainer();
	(fuses->_add)(fuses, (void*)"Ten (10) second delay\n", 10, &printThing);
	(fuses->_add)(fuses, (void*)"Five (5) second delay\n", 5, &printThing);
	(fuses->_add)(fuses, (void*)"One (1) second delay\n", 1, &printThing);
	(fuses->_add)(fuses, (void*)"Twelve (12) second delay\n", 12, &printThing);
	(fuses->_add)(fuses, (void*)"Six (6) second delay\n", 6, &printThing);
	(fuses->_add)(fuses, (void*)"Eight (8) second delay\n", 8, &printThing);

	while (fuses->numFuses != 0) {
		(fuses->_pollAll)(fuses);
	}

	(fuses->_close)(&fuses);

	return 0;
}
