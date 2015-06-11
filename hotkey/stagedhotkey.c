/* hotkey/stagedhotkey.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/dispatch.h"

namespace macro
{
	const std::string StagedHotkey::Name = "Stagedhotkey" ;
	StagedHotkey::dispatch_t StagedHotkey::Dispatcher = NULL ;

	StagedHotkey::reg_t StagedHotkey::DefaultRegistrar =
			& Dispatch::addStaged ;
	StagedHotkey::unreg_t StagedHotkey::DefaultUnregistrar =
			& Dispatch::removeStaged ;

	StagedHotkey::StagedHotkey ( const ISignal * callbackPt,
				const StageContainer::stage_vector_t & stages,
				const bool enable, const bool specify,
								const BlockingStyle style )
		: _reg ( DefaultRegistrar ), _unreg ( DefaultUnregistrar ),
		_callbackObjectPt ( nullptr ), _callback ( NULL ),
		_stageContainer ( stages ), _blockingStyle ( style ), _enable ( enable ),
		_specify ( specify ), _blocking ( true )
	{
		setCallbackObject ( callbackPt ) ;
		_register ( ) ;
	}

	StagedHotkey::StagedHotkey ( IHotkey::callback_t callback,
				const StageContainer::stage_vector_t & stages =
					StageContainer::stage_vector_t ( ),
				const bool enable = false, const bool specify = true,
								const BlockingStyle style = BLOCK_FINAL )
		: _reg ( DefaultRegistrar ), _unreg ( DefaultUnregistrar ),
		_callbackObjectPt ( nullptr ), _callback ( callback ),
		_stageContainer ( stages ), _blockingStyle ( style ), _enable ( enable ),
		_specify ( specify ), _blocking ( true )
	{
		_register ( ) ;
	}

	StagedHotkey::StagedHotkey ( const StagedHotkey & copytron )
		: _reg ( copytron._reg ), _unreg ( copytron._unreg ),
		_callbackObjectPt ( nullptr ), _callback ( copytron._callback ),
		_stageContainer ( copytron._stageContainer ),
		_blockingStyle ( copytron._blockingStyle ), _enable ( copytron._enable ),
		_specify ( copytron._specify ), _blocking ( copytron._blocking )
	{
		if ( copytron.getCallbackObject ( ) != NULL )
		{
			setCallbackObject ( copytron.getCallbackObject ( ) ) ;
		}
		_register ( ) ;
	}

	StagedHotkey & StagedHotkey::operator= ( const StagedHotkey & copytron )
	{
		if ( this != & copytron )
		{
			_unregister ( ) ;
			_reg = copytron._reg ;
			_unreg = copytron._unreg ;
			if ( copytron._callback == NULL )
			{
				setCallbackObject ( copytron._callbackObjectPt ) ;
			}
			else
			{
				setCallback ( copytron._callback ) ;
			}
			_stageContainer = copytron._stageContainer ;
			_blockingStyle = copytron._blockingStyle ;
			_specify = copytron._specify ;
			_blocking = copytron._blocking ;
			_register ( ) ;
		}
		return * this ;
	}

	StagedHotkey & StagedHotkey::operator= ( const StageContainer & newStages )
	{
		_unregister ( ) ;
		_stageContainer = newStages ;
		_register ( ) ;
		return * this ;
	}

	StagedHotkey & StagedHotkey::operator= ( const StageContainer::stage_vector_t & newStages )
	{
		_unregister ( ) ;
		_stageContainer.stages = newStages ;
		_register ( ) ;
		return * this ;
	}

	typename StagedHotkey::reg_t StagedHotkey::getRegistrar ( ) const
	{
		return _reg ;
	}
	void StagedHotkey::setRegistrar ( reg_t reg )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_reg = reg ;
		_register ( ) ;
		_lock.unlock ( ) ;
	}
	typename StagedHotkey::unreg_t StagedHotkey::getUnregistrar ( ) const
	{
		return _unreg ;
	}
	void StagedHotkey::setUnregistrar ( unreg_t reg )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_unreg = reg ;
		_register ( ) ;
		_lock.unlock ( ) ;
	}
	BlockingStyle StagedHotkey::getBlockStyle ( ) const
	{
		return _blockingStyle ;
	}
	void StagedHotkey::setBlockStyle ( const BlockingStyle style )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_blockingStyle = style ;
		if ( _blocking == true )
		{
			switch ( style )
			{
			case UNMANAGED :
				break ;
			case NO_BLOCK :
				for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) ;
					i++ )
				{
					_stageContainer.stages [ i ]->enableBlocking ( false ) ;
				}
				break ;
			case BLOCK_FINAL :
				for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) - 1 ;
					i++ )
				{
					_stageContainer.stages [ i ]->enableBlocking ( false ) ;
				}
				( * _stageContainer.stages.rbegin ( ) )->enableBlocking ( true ) ;
				break ;
			case BLOCK_ALL :
				for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) ;
					i++ )
				{
					_stageContainer.stages [ i ]->enableBlocking ( true ) ;
				}
				break ;
			}
		}

		_register ( ) ;
		_lock.unlock ( ) ;
	}

	StageContainer StagedHotkey::getStages ( ) const
	{
		return _stageContainer ;
	}
	void StagedHotkey::setStages ( const StageContainer & stageIn )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_stageContainer = stageIn ;
		_register ( ) ;
		_lock.unlock ( ) ;
	}

	StageContainer::stage_vector_t StagedHotkey::getStages ( ) const
	{
		return _stageContainer.stages ;
	}
	void StagedHotkey::setStages ( const StageContainer::stage_vector_t & stageIn )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_stageContainer.stages = stageIn ;
		_register ( ) ;
		_lock.unlock ( ) ;
	}
	void StagedHotkey::specify ( const bool useSpecific )
	{
		_lock.lock ( ) ;
		_specify = useSpecific ;
		_lock.unlock ( ) ;
	}

	bool StagedHotkey::isSpecified ( ) const
	{
		return _specify ;
	}
	bool StagedHotkey::isRegistered ( ) const
	{
		return canRegister ( ) && _enable ;
	}
	bool StagedHotkey::canRegister ( ) const
	{
		return _reg != NULL ;
	}

	void StagedHotkey::setCallbackObject ( const ISignal * newCallbackPt )
	{
		_lock.lock ( ) ;
		_callbackObjectPt.reset ( SignalFactory::copy ( newCallbackPt ) ) ;
		_callback = NULL ;
		_lock.unlock ( ) ;
	}

	void StagedHotkey::setCallback ( callback_t newCallback )
	{
		_lock.lock ( ) ;
		_callbackObjectPt.reset ( NULL ) ;
		_callback = newCallback ;
		_lock.unlock ( ) ;
	}
	const ISignal * StagedHotkey::getCallbackObject ( ) const
	{
		return _callbackObjectPt.get ( ) ;
	}
	StagedHotkey::callback_t StagedHotkey::getCallback ( ) const
	{
		return _callback ;
	}
	bool StagedHotkey::hasCallback ( ) const
	{
		return _callbackObjectPt.get ( ) != NULL || _callback != NULL ;
	}
	void StagedHotkey::setIntercept ( ISignal * interceptPt, const int interceptingMods )
	{
		StageContainer * typed = dynamic_cast < StageContainer * > ( interceptPt ) ;
		if ( typed == NULL ) return ;
		_lock.lock ( ) ;
		_stageContainer = * typed ;
		_lock.unlock ( ) ;
		unused ( interceptingMods ) ;
	}
	void StagedHotkey::setIntercept ( ISignal * interceptPt )
	{
		StageContainer * typed = dynamic_cast < StageContainer * > ( interceptPt ) ;
		if ( typed == NULL ) return ;
		_lock.lock ( ) ;
		_stageContainer = * typed ;
		_lock.unlock ( ) ;
	}
	void StagedHotkey::setInterceptMods ( const int interceptingMods )
	{
		unused ( interceptingMods ) ;
	}
	void StagedHotkey::setIntercept ( const std::string & interceptTypeName )
	{
		// Not stage container, no go.
		if ( ! containsAt ( interceptTypeName.c_str ( ), 0, StageContainer::Name, true ) )
			return ;
		_lock.lock ( ) ;
		_stageContainer = StageContainer ( ) ;
		_lock.unlock ( ) ;
	}
	const ISignal * StagedHotkey::getIntercept ( ) const
	{
		return & _stageContainer ;
	}
	int StagedHotkey::getInterceptMods ( ) const
	{
		return -1 ;
	}
	void StagedHotkey::clear ( )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_callbackObjectPt.reset ( NULL ) ;
		_callback = NULL ;
		// TODO reset intercept and interceptMods?
		_register ( ) ;
		_lock.unlock ( ) ;
	}
	void StagedHotkey::enable ( const bool enable )
	{
		_lock.lock ( ) ;
		_unregister ( ) ;
		_enable = enable ;
		_register ( ) ;
		_lock.unlock ( ) ;
	}
	bool StagedHotkey::isEnabled ( ) const
	{
		return _enable ;
	}
	void StagedHotkey::block ( const bool enable )
	{
		_lock.lock ( ) ;
		if ( enable )
		{
			switch ( _blockingStyle )
			{
			case UNMANAGED :
				break ;
			case NO_BLOCK :
				for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) ;
					i++ )
				{
					_stageContainer.stages [ i ]->enableBlocking ( false ) ;
				}
				break ;
			case BLOCK_FINAL :
				for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) - 1 ;
					i++ )
				{
					_stageContainer.stages [ i ]->enableBlocking ( false ) ;
				}
				( * _stageContainer.stages.rbegin ( ) )->enableBlocking ( true ) ;
				break ;
			case BLOCK_ALL :
				for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) ;
					i++ )
				{
					_stageContainer.stages [ i ]->enableBlocking ( true ) ;
				}
				break ;
			}
		}
		else
		{
			for ( unsigned int i = 0 ; i < _stageContainer.stages.size ( ) ;
				i++ )
			{
				_stageContainer.stages [ i ]->enableBlocking ( false ) ;
			}
		}
	}
	bool StagedHotkey::isBlocking ( ) const
	{
		return _blocking && _blockingStyle != NO_BLOCK ;
	}

	// ISignal impl
	std::string StagedHotkey::type ( ) const
	{
		return Name ;
	}

	void StagedHotkey::_sendThread ( )
	{
		// Separate thread, so we do locking.
		_lock.lock ( ) ;
		if ( _callbackObjectPt.get ( ) != NULL )
		{
			_callbackObjectPt->send ( ) ;
		}
		else if ( _callback != NULL )
		{
			_callback ( ) ;
		}
		_lock.unlock ( ) ;
	}
}
