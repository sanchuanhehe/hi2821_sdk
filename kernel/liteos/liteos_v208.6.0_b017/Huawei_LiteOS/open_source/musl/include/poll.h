#ifndef	_POLL_H
#define	_POLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#include <bits/poll.h>

#define POLLIN     0x001
#define POLLPRI    0x002
#define POLLOUT    0x004
#define POLLERR    0x008
#define POLLHUP    0x010
#define POLLNVAL   0x020
#define POLLRDNORM 0x040
#define POLLRDBAND 0x080
#ifndef POLLWRNORM
#define POLLWRNORM 0x100
#define POLLWRBAND 0x200
#endif
#ifndef POLLMSG
#define POLLMSG    0x400
#define POLLRDHUP  0x2000
#endif

typedef unsigned long nfds_t;

#ifndef __LITEOS__
struct pollfd {
	int fd;
	short events;
	short revents;
};
#else
typedef unsigned int pollevent_t;

#include "semaphore.h"
#include "linux/wait.h"

struct pollfd {
	int fd;               /* The descriptor being polled */
	sem_t *sem;           /* Pointer to semaphore used to post output event */
	pollevent_t events;   /* The input event flags */
	pollevent_t revents;  /* The output event flags */
	void *priv;           /* For use by drivers */
};

struct tag_poll_wait_entry;

typedef struct tag_poll_wait_entry* poll_wait_head;

typedef struct tag_poll_table {
	poll_wait_head wait;
	pollevent_t key;
} poll_table;

extern void notify_poll(wait_queue_head_t *);
extern void notify_poll_with_key(wait_queue_head_t *, pollevent_t);
#endif

int poll (struct pollfd *, nfds_t, int);

#ifdef _GNU_SOURCE
#define __NEED_time_t
#define __NEED_struct_timespec
#define __NEED_sigset_t
#include <bits/alltypes.h>
int ppoll(struct pollfd *, nfds_t, const struct timespec *, const sigset_t *);
#endif

#if _REDIR_TIME64
#ifdef _GNU_SOURCE
__REDIR(ppoll, __ppoll_time64);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
