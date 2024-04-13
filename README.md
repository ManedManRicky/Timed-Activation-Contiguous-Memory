# Timed-Activation

This simple C library allows for the timed activation of functions and other similar objects that are used with it. 
This library is not for a specific datatype as it is a generic library. All that needs to be created is the function 
that'll be executed once the time runs out. All of the data is stored within contiguous memory (An array). A version
that uses a linked list structure can be found [here](https://github.com/ManedManRicky/Timed-Activation-Linked-List)

### Structures and Definitions
---
**onFuseEnd**

` typedef void (*onFuseEnd)(void* fuseData); `

This is the type definition for the function that'll be executed once the fuse has run out of time. It has one 
parameter and that is a void pointer to the fuse data. The way that this data is interpreted is up to the programmer.

**fuseObj_t**

`void* fuseData;`

`time_t startTime;`

`size_t fuseTimeSec;`

`onFuseEnd fuseEndFunc;`

`struct fuseObj_t* nextFuse;`

This structure defines an individual fuse. It has five different variables:
1. A void pointer called `fuseData`, where the data associated with the fuse is located
2. A variable of the type `time_t` called `startTime`, which notes when the fuse started
3. A variable of the type `size_t` called `fuseTimeSec`, which notes how long the fuse is gonna last for in seconds
4. A function pointer of the type `onFuseEnd` called `fuseEndFunc`, which is the function that'll be executed once the fuse time ends 
5. A pointer to the next fuse instance called `nextFuse`

**fuseMethods_t**

This structure contains all of the methods that act upon a fuse container instance. Each method will be described 
in the methods section.

**fuses_t**

`fuseObj_t* fuses;`

`size_t numFuses;`

`fuseMethods_t* methods;`

This structure contains all of the fuses that are associated with this container. It has three different variables: 
1. A pointer of the type `fuseObj_t` called `fuses`, which contains all of the individual fuses
2. A variable of the type `size_t` called `numFuses`, which notes the number of fuses that are currently in the container
3. A pointer to the method table called `methods`

### Functions
----
**newFuseContainer**

` fuses_t* newFuseContainer(void); `
- This is the sole *true* function within this library. It is the constructor to create new fuse containers. It takes no 
parameters and returns a pointer to a new container.

#### Methods
----
All of these methods require a pointer to the `fuses_t` object as the first parameter, so won't be described in the 
method descriptions for the sake of brevity.

**add**

`int (*add)(fuses_t* fuses, void* fuseData, size_t fuseTimeSec, onFuseEnd fuseEndFunc);`
- Adds a new fuse to the fuse container. It returns an integer which represents its success.

**remove**

`int (*remove)(fuses_t* fuses, size_t idx);`
- Removes the fuse at the requested index. It returns an integer which represents its success.

**poll**

`int (*poll)(fuses_t* fuses, size_t idx);`
- Polls the status of the fuse at the requested index. If the time for the fuse is up, it'll be removed. It returns an 
integer which represents its success.

**pollAll**

`void (*pollAll)(fuses_t* fuses);`
- Polls all of the fuses in the fuse container. Fuses with finished timers will be removed.

**reset**

`int (*reset)(fuses_t* fuses, size_t idx);`
- Resets the timer of the fuse at the requested index. It returns an integer which represents its success.

**resetAll**

`void (*resetAll)(fuses_t* fuses);`
- Resets all of the timers of every fuse within the fuse container.

**clear**

`void (*clear)(fuses_t* fuses);`
- Clears all of the fuses from the fuse container.

**close**

`void (*close)(fuses_t** fuses);`
- Closes the current fuse container instance and deallocates its memory.
