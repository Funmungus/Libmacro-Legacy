/*! \file signal/signal.h
 * Usage :
 */

/* Given object data type Key
 * mcr_Key key ;
 * mcr_Key_init ( & key, 'A', 0, BOTH ) ;
 *
 * Or, data type specific if speed needed for 'native' types.
 * MCR_KEY_INIT ( & key, 'A', 0, BOTH ) ;
 * mcr_Signal keysig = { & mcr_IKey, & key } ;
 * int success = 1 ;
 * MCR_SEND ( & keysig, success ) ;
 *
 * Or, data type sending without dispatch, if not dispatch necessary.
 * MCR_KEY_SEND ( key ) ;
 *
 * Usage in an array set :
 * mcr_Key keys [ 2 ] ;
 * mcr_Key_init ( keys, 'A', 0, DOWN ) ;
 * mcr_Key_init ( keys + 1, 'A', 0, UP ) ;
 * mcr_NoOp noop = { { 1, 0 } } ;
 * mcr_MoveCursor curs [ 2 ] ;
 * mcr_MoveCursor_init ( curs, -5000, -5000, -5000, true ) ;
 * mcr_MoveCursor_init ( curs + 1, 50, 50, 50, true ) ;
 * mcr_Signal set [ ] =
 * {
 * 		{ & mcr_Signal_MoveCursor, curs },
 * 		{ & mcr_Signal_NoOp, & noop },
 * 		{ & mcr_Signal_MoveCursor, curs + 1 },
 * 		{ & mcr_Signal_NoOp, & noop },
 * 		{ & mcr_Signal_Key, keys },
 * 		{ & mcr_Signal_NoOp, & noop },
 * 		{ & mcr_Signal_Key, keys + 1 }
 * } ;
 * size_t len = sizeof ( set ) / sizeof ( mcr_Signal ) ;
 * int success = 1 ;
 * MCR_SEND_ARRAY ( set, len, success ) ;
 *
 * repeat sending as needed.
 *
 * All macros have a function that may be used insted,
 * except for MCR_SEND_ARRAY.
 * \ref \file signal/isignal.h
 */

# ifndef MCR_SIGNAL_H
# define MCR_SIGNAL_H

# include "signal/standard.h"

# endif
