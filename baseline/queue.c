/*
** SCCS ID:	@(#)queue.c	1.1	4/13/17
**
** File:	queue.c
**
** Author:	CSCI-452 class of 20165
**
** Contributor:
**
** Description:	implementation of the queue module
*/

#define	__SP_KERNEL__

#include <baseline/common.h>

#include <baseline/queue.h>

/*
** PRIVATE DEFINITIONS
*/

// number of qnodes to create
//
// need one per PCB and one per stack, plus a fudge factor

#define N_QNODES        (N_PCBS + N_STACKS + 5)

/*
** PRIVATE GLOBAL VARIABLES
*/

static qnode_t _qnodes[N_QNODES];	// all qnodes in the system
static qnode_t *_free_qnodes;		// list of available qnodes

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** _qn_alloc()
**
** allocate a free qnode
**
** returns a pointer to the qnode
*/
static qnode_t *_qn_alloc( void ) {
	qnode_t *new;
	
	// just take the first one from the free list
	new = _free_qnodes;
	if( new != NULL ) {
		// this was the last one!
		_free_qnodes = new->next;
	}
	
	return( new );
}

/*
** _qn_dealloc()
**
** return a qnode to the free list
*/
static void _qn_dealloc( qnode_t *old ) {

	// ensure that this qnode isn't pointing to anything any more
	old->data = NULL;    // JIC - do we really need to do this?

	// just add to the front of the list
	old->next = _free_qnodes;
	_free_qnodes = old;
}

/*
** _qn_init()
**
** initialize the list of available qnodes
**
** SHOULD ONLY BE CALLED IF NO QUEUES OR QNODES ARE CURRENTLY IN USE
*/
static void _qn_init( void ) {

	// release the qnodes!
	for( int i = 0; i < N_QNODES; ++i ) {
		_qn_dealloc( &_qnodes[i] );
	}
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _q_init()
**
** initialize the queue module
**
** SHOULD ONLY BE CALLED IF NO QUEUES OR QNODES ARE CURRENTLY IN USE
*/
void _q_init( void ) {

	// report that we're here
	c_puts( " QUEUE" );

	// really all we need to do is create the free qnode list
	_qn_init();	
}

/*
** _q_reset(que,cmp)
**
** reset the queue structure to its "empty" state,
** using 'cmp' as its comparison function
*/
void _q_reset( queue_t *que, cmpf_t cmp ) {

	// sanity check!
	if( que == NULL ) {
		_kpanic( "_q_reset", "NULL queue parameter" );
	};
	
	// make sure there's nothing in this queue
	que->head = que->tail = NULL;
	que->size = 0;

	// assign the ordering function (if there is one)
	que->cmp = cmp;
}	

/*
** _q_enque(que,data)
**
** insert the supplied data value into the queue, ordering the
** queue using its built-in comparison function
*/
status_t _q_enque( queue_t *que, void *data ) {
	qnode_t *new;
	
	// sanity check!
	if( que == NULL ) {
		_kpanic( "_q_enque", "NULL que" );
	}
	
	// need a qnode to put into the list
	new = _qn_alloc();
	if( new == NULL ) {
		return( E_NO_QNODES );
	}
	
	// got one - fill in the relevant fields
	new->data = data;
	new->next = NULL;
	
	// is this the first thing in the list?
	if( que->size == 0 ) {
		que->head = que->tail = new;
		que->size = 1;
		return( E_SUCCESS );
	}
	
	// non-empty list; is it FIFO?
	if( que->cmp == NULL ) {
		que->tail->next = new;
		que->tail = new;
		que->size += 1;
		return( E_SUCCESS );
	}
	
	/*
	** adding to a non-empty queue that isn't FIFO
	**
	** standard singly-linked list traversal
	*/

	qnode_t *prev, *curr;
	
	prev = NULL;
	curr = que->head;

	while( curr && que->cmp(data,curr->data) >= 0 ) {
		prev = curr;
		curr = curr->next;
	}
	
	/*
	** prev curr	meaning
	** ==== ====	=======================
	**   0    0	empty list - CAN'T HAPPEN
	**   0   !0	insert at front:  new->c, head->new
	**  !0   !0	insert in middle: new->c, prev->new
	**  !0    0	att to end:       prev->new, tail->new
	*/
	
	new->next = curr; // always correct
	
	if( prev == NULL ) {	// insert at front
		if( curr == NULL ) {
			_kpanic( "_q_enque", "empty non-empty queue" );
		}
		que->head = new;
	} else {		// insert in middle or end
		prev->next = new;
		if( curr == NULL ) {	// added a new "last entry"
			que->tail = new;
		}
	}
	
	que->size += 1;
	
	return( E_SUCCESS );
}

/*
** _q_deque(que)
**
** remove the first element from the queue
**
** returns the thing that was removed, or NULL
*/
status_t _q_deque( queue_t *que, void **data ) {
	status_t stat;

	// grab the data from the first qnode
	if( (stat=_q_peek(que,data)) != E_SUCCESS ) {
		return( stat );
	}
	
	// success, so we now need to unlink the qnode

	// remember the qnode
	qnode_t *qn = que->head;

	// unlink it
	que->head = qn->next;
	que->size -= 1;

	// if this was the only node, must also change the tail pointer
	if( que->head == NULL ) {
		que->tail = NULL;
		// size should now be zero
		if( que->size != 0 ) {    // oops
			_kpanic( "_q_deque", "'empty' queue has non-zero size" );
		}
	}
		
	// return the qnode to the free pool
	_qn_dealloc( qn );
	
	return( E_SUCCESS );
}

/*
** _q_remove(que,data)
**
** remove the entry for "data" from the queue
**
** returns the status of the removal
*/
status_t _q_remove( queue_t *que, void *data ) {
	qnode_t *prev, *curr;
	
	// sanity check!
	if( que == NULL ) {
		_kpanic( "_q_remove", "NULL que" );
	}
	
	// locate the desired entry
	// (standard singly-linked list traversal)
	prev = NULL;
	curr = que->head;
	while( curr && curr->data != data ) {
		prev = curr;
		curr = curr->next;
	}
	
	// did we find it?
	if( curr == NULL ) {
		return( E_NO_PROCESS );
	}
	
	// yes - unlink it from the predecessor
	if( prev == NULL ) {
		que->head = curr->next;
	} else {
		prev->next = curr->next;
	}
	
	// ... and from the successor
	if( que->tail == curr ) {
		que->tail = prev;
	}
	
	// one [less|fewer] thing to worry about
	que->size -= 1;
	
	// return it to the free list
	_qn_dealloc( curr );
	
	return( E_SUCCESS );
}

/*
** _q_peek(que)
**
** peek at the data of the first element in the queue
**
** returns the data through the second parameter, and a status
*/
status_t _q_peek( queue_t *que, void **data ) {	
	
	// sanity checks
	if( que == NULL ) {
		_kpanic( "_q_peek", "NULL que" );
	}
	
	if( data == NULL ) {
		_kpanic( "_q_peek", "NULL data" );
	}

	// if it's empty, nothing to return
	if( que->size == 0 ) {
		return( E_EMPTY_QUEUE );
	}
	
	// another sanity check!
	if( que->head == NULL || que->tail == NULL ) {
		_kpanic( "_q_peek", "empty queue with non-zero size" );
	}
	
	// return the data pointer
	*data = que->head->data;
	
	// and a "success!" status
	return( E_SUCCESS );
}

/*
** _cmp_wakeup(v1,v2)
**
** compare two PCBs' "wakeup" fields to determine ordering
**
** returns < 0 if v1 < v2, 0 if v1 == v2, else > 0
*/
int32_t _cmp_wakeup( const void *v1, const void *v2 ) {
	const pcb_t *p1 = (pcb_t *) v1;
	const pcb_t *p2 = (pcb_t *) v2;
	
	// could probably just cast the wakeup fields to int32 and
	// return (p1->wakeup - p2->wakeup), but this works, too

	if( p1->wakeup < p2->wakeup ) {
		return( -1 );
	} else if( p1->wakeup == p2->wakeup ) {
		return( 0 );
	} else {
		return( 1 );
	}
}

/*
** _q_dump(which,queue)
**
** dump the contents of the specified queue to the console
*/
void _q_dump( char *which, queue_t *q ) {
	qnode_t *tmp;
	int i;

	c_printf( "%s: ", which );
	if( q == NULL ) {
		c_puts( "NULL???\n" );
		return;
	}

	c_printf( "head %08x tail %08x cmp %08x (%d items)\n",
		  (uint32_t) q->head, (uint32_t) q->tail,
		  (uint32_t) q->cmp, q->size );
	
	if( q->size > 0 ) {
		c_puts( " data: " );
		i = 0;
		for( tmp = q->head; tmp != NULL; tmp = tmp->next ) {
			c_printf( " [%08x]", (uint32_t) tmp->data );
			// limit to one line
			if( ++i > 5 ) break;
		}
		if( tmp != NULL ) {
			c_puts( " ..." );
		}
		c_puts( "\n" );
	}
}
