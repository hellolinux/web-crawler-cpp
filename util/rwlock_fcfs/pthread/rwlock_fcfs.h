#ifndef __PTHREAD_RWLOCK_FCFS_H
#define __PTHREAD_RWLOCK_FCFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "./rwlock_fcfs_queue.h"

/* This is the struct that is pointed to by the elements of the queue of
 * the FCFS RWLock
 * */
struct pthread_rwlock_fcfs_item_struct
{
    /* A condition variable to wake up the threads that waits upon this item */
    pthread_cond_t cond;
    /* A flag that specifies if it's a reader or a writer */
    int is_writer;
    /* A flag that disables this item */
    int is_disabled;
    /* An integer specifying how many threads are waiting on it */
    int num_threads;
    /* A flag that indicates if the first thread in the pack has already
     * been accepted. (Used only for a writers' pack */
    int was_first_thr_accepted;
};

typedef struct pthread_rwlock_fcfs_item_struct pthread_rwlock_fcfs_item_t;

enum pthread_rwlock_fcfs_status
{
    PTHREAD_RWLOCK_FCFS_UNLOCKED,
    PTHREAD_RWLOCK_FCFS_USED_BY_READERS,
    PTHREAD_RWLOCK_FCFS_USED_BY_A_WRITER,
};

/* The RWLock Struct */
struct pthread_rwlock_fcfs_struct
{
    /* The queue that will be managed */
    pthread_rwlock_fcfs_queue_t * queue;
    /* The number of readers that are using the RWLock at the moment */
    int num_readers;
    /* The number of pending readers that didn't gain access
     * to the lock yet. */
    int num_pending_readers;
    /* Specifies if there is a writer locking the RWLock, some
     * readers or if it's un-occupied. */
    enum pthread_rwlock_fcfs_status status;
    /* A mutex to make sure no two threads _modify_ the RWLock struct
     * at the moment */
    pthread_mutex_t mutex;

    /* A flag that indicates that no new threads should be accepted */
    int is_destroyed;
};

typedef struct pthread_rwlock_fcfs_struct pthread_rwlock_fcfs_t;

/************** Functions ***********/

#ifdef PTHREAD_RWLOCK_FCFS_DEBUG
#define PTHREAD_RWLOCK_FCFS_DEBUG_ARGS , char * id
#define PTHREAD_RWLOCK_FCFS_DEBUG_CALL_ARGS , id
#else
#define PTHREAD_RWLOCK_FCFS_DEBUG_ARGS
#define PTHREAD_RWLOCK_FCFS_DEBUG_CALL_ARGS
#endif

/*
 * Allocate a new FCFS RWLock
 * */
extern pthread_rwlock_fcfs_t * pthread_rwlock_fcfs_alloc(void);

/*
 * Wait indefinitely until a read access to the lock is granted.
 * */
extern int pthread_rwlock_fcfs_gain_read(pthread_rwlock_fcfs_t * rwlock PTHREAD_RWLOCK_FCFS_DEBUG_ARGS);

/*
 * Wait indefinitely until a write access to the lock is granted.
 * */
extern int pthread_rwlock_fcfs_gain_write(pthread_rwlock_fcfs_t * rwlock PTHREAD_RWLOCK_FCFS_DEBUG_ARGS);

/*
 * Release a previously gained read or write access
 * */
extern void pthread_rwlock_fcfs_release(pthread_rwlock_fcfs_t * rwlock PTHREAD_RWLOCK_FCFS_DEBUG_ARGS);

/*
 * Destroy the RWLock
 * */
extern void pthread_rwlock_fcfs_destroy(pthread_rwlock_fcfs_t * rwlock);

/*
 * Wait until a certain time (abstime) to get a read access. If it is
 * not given, continue_callback is called and asks whether to continue
 * or not.
 *
 * If it wishes to continue, it is responsible for setting the new abstime.
 *
 * context is the context variable for continue_callback
 *
 * */
extern int pthread_rwlock_fcfs_timed_gain_read(
        pthread_rwlock_fcfs_t * rwlock,
        const struct timespec * abstime,
        int (*continue_callback)(void * context),
        void * context
        PTHREAD_RWLOCK_FCFS_DEBUG_ARGS
        );

/*
 * Wait until a certain time (abstime) to get a write access. If it is not
 * given, continue_callback is called and asks whether to continue or not.
 *
 * If it wishes to continue, it is responsible for setting the new abstime.
 *
 * context is the context variable for continue_callback
 *
 * */
extern int pthread_rwlock_fcfs_timed_gain_write(
        pthread_rwlock_fcfs_t * rwlock,
        const struct timespec * abstime,
        int (*continue_callback)(void * context),
        void * context
        PTHREAD_RWLOCK_FCFS_DEBUG_ARGS
        );

/*
 * Attempt to gain a read access. If it is not given immidiately it returns an
 * error code other than 0.
 *
 * Else, it returns 0
 * */
extern int pthread_rwlock_fcfs_try_gain_read(pthread_rwlock_fcfs_t * rwlock PTHREAD_RWLOCK_FCFS_DEBUG_ARGS);


/*
 * Attempt to gain a write access. If it is not given immidiately it returns an
 * error code other than 0.
 *
 * Else, it returns 0
 * */
extern int pthread_rwlock_fcfs_try_gain_write(pthread_rwlock_fcfs_t * rwlock PTHREAD_RWLOCK_FCFS_DEBUG_ARGS);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __PTHREAD_RWLOCK_FCFS_H */
