// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  winsync.c - Win32 OSD core synchronization functions
//
//============================================================

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

// MAME headers
#include "osdcore.h"
#include "osinline.h"


//============================================================
//  DEBUGGING
//============================================================

#define DEBUG_SLOW_LOCKS    0
#define USE_SCALABLE_LOCKS      (0)



//============================================================
//  TYPE DEFINITIONS
//============================================================

typedef BOOL (WINAPI *try_enter_critical_section_ptr)(LPCRITICAL_SECTION lpCriticalSection);

struct osd_lock
{
	CRITICAL_SECTION    critsect;
};

struct osd_scalable_lock
{
#if USE_SCALABLE_LOCKS
	struct
	{
		volatile INT32  haslock;        // do we have the lock?
		INT32           filler[64/4-1]; // assumes a 64-byte cache line
	} slot[WORK_MAX_THREADS];           // one slot per thread
	volatile INT32      nextindex;      // index of next slot to use
#else
	CRITICAL_SECTION    section;
#endif
};


//============================================================
//  GLOBAL VARIABLES
//============================================================

static try_enter_critical_section_ptr try_enter_critical_section = NULL;
static int checked_for_try_enter = FALSE;



//============================================================
//  osd_lock_alloc
//============================================================

osd_lock *osd_lock_alloc(void)
{
	osd_lock *lock = (osd_lock *)malloc(sizeof(*lock));
	if (lock == NULL)
		return NULL;
	InitializeCriticalSection(&lock->critsect);
	return lock;
}


//============================================================
//  osd_lock_acquire
//============================================================

void osd_lock_acquire(osd_lock *lock)
{
#if DEBUG_SLOW_LOCKS
	osd_ticks_t ticks = osd_ticks();
#endif

	// block until we can acquire the lock
	EnterCriticalSection(&lock->critsect);

#if DEBUG_SLOW_LOCKS
	// log any locks that take more than 1ms
	ticks = osd_ticks() - ticks;
	if (ticks > osd_ticks_per_second() / 1000) osd_printf_debug("Blocked %d ticks on lock acquire\n", (int)ticks);
#endif
}


//============================================================
//  osd_lock_try
//============================================================

int osd_lock_try(osd_lock *lock)
{
	int result = TRUE;

	// if we haven't yet checked for the TryEnter API, do it now
	if (!checked_for_try_enter)
	{
		// see if we can use TryEnterCriticalSection
		HMODULE library = LoadLibrary(TEXT("kernel32.dll"));
		if (library != NULL)
			try_enter_critical_section = (try_enter_critical_section_ptr)GetProcAddress(library, "TryEnterCriticalSection");
		checked_for_try_enter = TRUE;
	}

	// if we have it, use it, otherwise just block
	if (try_enter_critical_section != NULL)
		result = (*try_enter_critical_section)(&lock->critsect);
	else
		EnterCriticalSection(&lock->critsect);
	return result;
}


//============================================================
//  osd_lock_release
//============================================================

void osd_lock_release(osd_lock *lock)
{
	LeaveCriticalSection(&lock->critsect);
}


//============================================================
//  osd_lock_free
//============================================================

void osd_lock_free(osd_lock *lock)
{
	DeleteCriticalSection(&lock->critsect);
	free(lock);
}


//============================================================
//  win_compare_exchange32
//============================================================

INT32 win_compare_exchange32(INT32 volatile *ptr, INT32 compare, INT32 exchange)
{
	return InterlockedCompareExchange((LPLONG)ptr, (LONG)exchange, (LONG)compare);
}


//============================================================
//  win_compare_exchange64
//============================================================

#ifdef PTR64
INT64 win_compare_exchange64(INT64 volatile *ptr, INT64 compare, INT64 exchange)
{
	return InterlockedCompareExchange64((LONGLONG*)ptr, (LONGLONG)exchange, (LONGLONG)compare);
}
#endif


//============================================================
//  win_atomic_exchange32
//============================================================

INT32 win_atomic_exchange32(INT32 volatile *ptr, INT32 exchange)
{
	return InterlockedExchange((LONG *) ptr, exchange);
}


//============================================================
//  win_atomic_add32
//============================================================

INT32 win_atomic_add32(INT32 volatile *ptr, INT32 delta)
{
	return InterlockedExchangeAdd((LONG *) ptr, delta) + delta;
}

//============================================================
//  Scalable Locks
//============================================================

osd_scalable_lock *osd_scalable_lock_alloc(void)
{
	osd_scalable_lock *lock;

	lock = (osd_scalable_lock *)calloc(1, sizeof(*lock));

	memset(lock, 0, sizeof(*lock));
#if USE_SCALABLE_LOCKS
	lock->slot[0].haslock = TRUE;
#else
	InitializeCriticalSection(&lock->section);
#endif
	return lock;
}


INT32 osd_scalable_lock_acquire(osd_scalable_lock *lock)
{
#if USE_SCALABLE_LOCKS
	INT32 myslot = (atomic_increment32(&lock->nextindex) - 1) & (WORK_MAX_THREADS - 1);
	INT32 backoff = 1;

	while (!lock->slot[myslot].haslock)
	{
		INT32 backcount;
		for (backcount = 0; backcount < backoff; backcount++)
			osd_yield_processor();
		backoff <<= 1;
	}
	lock->slot[myslot].haslock = FALSE;
	return myslot;
#else
	EnterCriticalSection(&lock->section);
	return 0;
#endif
}


void osd_scalable_lock_release(osd_scalable_lock *lock, INT32 myslot)
{
#if USE_SCALABLE_LOCKS
	atomic_exchange32(&lock->slot[(myslot + 1) & (WORK_MAX_THREADS - 1)].haslock, TRUE);
#else
	LeaveCriticalSection(&lock->section);
#endif
}


void osd_scalable_lock_free(osd_scalable_lock *lock)
{
#if USE_SCALABLE_LOCKS
#else
	DeleteCriticalSection(&lock->section);
#endif
	free(lock);
}