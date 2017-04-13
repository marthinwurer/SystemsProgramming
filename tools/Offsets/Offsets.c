/*
** SCCS ID:	%W%	%G%
**
** File:	Offsets.c
**
** Author:		Warren R. Carithers
**
** Description:		Print byte offsets for fields in various structures.
**
** This program exists to simplify life.  If/when fields in a structure are
** changed, this can be modified, recompiled and executed to come up with
** byte offsets for use in accessing structure fields from assembly language.
**
*/

#define	__SP_KERNEL__

#include <baseline/common.h>

// avoid complaints about stdio.h
#undef NULL

#include <baseline/queue.h>
#include <baseline/process.h>

#include <stdio.h>

qnode_t qnode;
queue_t queue;
context_t context;
pcb_t pcb;

int main( void ) {

    printf( "Offsets into qnode_t (%lu bytes):\n", sizeof(qnode) );
    printf( "   next:\t%ld\n", (char *)&qnode.next - (char *)&qnode );
    printf( "   data:\t%ld\n", (char *)&qnode.data - (char *)&qnode );
    putchar( '\n' );

    printf( "Offsets into queue_t (%lu bytes):\n", sizeof(queue) );
    printf( "   head:\t%ld\n", (char *)&queue.head - (char *)&queue );
    printf( "   tail:\t%ld\n", (char *)&queue.tail - (char *)&queue );
    printf( "   size:\t%ld\n", (char *)&queue.size - (char *)&queue );
    printf( "   cmp:\t%ld\n", (char *)&queue.cmp - (char *)&queue );
    putchar( '\n' );

    printf( "Offsets into context_t (%lu bytes):\n", sizeof(context) );
    printf( "   ss:\t\t%ld\n", (char *)&context.ss - (char *)&context );
    printf( "   gs:\t\t%ld\n", (char *)&context.gs - (char *)&context );
    printf( "   fs:\t\t%ld\n", (char *)&context.fs - (char *)&context );
    printf( "   es:\t\t%ld\n", (char *)&context.es - (char *)&context );
    printf( "   ds:\t\t%ld\n", (char *)&context.ds - (char *)&context );
    printf( "   edi:\t\t%ld\n", (char *)&context.edi - (char *)&context );
    printf( "   esi:\t\t%ld\n", (char *)&context.esi - (char *)&context );
    printf( "   ebp:\t\t%ld\n", (char *)&context.ebp - (char *)&context );
    printf( "   esp:\t\t%ld\n", (char *)&context.esp - (char *)&context );
    printf( "   ebx:\t\t%ld\n", (char *)&context.ebx - (char *)&context );
    printf( "   edx:\t\t%ld\n", (char *)&context.edx - (char *)&context );
    printf( "   ecx:\t\t%ld\n", (char *)&context.ecx - (char *)&context );
    printf( "   eax:\t\t%ld\n", (char *)&context.eax - (char *)&context );
    printf( "   vector:\t%ld\n",(char *)&context.vector - (char *)&context);
    printf( "   code:\t%ld\n", (char *)&context.code - (char *)&context );
    printf( "   eip:\t\t%ld\n", (char *)&context.eip - (char *)&context );
    printf( "   cs:\t\t%ld\n", (char *)&context.cs - (char *)&context );
    printf( "   eflags:\t%ld\n",(char *)&context.eflags - (char *)&context);
    putchar( '\n' );

    printf( "Offsets into pcb_t (%lu bytes):\n", sizeof(pcb) );
    printf( "   context:\t%ld\n", (char *)&pcb.context - (char *)&pcb );
    printf( "   stack:\t%ld\n", (char *)&pcb.stack - (char *)&pcb );
    printf( "   wakeup:\t%ld\n", (char *)&pcb.wakeup - (char *)&pcb );
    printf( "   exitstatus:\t%ld\n", (char *)&pcb.exitstatus - (char *)&pcb );
    printf( "   pid:\t\t%ld\n", (char *)&pcb.pid - (char *)&pcb );
    printf( "   ppid:\t%ld\n", (char *)&pcb.ppid - (char *)&pcb );
    printf( "   children:\t%ld\n", (char *)&pcb.children - (char *)&pcb );
    printf( "   state:\t%ld\n", (char *)&pcb.state - (char *)&pcb );
    printf( "   prio:\t%ld\n", (char *)&pcb.prio - (char *)&pcb );
    printf( "   q_default:\t%ld\n", (char *)&pcb.q_default - (char *)&pcb );
    printf( "   q_ticks:\t%ld\n", (char *)&pcb.q_ticks - (char *)&pcb );

    return( 0 );
}
