/*! \file hotkey/hotkey.h
 * \brief Hotkey, A simple hotkey that activates with one set of Mods and one additional signal.
 */

# ifndef MCR_SIMPLEHOTKEY_H
# define MCR_SIMPLEHOTKEY_H

# include "hotkey/hot.h"
# include "hotkey/keymod.h"
# include "hotkey/signalmod.h"

// We currently have no reason to subclass the Hotkey object.
namespace macro
{
	typedef IHotkey * ( * hotkey_return_t ) ( ) ;
	// ! \brief getObject function for signal name, which becomes Hot<T> for named signal type.
	extern std::map<std::string, hotkey_return_t> HotMap ;
	// ! \brief Use HotMap to get a new Hot object, or NULL if none found.
	inline std::shared_ptr < IHotkey > hotFromSignal
		( const std::string & signalName )
	{
		auto it = HotMap.find ( signalName ) ;
		if ( it == HotMap.end ( ) ) return std::shared_ptr < IHotkey > ( nullptr ) ;
		return std::shared_ptr < IHotkey > ( it->second ( ) ) ;
	}
	// TODO remove, in favor of SDK template structure.
	std::map<std::string, hotkey_return_t> defaultHotMap ( ) ;
	// ! \brief A simplified hotkey that activates with one set of Mods and one additional signal.
	class Hotkey : public IHotkey, public ISignal
	{
	public :
		// ! \brief ctor
		MCR_API Hotkey ( const ISignal * callbackPt = NULL, ISignal * interceptPt = NULL,
						const int interceptMods = 0, const bool enable = false,
						const bool specify = true, const bool blocking = true ) ;
		// ! \brief ctor with static function callback.
		MCR_API Hotkey ( void ( * callback ) ( ), ISignal * interceptPt = NULL,
						const int interceptMods = 0, const bool enable = false,
						const bool specify = true, const bool blocking = true ) ;
		// ! \brief ctor with ITrigger callback object.
		MCR_API Hotkey ( ITrigger * callbackPt, ISignal * interceptPt = NULL,
						const int interceptMods = 0, const bool enable = false,
						const bool specify = true, const bool blocking = true ) ;
		// ! \brief ctor with a hot container.
		MCR_API Hotkey ( const std::shared_ptr<IHotkey> & hotty ) ;
		// ! \brief copy ctor
		MCR_API Hotkey ( const Hotkey & copytron ) ;
		// ! \brief copy assignment
		MCR_API Hotkey & operator= ( const Hotkey & copytron ) ;
//		// ! \brief dtor
		// Edit: Currently there is no reason to subclass Hotkey.
//		virtual ~Hotkey ( ) { }

		//
		// Hotkey specific
		//
		// ! \brief Get the internal representation of this hotkey.
		MCR_API const std::shared_ptr<IHotkey > get ( ) const ;
		// ! \brief Set all values of this hotkey, from any IHotkey.
		MCR_API void set ( const std::shared_ptr<IHotkey> & newHotkey ) ;

		//
		// IHotkey Triggering
		//
		// ! \brief \ref IHotkey#trigger ( ISignal &, Mods & )
		inline bool trigger ( ISignal * interceptedPt = NULL,
							int * modsPt = NULL ) ;

		//
		// Functions to modify how registering is handled.
		//
		// ! \brief \ref IHotkey#specify ( const bool )
		MCR_API void specify ( const bool useSpecific ) ;
		// ! \brief \ref IHotkey#specify ( ) const
		MCR_API bool isSpecified ( ) const ;
//		// ! \brief \ref IHotkey#isRegistered ( ) const
//		MCR_API bool isRegistered ( ) const ;
//		// ! \brief \ref IHotkey#canRegister ( ) const
//		MCR_API bool canRegister ( ) const ;

		//
		// Functions to modify how signals are intercepted ( This is logically after hotkeys are registered ).
		//
		// ! \brief \ref IHotkey#setCallbackObject ( const ISignal * )
		MCR_API void setCallbackObject ( const ISignal * newCallbackPt ) ;
		// ! \brief \ref IHotkey#getCallbackObject ( )
		MCR_API const ISignal * getCallbackObject ( ) const ;
		// ! \brief \ref IHotkey#setCallback ( void ( * ) ( ) )
		MCR_API void setCallback ( callback_t newCallback ) ;
		// ! \brief \ref IHotkey#getCallback ( )
		MCR_API callback_t getCallback ( ) const ;
		// ! \brief \ref IHotkey#setTriggerObject ( const ITrigger * )
		MCR_API void setTriggerObject ( ITrigger * newCallbackPt ) ;
		// ! \brief \ref IHotkey#getTriggerObject ( ) const
		MCR_API const ITrigger * getTriggerObject ( ) const ;
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
		MCR_ISIGNAL_COMMONS ( Hotkey )
		/*! \brief Trigger enclosed hotkey with no parameters. */
		inline bool send ( ) ;

//		MCR_EQUALITY_OPERATORS ( Hotkey )
	private :
		// Since we are using only one object, which should lock itself,
		// a shared pointer is allowed to modify object outside. This means
		// which pointer currently contained should be tracked in the client
		// classes.
		std::shared_ptr < IHotkey > _hot ;
		//std::unique_ptr < ISignal > _callbackObjPt ;
		//IHotkey::callback_t _callback ;
		//bool _specify, _enable, _blocking ;

		inline void _ensureMember ( ) ;
	} ;
}

# include "hotkey/inline/simplehotkey.h"

# endif
