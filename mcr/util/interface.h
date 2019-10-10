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
 *  \brief \ref mcr_Interface Interface to object references
 *  \ref mcr_Data Object reference
 *
 *  Interface definitions are typecasted, data definitions are not.
 */

#ifndef MCR_UTIL_INTERFACE_H_
#define MCR_UTIL_INTERFACE_H_

#include "mcr/util/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! An object reference that may be heap memory to deallocate
 *
 *  \ref mcr_Data_deinit\n
 *  If data is allocated with \ref malloc, set deallocate to
 *  \ref free.  If allocated with new, set deallocate to a function that
 *  will delete the data reference.
 */
struct mcr_Data {
	/*! Object reference */
	void *data;
	/*! If set, this will be called on data in \ref mcr_Data_deinit
	 *
	 *  If data is allocated with C++ new or new[] operators, please
	 *  create functions to delete correctly. */
	void (*deallocate) (void *);
};

/*! A base interface type to objects of a specific type */
struct mcr_Interface {
	/*! The unique identifier for this object type,
	 *  -1 for unregistered types */
	size_t id;
	/*! Context reference */
	struct mcr_context *context;
	/*! Byte size of an object of this type */
	size_t data_size;
	/*! Initialize object resources, default memset 0 */
	mcr_data_fnc init;
	/*! Free object resources, default do nothing */
	mcr_data_fnc deinit;
	/*! \ref mcr_compare_fnc for an object, default memcmp */
	mcr_compare_fnc compare;
	/*! Copy from source object to destination object,
	 *  default memcpy */
	mcr_copy_fnc copy;
};

/* Data */
/*! Create data object
 *
 *  \param dataPt \ref opt \ref mcr_Data.data
 *  \param deallocate \ref opt \ref mcr_Data.deallocate
 *  \return Data object
 */
MCR_API struct mcr_Data mcr_Data_new(void *dataPt, void (*deallocate) (void *));
/*! Set data reference and deallocator.
 *
 *  \param dataIn \ref mcr_Data
 *  \param dataPt \ref opt \ref mcr_Data.data
 *  \param deallocateFnc \ref opt \ref mcr_Data.deallocate
 */
#define MCR_DATA_SET_ALL(dataIn, dataPt, deallocateFnc) \
(dataIn).data = (dataPt); \
(dataIn).deallocate = (deallocateFnc);

/*! Set data reference and deallocator.
 *
 *  \param dataInPt \ref mcr_Data
 *  \param dataPt \ref opt \ref mcr_Data.data
 *  \param deallocateFnc \ref opt \ref mcr_Data.deallocate
 */
#define mcr_Data_set_all(dataInPt, dataPt, deallocateFnc) \
if (dataInPt) { \
	MCR_DATA_SET_ALL(*(dataInPt), dataPt, deallocateFnc) \
}

/*! Is data heap allocated?
 *
 *  \param dataIn \ref mcr_Data
 *  \return bool
 */
#define MCR_DATA_IS_HEAP(dataIn) \
(!!(dataIn).deallocate)

/*! Is data heap allocated?
 *
 *  \param dataInPt \ref opt \ref mcr_Data *
 *  \return bool
 */
#define mcr_Data_is_heap(dataInPt) \
((dataInPt) ? MCR_DATA_IS_HEAP(*(dataInPt)) : false)

/*! Free data if needed, and nullify.
 *
 *  \param dataIn \ref mcr_Data
 */
#define MCR_DATA_FREE(dataIn) \
if (MCR_DATA_IS_HEAP(dataIn)) { \
	(dataIn).deallocate((dataIn).data); \
	(dataIn).deallocate = mcr_null; \
} \
(dataIn).data = mcr_null;

/*! Free data if needed, and nullify.
 *
 *  \param dataInPt \ref opt \ref mcr_Data *
 */
#define mcr_Data_deinit(dataInPt) \
if (dataInPt) { \
	MCR_DATA_FREE(*(dataInPt)); \
}

/* Interface */
/*! \ref mcr_Interface ctor
 *
 *  No deinitialize needed
 *  \param interfacePt \ref opt \ref mcr_Interface *
 *  \return 0
 */
MCR_API int mcr_Interface_init(void *interfacePt);
/*! \ref mcr_Interface_init and \ref mcr_Interface_set_all
 *
 *  \param context \ref opt \ref mcr_Interface.context
 *  \param dataSize \ref opt \ref mcr_Interface.data_size
 *  \param init \ref opt \ref mcr_Interface.init
 *  \param deinit \ref opt \ref mcr_Interface.deinit
 *  \param compare \ref opt \ref mcr_Interface.compare
 *  \param copy \ref opt \ref mcr_Interface.copy
 *  \return New interface
 */
MCR_API struct mcr_Interface mcr_Interface_new(struct mcr_context *context,
		size_t dataSize,
		mcr_data_fnc init, mcr_data_fnc deinit, mcr_compare_fnc compare,
		mcr_copy_fnc copy);

/*! Set all \ref mcr_Interface functions
 *
 *  \param context \ref opt \ref mcr_Interface.context
 *  \param interfacePt \ref mcr_Interface *
 *  \param dataSize \ref opt \ref mcr_Interface.data_size
 *  \param init \ref opt \ref mcr_Interface.init
 *  \param deinit \ref opt \ref mcr_Interface.deinit
 *  \param compare \ref opt \ref mcr_Interface.compare
 *  \param copy \ref opt \ref mcr_Interface.copy
 */
MCR_API void mcr_Interface_set_all(void *interfacePt,
								   struct mcr_context *context,
								   size_t dataSize, mcr_data_fnc init, mcr_data_fnc deinit,
								   mcr_compare_fnc compare, mcr_copy_fnc copy);

/*! Get the id of a pointer to \ref mcr_Interface
 *
 *  \param interfacePt \ref opt \ref mcr_Interface *
 *  \return \ref retid
 */
#define mcr_iid(interfacePt) (interfacePt ? \
mcr_castpt(struct mcr_Interface, interfacePt)->id : mcr_cast(size_t, -1))

/*! \ref mcr_iid typecasted from \ref mcr_Interface **
 *
 *  \param interfacePtPt \ref opt \ref mcr_Interface **
 */
#define mcr_iref_id(interfacePtPt) \
(interfacePtPt ? mcr_iid(*mcr_castpt(struct mcr_Interface *, interfacePtPt)) : \
			mcr_cast(size_t, -1))

/* Interface functions on data */
/*! Heap-allocate and initialize an object
 *
 *  1) Use \ref malloc to allocate object\n
 *  2) Initialize object with \ref mcr_Interface.init\n
 *  3) Set \ref mcr_Data.deallocate to \ref free\n
 *  \param interfacePt \ref opt \ref mcr_Interface * The initialized object
 *  will be empty without this
 *  \param dataPt \ref opt Object to initialize
 *  \return \ref reterr
 */
MCR_API int mcr_iinit(const void *interfacePt, struct mcr_Data *dataPt);

/*! \ref mcr_iinit with the contained \ref mcr_Interface *
 *
 *  \param interfacePtPt \ref opt \ref mcr_Interface **
 *  \param dataPt \ref opt Object to initialize
 *  \return \ref reterr
 */
MCR_API int mcr_iref_init(const void *const *interfacePtPt,
						  struct mcr_Data *dataPt);

/*! Release object resources and deallocate if needed.
 *
 *  1) Release resources with \ref mcr_Interface.deinit\n
 *  2) Use \ref mcr_Data.deallocate if it is set\n
 *  3) Nullify \ref mcr_Data object\n
 *  \param interfacePt \ref opt \ref mcr_Interface *
 *  \param dataPt \ref opt Object to deallocate or free
 *  \return \ref reterr
 */
MCR_API int mcr_ideinit(const void *interfacePt, struct mcr_Data *dataPt);

/*! Use an interface to compare two objects
 *
 *  \ref mcr_compare_fnc
 *  \param interfacePt \ref mcr_Interface *
 *  \param lhs \ref opt Left side of comparison
 *  \param rhs \ref opt Right side of comparison
 *  \return \ref retcmp
 */
MCR_API int mcr_icmp(const void *interfacePt,
					 const struct mcr_Data *lhs, const struct mcr_Data *rhs);

/*! Use an interface to copy an object
 *
 *  \ref mcr_copy_fnc
 *  If no source is given, destination will be freed.
 *  \param interfacePt \ref mcr_Interface *
 *  \param dstPt Object data is copied into
 *  \param srcPt \ref opt Object data is copied from
 *  \return \ref reterr
 */
MCR_API int mcr_icpy(const void *interfacePt, struct mcr_Data *dstPt,
					 const struct mcr_Data *srcPt);

/*! Replace object reference using an interface
 *
 *  Existing data will be freed. The assigned data will be copied directly
 *  and without modification.
 *  \param interfacePt \ref mcr_Interface *
 *  \param dataPt \ref opt Object to assign to
 *  \param data \ref opt \ref mcr_Data.data
 *  \param deallocate \ref opt \ref mcr_Data.deallocate
 *  \return \ref reterr
 */
MCR_API int mcr_iset_data(const void *interfacePt,
						  struct mcr_Data *dataPt, void *data, void (*deallocate) (void *));

/*! Initialize and set data.  Free previous data if it exists.
 *
 *  \param interfacePt \ref mcr_Interface *
 *  \param dataPt \ref opt Data to reset.  Do nothing if this is null
 *  \return \ref reterr
 */
MCR_API int mcr_ireset(const void *interfacePt, struct mcr_Data *dataPt);

#ifdef __cplusplus
}
#endif

#endif
