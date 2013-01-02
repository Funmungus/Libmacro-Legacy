/*! \file macro.h
 * \brief Macro, When the macro's hotkey is triggered,
 * its set of signals is sent one after the other.
 */

# ifndef FUN_MACRO_H
# define FUN_MACRO_H

# include "macro/def.h"

namespace funlibrary
{
	//! \brief Superclass of all macros.
	class IMacro : public ISignal, public ICallNamedCString, public ICall < void >
	{
	public :
		//! \brief \ref ISignal::name
		static const std::string name ;

		IMacro ( std::string name = "" ) ;
		virtual ~IMacro ( ) { }

		//! \brief Get the user-defined name of this macro.
		std::string getName ( ) const ;
		//! \brief Set the user-defined name of this macro.
		void setName ( const std::string name ) ;
		//! \brief Get the hotkey used to trigger this macro.
		virtual void getHotkey
			( std::unique_ptr < IHotkey > * outVal ) const = 0 ;
		//! \brief Set the hotkey used to trigger this macro.
		virtual void setHotkey ( const IHotkey * inVal ) = 0 ;
		//! \brief True for being able to trigger the signal set.
		virtual bool isEnabled ( ) const = 0 ;
		//! \brief True for being able to trigger the signal set.
		virtual void enable ( const bool enable ) = 0 ;

		/*! \brief Receive a named C-string
		 * \ref ICallNamedCString::call(char *, char *)
		 */
		virtual void call ( char * name, char * value ) ;
		// Hotkey callback
		//! \brief \ref IHotkey::setCallbackObject(ICall<void>*)
		virtual void call ( ) = 0 ;
		/* Also inherits ISignal as pure virtual */
	private :
		std::string _name ;
	};

	//! \brief When the macro's hotkey is triggered,
	//! its set of signals is sent one after the other.
	class Macro : public IMacro, public ICallXmlNode
	{
	public :
		//! \brief \ref ISignal::name
		static const std::string name ;

		//! \brief ctor
		Macro ( std::string name = "", IHotkey * hotkey = NULL,
						std::vector < std::unique_ptr < ISignal > > * signalSet = NULL ) ;
		//! \brief dtor
		virtual ~Macro ( ) { }

		//! \brief outVal is populated with copies of this macro's signal set.
		void getSignals ( std::vector < std::unique_ptr < ISignal > > * outVal ) const ;
		//! \brief Set this macro's signal set as the same values from inVal.
		void setSignals ( const std::vector < std::unique_ptr < ISignal > > * inVal ) ;
		//! \brief Add one signal at index of pos.
		void setSignal ( const ISignal * val, const size_t pos ) ;
		//! \brief Remove the signal at pos.
		void removeSignal ( const size_t pos ) ;
		//! \brief Add a signal to the end of current set.
		void push_back ( const ISignal * val ) ;
		//! \brief Remove the signal at the end of current set.
		std::unique_ptr < ISignal > pop_back ( ) ;

		/* IMacro impl */
		//! \brief \ref IMacro#getHotkey(IHotkey*) const
		virtual void getHotkey
			( std::unique_ptr < IHotkey > * outVal ) const ;
		//! \brief \ref IMacro#setHotkey(const IHotkey *)
		virtual void setHotkey ( const IHotkey * inVal ) ;
		//! \brief \ref IMacro#isEnabled() const
		virtual bool isEnabled ( ) const ;
		//! \brief \ref IMacro#enable(const bool)
		virtual void enable ( const bool enable ) ;

//		virtual void call ( char *, char * ) ;
		// Hotkey callback
		//! \brief \ref IHotkey::cal()
		virtual void call ( ) ;
		/*! \brief Receive an xml node as an xml object.
		 * \ref ICallXmlNode::call(rapidxml::xml_node<char>*)
		 */
		virtual void call ( rapidxml::xml_node < char > * obj ) ;

		/* ISignal impl */		
		/*! \brief \ref ISignal#type() const */
		virtual std::string type ( ) const ;
		/*! \brief \ref ISignal#send() */
		virtual SIGNAL_API bool send ( ) ;
		/*! \brief \ref ISignal#xml() const */
		virtual SIGNAL_API std::string xml ( ) const ;
		/*! \brief \ref ISignal#serialize() const */
		virtual SIGNAL_API std::string serialize ( ) const ;
		/*! \brief \ref ISignal#deserialize(std::string, unsigned int) */
		virtual SIGNAL_API std::string deserialize
			( std::string container, unsigned int pos = 0 ) ;

	private :
		unique_ptr < IHotkey > _hotkey ;
		std::vector < unique_ptr < ISignal > > _signalSet ;
		bool _enable ;

		void setHotkey ( const Hotkey * inVal ) ;
		void setHotkey ( const StagedHotkey * inVal ) ;
	};

	//! \brief Create macro dynamically,
	//! from the name of a macro type.
	class MacroFactory
	{
	public :
		/*! \brief Get a new macro from the type name.
		 *
		 * The returned macro will be made with the new operator, or
		 * Null if no correct signal is found for the name.
		 * \param macroName This macro name should be the same as
		 * what is returned from its ISignal#type() function, and
		 * its public name variable.
		 */
		static HOTKEY_API IMacro * get ( std::string macroName ) ;
		/*! \brief Get a new macro of inVal's type, and copy inVal
		 * into the new macro.
		 *
		 * If no macro is found for this type, NULL is returned.
		 */
		static HOTKEY_API IMacro * copy ( const IMacro * inVal ) ;
	protected :
	private :
		static HotkeyFactory _instance ;
		HotkeyFactory ( ) ;
		typedef IMacro * (* MacroReturn) ( void ) ;
		typedef IMacro * (* CopyReturn) ( const IHotkey * ) ;
		// Map from keyString to keycode.
		static std::map < std::string, MacroReturn > _functionMap ;
		static std::map < std::string, CopyReturn > _copyMap ;

		static IMacro * getSimple ( ) ;
		static IMacro * copySimple ( const IMacro * inVal ) ;
		static IMacro * getStaged ( ) ;
		static IMacro * copyStaged ( const IMacro * inVal ) ;

		static void add ( std::string lowerName, HotkeyReturn func , CopyReturn cFunc) ;
	};
}

# endif

