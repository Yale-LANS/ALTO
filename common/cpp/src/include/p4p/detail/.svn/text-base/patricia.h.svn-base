/*
 * $Id: patricia.h,v 1.6 2005/12/07 20:53:01 dplonka Exp $
 * Dave Plonka <plonka@doit.wisc.edu>
 *
 * This product includes software developed by the University of Michigan,
 * Merit Network, Inc., and their contributors. 
 *
 * This file had been called "radix.h" in the MRT sources.
 *
 * I renamed it to "patricia.h" since it's not an implementation of a general
 * radix trie.  Also, pulled in various requirements from "mrt.h" and added
 * some other things it could be used as a standalone API.
 * 
 * Changes:
 *   - Integration into P4P distribution (Richard Alimi)
 */

#ifndef _PATRICIA_H
#define _PATRICIA_H

namespace p4p {
namespace detail {

#define HAVE_IPV6

/* typedef unsigned int u_int; */
/* { from defs.h */
#define prefix_touchar(prefix) ((u_char *)&(prefix)->add.sin)
#define MAXLINE 1024
#define BIT_TEST(f, b)  ((f) & (b))
/* } */

#define addroute make_and_lookup

#include <sys/types.h> /* for u_* definitions (on FreeBSD 5) */
#include <p4p/detail/compiler.h>

#include <errno.h> /* for EAFNOSUPPORT */

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h> /* for struct in_addr */
	#include <netdb.h>
#endif

/* { from mrt.h */

typedef struct _prefix4_t {
    u_short family;		/* AF_INET | AF_INET6 */
    u_short bitlen;		/* same as mask? */
    int ref_count;		/* reference count */
    struct in_addr sin;
} prefix4_t;

typedef struct _prefix_t {
    u_short family;		/* AF_INET | AF_INET6 */
    u_short bitlen;		/* same as mask? */
    int ref_count;		/* reference count */
    union {
		struct in_addr sin;
#ifdef HAVE_IPV6
		struct in6_addr sin6;
#endif /* IPV6 */
    } add;
} prefix_t;

/* } */

typedef void (*void_fn_t)(prefix_t*, void*);

typedef struct _patricia_node_t {
   u_int bit;			/* flag if this node used */
   prefix_t *prefix;		/* who we are in patricia tree */
   struct _patricia_node_t *l, *r;	/* left and right children */
   struct _patricia_node_t *parent;/* may be used */
   void *data;			/* pointer to data */
   void	*user1;			/* pointer to usr data (ex. route flap info) */
} patricia_node_t;

typedef struct _patricia_tree_t {
   patricia_node_t 	*head;
   u_int		maxbits;	/* for IP, 32 bit addresses */
   int num_active_node;		/* for debug purpose */
} patricia_tree_t;


p4p_common_cpp_EXPORT patricia_node_t *patricia_search_exact (patricia_tree_t *patricia, prefix_t *prefix);
p4p_common_cpp_EXPORT patricia_node_t *patricia_search_best (patricia_tree_t *patricia, prefix_t *prefix);
p4p_common_cpp_EXPORT patricia_node_t * patricia_search_best2 (patricia_tree_t *patricia, prefix_t *prefix, 
				   int inclusive);
p4p_common_cpp_EXPORT patricia_node_t *patricia_lookup (patricia_tree_t *patricia, prefix_t *prefix);
p4p_common_cpp_EXPORT void patricia_remove (patricia_tree_t *patricia, patricia_node_t *node);
p4p_common_cpp_EXPORT patricia_tree_t *New_Patricia (int maxbits);
p4p_common_cpp_EXPORT void Clear_Patricia (patricia_tree_t *patricia, void_fn_t func);
p4p_common_cpp_EXPORT void Destroy_Patricia (patricia_tree_t *patricia, void_fn_t func);
p4p_common_cpp_EXPORT void patricia_process (patricia_tree_t *patricia, void_fn_t func);

p4p_common_cpp_EXPORT prefix_t *New_Prefix (int family, const void *dest, int bitlen);
p4p_common_cpp_EXPORT void Deref_Prefix (prefix_t * prefix);

/* { from demo.c */

char * prefix_toa2x (prefix_t *prefix, char *buff, int with_len);

/* } */

#define PATRICIA_MAXBITS 128
#define PATRICIA_NBIT(x)        (0x80 >> ((x) & 0x7f))
#define PATRICIA_NBYTE(x)       ((x) >> 3)

#define PATRICIA_DATA_GET(node, type) (type *)((node)->data)
#define PATRICIA_DATA_SET(node, value) ((node)->data = (void *)(value))

#define PATRICIA_WALK(Xhead, Xnode) \
    do { \
        patricia_node_t *Xstack[PATRICIA_MAXBITS+1]; \
        patricia_node_t **Xsp = Xstack; \
        patricia_node_t *Xrn = (Xhead); \
        while ((Xnode = Xrn)) { \
            if (Xnode->prefix)

#define PATRICIA_WALK_ALL(Xhead, Xnode) \
do { \
        patricia_node_t *Xstack[PATRICIA_MAXBITS+1]; \
        patricia_node_t **Xsp = Xstack; \
        patricia_node_t *Xrn = (Xhead); \
        while ((Xnode = Xrn)) { \
	    if (1)

#define PATRICIA_WALK_BREAK { \
	    if (Xsp != Xstack) { \
		Xrn = *(--Xsp); \
	     } else { \
		Xrn = (patricia_node_t *) 0; \
	    } \
	    continue; }

#define PATRICIA_WALK_END \
            if (Xrn->l) { \
                if (Xrn->r) { \
                    *Xsp++ = Xrn->r; \
                } \
                Xrn = Xrn->l; \
            } else if (Xrn->r) { \
                Xrn = Xrn->r; \
            } else if (Xsp != Xstack) { \
                Xrn = *(--Xsp); \
            } else { \
                Xrn = (patricia_node_t *) 0; \
            } \
        } \
    } while (0)

}; // namespace detail
}; // namespace p4p

#endif /* _PATRICIA_H */
