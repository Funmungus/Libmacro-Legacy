/*! \file macro/macro.h
 * \brief Macro, When the macro's hotkey is triggered,
 * its set of signals is sent one after the other.
 */

# ifndef MCR_MACRO_H
# define MCR_MACRO_H

# include "macro/def.h"

namespace macro
{
	// ! \brief When the hotkey is triggered its set of signals is sent
	// ! \brief one after another.
	class Macro : public ISignal
	{
	public :
		// ! \brief \ref ISignal::name
		static const std::string name ;

		// ! \brief ctor
		Macro ( std::string name = "", IHotkey * hotkey = NULL,
				std::vector < std::unique_ptr < ISignal > > * signalSet = NULL,
				const bool enable = false, const bool sticky = false ) ;
		// ! \brief ctor
		Macro ( const Macro & copytron ) ;
		Macro & operator= ( const Macro & copytron ) ;
		// ! \brief dtor
		~Macro ( ) ;

		// ! \brief Get the user-defined name of this macro.
		std::string getName ( ) const ;
		// ! \brief Set the user-defined name of this macro.
		void setName ( const std::string & name ) ;
		// ! \brief Get the hotkey used to trigger this macro.
		void getHotkey
			( std::unique_ptr < IHotkey > * outVal ) const ;
		// ! \brief Set the hotkey used to trigger this macro.
		void setHotkey ( const IHotkey * inVal ) ;
		// ! \brief True for being able to trigger the signal set.
		bool isEnabled ( ) const ;
		// ! \brief True for being able to trigger the signal set.
		void enable ( const bool enable ) ;
		// ! \brief True if sending is repeated until interrupt is called.
		bool isSticky ( ) const ;
		// ! \brief Set with true to repeat sending until interrupt is called.
		void setSticky ( const bool sticky ) ;
		// ! \brief True if currently sending contained signal set.
		bool isRunning ( ) const ;
		// ! \brief If currently runnint, this will interrupt execution after
		// ! \brief the current signal is sent.
		void interrupt ( ) ;


		//
		// Signal Set
		//

		// ! \brief outVal is populated with copies of this macro's signal set.
		void getSignals ( std::vector < std::unique_ptr < ISignal > > * outVal ) const ;
		// ! \brief Set this macro's signal set as the same values from inVal.
		void setSignals ( const std::vector < std::unique_ptr < ISignal > > * inVal ) ;
		// ! \brief Add one signal at index of pos.
		void setSignals ( const ISignal * vals, const size_t len ) ;
		// ! \brief Remove the signal at pos.
		void removeSignal ( const size_t pos ) ;
		// ! \brief Add a signal to the end of current set.
		void push_back ( const ISignal * val ) ;
		// ! \brief Remove the signal at the end of current set.
		std::unique_ptr < ISignal > pop_back ( ) ;

		/* ISignal impl */
		/*! \brief \ref Macro::name */
		std::string type ( ) const ;
		/*! \brief Tag : \ref Macro::name
		*
		* \ref ISignal#xml ( ) const
		* Other tags : name, \ref ISignal#type ( ) const, set, enable, sticky
		* set child tags : \ref ISignal#type ( )
		 */
		std::string xml ( const std::string & extraAttributes = "" ) const ;
		// ! \brief \ref ISignal#receive ( const std::string &, const std::string )
		void receive ( const std::string & name, const std::string & value ) ;

		/*!
		* \brief send Send all signals in a set.
		*
		* Sends \ref Macro#sendCurrentThread ( ) in separate thread.
		 **/
		inline bool send ( )
		{
			std::thread trd ( & Macro::sendCurrentThread, this ) ;
			trd.detach ( ) ;
			return true ;
		}
		/*!
		* \brief sendCurrentThread Send all signals in a set.
		* If this macro is sticky, this will be repeated until
		* \ref Macro#interrupt ( )
		 */
		inline void sendCurrentThread ( )
		{
			_continuing = true ;
			while ( ! _triggerLock.try_lock ( ) )
			{
				if ( ! _continuing )
				{
					return ;
				}
			}
			// Now locked and continuing.
			if ( _sticky )
			{
				while ( _continuing )
				{
					trigger ( ) ;
					if ( ! _continuing )
					{
						_triggerLock.unlock ( ) ;
						return ;
					}
				}
			}
			else
				trigger ( ) ;

			_continuing = false ;
			_triggerLock.unlock ( ) ;
		}
	private :
		std::string _name ;
		std::unique_ptr < IHotkey > _hotkey ;
		std::vector < std::unique_ptr < ISignal > > _signalSet ;
		bool _enable ;
		bool _sticky ;

		bool _continuing ;
		std::mutex _triggerLock ;
		inline void trigger ( )
		{
			for ( auto it = _signalSet.begin ( ) ;
				it != _signalSet.end ( ) ; it++ )
			{
				if ( ! _continuing ) return ;
				( * it )->send ( ) ;
			}
		}
	} ;
}

# endif
