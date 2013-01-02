/*! \file signal.h
 * \brief SignalFactory, and functions to receive sets of signals from xml and serial strings.
 */

# ifndef FUN_SIGNAL_H
# define FUN_SIGNAL_H

# include "signal/keysignal.h"
# include "signal/spacesignal.h"
# include "signal/mods.h"
# include "signal/scriptsignal.h"
# include "signal/comsignal.h"
# include "signal/noop.h"
# include "signal/keystring.h"

namespace funlibrary
{

	/*! \brief How to create a set of signals from xml.
	 * 
	 * To create a set of signals... <br>
	 * Firstly, one must have an xml C-string (char *), and a vector of DPointers to contain ISignals. <br>
	 * Both objects are referenced as pointers.  If either object is NULL, gimmeSiggy will do nothing. <br>
	 * Both references will be modified, and cannot be constant. <br>
	 * Items added to siggySet will be dynamically allocated ISignal objects. DPointers delete automatically on destruction, so items are added with the "new" operator. <br>
	 */
	SIGNAL_API void usage_xmlSignalSet ( ) ;

	/*! \brief Put in xml string all_signal_xml, and siggySet will have signals added
	 * to it from the xml string.
	 */
	SIGNAL_API void xmlToSiggy ( char * all_signal_xml,
		std::vector < std::unique_ptr < ISignal > > * siggySet ) ;

	/*! \brief Put in serial string all_signal_serial, and siggySet will have signals added
	 * to it from the serial string.
	 */
	SIGNAL_API void serialToSiggy ( std::string all_signal_serial,
		std::vector < std::unique_ptr < ISignal > > * siggySet ) ;

}

# endif

