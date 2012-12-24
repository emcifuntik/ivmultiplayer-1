/*
******************************************************************************
*                                                                            *
* Copyright (C) 2001-2005, International Business Machines                   *
*                Corporation and others. All Rights Reserved.                *
*                                                                            *
******************************************************************************
*   file name:  uclean.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001July05
*   created by: George Rhoten
*/

#ifndef __UCLEAN_H__
#define __UCLEAN_H__

#include "unicode/utypes.h"
/**
 * \file
 * \brief C API: Initialize and clean up ICU
 */
 
/**
 *  Initialize ICU. The description further below applies to ICU 2.6 to ICU 3.4.
 *  Starting with ICU 3.4, u_init() needs not be called any more for
 *  ensuring thread safety, but it can give an indication for whether ICU
 *  can load its data. In ICU 3.4, it will try to load the converter alias table
 *  (cnvalias.icu) and give an error code if that fails.
 *  This may change in the future.
 *  <p>
 *  For ensuring the availability of necessary data, an application should
 *  open the service objects (converters, collators, etc.) that it will use
 *  and check for error codes there.
 *  <p>
 *  Documentation for ICU 2.6 to ICU 3.4:
 *  <p>
 *  This function loads and initializes data items
 *  that are required internally by various ICU functions.  Use of this explicit
 *  initialization is required in multi-threaded applications; in 
 *  single threaded apps, use is optional, but incurs little additional
 *  cost, and is thus recommended.
 *  <p>
 *  In multi-threaded applications, u_init() should be called  in the
 *  main thread before starting additional threads, or, alternatively
 *  it can be called in each individual thread once, before other ICU
 *  functions are called in that thread.  In this second scenario, the
 *  application must guarantee that the first call to u_init() happen
 *  without contention, in a single thread only.
 *  <p>
 *  If <code>u_setMemoryFunctions()</code> or 
 *  <code>u_setMutexFunctions</code> are needed (uncommon), they must be
 *  called _before_ <code>u_init()</code>.
 *  <p>
 *  Extra, repeated, or otherwise unneeded calls to u_init() do no harm,
 *  other than taking a small amount of time.
 *
 * @param status An ICU UErrorCode parameter. It must not be <code>NULL</code>.
 *    An Error will be returned if some required part of ICU data can not
 *    be loaded or initialized.
 *    The function returns immediately if the input error code indicates a
 *    failure, as usual.
 *
 * @stable ICU 2.6
 */  
U_STABLE void U_EXPORT2 
u_init(UErrorCode *status);

/**
 * Clean up the system resources, such as allocated memory or open files,
 * used in all ICU libraries. This will free/delete all memory owned by the
 * ICU libraries, and return them to their original load state. All open ICU
 * items (collators, resource bundles, converters, etc.) must be closed before
 * calling this function, otherwise ICU may not free its allocated memory
 * (e.g. close your converters and resource bundles before calling this
 * function). Generally, this function should be called once just before
 * an application exits. For applications that dynamically load and unload
 * the ICU libraries (relatively uncommon), u_cleanup() should be called
 * just before the library unload.
 * <p>
 * u_cleanup() also clears any ICU heap functions, mutex functions or
 * trace functions that may have been set for the process.  
 * This has the effect of restoring ICU to its initial condition, before
 * any of these override functions were installed.  Refer to
 * u_setMemoryFunctions(), u_setMutexFunctions and 
 * utrace_setFunctions().  If ICU is to be reinitialized after after
 * calling u_cleanup(), these runtime override functions will need to
 * be set up again if they are still required.
 * <p>
 * u_cleanup() is not thread safe.  All other threads should stop using ICU
 * before calling this function.
 * <p>
 * Any open ICU items will be left in an undefined state by u_cleanup(),
 * and any subsequent attempt to use such an item will give unpredictable
 * results.
 * <p>
 * After calling u_cleanup(), an application may continue to use ICU by
 * calling u_init().  An application must invoke u_init() first from one single
 * thread before allowing other threads call u_init().  All threads existing
 * at the time of the first thread's call to u_init() must also call
 * u_init() themselves before continuing with other ICU operations.  
 * <p>
 * The use of u_cleanup() just before an application terminates is optional,
 * but it should be called only once for performance reasons. The primary
 * benefit is to eliminate reports of memory or resource leaks originating
 * in ICU code from the results generated by heap analysis tools.
 * <p>
 * <strong>Use this function with great care!</strong>
 * </p>
 *
 * @stable ICU 2.0
 * @system
 */
U_STABLE void U_EXPORT2 
u_cleanup(void);




/**
  * An opaque pointer type that represents an ICU mutex.
  * For user-implemented mutexes, the value will typically point to a
  *  struct or object that implements the mutex.
  * @stable ICU 2.8
  * @system
  */
typedef void *UMTX;

/**
  *  Function Pointer type for a user supplied mutex initialization function.
  *  The user-supplied function will be called by ICU whenever ICU needs to create a
  *  new mutex.  The function implementation should create a mutex, and store a pointer
  *  to something that uniquely identifies the mutex into the UMTX that is supplied
  *  as a paramter.
  *  @param context user supplied value, obtained from from u_setMutexFunctions().
  *  @param mutex   Receives a pointer that identifies the new mutex.
  *                 The mutex init function must set the UMTX to a non-null value.   
  *                 Subsequent calls by ICU to lock, unlock, or destroy a mutex will 
  *                 identify the mutex by the UMTX value.
  *  @param status  Error status.  Report errors back to ICU by setting this variable
  *                 with an error code.
  *  @stable ICU 2.8
  *  @system
  */
typedef void U_CALLCONV UMtxInitFn (const void *context, UMTX  *mutex, UErrorCode* status);


/**
  *  Function Pointer type for a user supplied mutex functions.
  *  One of the  user-supplied functions with this signature will be called by ICU
  *  whenever ICU needs to lock, unlock, or destroy a mutex.
  *  @param context user supplied value, obtained from from u_setMutexFunctions().
  *  @param mutex   specify the mutex on which to operate.
  *  @stable ICU 2.8
  *  @system
  */
typedef void U_CALLCONV UMtxFn   (const void *context, UMTX  *mutex);


/**
  *  Set the functions that ICU will use for mutex operations
  *  Use of this function is optional; by default (without this function), ICU will
  *  directly access system functions for mutex operations
  *  This function can only be used when ICU is in an initial, unused state, before
  *  u_init() has been called.
  *  This function may be used even when ICU has been built without multi-threaded
  *  support  (see ICU_USE_THREADS pre-processor variable, umutex.h)
  *  @param context This pointer value will be saved, and then (later) passed as
  *                 a parameter to the user-supplied mutex functions each time they
  *                 are called. 
  *  @param init    Pointer to a mutex initialization function.  Must be non-null.
  *  @param destroy Pointer to the mutex destroy function.  Must be non-null.
  *  @param lock    pointer to the mutex lock function.  Must be non-null.
  *  @param unlock  Pointer to the mutex unlock function.  Must be non-null.
  *  @param status  Receives error values.
  *  @stable ICU 2.8
  *  @system
  */  
U_STABLE void U_EXPORT2 
u_setMutexFunctions(const void *context, UMtxInitFn *init, UMtxFn *destroy, UMtxFn *lock, UMtxFn *unlock,
                    UErrorCode *status);


/**
  *  Pointer type for a user supplied atomic increment or decrement function.
  *  @param context user supplied value, obtained from from u_setAtomicIncDecFunctions().
  *  @param p   Pointer to a 32 bit int to be incremented or decremented
  *  @return    The value of the variable after the inc or dec operation.
  *  @stable ICU 2.8
  *  @system
  */
typedef int32_t U_CALLCONV UMtxAtomicFn(const void *context, int32_t *p);

/**
 *  Set the functions that ICU will use for atomic increment and decrement of int32_t values.
 *  Use of this function is optional; by default (without this function), ICU will
 *  use its own internal implementation of atomic increment/decrement.
 *  This function can only be used when ICU is in an initial, unused state, before
 *  u_init() has been called.
 *  @param context This pointer value will be saved, and then (later) passed as
 *                 a parameter to the increment and decrement functions each time they
 *                 are called.  This function can only be called 
 *  @param inc     Pointer to a function to do an atomic increment operation.  Must be non-null.
 *  @param dec     Pointer to a function to do an atomic decrement operation.  Must be non-null.
 *  @param status  Receives error values.
 *  @stable ICU 2.8
 *  @system
 */  
U_STABLE void U_EXPORT2 
u_setAtomicIncDecFunctions(const void *context, UMtxAtomicFn *inc, UMtxAtomicFn *dec,
                    UErrorCode *status);



/**
  *  Pointer type for a user supplied memory allocation function.
  *  @param context user supplied value, obtained from from u_setMemoryFunctions().
  *  @param size    The number of bytes to be allocated
  *  @return        Pointer to the newly allocated memory, or NULL if the allocation failed.
  *  @stable ICU 2.8
  *  @system
  */
typedef void *U_CALLCONV UMemAllocFn(const void *context, size_t size);
/**
  *  Pointer type for a user supplied memory re-allocation function.
  *  @param context user supplied value, obtained from from u_setMemoryFunctions().
  *  @param size    The number of bytes to be allocated
  *  @return        Pointer to the newly allocated memory, or NULL if the allocation failed.
  *  @stable ICU 2.8
  *  @system
  */
typedef void *U_CALLCONV UMemReallocFn(const void *context, void *mem, size_t size);
/**
  *  Pointer type for a user supplied memory free  function.  Behavior should be
  *  similar the standard C library free().
  *  @param context user supplied value, obtained from from u_setMemoryFunctions().
  *  @param mem     Pointer to the memory block to be resized
  *  @param size    The new size for the block
  *  @return        Pointer to the resized memory block, or NULL if the resizing failed.
  *  @stable ICU 2.8
  *  @system
  */
typedef void  U_CALLCONV UMemFreeFn (const void *context, void *mem);

/**
 *  Set the functions that ICU will use for memory allocation.
 *  Use of this function is optional; by default (without this function), ICU will
 *  use the standard C library malloc() and free() functions.
 *  This function can only be used when ICU is in an initial, unused state, before
 *  u_init() has been called.
 *  @param context This pointer value will be saved, and then (later) passed as
 *                 a parameter to the memory functions each time they
 *                 are called.
 *  @param a       Pointer to a user-supplied malloc function.
 *  @param r       Pointer to a user-supplied realloc function.
 *  @param f       Pointer to a user-supplied free function.
 *  @param status  Receives error values.
 *  @stable ICU 2.8
 *  @system
 */  
U_STABLE void U_EXPORT2 
u_setMemoryFunctions(const void *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f, 
                    UErrorCode *status);

#endif
