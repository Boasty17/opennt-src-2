#ifdef MTHREAD

/***
*mlock.c - Multi-thread locking routines
*
*	Copyright (c) 1987-1993, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*Revision History:
*	05-07-90  JCR	Module created.
*	06-04-90  GJF	Changed error message interface.
*	08-08-90  GJF	Removed 32 from API names.
*	08-08-90  SBM	_lockmap no longer 8 times required size
*	10-08-90  GJF	New-style function declarators. Removed questionable
*			return statements from void functions (weren't needed
*			and the compiler was bitching).
*	10-09-90  GJF	Thread ids are unsigned longs.
*	06-06-91  GJF	Adapted for Win32 [_WIN32_].
*	09-29-91  GJF	Fixed infinite recursion problem with DEBUG version
*			of _lock [_WIN32_].
*	03-06-92  GJF	Removed _[un]lock_fh() and _[un]lock_stream for Win32
*			targets.
*	05-28-92  GJF	Added _mtdeletelocks() for Win32 for DLLs with contain
*			the C runtime (e.g., crtdll.dll).
*	10-06-92  SRW	Make _locktable an array of PCRITICAL_SECTION pointers
*			instead of structures.	Allocate each critical section
*			as it is needed.
*	02-25-93  GJF	Substantially revised. Restored static critical section
*			structures for some locks. Replaced bit-array scheme
*			of keeping track of locks. Removed Cruiser support and
*			replaced obsolete DEBUG code.
*	03-03-93  GJF	Made CRITICAL_SECTION structure for _HEAP_LOCK static.
*	03-08-93  SKS	Fix ptr use error in DEBUG version of _mtdeletelocks
*	03-08-93  SKS	Fix deletion of the special critical sections,
*			especially the heap lock.
*	05-05-93  GJF	Turned DEBUG code off.
*       06-03-93  SRW   Disable FPO optimizations for this file so it can call
*                       CriticalSection routines on a checked build even though
*                       the C Runtimes are compiled free.
*
*******************************************************************************/

#include <cruntime.h>
#include <oscalls.h>
#include <internal.h>
#include <os2dll.h>
#include <rterr.h>
#include <stddef.h>
#include <malloc.h>
#include <limits.h>


/*
 * Local routines
 */
void _CRTAPI3 _lockerr_exit(char *);


/*
 * Global Data
 */

/*
 * Statically allocated critical section structures for _LOCKTAB_LOCK,
 * _EXIT_LOCK1 and _HEAP_LOCK.
 */
static CRITICAL_SECTION ltlcritsect;
static CRITICAL_SECTION xlcritsect;
static CRITICAL_SECTION hlcritsect;

/*
 * Lock Table
 * This table contains a pointer to the critical section management structure
 * for each lock.
 */
PCRITICAL_SECTION _locktable[_TOTAL_LOCKS] = {
	NULL,		/* 0  == no lock defined   *** OBSOLETE ***	*/
	NULL,		/* 1  == _SIGNAL_LOCK	   */
	NULL,		/* 2  == _IOB_SCAN_LOCK	   */
	NULL,		/* 3  == _TMPNAM_LOCK	   */
	NULL,		/* 4  == _INPUT_LOCK	   */
	NULL,		/* 5  == _OUTPUT_LOCK	   */
	NULL,		/* 6  == _CSCANF_LOCK	   */
	NULL,		/* 7  == _CPRINTF_LOCK	   */
	NULL,		/* 8  == _CONIO_LOCK	   */
	&hlcritsect,	/* 9  == _HEAP_LOCK	   */
	NULL,		/* 10 == _BHEAP_LOCK	   *** OBSOLETE ***	*/
	NULL,		/* 11 == _TIME_LOCK	   */
	NULL,		/* 12 == _ENV_LOCK	   */
	&xlcritsect,	/* 13 == _EXIT_LOCK1	   */
	NULL,		/* 14 == _EXIT_LOCK2	   *** OBSOLETE	***	*/
	NULL,		/* 15 == _THREADDATA_LOCK  *** OBSOLETE ***	*/
	NULL,		/* 16 == _POPEN_LOCK	   */
	&ltlcritsect,	/* 17 == _LOCKTAB_LOCK	   */
	NULL,		/* 18 == _OSFHND_LOCK	   */
	NULL,		/* 19 == _SETLOCALE_LOCK   */
	NULL,		/* 20 == _LC_COLLATE_LOCK  */
	NULL,		/* 21 == _LC_CTYPE_LOCK	   */
	NULL,		/* 22 == _LC_MONETARY_LOCK */
	NULL,		/* 23 == _LC_NUMERIC_LOCK  */
	NULL,		/* 24 == _LC_TIME_LOCK	   */
	NULL,		/* 25 == _STREAM_LOCKS	   */
	NULL		/* ... */
	};


#define _FATAL  _amsg_exit(_RT_LOCK)

#pragma optimize("y",off)

/***
*_mtinitlocks() - Initialize multi-thread lock scheme
*
*Purpose:
*	Perform whatever initialization is required for the multi-thread
*	locking (synchronization) scheme. This routine should be called
*	exactly once, during startup, and this must be before any requests
*	are made to assert locks.
*
*	NOTES: In Win32, the multi-thread locks are created individually,
*	each upon its first use. That is when any particular lock is asserted
*	for the first time, the underlying critical section is then allocated,
*	initialized and (finally) entered. This allocation and initialization
*	is protected under _LOCKTAB_LOCK. It is _mtinitlocks' job to set up
*	_LOCKTAB_LOCK. _EXIT_LOCK1 is also set up by _mtinitlock
*
*Entry:
*       <none>
*
*Exit:
*       returns on success
*       calls _amsg_exit on failure
*
*Exceptions:
*
*******************************************************************************/

void _CRTAPI1 _mtinitlocks (
        void
        )
{
#ifdef	DEBUG
	int i;

	/*
	 * Scan _locktable[] and make sure everything was initialized properly
	 */
	for ( i = 0 ; i < _TOTAL_LOCKS ; i++ ) {
		if ( i == _LOCKTAB_LOCK ) {
			if ( _locktable[i] != &ltlcritsect )
				_lockerr_exit("fatal error in _mtinitlocks #1\n");
		}
		else if ( i == _EXIT_LOCK1 ) {
			if ( _locktable[i] != &xlcritsect )
				_lockerr_exit("fatal error in _mtinitlocks #2\n");
		}
		else if ( i == _HEAP_LOCK ) {
			if ( _locktable[i] != &hlcritsect )
				_lockerr_exit("fatal error in _mtinitlocks #3\n");
		}
		else if ( _locktable[i] != NULL )
			_lockerr_exit("fatal error in _mtinitlocks #3\n");
	}
#endif	/* DEBUG */

        /*
	 * All we need to do is initialize _LOCKTAB_LOCK and _EXIT_LOCK1.
	 */
	InitializeCriticalSection( _locktable[_LOCKTAB_LOCK] );
	InitializeCriticalSection( _locktable[_EXIT_LOCK1] );
	InitializeCriticalSection( _locktable[_HEAP_LOCK] );
}


/***
*_mtdeletelocks() - Delete all initialized locks
*
*Purpose:
*       Walks _locktable[] and _lockmap, and deletes every 'lock' (i.e.,
*       critical section) which has been initialized.
*
*       This function is intended for use in DLLs containing the C runtime
*       (i.e., crtdll.dll and user DLLs built using libcmt.lib and the
*       special startup objects). It is to be called from within the DLL's
*       entrypoint function when that function is called with
*       DLL_PROCESS_DETACH.
*
*Entry:
*       <none>
*
*Exit:
*
*Exceptions:
*       behavior undefined/unknown if a lock is being held when this routine
*       is called.
*
*******************************************************************************/

void _CRTAPI1 _mtdeletelocks(
        void
        )
{
        int locknum;

        for ( locknum = 0 ; locknum < _TOTAL_LOCKS ; locknum++ ) {

                /*
                 * If the 'lock' has been created, delete it
		 */
#ifdef	DEBUG
		/*
		 * See if the lock has already been deleted.
		 */
		if ( _locktable[locknum] == (PCRITICAL_SECTION)(-1L) )
			_lockerr_exit("fatal error in _mtdeletelocks #1\n");

		/*
		 * Delete the lock if it had been created.
		 */
		if ( _locktable[locknum] != NULL ) {
		    if ( (locknum != _LOCKTAB_LOCK) && (locknum !=
			_EXIT_LOCK1) && (locknum != _HEAP_LOCK) )
		    {
			PCRITICAL_SECTION pcs = _locktable[locknum];

			/* mark as deleted */
			_locktable[locknum] = (PCRITICAL_SECTION)(-1L);

			/* double check that it wasn't already deleted */
			if ( pcs == (PCRITICAL_SECTION)(-1L) )
				_lockerr_exit("fatal error in _mtdeletelocks #2\n");

			DeleteCriticalSection(pcs);
			free(pcs);
		    }
		}
#else	/* non DEBUG */
		/*
		 * Delete the lock if it had been created
		 */
		if ( _locktable[locknum] != NULL ) {
		    if ( (locknum != _LOCKTAB_LOCK) && (locknum !=
			    _EXIT_LOCK1) && (locknum != _HEAP_LOCK) )
		    {
			/*
			 * Free the memory for the CritSect after deleting
			 * it.	It is okay to call free() if the heap lock
			 * is kept valid until after all calls to the heap.
			 */
			DeleteCriticalSection(_locktable[locknum]);
			free(_locktable[locknum]);
		    }
		}
#endif	/* DEBUG */

        }

        /*
	 * Finally, clean up the special locks
	 */
	DeleteCriticalSection( _locktable[_HEAP_LOCK] );
	DeleteCriticalSection( _locktable[_EXIT_LOCK1] );
	DeleteCriticalSection( _locktable[_LOCKTAB_LOCK] );
}


/***
* _lock - Acquire a multi-thread lock
*
*Purpose:
*       Note that it is legal for a thread to aquire _EXIT_LOCK1
*       multiple times.
*
*Entry:
*       locknum = number of the lock to aquire
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

void _CRTAPI1 _lock (
        int locknum
        )
{
	PCRITICAL_SECTION pcs;

#ifdef	DEBUG
	if ( _locktable[locknum] == (PCRITICAL_SECTION)(-1L) )
		_lockerr_exit("fatal error in _lock #1\n");
#endif	/* DEBUG */

	/*
         * Create/open the lock, if necessary
	 */
	if ( _locktable[locknum] == NULL ) {

		if ( (pcs = malloc(sizeof(CRITICAL_SECTION))) == NULL )
			_amsg_exit(_RT_LOCK);

		_mlock(_LOCKTAB_LOCK);	/*** WARNING: Recursive lock call ***/

		if ( _locktable[locknum] == NULL ) {
			InitializeCriticalSection(pcs);
			_locktable[locknum] = pcs;
		}
		else {
#ifdef	DEBUG
			if ( _locktable[locknum] == (PCRITICAL_SECTION)(-1L) )
				_lockerr_exit("fatal error in _lock #2\n");
#endif	/* DEBUG */
			free(pcs);
		}

		_munlock(_LOCKTAB_LOCK);
        }

        /*
         * Enter the critical section.
         */

	EnterCriticalSection( _locktable[locknum] );
}


/***
* _unlock - Release multi-thread lock
*
*Purpose:
*       Note that it is legal for a thread to aquire _EXIT_LOCK1
*       multiple times.
*
*Entry:
*       locknum = number of the lock to release
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

void _CRTAPI1 _unlock (
        int locknum
        )
{
        /*
         * leave the critical section.
         */
	LeaveCriticalSection( _locktable[locknum] );
}

#pragma optimize("y",)

/***
*_lockerr_exit() - Write error message and die
*
*Purpose:
*	Attempt to write out the unexpected lock error message, then terminate
*	the program by a direct API call.  This function is used in place of
*	amsg_exit(_RT_LOCK) when it is judged unsafe to allow further lock
*	or unlock calls.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

void _CRTAPI3 _lockerr_exit (
	char *msg
        )
{
	FatalAppExit(0, msg);		/* Die with message box */
	ExitProcess(255);		/* Just die */
}


#endif  /* MTHREAD */
