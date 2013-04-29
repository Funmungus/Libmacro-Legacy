
# ifndef MCR_INLINE_STAGE_H
# define MCR_INLINE_STAGE_H

# include "hotkey/stage.h"

namespace macro
{
	template < typename T >
	Stage<T>::check_blocking_t Stage<T>::IsMeFunction =
			Stage<T>::isMeCannotSpecify ;

	bool IStage::operator == ( const IStage & rhs ) const
	{
		const ISignal * cept = getIntercept ( ) ;
		const ISignal * them = rhs.getIntercept ( ) ;
		// If either intercept is NULL, qualify them by address.
		if ( cept == NULL || them == NULL )
		{
			if ( cept != them )
				return false ;
		}
		// Qualify intercept names.
		else if ( cept->type ( ) != them->type ( ) )
			return false ;
		if ( interceptMods != rhs.interceptMods )
			return false ;
		if ( blocking != rhs.blocking )
			return false ;
		if ( activated != rhs.activated )
			return false ;
		if ( measurementError != rhs.measurementError )
			return false ;
		return true ;
	}
	bool IStage::operator < ( const IStage & rhs ) const
	{
		const ISignal * cept = getIntercept ( ) ;
		const ISignal * them = rhs.getIntercept ( ) ;
		// If either intercept is NULL, qualify them by address.
		if ( cept == NULL || them == NULL )
		{
			if ( cept >= them )
				return false ;
		}
		// Qualify intercept names.
		else if ( cept->type ( ) >= them->type ( ) )
			return false ;
		if ( interceptMods >= rhs.interceptMods )
			return false ;
		if ( blocking >= rhs.blocking )
			return false ;
		if ( activated >= rhs.activated )
			return false ;
		if ( measurementError >= rhs.measurementError )
			return false ;
		return true ;
	}
//	bool IStage::operator != ( const IStage & rhs ) const
//	{
//		return ! ( * this == rhs ) ;
//	}
//	bool IStage::operator > ( const IStage & rhs ) const
//	{
//		const ISignal * cept = getIntercept ( ) ;
//		const ISignal * them = rhs.getIntercept ( ) ;
//		// If either intercept is NULL, qualify them by address.
//		if ( cept == NULL || them == NULL )
//		{
//			if ( cept <= them )
//				return false ;
//		}
//		// Qualify intercept names.
//		else if ( cept->type ( ) <= them->type ( ) )
//			return false ;
//		if ( interceptMods <= rhs.interceptMods )
//			return false ;
//		if ( blocking <= rhs.blocking )
//			return false ;
//		if ( activated <= rhs.activated )
//			return false ;
//		if ( measurementError <= rhs.measurementError )
//			return false ;
//		return true ;
//	}
//	bool IStage::operator <= ( const IStage & rhs ) const
//	{
//		return ! ( * this > rhs ) ;
//	}
//	bool IStage::operator >= ( const IStage & rhs ) const
//	{
//		return ! ( * this < rhs ) ;
//	}

	template < typename T >
	Stage<T>::Stage ( const ISignal * interceptPt, const int interceptMods,
			const bool blocking, const bool activated,
			const int measurementError )
		: intercept ( ), interceptMods ( interceptMods ), blocking ( blocking ),
		activated ( activated ), measurementError ( measurementError )
	{
		setIntercept ( interceptPt ) ;
	}

	template < typename T >
	Stage<T>::Stage ( const Stage<T> & copytron )
		: intercept ( copytron.intercept ), interceptMods ( interceptMods ),
		blocking ( blocking ), activated ( activated ),
		measurementError ( measurementError )
	{}

	template < typename T >
	Stage<T> & Stage<T>::operator= ( const Stage<T> & copytron )
	{
		if ( & copytron != this )
		{
			intercept = copytron.intercept ;
			interceptMods = copytron.interceptMods ;
			blocking = copytron.blocking ;
			activated = copytron.activated ;
			measurementError = copytron.measurementError ;
		}
		return * this ;
	}

	template < typename T >
	const ISignal * Stage<T>::getIntercept ( ) const
	{
		return & intercept ;
	}
	template < typename T >
	void Stage<T>::setIntercept ( ISignal * interceptPt )
	{
		T * inTyped = dynamic_cast < T * > ( interceptPt ) ;
		if ( inTyped != NULL )
		{
			intercept = * inTyped ;
		}
	}

//	template < typename T >
//	int Stage<T>::getInterceptMods ( ) const
//	{
//		return interceptMods ;
//	}
//	template < typename T >
//	void Stage<T>::setInterceptMods ( const int mods )
//	{
//		interceptMods = mods ;
//	}

//	template < typename T >
//	bool Stage<T>::isBlocking ( ) const
//	{
//		return blocking ;
//	}
//	template < typename T >
//	void Stage<T>::enableBlocking ( const bool in )
//	{
//		blocking = in ;
//	}

//	template < typename T >
//	bool Stage<T>::isActivated ( ) const
//	{
//		return activated ;
//	}
//	template < typename T >
//	void Stage<T>::enableActivated ( const bool in )
//	{
//		activated = in ;
//	}

//	template < typename T >
//	int Stage<T>::getMeasurementError ( ) const
//	{
//		return measurementError ;
//	}
//	template < typename T >
//	void Stage<T>::setMeasurementError ( const int in )
//	{
//		measurementError = in ;
//	}

	template < typename T >
	inline bool Stage<T>::isMe ( const ISignal * interceptPt, const int mods,
							bool & blockingReturn ) const
	{
		return IsMeFunction ( * this, interceptPt, mods, blockingReturn ) ;
	}

	template < typename T >
	inline bool Stage<T>::isMeCannotSpecify ( const Stage<T> & obj,
									const ISignal * interceptPt,
									const int mods,
									bool & blockingReturn )
	{
		return ( obj.interceptMods == -1 || obj.interceptMods == mods ) &&
				obj.myType ( interceptPt, blockingReturn ) ;
	}

	template < typename T >
	inline bool Stage<T>::activateMe ( const ISignal * interceptPt,
									const int mods, bool & blockingReturn )
	{
		// If activated, preserve activation for signals of same type.
		// The next stage should already be activated, so do not bother with
		// isMe after already activated. Same type: preserve activation.
		if ( activated )
		{
			return activateUnspecific ( interceptPt, blockingReturn ) ;
		}
		activated = isMe ( interceptPt, mods, blockingReturn ) ;
		return activated ;
	}

	template < typename T >
	inline bool Stage<T>::myType ( const ISignal * interceptPt,
								bool & blockingReturn ) const
	{
		const T * inTyped = dynamic_cast < const T * > ( interceptPt ) ;
		if ( inTyped == NULL )
		{
			blockingReturn = false ;
			return false ;
		}
		blockingReturn = blocking ;
		return true ;
	}

	template < typename T >
	inline bool Stage<T>::activateUnspecific ( const ISignal * interceptPt,
										bool & blockingReturn )
	{
		activated = myType ( interceptPt, blockingReturn ) ;
		return activated ;
	}

	template <>
	bool stageIsMe ( const Stage<ISignal> & obj,
					const ISignal * interceptPt,
					const int mods, bool & blockingReturn )
	{
		blockingReturn = false ;
		if ( obj.interceptMods != -1 && obj.interceptMods != mods )
			return false ;
		// Case 1: Null intercept accepts all.
		if ( obj.interceptPt == NULL )
		{
			blockingReturn = obj.blocking ;
			return true ;
		}
		// Case 2: Existing intercept does not accept NULL incomming.
		if ( interceptPt == NULL ) return false ;
		// Case 3: If both exist, the address must be the same.
		if ( obj.interceptPt == interceptPt )
		{
			blockingReturn = obj.blocking ;
			return true ;
		}
		return false ;
	}

	template <>
	bool stageIsMe ( const Stage<Key> & obj, const ISignal * interceptedPt,
				const int mods, bool & blockingReturn )
	{
		blockingReturn = false ;
		const Key * keyPt = dynamic_cast < const Key * > ( interceptedPt ) ;
		// No key, no deal.
		if ( keyPt == NULL ) return false ;
		// Modifiers must either be equal, or non-specific.
		if ( obj.interceptMods != -1 && obj.interceptMods != mods )
			return false ;
		// Key of 0 allows all. Otherwise keys must be equal.
		if ( obj.intercept.getKey ( ) != 0 &&
			obj.intercept.getKey ( ) != keyPt->getKey ( ) ) return false ;
		// Scan of 0 allows all. Otherwise scans must be equal.
		if ( obj.intercept.getScan ( ) != 0 &&
			obj.intercept.getScan ( ) != keyPt->getScan ( ) ) return false ;
		// Key up of BOTH allows all. Otherwise key up must be equal.
		if ( obj.intercept.getKeyUp ( ) != BOTH &&
			obj.intercept.getKeyUp ( ) != keyPt->getKeyUp ( ) ) return false ;
		// All inequalities taken care of. Here we go.
		blockingReturn = blocking ;
		return true ;
	}

	template <>
	bool stageIsMe ( const Stage<MoveCursor> & obj,
					const ISignal * interceptedPt, const int mods,
								bool & blockingReturn )
	{
		blockingReturn = false ;
		const MoveCursor * cursorPt
				= dynamic_cast < const MoveCursor * > ( interceptedPt ) ;
		// No cursor, no deal.
		if ( cursorPt == NULL ) return false ;
		// Modifiers must either be equal, or non-specific.
		if ( obj.interceptMods != -1 && obj.interceptMods != mods )
			return false ;
		bool ret = obj.intercept.resembles ( * cursorPt, measurementError ) ;
		if ( ret )
		{
			blockingReturn = obj.blocking ;
			return true ;
		}
		return false ;
	}

	template <>
	bool stageIsMe ( const Stage<HIDEcho> & obj,
								const ISignal * interceptedPt, const int mods,
								bool & blockingReturn )
	{
		blockingReturn = false ;
		const HIDEcho * echoPt =
				dynamic_cast < const HIDEcho * > ( interceptedPt ) ;
		// No echo, no deal.
		if ( echoPt == NULL ) return false ;
		// Modifiers must either be equal, or non-specific.
		if ( obj.interceptMods != -1 && obj.interceptMods != mods ) return false ;
		if ( obj.intercept.getEvent ( ) != -1 &&
			obj.intercept.getEvent ( ) != echoPt->getEvent ( ) ) return false ;
		blockingReturn = blocking ;
		return true ;
	}

	template <>
	bool stageIsMe ( const Stage<Scroll> & obj,
					const ISignal * interceptedPt, const int mods,
							bool & blockingReturn )
	{
		blockingReturn = false ;
		const Scroll * scrollPt =
				dynamic_cast < const Scroll * > ( interceptedPt ) ;
		// No scroll, no deal.
		if ( scrollPt == NULL ) return false ;
		// Modifiers must either be equal, or non-specific.
		if ( obj.interceptMods != -1 && obj.interceptMods != mods ) return false ;

		Dimensions pos = {0}, posThem = {0} ;
		obj.intercept.getDimensions ( pos ) ;
		scrollPt->getDimensions ( posThem ) ;
		bool ret = resemblesFromZero ( pos, posThem ) ;
		if ( ret )
			blockingReturn = blocking ;
		return ret ;
	}

	bool StageContainer::activateMe ( const ISignal * interceptPt, const int mods,
									bool & blockingReturn )
	{
		// Will activate will affect blockingReturn. Always false if return is false.
		if ( willActivate ( interceptPt, mods, blockingReturn ) )
		{
			return true ;
		}
		// Activate stages will be true if any activated stage is a blocking stage.
		activateStages ( interceptPt, mods, blockingReturn ) ;
		return false ;
	}

	bool StageContainer::willActivate ( const ISignal * interceptPt, const int mods,
										bool & blockingReturn )
	{
		// If no elements, assume consistant activation.
		if ( stages.size ( ) == 0 )
		{
			blockingReturn = false ;
			return true ;
		}
		// Also, if this is the only stage, assume consistant activation.
		if ( stages.size ( ) == 1 )
		{
			return stages [ 0 ]->isMe ( interceptPt, mods, blockingReturn ) ;
		}
		// Last element itself should never be activated naturally,
		// but it is possible that it was affected programmatically.
		auto it = stages.rbegin ( ) ;
		if ( ( * ( it + 1 ) )->activated || ( * it )->activated )
		{
			return ( * it )->isMe ( interceptPt, mods, blockingReturn ) ;
		}
		// No possible activation, no blocking.
		blockingReturn = false ;
		return false ;
	}
	void StageContainer::activateStages ( const ISignal * interceptPt, const int mods,
										bool & blockingReturn )
	{
		bool blocking = false ;
		// Case 1: No stages, no effect.
		if ( stages.size ( ) == 0 )
			return ;
		// Case 3: more stages, check all possible activations, then check activation
		// of first element.
		// Check all except first element.
		if ( stages.size ( ) > 1 )
		{
			for ( unsigned int i = stages.size ( ) - 1 ; i > 0 ; i-- )
			{
				// If currently activated, we do not want it to block again.
				// Even if previous is activated too, we do not want to block
				// with already-activated stages.
				if ( stages [ i ]->activated )
				{
					stages [ i ]->activateMe ( interceptPt, mods, blocking ) ;
					blocking = false ;
				}
				// If current not activated, but previous is, we may block.
				else if ( stages [ i - 1 ]->activated )
				{
					stages [ i ]->activateMe ( interceptPt, mods, blocking ) ;
					// activateMe will always change blocking to false or true,
					// but resetting it to false is a fail-safe.
					if ( blocking )
					{
						blockingReturn = true ;
						blocking = false ;
					}
				}
			}
		}
		// Case 2: 1 stage, only affect first.
		stages [ 0 ]->activateMe ( interceptPt, mods, blocking ) ;
		if ( blocking ) blockingReturn = blocking ;
	}

	/* ISignal implementation */
	bool StageContainer::send ( )
	{
		for ( auto it = stages.begin ( ) ; it != stages.end ( ) ; it++ )
		{
			( * it )->activated = false ;
		}
		return true ;
	}
	bool StageContainer::operator ( ) ( )
	{
		return send ( ) ;
	}
	bool StageContainer::dispatch ( )
	{
		return Signal < StageContainer >::Dispatcher ( * this ) ||
				ISignal::dispatch ( ) ;
	}

	bool StageContainer::operator == ( const StageContainer & rhs ) const
	{
		if ( stages.size ( ) != rhs.stages.size ( ) ) return false ;
		for ( unsigned int i = 0 ; i < stages.size ( ) ; i++ )
		{
			if ( stages [ i ] != rhs.stages [ i ] )
				return false ;
		}
		return true ;
	}
	bool StageContainer::operator != ( const StageContainer & rhs ) const
	{
		return ! ( * this == rhs ) ;
	}
	bool StageContainer::operator < ( const StageContainer & rhs ) const
	{
		// If sizes different, return equality of them.
		if ( stages.size ( ) != rhs.stages.size ( ) )
			return stages.size ( ) < rhs.stages.size ( ) ;
		// Otherwise equality determined by contained stages.
		for ( unsigned int i = 0 ; i < stages.size ( ) ; i++ )
		{
			if ( stages [ i ] >= rhs.stages [ i ] )
				return false ;
		}
		return true ;
	}
	bool StageContainer::operator > ( const StageContainer & rhs ) const
	{
		// If sizes different, return equality of them.
		if ( stages.size ( ) != rhs.stages.size ( ) )
			return stages.size ( ) < rhs.stages.size ( ) ;
		// Otherwise equality determined by contained stages.
		for ( unsigned int i = 0 ; i < stages.size ( ) ; i++ )
		{
			if ( stages [ i ] <= rhs.stages [ i ] )
				return false ;
		}
		return true ;
	}
	bool StageContainer::operator <= ( const StageContainer & rhs ) const
	{
		return ! ( * this > rhs ) ;
	}
	bool StageContainer::operator >= ( const StageContainer & rhs ) const
	{
		return ! ( * this < rhs ) ;
	}

	bool Stage<ISignal>::isMe ( const ISignal * interceptPt, const int mods,
							bool & blockingReturn ) const
	{
		return IsMeFunction ( * this, interceptPt, mods, blockingReturn ) ;
	}

	bool Stage<ISignal>::activateMe ( const ISignal * interceptPt,
									const int mods, bool & blockingReturn )
	{
		// If activated, preserve activation for signals of same type.
		// The next stage should already be activated, so do not bother with
		// isMe after already activated. Same type: preserve activation.
		if ( activated )
		{
			return activateUnspecific ( interceptPt, blockingReturn ) ;
		}
		activated = isMe ( interceptPt, mods, blockingReturn ) ;
		return activated ;
	}

	bool Stage<ISignal>::myType ( const ISignal * interceptPt, bool & blockingReturn ) const
	{
		// Case 1: Null intercept accepts all.
		if ( this->interceptPt == NULL )
		{
			blockingReturn = blocking ;
			return true ;
		}
		// Case 2: Existing intercept does not accept NULL incomming.
		if ( interceptPt == NULL )
		{
			blockingReturn = false ;
			return false ;
		}
		// Case 3: If both exist, the type must be the same.
		if ( this->interceptPt->type ( ) == interceptPt->type ( ) )
		{
			blockingReturn = blocking ;
			return true ;
		}
		blockingReturn = false ;
		return false ;
	}

	bool Stage<ISignal>::activateUnspecific ( const ISignal * interceptPt, bool & blockingReturn )
	{
		activated = myType ( interceptPt, blockingReturn ) ;
		return activated ;
	}

}
# endif // MCR_INLINE_STAGE_H
