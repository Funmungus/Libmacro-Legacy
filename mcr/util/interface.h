/* Libmacro - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

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
 * \brief Interface to be extended by structures, and grouped by typedef
 */

#ifndef MCR_INTERFACE_H
#define MCR_INTERFACE_H

#include "mcr/util/def.h"

/*! \brief An interface for functions on data */
struct mcr_Interface {
	/*! \brief Unique identifier for this type */
	size_t id;
	/*! \brief \ref mcr_compare_fnc for data, default memcmp */
	mcr_compare_fnc compare;
	/*! \pre Destination must be initialized
	 * \brief Copy data from source, default memcpy */
	mcr_copy_fnc copy;
	/*! \brief Byte size of a data element */
	size_t data_size;
	/*! \brief Initializer for data, default memset 0 */
	mcr_data_fnc init;
	/*! \brief Free function for data, default do nothing */
	mcr_data_fnc free;
};

/*!
 * \brief Data in this structure can be programmatically deallocated.
 *
 * \ref mcr_Data_free
 */
struct mcr_Data {
	/*!
	 * \brief Pointer to an object, which may or may not need to be
	 * deallocated
	 */
	void *data;
	/*!
	 * \brief If this exists it will be called after the interface
	 * free function.  Logically this is interpreted as freeing dynamic
	 * memory.
	 *
	 * If data is allocated with C++ new or new[] operators, please
	 * create functions to delete correctly.
	 */
	mcr_data_fnc deallocate;
};

/* Data */
/*!
 * \brief Set values data member with deallocator.
 *
 * \param dataIn \ref mcr_Data
 * \param dataPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
#define MCR_DATA_SET_ALL(dataIn, dataPt, deallocateFnc) \
(dataIn).data = (dataPt); \
(dataIn).deallocate = (deallocateFnc);
/*!
 * \brief Set values data member with deallocator.
 *
 * \param dataPt \ref mcr_Data
 * \param dataValPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
#define mcr_Data_set_all(dataPt, dataValPt, deallocateFnc) \
if (dataPt) { \
	MCR_DATA_SET_ALL(*(dataPt), dataValPt, deallocateFnc); \
}
/*!
 * \brief Is data heap allocated?
 *
 * \param dataIn \ref mcr_Data
 * \return bool */
#define MCR_DATA_IS_HEAP(dataIn) \
(!!(dataIn).deallocate)
/*!
 * \brief Is data heap allocated?
 *
 * \param dataInPt \ref mcr_Data *(opt)
 * \return bool */
#define mcr_Data_is_heap(dataInPt) \
(dataInPt ? MCR_DATA_IS_HEAP(*(dataInPt)) : false)
/*!
 * \brief Free data if needed, and nullify.
 *
 * \param dataIn \ref mcr_Data
 */
#define MCR_DATA_FREE(dataIn) \
if (MCR_DATA_IS_HEAP(dataIn)) { \
	(dataIn).deallocate((dataIn).data); \
	(dataIn).data = (dataIn).deallocate = NULL; \
}
/*!
 * \brief Free data if needed, and nullify.
 *
 * \param dataInPt \ref mcr_Data *(opt)
 */
#define mcr_Data_free(dataInPt) \
if (dataInPt) { \
	MCR_DATA_FREE(*(dataInPt)); \
}
/*!
 * \brief \ref mcr_data_free
 *
 * \param dataInPt \ref mcr_Data *(opt)
 */
#define mcr_Data_free_foreach(dataInPt, ignore) \
if (dataInPt) { \
	MCR_DATA_FREE(*(dataInPt)); \
}

/* Interface */
/*! ctor */
MCR_API void mcr_iinit(void *ifaceDataPt);
/*!
 * \brief Set all functions in function table
 *
 * \param compare(opt) \ref mcr_Interface.compare
 * \param copy(opt) \ref mcr_Interface.copy
 * \param dataSize \ref mcr_Interface.data_size
 * \param init(opt) \ref mcr_Interface.init
 * \param free(opt) \ref mcr_Interface.free
 */
MCR_API void mcr_iset_all(void *ifaceDataPt,
	mcr_compare_fnc compare, mcr_copy_fnc copy,
	size_t dataSize, mcr_data_fnc init, mcr_data_fnc free);
/*!
 * \brief Get the id of a pointer to \ref mcr_Interface
 *
 * \param iPt \ref mcr_Interface *(opt)
 * \return \ref retind
 */
#define mcr_iid(iPt) (iPt ? ((struct mcr_Interface *)iPt)->id : (size_t)~0)
/*!
 * \brief \ref mcr_iid typecasted from \ref mcr_Interface **
 *
 * \param iPtPt \ref mcr_Interface **(opt)
 */
#define mcr_iref_id(iPtPt) \
(iPtPt ? mcr_iid(*(struct mcr_Interface **)iPtPt) : \
		(size_t)~0)

/* Interface functions on data */
/*!
 * \brief Heap-allocate data, and set deallocate to the correct free
 * function.
 *
 * Initialized with \ref mcr_Interface.init
 * \param iPt(opt) If null return data will be empty.
 */
MCR_API struct mcr_Data mcr_imkdata(const void *ifaceDataPt);
/*!
 * \brief \ref mcr_imkdata with the contained \ref mcr_Interface *
 *
 * \param iPtPt(opt) If null or contains null, return data will be empty.
 */
MCR_API struct mcr_Data mcr_iref_mkdata(const void *const *ifaceDataPtPt);
/*!
 * \brief Free data of given interface, and deallocate if needed.
 *
 * Freed with \ref mcr_Interface.free
 * If either parameter is NULL this will do nothing.
 * \param dataPt Data to free, if needed.
 */
MCR_API void mcr_ifree(const void *ifaceDataPt, struct mcr_Data *dataPt);
/*! \ref mcr_ifree */
#define mcr_ifree_foreach(iPt, dataPt, ignore) mcr_ifree(iPt, dataPt)
/*!
 * \brief Copy data for given interface type.
 *
 * \param dstPt Data object in which data is copied into
 * \param srcPt Data object from which data is copied.  If NULL destination is
 * freed and emptied.
 * \return \ref reterr
 */
MCR_API int mcr_icpy(const void *ifaceDataPt, struct mcr_Data *dstPt,
	struct mcr_Data *srcPt);
/*!
 * \brief Compare data for given interface type.
 *
 * \param lhs Left side of comparison
 * \param rhs Right side of comparison
 * \return \ref retcmp
 */
MCR_API int mcr_icmp(const void *ifaceDataPt,
	const struct mcr_Data *lhs, const struct mcr_Data *rhs);
/*!
 * \brief Change data in \ref mcr_Data
 *
 * The assigned data will not be initialized with
 * \ref mcr_Interface.init
 * \param dataPt(opt)
 * \param data \ref mcr_Data.data(opt)
 * \param deallocate \ref mcr_Data.deallocate(opt)
 */
MCR_API void mcr_iset_data(const void *ifaceDataPt,
	struct mcr_Data *dataPt, void *data, mcr_data_fnc deallocate);

/*!
 * \brief \ref mcr_ifree
 *
 * \param interface \ref mcr_Interface
 * \param freeData \ref mcr_Data
 */
#define MCR_IFREE(interface, freeData) \
if ((freeData).data) \
{ \
	if ((interface).free) \
		(interface).free((freeData).data); \
	MCR_DATA_FREE(freeData); \
}
/*!
 * \brief Change data in \ref mcr_Data
 *
 * The assigned data will not be initialized with
 * \ref mcr_Interface.init
 * \param interface \ref mcr_Interface
 * \param dataInOut \ref mcr_Data
 * \param dataPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
#define MCR_ISET_DATA(interface, dataInOut, dataPt, deallocateFnc) \
if ((dataInOut).data != (dataPt)) { \
	if ((dataInOut).data && (interface).free) { \
		(interface).free((dataInOut).data); \
	} \
	MCR_DATA_FREE(dataInOut); \
	MCR_DATA_SET_ALL(dataInOut, dataPt, deallocateFnc); \
}
/*!
 * \brief Initialize and set data.
 *
 * \param interface \ref mcr_Interface
 * \param dataOut \ref mcr_Data, assumed to not be initialized
 */
#define MCR_IMKDATA(interface, dataOut) \
if (((dataOut).data = malloc((interface).data_size))) \
{ \
	(dataOut).deallocate = free; \
	if ((interface).init) \
		(interface).init((dataOut).data); \
	else \
		memset((dataOut).data, 0, (interface).data_size); \
} \
else {(dataOut).deallocate = NULL; }
/*!
 * \brief Initialize and set data.  Free previous data if it exists.
 *
 * \param interface \ref mcr_Interface
 * \param dataOut \ref mcr_Data, assumed to be initialized
 */
#define MCR_IRESET(interface, dataInOut) \
if ((dataInOut).data) \
{ \
	if ((interface).free) \
		(interface).free((dataInOut).data); \
	if ((interface).init) \
		(interface).init((dataInOut).data); \
	else \
		memset((dataInOut).data, 0, (interface).data_size); \
} \
else \
{ MCR_IMKDATA(interface, dataInOut); }
/*!
 * \brief \ref mcr_icmp
 *
 * \param interface \ref mcr_Interface
 * \param lData \ref mcr_Data
 * \param rData \ref mcr_Data
 */
#define MCR_ICMP(interface, lData, rData) \
((rData).data ? \
	(lData).data ? \
		(interface).compare ? \
			(interface).compare((lData).data, (rData).data) : \
		memcmp((lData).data, (rData).data, \
				(interface).data_size) : \
	-1 : \
!!(lData).data)

#endif
