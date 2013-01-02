/*! \file serial.h
 * \brief Serialization functions for funlibrary.
 *
 * A serial item is defined as a string with the class name, followed by
 * SerialSeparator, followed by the data/value section, and
 * finally another SerialSeparator to end the object definition.
 * 
 */

# ifndef FUN_SERIAL_H
# define FUN_SERIAL_H

# include "util/funstrings.h"

namespace funlibrary
{
	/*!
	 * \brief SerialSeparator is the character separation between items,
	 * as well as between name and values.
	 */
	extern char SerialSeparator ;

	/*! \brief type/name + SignalSeparator */
	UTIL_API std::string serialHeader ( std::string name ) ;

	/*! \brief Get a data node, after a SerialSeparator
	 *
	 * Format in container should be name + SerialSeparator + data + SerialSeparator,
	 * without spaces.
	 * \param container The string which contains the serialized object.
	 * \param name Name or type of this object in container.
	 * \param pos The position in container to start searching.
	 */
	UTIL_API std::string dataNodeFor ( std::string container,
		std::string name, unsigned int pos ) ;

	/*! \brief Get position for the data node associated with a serialized object.
	 *
	 * Format in container should be name + SerialSeparator + data + SerialSeparator,
	 * without spaces.
	 * \param container The string which contains the serialized object.
	 * \param name Name or type of this object in container.
	 * \param pos The position in container to start searching.
	 */
	UTIL_API unsigned int dataPositionFor ( std::string container,
		std::string name, unsigned int pos ) ;

	/*! \brief Get the subsection ending with SerialSeparator
	 */
	UTIL_API std::string nodeAt ( std::string container,
		unsigned int pos ) ;

	/*! \brief Get the position of the next serialized object node.
	 *
	 * \param container The string which contains the serialized object.
	 * \param pos The position in container to start searching.
	 * \return The position of first letter of the name/type of the next
	 * serialized object in container.
	 */
	UTIL_API unsigned int nextNodePosition ( std::string container,
		unsigned int pos ) ;

}

# endif

