/*
 * Copyright (C) 1999, 2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* $Id: dispatch.h,v 1.32 2000/06/23 17:30:59 marka Exp $ */

#ifndef DNS_DISPATCH_H
#define DNS_DISPATCH_H 1

/*****
 ***** Module Info
 *****/

/*
 * DNS Dispatch Management
 *
 * 	Shared UDP and single-use TCP dispatches for queries and responses.
 *
 * MP:
 *
 *     	All locking is performed internally to each dispatch.
 *
 * Reliability:
 *
 * Resources:
 *
 * Security:
 *
 *	Depends on the isc_socket_t and dns_message_t for prevention of
 *	buffer overruns.
 *
 * Standards:
 *
 *	None.
 */

/***
 *** Imports
 ***/

#include <isc/buffer.h>
#include <isc/lang.h>
#include <isc/socket.h>
#include <dns/types.h>

#include <dns/types.h>

ISC_LANG_BEGINDECLS

/*
 * This event is sent to a task when a response (or request) comes in.
 * No part of this structure should ever be modified by the caller,
 * other than parts of the buffer.  The holy parts of the buffer are
 * the base and size of the buffer.  All other parts of the buffer may
 * be used.  On event delivery the used region contains the packet.
 *
 * "id" is the received message id,
 *
 * "addr" is the host that sent it to us,
 *
 * "buffer" holds state on the received data.
 *
 * The "free" routine for this event will clean up itself as well as
 * any buffer space allocated from common pools.
 */

struct dns_dispatchevent {
	ISC_EVENT_COMMON(dns_dispatchevent_t);	/* standard event common */
	isc_result_t		result;		/* result code */
	isc_int32_t		id;		/* message id */
	isc_sockaddr_t		addr;		/* address recv'd from */
	struct in6_pktinfo	pktinfo;	/* reply info for v6 */
	isc_buffer_t	        buffer;		/* data buffer */
	isc_uint32_t		attributes;	/* mirrored from socket.h */
};

/*
 * Attributes for added dispatchers.
 *
 * Values with the mask 0xffff0000 are application defined.
 * Values with the mask 0x0000ffff are library defined.
 *
 * Insane values (like setting both TCP and UDP) are not caught.  Don't
 * do that.
 *
 * _PRIVATE
 *	The dispatcher cannot be shared.
 *
 * _TCP, _UDP
 *	The dispatcher is a TCP or UDP socket.
 *
 * _IPV4, _IPV6
 *	The dispatcher uses an ipv4 or ipv6 socket.
 *
 * _ACCEPTREQUEST
 *	The dispatcher can be used to accept requests.
 *
 * _MAKEQUERY
 *	The dispatcher can be used to issue queries to other servers, and
 *	accept replies from them.
 */
#define DNS_DISPATCHATTR_PRIVATE	0x00000001U
#define DNS_DISPATCHATTR_TCP		0x00000002U
#define DNS_DISPATCHATTR_UDP		0x00000004U
#define DNS_DISPATCHATTR_IPV4		0x00000008U
#define DNS_DISPATCHATTR_IPV6		0x00000010U
#define DNS_DISPATCHATTR_ACCEPTREQUEST	0x00000020U
#define DNS_DISPATCHATTR_MAKEQUERY	0x00000040U
#define DNS_DISPATCHATTR_CONNECTED	0x00000080U

isc_result_t
dns_dispatchmgr_create(isc_mem_t *mctx, isc_entropy_t *entropy,
		       dns_dispatchmgr_t **mgrp);
/*
 * Creates a new dispatchmgr object.
 *
 * Requires:
 *	"mctx" be a valid memory context.
 *
 *	mgrp != NULL && *mgrp == NULL
 *
 *	"entropy" may be NULL, in which case an insecure random generator
 *	will be used.  If it is non-NULL, it must be a valid entropy
 *	source.
 *
 * Returns:
 *	ISC_R_SUCCESS	-- all ok
 *
 *	anything else	-- failure
 */


void
dns_dispatchmgr_destroy(dns_dispatchmgr_t **mgrp);
/*
 * Destroys the dispatchmgr when it becomes empty.  This could be
 * immediately.
 *
 * Requires:
 *	mgrp != NULL && *mgrp is a valid dispatchmgr.
 */


isc_result_t
dns_dispatchmgr_find(dns_dispatchmgr_t *mgr,
		     isc_sockaddr_t *local, isc_sockaddr_t *remote,
		     unsigned int attributes, unsigned int mask,
		     dns_dispatch_t **dispp);
/*
 * Search for a dispatcher that has the attributes specified by
 *	(attributes & mask)
 *
 * Requires:
 *	"mgr" be a valid dispatchmgr.
 *
 *	dispp != NULL && *dispp == NULL.
 *
 * Ensures:
 *	The dispatcher returned into *dispp is attached on behalf of the
 *	caller.  It is required that the caller detach from it when it is
 *	no longer needed.
 *
 * Returns:
 *	ISC_R_SUCCESS	-- found.
 *
 *	ISC_R_NOTFOUND	-- no dispatcher matching the requirements found.
 *
 *	anything else	-- failure.
 */


isc_result_t
dns_dispatch_getudp(dns_dispatchmgr_t *mgr, isc_socketmgr_t *sockmgr,
		    isc_taskmgr_t *taskmgr, isc_sockaddr_t *localaddr,
		    unsigned int buffersize,
		    unsigned int maxbuffers, unsigned int maxrequests,
		    unsigned int buckets, unsigned int increment,
		    unsigned int attributes, unsigned int mask,
		    dns_dispatch_t **dispp);

isc_result_t
dns_dispatch_createtcp(dns_dispatchmgr_t *mgr, isc_socket_t *sock,
		       isc_taskmgr_t *taskmgr, unsigned int buffersize,
		       unsigned int maxbuffers, unsigned int maxrequests,
		       unsigned int buckets, unsigned int increment,
		       unsigned int attributes, dns_dispatch_t **dispp);
/*
 * Create a new dns_dispatch and attach it to the provided isc_socket_t.
 *
 * For all dispatches, "buffersize" is the maximum packet size we will
 * accept.
 *
 * "maxbuffers" and "maxrequests" control the number of buffers in the
 * overall system and the number of buffers which can be allocated to
 * requests.
 *
 * "buckets" is the number of buckets to use, and should be prime.
 *
 * "increment" is used in a collision avoidance function, and needs to be
 * a prime > buckets, and not 2.
 *
 * Requires:
 *
 *	mgr is a valid dispatch manager.
 *
 *	sock is a valid.
 *
 *	task is a valid task that can be used internally to this dispatcher.
 *
 * 	"buffersize" >= 512, which is the minimum receive size for a
 *	DNS message.
 *
 *	maxbuffers > 0.
 *
 *	maxrequests <= maxbuffers.
 *
 *	buckets < 2097169 (the next prime after 65536 * 32)
 *
 *	increment > buckets (and prime)
 */


void
dns_dispatch_attach(dns_dispatch_t *disp, dns_dispatch_t **dispp);
/*
 * Attach to a dispatch handle.
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */


void
dns_dispatch_detach(dns_dispatch_t **dispp);
/*
 * Detaches from the dispatch.
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */

void    
dns_dispatch_starttcp(dns_dispatch_t *disp);
/*
 * Start processing of a TCP dispatch once the socket connects.
 *
 * Requires:
 *	'disp' is valid.
 */

isc_result_t
dns_dispatch_addresponse(dns_dispatch_t *disp, isc_sockaddr_t *dest,
			 isc_task_t *task, isc_taskaction_t action, void *arg,
			 isc_uint16_t *idp, dns_dispentry_t **resp);
/*
 * Add a response entry for this dispatch.
 *
 * "*idp" is filled in with the assigned message ID, and *resp is filled in
 * to contain the magic token used to request event flow stop.
 *
 * Arranges for the given task to get a callback for response packets.  When
 * the event is delivered, it must be returned using dns_dispatch_freeevent()
 * or through dns_dispatch_removeresponse() for another to be delivered.
 *
 * Requires:
 *	"idp" be non-NULL.
 *
 *	"task" "action" and "arg" be set as appropriate.
 *
 *	"dest" be non-NULL and valid.
 *
 *	"resp" be non-NULL and *resp be NULL
 *
 * Ensures:
 *
 *	<id, dest> is a unique tuple.  That means incoming messages
 *	are identifiable.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS		-- all is well.
 *	ISC_R_NOMEMORY		-- memory could not be allocated.
 *	ISC_R_NOMORE		-- no more message ids can be allocated
 *				   for this destination.
 */


void
dns_dispatch_removeresponse(dns_dispentry_t **resp,
			    dns_dispatchevent_t **sockevent);
/*
 * Stops the flow of responses for the provided id and destination.
 * If "sockevent" is non-NULL, the dispatch event and associated buffer is
 * also returned to the system.
 *
 * Requires:
 *	"resp" != NULL and "*resp" contain a value previously allocated
 *	by dns_dispatch_addresponse();
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */


isc_result_t
dns_dispatch_addrequest(dns_dispatch_t *disp,
			isc_task_t *task, isc_taskaction_t action, void *arg,
			dns_dispentry_t **resp);
/*
 * Arranges for the given task to get a callback for request packets.  When
 * the event is delivered, it must be returned using dns_dispatch_freeevent()
 * or through dns_dispatch_removerequest() for another to be delivered.
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */


void
dns_dispatch_removerequest(dns_dispentry_t **resp,
			   dns_dispatchevent_t **sockevent);
/*
 * Stops the flow of requests for the provided id and destination.
 * If "sockevent" is non-NULL, the dispatch event and associated buffer is
 * also returned to the system.
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */


void
dns_dispatch_freeevent(dns_dispatch_t *disp, dns_dispentry_t *resp,
		       dns_dispatchevent_t **sockevent);
/*
 * Return a dispatchevent and associated buffer to the dispatch.  This needs
 * to be called if more events are desired but a particular event is fully
 * processed, and the associated buffer is no longer needed.
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */


isc_socket_t *
dns_dispatch_getsocket(dns_dispatch_t *disp);
/*
 * Return the socket associated with this dispatcher.
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */


void
dns_dispatch_cancel(dns_dispatch_t *disp);
/*
 * cancel outstanding clients
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */

void
dns_dispatch_changeattributes(dns_dispatch_t *disp,
			      unsigned int attributes, unsigned int mask);
/*
 * Set the bits described by "mask" to the corresponding values in
 * "attributes".
 *
 * Requires:
 *	< mumble >
 *
 * Ensures:
 *	< mumble >
 *
 * Returns:
 *	< mumble >
 */

ISC_LANG_ENDDECLS

#endif /* DNS_DISPATCH_H */
