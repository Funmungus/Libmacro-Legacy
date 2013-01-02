
# ifndef FUN_IINTERCEPT_H
# define FUN_IINTERCEPT_H

# include "hotkey/def.h"
# include "signal/signal.h"

namespace funlibrary {
# ifdef LNX

	unsigned int eventState_index ( const int keyValue ) ;

	uint8_t eventState_bit ( const int keyValue ) ;

# endif

	template < class T >
	class IIntercept
	{
	public:
		/// <summary>
		/// An InterceptCallback which returns true if call is blocking.
		/// The parameter may be modified directly, changing the value returned
		/// to the system.  Changing the modifier has limited
		/// </summary>
		/// <returns>True to block this input from further use.  False to allow, and not block.</returns>
		typedef bool ( * InterceptCallback ) ( Modified < T > ) ;

		/// <summary>
		/// Activates or deactivates this intercept.
		/// </summary>
		/// <param name="enable">true to enable, otherwise disable</param>
		virtual void enable ( bool enable ) = 0 ;
		/// <summary>
		/// True if this intercept is enabled.
		/// </summary>
		virtual bool isEnabled ( ) const = 0 ;
		/// <summary>
		/// Read more details in Disable and Enable.  This does both.
		/// </summary>
		virtual void reset ( ) = 0 ;
		/// <summary>
		/// Add a non-blocking callback.  If intercepted is 0, will intercept all signals of type T.
		/// </summary>
		virtual void setCallback ( const InterceptCallback newCallback ) = 0 ;
		/// <summary>
		/// Removes all callbacks, and no longer blocks any input.
		/// </summary>
		virtual void release ( ) = 0 ;
	protected:
	private:
	};
}

# endif

