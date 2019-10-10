/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*! \file
 *  \brief Register \ref mcr_Interface instances with ids and names.
 */

#ifndef MCR_UTIL_REGISTRY_H_
#define MCR_UTIL_REGISTRY_H_

#include "mcr/util/interface.h"
#include "mcr/util/map.h"
#include "mcr/util/string_set.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! For a given category of interfaces, register an id and name
 *  for each interface definition.
 */
struct mcr_IRegistry {
	/*! Each index is a pointer to an interface definition.
	 *  The id of that interface should also be the same index. */
	struct mcr_Array iset;
	/*! The index of this array is the id of an interface.
	 *  Every element is a \ref mcr_String, which is the name of
	 *  that interface. */
	mcr_StringSet names;
	/*! Map from name to interface pointer, many names
	 *  may map to the same pointer.
	 *
	 *  \ref mcr_name_compare */
	struct mcr_Map name_map;
};

/*! \ref mcr_IRegistry ctor
 *
 *  \param regPt \ref mcr_IRegistry * \ref opt
 *  \return 0
 */
MCR_API int mcr_reg_init(void *regPt);
/*! \ref mcr_IRegistry ctor
 *
 *  \return Empty registry
 */
MCR_API struct mcr_IRegistry mcr_reg_new();
/*! \ref mcr_IRegistry dtor
 *
 *  \param regPt \ref mcr_IRegistry * \ref opt
 *  \return 0
 */
MCR_API int mcr_reg_deinit(void *regPt);
/*! Register a new interface.  The interface id will be
 *  set as the next possible id to be registered.  If names are provided, also
 *  \ref mcr_reg_set_name, \ref mcr_reg_add_names
 *
 *  \param interfacePt The new interface to register
 *  \param name \ref opt Name to map to and from newType
 *  \param addNames \ref opt Set of additional names to map to newType
 *  \param bufferLen Number of names to map in addNames,
 *  required if addNames is to be used
 *  \return \ref reterr
 */
MCR_API int mcr_register(struct mcr_IRegistry *iRegPt,
						 void *interfacePt, const char *name, const char **addNames,
						 size_t bufferLen);
/*! Get a reference to the interface of the same id.
 *
 *  \param iRegPt \ref opt
 *  \param typeId Id of interface
 *  \return Interface of given id, or null if not found
 */
MCR_API void *mcr_reg_from_id(const struct mcr_IRegistry *iRegPt,
							  size_t typeId);
/*! Get a reference to the interface from its registered name.
 *
 *  \param iRegPt \ref opt
 *  \param typeName \ref opt Name of interface
 *  \return Interface of given name, or null if not found
 */
MCR_API void *mcr_reg_from_name(const struct mcr_IRegistry *iRegPt,
								const char *typeName);
/*! Get the name of the interface of given id, or null if
 *  not found.
 *
 *  \param iRegPt \ref opt
 *  \param id Id of the interface
 *  \return Name of the interface, or null if not found
 */
MCR_API const char *mcr_reg_name(const struct mcr_IRegistry *iRegPt,
								 size_t id);
/*! Map a name to an interface, and the interface to that name.
 *
 *  \param interfacePt \ref opt Interface to map
 *  \param name \ref opt Name to map
 *  \return \ref reterr
 */
MCR_API int mcr_reg_set_name(struct mcr_IRegistry *iRegPt,
							 void *interfacePt, const char *name);
/*! Add a mapping from name to interface.
 *
 *  \param interfacePt Interface to map to
 *  \param name \ref opt Name to map from
 *  \return \ref reterr
 */
MCR_API int mcr_reg_add_name(struct mcr_IRegistry *iRegPt,
							 void *interfacePt, const char *name);
/*! \ref mcr_reg_add_name for all given names
 *
 *  \param interfacePt Interface to map to
 *  \param names \ref opt Array of names to map from
 *  \param bufferLen Number of elements to use in names
 *  \return \ref reterr
 */
MCR_API int mcr_reg_add_names(struct mcr_IRegistry *iRegPt,
							  void *interfacePt, const char **names, size_t bufferLen);
/*! \ref mcr_reg_set_name and \ref mcr_reg_add_names
 *
 *  \param interfacePt Interface to map
 *  \param name \ref opt Name to map
 *  \param names \ref opt Array of names to map from
 *  \param bufferLen Number of elements to use in names
 *  \return \ref reterr
 */
MCR_API int mcr_reg_set_names(struct mcr_IRegistry *iRegPt,
							  void *interfacePt, const char *name, const char **names,
							  size_t bufferLen);
/*! Move an interface from one name to another.
 *
 *  \param oldName \ref opt Remove mapping from this name.
 *  \param newName \ref opt Add mapping from this name.
 *  \return \ref reterr
 */
MCR_API int mcr_reg_rename(struct mcr_IRegistry *iRegPt, const char *oldName,
						   const char *newName);
/*! Get the number of registered interfaces.
 *
 *  \param iRegPt \ref opt
 *  \return \ref retind
 */
MCR_API size_t mcr_reg_count(const struct mcr_IRegistry *iRegPt);
/*! Minimize allocation */
MCR_API void mcr_reg_trim(struct mcr_IRegistry *iRegPt);
/*! Remove all registered interfaces. */
MCR_API void mcr_reg_clear(struct mcr_IRegistry *iRegPt);

#ifdef __cplusplus
}
#endif

#endif
