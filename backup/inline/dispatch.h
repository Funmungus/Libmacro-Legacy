
# ifndef MCR_INLINE_DISPATCH_H
# define MCR_INLINE_DISPATCH_H

# include "hotkey/dispatch.h"

namespace macro
{
	template < class VectorHotType >
	void DispatchToVector ( std::vector < Hot < VectorHotType > * > &
			containerVector, ISignal * interceptedPt,
			int * modsPt, bool & blocking )
	{
		for ( auto it = containerVector.begin ( ) ;
			it != containerVector.end ( ) ;
			it++ )
		{
			blocking = ( * it )->trigger ( interceptedPt, modsPt ) ?
						true : blocking ;
		}
	}

	template < class MapFromType, class VectorHotType >
	void DispatchToMap ( std::map < MapFromType,
					std::vector < Hot < VectorHotType > * > > & containerMap,
					MapFromType & mapping, ISignal * interceptedPt,
					int * modsPt, bool & blocking )
	{
		auto it = containerMap.find ( mapping ) ;
		// dispatch to all of given address and modifier.
		if ( it != containerMap.end ( ) )
		{
			DispatchToVector < VectorHotType > ( it->second, interceptedPt,
												modsPt, blocking ) ;
		}
	}

	//
	// Dispatch
	//
	template < class T >
	typename Signal<T>::dispatch_t * Dispatch<T>::DispatcherPt =
		& Signal<T>::Dispatcher ;
	template < class T >
	typename Signal<T>::dispatch_t Dispatch<T>::DispatchFunction =
		Dispatch<T>::dispatch ;
	template < class T >
	bool Dispatch<T>::EnableUnspecific = true ;
	template < class T >
	typename Dispatch<T>::hotvector_t Dispatch<T>::HotSet ;
	template < class T >
	bool Dispatch<T>::EnableSpecific = true ;

	// Trigger and dispatch.
	// Unspecifics.
	template < class T >
	bool Dispatch<T>::isEnabled ( )
	{
		// Container of dispatch function must exist.
		if ( DispatcherPt == NULL ) return false ;
		// Must be enabled with at least one of specific or unspecific.
		if ( ! EnableSpecific && ! EnableUnspecific ) return false ;
		// If specific dispatcher exists, the container may use this.
//		return DispatchFunction != NULL &&
		return * DispatcherPt == DispatchFunction ;
	}
	template < class T >
	void Dispatch<T>::enable ( const bool enable )
	{
		// Cannot do anything without a dispatch function container.
		if ( DispatcherPt == NULL )
		{
			return ;
		}
		if ( enable )
		{
			 * DispatcherPt = DispatchFunction ;
		}
		else
		{
			 * DispatcherPt = Signal < T >::noDispatch ;
		}
	}
	template < class T >
	void Dispatch<T>::reset ( )
	{
		EnableUnspecific = true ;
		EnableSpecific = true ;
		bool en = isEnabled ( ) ;
		release ( ) ;
		enable ( en ) ;
	}
	template < class T >
	void Dispatch<T>::release ( )
	{
		HotSet.clear ( ) ;
	}

	// Trigger and dispatch.
	// Unspecifics.
	template < class T >
	void Dispatch<T>::add ( Hot<T> & newHotkey )
	{
		HotSet.push_back ( & newHotkey ) ;
	}
	template < class T >
	void Dispatch<T>::noSpecifics ( Hot<T> & newCallback, ISignal *, int * )
	{
		add ( newCallback ) ;
	}

	template < class T >
	void Dispatch<T>::remove ( Hot<T> & delHotkey )
	{
		for ( auto it = HotSet.begin ( ) ;
			it != HotSet.end ( ) ; it++ )
		{
			if ( * it == & delHotkey )
			{
				HotSet.erase ( it ) ;
				it-- ;
			}
		}
	}
	template < class T >
	inline bool Dispatch<T>::dispatch ( T & intercepted )
	{
		// Get InternalModifiers and
		InternalModLock.lock ( ) ;
		bool blocking = dispatchModified ( intercepted, InternalMods ) ;
		InternalModLock.unlock ( ) ;
		return blocking ;
	}
	template < class T >
	inline bool Dispatch<T>::dispatchModified ( T & intercepted, int & mods )
	{
		bool blocking = false ;
		if ( EnableUnspecific )
			DispatchToVector < T > ( HotSet, & intercepted, & mods, blocking ) ;
		return blocking ;
	}

	//
	// SignalDispatch
	//
	// And finally, the dispatching function.
	bool SignalDispatch::dispatch ( ISignal & intercepted )
	{
		bool blocking = false ;
		InternalModLock.lock ( ) ;

		if ( Dispatch<ISignal>::EnableSpecific )
		{
			// We have modified from intercepted signal, and have the value in m.
			// dispatch to specifics
			address_mod_t mapping ( & intercepted, InternalMods ) ;
			DispatchToMap < SignalDispatch::address_mod_t, ISignal >
					( HotSpecified, mapping, & intercepted,
					& InternalMods, blocking ) ;

			// dispatch to staged hotkeys.
			DispatchToVector < ISignal > ( StagedSet, & intercepted,
										& InternalMods, blocking ) ;

			// dispatch to unspecific address.
			mapping.first = NULL ;
			DispatchToMap < SignalDispatch::address_mod_t, ISignal >
					( HotSpecified, mapping, & intercepted,
					& InternalMods, blocking ) ;
			mapping.first = & intercepted ;

			// dispatch to unspecific modifiers
			// Assume modifier is not -1 ( i.e. unspecific modifier ).
			// mapping modifier will return to modMem.
			mapping.second = -1 ;
			DispatchToMap < SignalDispatch::address_mod_t, ISignal >
					( HotSpecified, mapping, & intercepted,
					& InternalMods, blocking ) ;

			// Removed. Completely unspecific should be added into generic dispatch instead.
	//		mapping.second = -1 ;
	//		DispatchTo < SignalDispatch::address_mod_t, ISignal > ( HotSpecified, mapping, & intercepted, & m.first, blocking ) ;
		}

		// dispatch to completely generic dispatch.
		blocking = Dispatch<ISignal>::dispatchModified
				( intercepted, InternalMods ) ? true : blocking ;

		// Modify InternalMods with intercepted signal.
		Mods::mod_t m = SignalMod::SigToMod.second ( & intercepted ) ;
		Mods::modify ( InternalMods, m.first, m.second ) ;
		InternalModLock.unlock ( ) ;

		return blocking ;
	}

	//
	// Hot<T> types, statics, and public members.
	//
	template<class T>
	typename Hot<T>::reg_t Hot<T>::DefaultRegistrar =
			Dispatch<T>::add ;
	template<class T>
	typename Hot<T>::regSpecific_t Hot<T>::DefaultSpecificRegistrar =
			Dispatch<T>::noSpecifics ;
	template<class T>
	typename Hot<T>::unreg_t Hot<T>::DefaultUnregistrar =
			Dispatch<T>::remove ;
}

# endif // MCR_INLINE_DISPATCH_H
