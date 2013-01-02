/*! \file hotkey.h
 * \brief Hotkey, A simple hotkey that activates with one set of Mods and one additional signal.
 * StagedHotkey, A complex hotkey that activates after a set of combinations
 * have been activated.
 */

# ifndef FUN_HOTKEY_H
# define FUN_HOTKEY_H

# include "hotkey/keyintercept.h"
# include "hotkey/spaceintercept.h"

namespace funlibrary
{
	//! \brief TODO This may be removed.
	enum InterceptType
	{
		NONE = 0, KEY = 1, SPACE = 2
	};

	//! \brief Hotkey base intercept.
	class IHotkey
	{
	public :
		//! \brief hotkey, subclasses should also implement this with their own type.
		//! Used to describe a type of hotkey.
		static const std::string name ;
		virtual ~IHotkey ( ) { }
		//! \brief Get \ref IHotkey::name
		virtual std::string type ( ) const = 0 ;
		//! \brief Set callback as an \ref ICall<void> object.
		virtual void setCallbackObject ( ICall < void > * newCallback ) = 0 ;
		//! \brief Set callback as a static function with no
		//! parameters or out value.
		virtual void setCallback ( void (* newCallback) () ) = 0 ;
		//! \brief Free any pointers.  This should not disable.
		virtual void clear ( ) = 0 ;
		//! \brief Enable will activate this object in the interceptors.
		virtual void enable ( const bool enable ) = 0 ;
		//! \brief If enabled, this object is activated in the interceptors.
		virtual bool isEnabled ( ) const = 0 ;
		//! \brief Activate blocking of captured signals.
		virtual void setBlocking ( const bool enable ) = 0 ;
		//! \brief If blocking, this object drops captured signals from passing
		//! to other functions and objects.
		virtual bool isBlocking ( ) const = 0 ;
	};

	//! \brief A simple hotkey that activates with one set of Mods and one additional signal.
	class Hotkey : public IHotkey, public ICall<bool, KeySignal*, Mods*>,
			public ICall<bool, SpaceSignal*, Mods*>
	{
	public :
		//! \brief \ref IHotkey::name
		static const std::string name ;

		//! \brief ctor with ICall object callback.
		Hotkey ( ICall < void > *callback = NULL, const ISignal * intercept = NULL,
						 const Mods & interceptMods = Mods ( ), const bool enable = false,
						 const bool blocking = true ) ;
		//! \brief ctor with static function callback.
		Hotkey ( void (*callback) ( ), const ISignal * intercept = NULL,
						 const Mods & interceptMods = Mods ( ), const bool enable = false,
						 const bool blocking = true ) ;
		Hotkey ( const Hotkey & copytron ) ;
		Hotkey & operator= ( const Hotkey & copytron ) ;
		virtual ~Hotkey ( ) { }
		// TODO currently cannot setIntercept from ctor

		// IHotkey impl
		//! \brief \ref IHotkey::type() const
		virtual std::string type ( ) const ;
		//! \brief \ref IHotkey::setCallbackObject(ICall<void>*)
		virtual void setCallbackObject ( ICall < void > * newCallback ) ;
		//! \brief \ref IHotkey::setCallback(void(*)())
		virtual void setCallback ( void (* newCallback) () ) ;
		//! \brief \ref IHotkey::clear()
		virtual void clear ( ) ;
		//! \brief \ref IHotkey::enable(const bool)
		virtual void enable ( const bool enable ) ;
		//! \brief \ref IHotkey::isEnabled() const
		virtual bool isEnabled ( ) const ;
		//! \brief \ref IHotkey::setBlocking(const bool)
		virtual void setBlocking ( const bool blocking ) ;
		//! \brief \ref IHotkey::isBlocking() const
		virtual bool isBlocking ( ) const ;

		// ICall impl
		//! \brief \ref KeyIntercept::add(ICall<bool, KeySignal*, Mods*>* )
		virtual bool call ( KeySignal * intercepted, Mods * mod ) ;
		//! \brief \ref KeyIntercept::add(ICall<bool, SpaceSignal*, Mods*>* )
		virtual bool call ( SpaceSignal * intercepted, Mods * mod ) ;

		// Simple Hotkey specific.
		//! \brief Set the signal and mods to intercept and activate this hotkey.
		void setIntercept ( const ISignal * intercept, const Mods & interceptMods ) ;
		//! \brief Set only the signal to activate this hotkey.  Mods stay the same.
		void setIntercept ( const ISignal * intercept ) ;
		//! \brief Set only the mods to activate this hotkey.  Signal stays the same.
		void setInterceptMod ( const Mods & interceptMods ) ;
		/*! \brief Get the signal and mod combo being used to activate this hotkey.
		 *
		 * \param interceptedSignal The intercepting signal is copied
		 * to this pointer.
		 * \param interceptedMods The intercepting mods are copied
		 * to this pointer.
		 */
		void getIntercept ( std::unique_ptr < ISignal > * interceptedSignal,
				Mods * interceptedMods = NULL ) const ;

		//! \brief Internal usage, but publicly available.
		void setIntercept ( const KeySignal & intercept, const Mods & interceptMods ) ;
		//! \brief Internal usage, but publicly available.
		void setIntercept ( const SpaceSignal & intercept, const Mods & interceptMods ) ;
		//! \brief Internal usage, but publicly available.
		void setIntercept ( const KeySignal & intercept ) ;
		//! \brief Internal usage, but publicly available.
		void setIntercept ( const SpaceSignal & intercept ) ;
		//! \brief Internal usage, but publicly available.
		InterceptType getInterceptType ( ) const ;
	private :
		ICall < void > * _callObj ;
		void (* _callFunc) ( ) ;
		std::unique_ptr < ISignal > _intercept ;
		Mods _interceptMod ;
		bool _enable ;
		bool _blocking ;
		InterceptType _interceptType ;

		void add () ;
		void remove () ;
	};

	//! \brief Stages of activation, for StagedHotkey.
	struct HotkeyStage
	{
		//! \brief \ref IHotkey::name
		static const std::string name ;

		//! \brief The signal to intercept for this to activate.
		std::unique_ptr < ISignal > intercepting ;
		//! \brief The mods to intercept for this to activate.
		Mods interceptingMods ;
		//! \brief Whether this stage is activated or not.
		bool activated ;
		//! \brief If activated, and blocking, that activating stignal will
		//! be nullified.
		bool blocking ;

		//! \brief ctor
		HotkeyStage ( const ISignal * intercepting = NULL, const Mods & interceptingMods = Mods ( ),
									const bool activated = false, const bool blocking = false ) ;

		HotkeyStage ( const HotkeyStage & copytron ) ;

		HotkeyStage & operator= ( const HotkeyStage & rhs ) ;
/*
Table of outcome for incoming key, assuming either key or scan are the same
currently active	my KeyUpType	incoming KeyUpType
y			D		D			o
y			D		U			o
y			D		B			o
y			U		D			o
y			U		U			o
y			U		B			o
n			D		D			o
n			D		U			x
n			D		B			o
n			U		D			x
n			U		U			o
n			U		B			o
*/		
		//! \brief Return true if this is the correct combo.
		//! \ref HotkeyStage#activated will be set to the same value.
		bool isMe ( KeySignal * key, Mods * mod ) ;
		//! \brief Return true if this is the correct combo.
		//! \ref HotkeyStage#activated will be set to the same value.
		bool isMe ( SpaceSignal * space, Mods * mod ) ;
	};

	//! \brief Style of setting blocked value of all stages in the set of stages.
	enum BlockingStyle
	{
		//! \brief No effect.  Every stage must be set manually.
		UNMANAGED = 0,
		//! \brief Do not block any.
		NO_BLOCK = 1,
		//! \brief Block only the last signal.
		BLOCK_FINAL = 2,
		//! \brief Block every signal that activates a stage.
		BLOCK_ALL = 3
	};

	//! \brief A complex hotkey that activates after a set of combinations
	//! have been activated.
	class StagedHotkey : public IHotkey, public ICall<bool, KeySignal*, Mods*>,
			public ICall<bool, SpaceSignal*, Mods*>
	{
	public :
		//! \brief \ref IHotkey::name
		static const std::string name ;

		//! \brief ctor
		StagedHotkey ( ICall < void > *callback = NULL, const std::vector < HotkeyStage > & stages = std::vector < HotkeyStage > ( ),
									 const bool enable = false, const BlockingStyle style = BLOCK_FINAL ) ;
		//! \brief ctor
		StagedHotkey ( void (*callback) ( ), const std::vector < HotkeyStage > & stages = std::vector < HotkeyStage > ( ),
									 const bool enable = false, const BlockingStyle style = BLOCK_FINAL ) ;
		StagedHotkey ( const StagedHotkey & copytron ) ;
		StagedHotkey & operator= ( const StagedHotkey & copytron ) ;
		virtual ~StagedHotkey ( ) { }

		// IHotkey impl
		//! \brief \ref IHotkey::type() const
		virtual std::string type ( ) const ;
		//! \brief \ref IHotkey::setCallbackObject(ICall<void>*)
		virtual void setCallbackObject ( ICall < void > * newCallback ) ;
		//! \brief \ref IHotkey::setCallback(void(*)())
		virtual void setCallback ( void (* newCallback) () ) ;
		//! \brief \ref IHotkey::clear()
		virtual void clear ( ) ;
		//! \brief \ref IHotkey::enable(const bool)
		virtual void enable ( const bool enable ) ;
		//! \brief \ref IHotkey::isEnabled() const
		virtual bool isEnabled ( ) const ;
		//! \brief \ref IHotkey::setBlocking(const bool)
		virtual void setBlocking ( const bool blocking ) ;
		//! \brief \ref IHotkey::isBlocking() const
		virtual bool isBlocking ( ) const ;

		// ICall impl
		/*! \brief Receive intercepted KeySignal and Mods.
		 * \ref ICall<KeySignal *,Mods *>::call(KeySignal *, Mods *)
		 */
		virtual bool call ( KeySignal * intercepted, Mods * mod ) ;
		/*! \brief Receive intercepted SpaceSignal and Mods.
		 * \ref ICall<SpaceSignal *,Mods *>::call(SpaceSignal *, Mods *)
		 */
		virtual bool call ( SpaceSignal * intercepted, Mods * mod ) ;

		// Staged Hotkey specific.
		//! \brief Get the current style of blocking activating signals.
		BlockingStyle getBlockStyle ( ) const ;
		//! \brief Set a new style to block activating signals
		void setBlockStyle ( const BlockingStyle style ) ;
		//! \brief Get the current set of HotkeyStages, including activation states.
		std::vector < HotkeyStage > getStages ( ) const ;
		//! \brief Set a new set of HotkeyStages.  Activated states will be kept.
		void setStages ( const std::vector < HotkeyStage > & stageIn ) ;
		//! \brief Get the number of HotkeyStages.
		size_t stageLength ( ) const ;
		//! \brief Get one stage from its index.
		HotkeyStage getStage ( const size_t index ) const ;
		//! \brief Set one stage at given index.
		void setStage ( const HotkeyStage & stage, const size_t index ) ;
		//! \brief Add a stage to the back of the set.
		void addStage ( const HotkeyStage & stage ) ;
		//! \brief Remove a stage from given index.
		void removeStage ( const size_t index ) ;
	private :
		ICall < void > * _callObj ;
		void (* _callFunc) ( ) ;
		std::vector < HotkeyStage > _stages ;
		bool _enable ;
		BlockingStyle _blockingStyle ;

		void add ( ) ;
		void remove ( ) ;
		bool hasKey ( ) const ;
		bool hasSpace ( ) const ;
		void trigger ( ) ;
	};

	//! \brief Create hotkeys dynamically, from the name of a hotkey type.
	class HotkeyFactory
	{
	public :
		/*! \brief Get a new hotkey from the type name.
		 *
		 * The returned hotkey will be made with the new operator, or
		 * Null if no correct signal is found for the name.
		 * \param hotkeyName This hotkey name should be the same as
		 * what is returned from its IHotkey#type() function, and
		 * its public name variable.
		 */
		static HOTKEY_API IHotkey * get ( std::string hotkeyName ) ;
		/*! \brief Get a new hotkey of inVal's type, and copy inVal
		 * into the new hotkey.
		 *
		 * If no hotkey is found for this type, NULL is returned.
		 */
		static HOTKEY_API IHotkey * copy ( const IHotkey * inVal ) ;
	protected :
	private :
		static HotkeyFactory _instance ;
		HotkeyFactory ( ) ;
		typedef IHotkey * (* HotkeyReturn) ( void ) ;
		typedef IHotkey * (* CopyReturn) ( const IHotkey * ) ;
		// Map from keyString to keycode.
		static std::map < std::string, HotkeyReturn > _functionMap ;
		static std::map < std::string, CopyReturn > _copyMap ;

		static IHotkey * getSimple ( ) ;
		static IHotkey * copySimple ( const IHotkey * inVal ) ;
		static IHotkey * getStaged ( ) ;
		static IHotkey * copyStaged ( const IHotkey * inVal ) ;

		static void add ( std::string lowerName, HotkeyReturn func , CopyReturn cFunc) ;
	};
}

# endif
