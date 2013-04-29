
# ifndef MCR_INLINE_HOT_H
# define MCR_INLINE_HOT_H

# include "hotkey/hot.h"

namespace macro
{
	//
	// Hot<ISignal>
	//
	//
	// IHotkey Triggering
	//
	inline bool Hot<ISignal>::trigger ( ISignal * inteceptedPt, int * modsPt )
	{
		if ( _triggerPt != NULL )
			return _triggerPt->trigger ( inteceptedPt, modsPt ) ;
		if ( ! hasCallback ( ) )
		{
			return false ;
		}

		std::thread ( & Hot<ISignal>::_sendThread, this, interceptedPt, modsPt )
				.detach ( ) ;
		return _blocking ;
	}

	//
	// Hot<T> types, statics, and public members.
	//
	// Edit: Initial registrars come from Dispatch<T>
//	template<class T>
//	typename Hot<T>::reg_t Hot<T>::DefaultRegistrar = NULL ;
//	template<class T>
//	typename Hot<T>::regSpecific_t Hot<T>::DefaultSpecificRegistrar = NULL ;
//	template<class T>
//	typename Hot<T>::unreg_t Hot<T>::DefaultUnregistrar = NULL ;

	//
	// Hot<T>
	//
	template<class T>
	Hot<T>::Hot ( const ISignal * callbackPt, ISignal * interceptPt,
		const int interceptMods, const bool enable,
		const bool specify, const bool blocking )
		: _reg ( DefaultRegistrar ), _regSpecific ( DefaultSpecificRegistrar ),
		_unreg ( DefaultUnregistrar ), _callbackObjectPt ( nullptr ),
		_callback ( NULL ), _triggerPt ( NULL ),
		_interceptMods ( interceptMods ), _enable ( enable ),
		_specify ( specify ), _blocking ( blocking )
	{
		setCallbackObject ( callbackPt ) ;
		setIntercept ( interceptPt ) ;
		// All set, enable or leave disabled.
		_register ( ) ;
	}
	template<class T>
	Hot<T>::Hot ( callback_t callback, ISignal * interceptPt,
		const int interceptMods, const bool enable,
		const bool specify, const bool blocking )
		: _reg ( DefaultRegistrar ), _regSpecific ( DefaultSpecificRegistrar ),
		_unreg ( DefaultUnregistrar ), _callbackObjectPt ( nullptr ),
		_callback ( callback ), _triggerPt ( NULL ),
		_interceptMods ( interceptMods ), _enable ( enable ),
		_specify ( specify ), _blocking ( blocking )
	{
		setIntercept ( interceptPt ) ;
		// All set, enable or leave disabled.
		_register ( ) ;
	}
	template<class T>
	Hot<T>::Hot ( ITrigger * callbackPt, ISignal * interceptPt,
		const int interceptMods, const bool enable,
		const bool specify, const bool blocking )
		: _reg ( DefaultRegistrar ), _regSpecific ( DefaultSpecificRegistrar ),
		_unreg ( DefaultUnregistrar ), _callbackObjectPt ( nullptr ),
		_callback ( NULL ), _triggerPt ( callbackPt ),
		_interceptMods ( interceptMods ), _enable ( enable ),
		_specify ( specify ), _blocking ( blocking )
	{
		setIntercept ( interceptPt ) ;
		// All set, enable or leave disabled.
		_register ( ) ;
	}

	template<class T>
	Hot<T>::Hot ( const Hot<T> & copytron )
		: _reg ( copytron._reg ), _regSpecific ( copytron._regSpecific ),
		_unreg ( copytron._unreg ), _callbackObjectPt ( nullptr ),
		_callback ( copytron._callback ), _triggerPt ( copytron._triggerPt ), _intercept ( copytron._intercept ),
		_interceptMods ( copytron._interceptMods ), _enable ( copytron._enable ),
		_specify ( copytron._specify ), _blocking ( copytron._blocking )
	{
		_callbackObjectPt = SignalFactory::copy ( copytron._callbackObjectPt.get ( ) ) ;
		// All set, enable or leave disabled.
		_register ( ) ;
	}
	template<class T>
	Hot<T> & Hot<T>::operator= ( const Hot & copytron )
	{
		if ( & copytron != this )
		{
			_unregister ( ) ;
			_reg = copytron._reg ;
			_regSpecific = copytron._regSpecific ;
			_unreg = copytron._unreg ;
			_callbackObjectPt = SignalFactory::copy ( copytron._callbackObjectPt.get ( ) ) ;
			_callback = copytron._callback ;
			_triggerPt = copytron._triggerPt ;
			_intercept = copytron._intercept ;
			_interceptMods = copytron._interceptMods ;
			_enable = copytron._enable ;
			_specify = copytron._specify ;
			_blocking = copytron._blocking ;
			// All set, enable or leave disabled.
			_register ( ) ;
		}
		return * this ;
	}

	//
	// Hot<T> Triggering from intercept.
	//
	template<class T>
	inline bool Hot<T>::trigger ( T * interceptedPt, int * modsPt )
	{
		if ( _triggerPt != NULL )
			return _triggerPt->trigger ( inteceptedPt, modsPt ) ;
		if ( ! hasCallback ( ) )
		{
			return _blocking ;
		}

		std::thread t ( & Hot<T>::_sendThread, this, interceptedPt, modsPt ) ;
		t.detach ( ) ;
		return _blocking ;
	}

	//
	// Hot<T> Registration
	//
	template<class T>
	typename Hot<T>::reg_t Hot<T>::getRegistrar ( ) const
	{
		return _reg ;
	}
	template<class T>
	void Hot<T>::setRegistrar ( reg_t reg )
	{
		_unregister ( ) ;
		_reg = reg ;
		_register ( ) ;
	}
	template<class T>
	typename Hot<T>::regSpecific_t Hot<T>::getSpecificRegistrar ( ) const
	{
		return _regSpecific ;
	}
	template<class T>
	void Hot<T>::setSpecificRegistrar ( regSpecific_t reg )
	{
		_unregister ( ) ;
		_regSpecific = reg ;
		_register ( ) ;
	}
	template<class T>
	typename Hot<T>::unreg_t Hot<T>::getUnregistrar ( ) const
	{
		return _unreg ;
	}
	template<class T>
	void Hot<T>::setUnregistrar ( unreg_t reg )
	{
		_unregister ( ) ;
		_unreg = reg ;
		_register ( ) ;
	}

	//
	// IHotkey Triggering
	//
	template<class T>
	inline bool Hot<T>::trigger ( ISignal * interceptedPt, int * modsPt )
	{
		if ( _triggerPt != NULL )
			return _triggerPt->trigger ( interceptedPt, modsPt ) ;
		if ( ! hasCallback ( ) )
		{
			return _blocking ;
		}

		std::thread t ( & Hot<T>::_sendThread, this, interceptedPt, modsPt ) ;
		t.detach ( ) ;
		return _blocking ;
	}

	//
	// Functions to modify how registering is handled.
	//
	template<class T>
	void Hot<T>::specify ( const bool useSpecific )
	{
		_unregister ( ) ;
		_specify = useSpecific ;
		_register ( ) ;
	}
	template<class T>
	bool Hot<T>::isSpecified ( ) const
	{
		return _specify ;
	}
//	template<class T>
//	bool Hot<T>::isRegistered ( ) const
//	{
//		return canRegister ( ) && _enable ;
//	}
//	template<class T>
//	bool Hot<T>::canRegister ( ) const
//	{
//		if ( _specify )
//			return _regSpecific != NULL ;
//		else
//			return _reg != NULL ;
//	}

	//
	// Functions to modify how signals are intercepted ( This is logically after hotkeys are registered ).
	//
	template<class T>
	void Hot<T>::setCallbackObject ( const ISignal * newCallbackPt )
	{
		// No need to unregister, this is just the callback.
		_callbackObjectPt = SignalFactory::copy ( newCallbackPt ) ;
		_callback = NULL ;
		_triggerPt = NULL ;
	}
	template<class T>
	const ISignal * Hot<T>::getCallbackObject ( ) const
	{
		return _callbackObjectPt.get ( ) ;
	}
	template<class T>
	void Hot<T>::setCallback ( callback_t newCallback )
	{
		// No need to unregister, this is just the callback.
		_callback = newCallback ;
		_callbackObjectPt.reset ( NULL ) ;
		_triggerPt = NULL ;
	}
	template<class T>
	typename IHotkey::callback_t Hot<T>::getCallback ( ) const
	{
		return _callback ;
	}
	template<class T>
	void Hot<T>::setTriggerObject ( ITrigger * newCallbackPt )
	{
		// No need to unregister, this is just the callback.
		_triggerPt = newCallbackPt ;
		_callback = NULL ;
		_callbackObjectPt.reset ( NULL ) ;
	}
	template<class T>
	const ITrigger * Hot<T>::getTriggerObject ( ) const
	{
		return _triggerPt ;
	}

	template<class T>
	void Hot<T>::setIntercept ( ISignal * interceptPt, const int interceptingMods )
	{
		_unregister ( ) ;
		// If wrong type, but not setting to NULL, then we do not want to
		// reset the intercept unerringly.
		if ( interceptPt == NULL )
			_intercept = T ( ) ;
		else
		{
			T * obj = dynamic_cast < T * > ( interceptPt ) ;
			if ( obj != NULL )
			{
				_intercept = * obj ;
			}
		}
		_interceptMods = interceptingMods ;
		_register ( ) ;
	}
	template<class T>
	void Hot<T>::setIntercept ( ISignal * interceptPt )
	{
		_unregister ( ) ;
		if ( interceptPt == NULL )
			_intercept = T ( ) ;
		else
		{
			T * obj = dynamic_cast < T * > ( interceptPt ) ;
			if ( obj != NULL )
				_intercept = * obj ;
		}
		_register ( ) ;
	}
	template<class T>
	void Hot<T>::setInterceptMods ( const int interceptMods )
	{
		_unregister ( ) ;
		_interceptMods = interceptMods ;
		_register ( ) ;
	}
	template<class T>
	void Hot<T>::setIntercept ( const std::string & interceptTypeName )
	{
		if ( containsAt ( interceptTypeName.c_str ( ), 0,
						_intercept.type ( ).c_str ( ), true ) )
		{
			_unregister ( ) ;
			_intercept = T ( ) ;
			_register ( ) ;
		}
	}
	template<class T>
	const ISignal * Hot<T>::getIntercept ( ) const
	{
		return & _intercept ;
	}
	template<class T>
	int Hot<T>::getInterceptMods ( ) const
	{
		return _interceptMods ;
	}
	template<class T>
	bool Hot<T>::hasCallback ( ) const
	{
		return _callbackObjectPt.get ( ) != NULL || _callback != NULL ||
				_triggerPt != NULL ;
	}
	template<class T>
	void Hot<T>::clear ( )
	{
		_unregister ( ) ;
		_callbackObjectPt.reset ( NULL ) ;
		_callback = NULL ;
		_triggerPt = NULL ;
		// TODO reset intercept and interceptMods?
		_register ( ) ;
	}
	template<class T>
	void Hot<T>::enable ( const bool enable )
	{
		_unregister ( ) ;
		_enable = enable ;
		_register ( ) ;
	}
	template<class T>
	bool Hot<T>::isEnabled ( ) const
	{
		return _enable ;
	}
	template<class T>
	void Hot<T>::block ( const bool enable )
	{
		// No need to unregister, this is just the blocking style.
		_blocking = enable ;
	}
	template<class T>
	bool Hot<T>::isBlocking ( ) const
	{
		return _blocking ;
	}

	//
	// private
	//
	template<class T>
	void Hot<T>::_unregister ( )
	{
		_unreg ( * this ) ;
	}
	template<class T>
	void Hot<T>::_register ( )
	{
		if ( ! _enable ) return ;
		if ( _specify )
		{
			_regSpecific ( * this, & _intercept, & _interceptMods ) ;
		}
		else
		{
			_reg ( * this ) ;
		}
	}
	template<class T>
	void Hot<T>::_sendThread ( ISignal * interceptedPt, int * modsPt )
	{
		if ( _callbackObjectPt.get ( ) != NULL )
		{
			_callbackObjectPt->send ( ) ;
		}
		else if ( _callback != NULL )
		{
			_callback ( ) ;
		}
	}
}

# endif
