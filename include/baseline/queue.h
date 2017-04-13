/*
** SCCS ID:	@(#)queue.h	1.1	4/13/17
**
** File:	queue.h
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	declarations for the queue module
*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

// pseudo-function for accessing queue size

#define	_q_size(que)	((que)->size)

/*
** Types
**
** Our queues are generic, self-ordering queues.  Each queue has an
** associated ordering routine which is used by the insertion routine to
** order the queue elements.  This allows us to have different sorting
** criteria for different queues, but manage them with one set of functions.
*/

// prototype for the ordering function

typedef int32_t (*cmpf_t)( const void *, const void * );

// queue nodes (used internally to keep track of queue entries)

typedef struct st_qnode {
	struct st_qnode *next;
	void *data;
} qnode_t;

// the queue structure

typedef struct st_queue {
	qnode_t *head;		// first entry in queue
	qnode_t *tail;		// last entry in queue
	uint32_t size;		// occupancy of queue
	cmpf_t cmp;		// queue ordering comparison function
} queue_t;

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _q_init()
**
** initialize the queue module
*/
void _q_init( void );

/*
** _q_reset(que,cmp)
**
** reset the queue structure to its "empty" state,
** using 'cmp' as its comparison function
*/
void _q_reset( queue_t *que, cmpf_t cmp );

/*
** _q_enque(que,data)
**
** insert the supplied data value into the queue, ordering the
** queue using its built-in comparison function
*/
status_t _q_enque( queue_t *que, void *data );

/*
** _q_deque(que)
**
** remove the first element from the queue
**
** returns the thing that was removed, or NULL
*/
status_t _q_deque( queue_t *que, void **data );

/*
** _q_remove(que,data)
**
** remove the entry for "data" from the queue
**
** returns the status of the removal
*/
status_t _q_remove( queue_t *que, void *data );

/*
** _q_peek(que)
**
** peek at the data of the first element in the queue
**
** returns the data through the second parameter, and a status
*/
status_t _q_peek( queue_t *que, void **data );

/*
** _cmp_wakeup(v1,v2)
**
** compare two PCBs' "wakeup" fields to determine ordering
**
** returns < 0 if v1 < v2, 0 if v1 == v2, else > 0
*/
int32_t _cmp_wakeup( const void *v1, const void *v2 );

/*
** _q_dump(which,que)
**
** dump the contents of the specified queue to the console
*/
void _q_dump( char *which, queue_t *que );

#endif

#endif
