/*! \file signal/def.h
 * \brief Definitions for signals.
 */

# ifndef MCR_SIGNAL_DEFINES_H
# define MCR_SIGNAL_DEFINES_H

# include "util/util.h"

// Usable spatial dimensions for cursor/spatial events
# define MCR_DIMENSION_MIN 0
# define MCR_X 0
# define MCR_Y1 0
# define MCR_Y 1
# define MCR_Y2 1
# define MCR_Z 2
# define MCR_Y3 2
# define MCR_W 3
# define MCR_Y4 3
# ifndef MCR_DIMENSION_MAX
# define MCR_DIMENSION_MAX MCR_Z
# endif
# define MCR_DIMENSION_CNT (MCR_DIMENSION_MAX + 1)

/*!
 * \brief To signify a signal is being set released, or both.
 */
typedef enum mcr_KeyUpType
{
	MCR_DOWN = 0,
	MCR_UP = 1,
	MCR_BOTH = 2
} mcr_KeyUpType ;

/*! \brief Spatial position, indices are coordinates.
 * \ref MCR_DIMENSION_CNT
 * This is logically a spatial vector for MCR_DIMENSION_CNT coordinates.
 * */
typedef long long mcr_SpacePosition [ MCR_DIMENSION_CNT ] ;
//! \brief \ref mcr_SpacePosition
typedef mcr_SpacePosition mcr_Dimensions ;

/*!
 * \brief Inline: 1 ) dispatch specific, if not blocking
 * 2 ) dispatch unspecific, if not blocking
 * 3 ) send signal.
 *
 * \param signalPt mcr_Signal *
 * \param success int Set to 0 on failure.
 * */
# define MCR_SEND( signalPt, success ) \
	if ( ! ( signalPt )->type->dispatch || \
			! ( signalPt )->type->dispatch ( ( signalPt ) ) ) \
	{ \
		if ( ! mcr_AllDispatch || \
			! mcr_AllDispatch ( ( signalPt ) ) ) \
		{ \
			if ( ! ( signalPt )->type->send ( ( signalPt ) ) ) \
				success = 0 ; \
		} \
	}

/*!
 * \brief MCR_SEND with array of pointers to mcr_Signal
 *
 * \param signalPtArray mcr_Signal * [ ]
 * \param arrLen size_t Length of signalPtArray.
 * \param success int Set to 0 on failure.
 * */
# define MCR_SEND_ARRAY( signalPtArray, arrLen, success ) \
	for ( size_t _arrayIterator_ = 0 ; _arrayIterator_ < arrLen ; \
			_arrayIterator_ ++ ) \
	{ \
		MCR_SEND ( ( signalPtArray ) [ _arrayIterator_ ], success ) ; \
	}

/*!
 * \brief For pointer to mcr_ISignal set the name, send,
 * data_allocator, and copy_allocator.
 *
 * Should we reset id to 0?
 * \param isignalPt mcr_ISignal * Object to set values into.
 * \param namePt const char * mcr_ISignal name.
 * \param sender mcr_signal_fnc mcr_ISignal send.
 * \param dataSize size_t mcr_ISignal data_size.
 * */
# define MCR_ISIGNAL_INIT( isignalPt, namePt, sender, dataSize ) \
	( isignalPt )->name = ( namePt ) ; \
	( isignalPt )->send = ( sender ) ; \
	( isignalPt )->data_size = ( dataSize ) ;

# include STRINGIFY(signal/MCR_NATIVE_DIR/def.h)

# endif
