/*
 * Copyright (C) 2009 Andrzej K. Haczewski <ahaczewski@gmail.com>
 *
 * DISCLAIMER: The implementation is Git-specific, it is subset of original
 * Pthreads API, without lots of other features that Git doesn't use.
 * Git also makes sure that the passed arguments are valid, so there's
 * no need for double-checking.
 */

#include "../../git-compat-util.h"
#include "pthread.h"

#include <errno.h>
#include <limits.h>

int pthread_create(pthread_t *thread, const void *unused,
		   void *(*start_routine)(void *), void *arg)
{
	thread->handle = (HANDLE)_beginthreadex(NULL, 0, start_routine, arg, 0,
						&thread->tid);

	if (!thread->handle)
		return errno;
	else
		return 0;
}

int win32_pthread_join(pthread_t *thread, void **value_ptr)
{
	DWORD result = WaitForSingleObject(thread->handle, INFINITE);
	switch (result) {
	case WAIT_OBJECT_0:
		if (value_ptr)
			*value_ptr = thread->arg;
		if (!CloseHandle(thread->handle))
			return err_win_to_posix(GetLastError());
		return 0;
	case WAIT_ABANDONED:
		CloseHandle(thread->handle);
		return EINVAL;
	default:
		CloseHandle(thread->handle);
		/* fall through */
	case WAIT_FAILED:
		/* the function failed so we do not close any handle*/
		return err_win_to_posix(GetLastError());
	}
}

pthread_t pthread_self(void)
{
	pthread_t t = { NULL };
	t.tid = GetCurrentThreadId();
	return t;
}
