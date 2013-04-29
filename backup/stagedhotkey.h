/*! \file hotkey/stagedhotkey.h
 * \brief StagedHotkey, A complex hotkey that activates after a set of combinations
 * have been activated.
 */

# ifndef MCR_STAGEDHOTKEY_H
# define MCR_STAGEDHOTKEY_H

# include "hotkey/hot.h"
# include "hotkey/stage.h"

namespace macro
{
	// ! \brief Style of setting blocked value of all stages in the set of stages.
	enum BlockingStyle
	{
		// ! \brief No effect. Every stage must be set manually.
		UNMANAGED = 0,
		// ! \brief Do not block any.
		NO_BLOCK = 1,
		// ! \brief Block only the last signal.
		BLOCK_FINAL = 2,
		// ! \brief Block every signal that activates a stage.
		BLOCK_ALL = 3
	} ;

	// ! \brief A hotkey that activates after a set of combinations
	// ! have been activated.
	class StagedHotkey : public IHotkey, public ISignal
	{
	public :
		//
		// StagedHotkey types, statics, and public members.
		//
		typedef void ( * reg_t ) ( StagedHotkey & ) ;
		typedef void ( * unreg_t ) ( StagedHotkey & ) ;
		typedef bool ( * dispatch_t ) ( StagedHotkey & ) ;

		/*! DefaultRegistrar
		 * The reg_t given to all new objects.
		 */
		static reg_t DefaultRegistrar ;
		/*! DefaultUnregistrar
		 * The unreg_t given to all new objects.
		 */
		static unreg_t DefaultUnregistrar ;

		// ! \brief ctor, callback object, stages, enable, blocking style, specification
		StagedHotkey ( const ISignal * callbackPt = NULL,
					const StageContainer::stage_vector_t & stages =
						StageContainer::stage_vector_t ( ),
					const bool enable = false, const bool specify = true,
					const BlockingStyle style = BLOCK_FINAL ) ;
		// ! \brief ctor, callback function, stages, enable, blocking style, specification
		StagedHotkey ( IHotkey::callback_t callback,
					const StageContainer::stage_vector_t & stages =
						StageContainer::stage_vector_t ( ),
					const bool enable = false, const bool specify = true,
					const BlockingStyle style = BLOCK_FINAL ) ;
		StagedHotkey ( const StagedHotkey & copytron ) ;
		StagedHotkey & operator= ( const StagedHotkey & copytron ) ;
		StagedHotkey & operator= ( const StageContainer & newStages ) ;
		StagedHotkey & operator= ( const StageContainer::stage_vector_t & newStages ) ;
		~StagedHotkey ( ) { }

		// ! \brief Get this object's registrar.
		reg_t getRegistrar ( ) const ;
		// ! \brief Set this object's registrar.
		void setRegistrar ( reg_t reg ) ;
		// ! \brief Get this object's unregistrar.
		unreg_t getUnregistrar ( ) const ;
		// ! \brief Set this object's unregistrar.
		void setUnregistrar ( unreg_t reg ) ;

		// ! \brief Get the current style of blocking activating signals.
		BlockingStyle getBlockStyle ( ) const ;
		// ! \brief Set a new style to block activating signals
		void setBlockStyle ( const BlockingStyle style ) ;
		// ! \brief Get the current set of Stages.
		StageContainer getStages ( ) const ;
		// ! \brief Set a new set of HotkeyStages. Activated states will be kept.
		void setStages ( const StageContainer & stageIn ) ;
		// ! \brief Get the current set of Stages.
		StageContainer::stage_vector_t getStages ( ) const ;
		// ! \brief Set a new set of HotkeyStages. Activated states will be kept.
		void setStages ( const StageContainer::stage_vector_t & stageIn ) ;

		//
		// IHotkey Triggering
		//
		// ! \brief \ref IHotkey#trigger ( ISignal &, Mods & )
		inline bool trigger ( ISignal * interceptedPt = NULL, int * modsPt = NULL ) ;

		//
		// Functions to modify how registering is handled.
		//
		// ! \brief \ref IHotkey#specify ( const bool )
		MCR_API void specify ( const bool useSpecific ) ;
		// ! \brief \ref IHotkey#specify ( ) const
		MCR_API bool isSpecified ( ) const ;
		// ! \brief \ref IHotkey#isRegistered ( ) const
		MCR_API bool isRegistered ( ) const ;
		// ! \brief \ref IHotkey#canRegister ( ) const
		MCR_API bool canRegister ( ) const ;

		//
		// Functions to modify how signals are intercepted ( This is logically after hotkeys are registered ).
		//
		// ! \brief \ref IHotkey#setCallbackObject ( const ISignal * )
		MCR_API void setCallbackObject ( const ISignal * newCallbackPt ) ;
		// ! \brief \ref IHotkey#setCallback ( void ( * ) ( ) )
		MCR_API void setCallback ( callback_t newCallback ) ;
		// ! \brief \ref IHotkey#getCallbackObject ( )
		MCR_API const ISignal * getCallbackObject ( ) const ;
		// ! \brief \ref IHotkey#getCallback ( )
		MCR_API callback_t getCallback ( ) const ;
		// ! \brief \ref IHotkey#hasCallback ( ) const
		MCR_API bool hasCallback ( ) const ;
		// ! \brief \ref IHotkey#setIntercept ( ISignal *, const int )
		MCR_API void setIntercept ( ISignal * interceptPt, const int interceptingMods ) ;
		// ! \brief \ref IHotkey#setIntercept ( ISignal * )
		MCR_API void setIntercept ( ISignal * interceptPt ) ;
		// ! \brief \ref IHotkey#setInterceptMods ( const int )
		MCR_API void setInterceptMods ( const int interceptingMods ) ;
		// ! \brief \ref IHotkey#setIntercept ( const std::string & )
		MCR_API void setIntercept ( const std::string & interceptTypeName = "" ) ;
		// ! \brief \ref IHotkey#getIntercept ( ) const
		MCR_API const ISignal * getIntercept ( ) const ;
		// ! \brief \ref IHotkey#getInterceptMods ( ) const
		MCR_API int getInterceptMods ( ) const ;
		// ! \brief \ref IHotkey#clear ( )
		MCR_API void clear ( ) ;
		// ! \brief \ref IHotkey# ( const bool )
		MCR_API void enable ( const bool enable ) ;
		// ! \brief \ref IHotkey#isEnabled ( ) const
		MCR_API bool isEnabled ( ) const ;
		// ! \brief \ref IHotkey#setBlocking ( const bool )
		MCR_API void block ( const bool enable ) ;
		// ! \brief \ref IHotkey#isBlocking ( ) const
		MCR_API bool isBlocking ( ) const ;

		// ISignal impl
		/*! \brief \ref ISignal#type ( ) const */
		MCR_API std::string type ( ) const ;
		/*! \brief Trigger enclosed hotkey with no parameters. */
		inline bool send ( ) ;
		// ! \brief \ref ISignal#operator ( ) ( )
		inline bool operator ( ) ( ) ;
		// ! \brief \ref ISignal#dispatch ( )
		inline bool dispatch ( ) ;
	private :
		// reg
		reg_t _reg ;
		unreg_t _unreg ;
		// callback
		std::unique_ptr<ISignal> _callbackObjectPt ;
		callback_t _callback ;
		// intercept modification
		StageContainer _stageContainer ;
		BlockingStyle _blockingStyle ;
		bool _enable, _specify, _blocking ;
		std::mutex _lock ;

		void _sendThread ( ) ;
		inline void _register ( ) ;
		inline void _unregister ( ) ;
	} ;
}

# include "hotkey/inline/stagedhotkey.h"

# endif
