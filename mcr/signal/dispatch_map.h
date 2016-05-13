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
 * \brief \ref mcr_DispatchMap use key interfaces to dispatch.
 */

#ifndef MCR_DISPATCH_MAP_H
#define MCR_DISPATCH_MAP_H

#include "mcr/signal/isignal.h"
#include "mcr/signal/dispatch_pair.h"

/*!
 * \brief Mapped dispatch logic
 *
 * All maps are either:\n
 * map => next key type map\n
 * or\n
 * map => array of \ref mcr_DispatchPair (will dispatch to all receivers)\n
 * The final value set will be an array with \ref mcr_Array_DispatchPair_iface.
 */
struct mcr_DispatchMap {
	/*!
	 * \brief
	 */
	struct mcr_Map map;
	/*!
	 * \brief Array of \ref mcr_Interface for the key of each map.
	 *
	 * No interface may be NULL.  We have alloted the space, just use it.
	 * The first interface of this set is the first mapped from modifiers.
	 * If it is empty, modifiers will dispatch directly into \ref
	 * mcr_Array_DispatchPair_iface. The value interface to create a map
	 * or array will be assigned automatically during key-value mapping.
	 */
	struct mcr_Array key_iface_set;
};

/*! */
MCR_API void mcr_DispatchMap_init(void *dataPt);
/*! */
MCR_API void mcr_DispatchMap_free(void *dataPt);
/*!
 * \brief \ref mcr_DispatchMap_set_key_count and
 * \brief \ref mcr_DispatchMap_set_key_iface for all interfaces
 */
MCR_API int mcr_DispatchMap_set_all(struct mcr_DispatchMap *dmPt,
	struct mcr_Interface *keyIfaceSet, size_t keyIfaceCount);
/*!
 * \brief Get the number of interfaces for user-defined keys.
 *
 * \return \ref retind
 */
MCR_API size_t mcr_DispatchMap_key_count(struct mcr_DispatchMap *dmPt);
/*!
 * \brief Set the number of interfaces for user-defined keys.
 *
 * This will remove any mapped values and free all maps.
 */
MCR_API int mcr_DispatchMap_set_key_count
	(struct mcr_DispatchMap *dmPt, size_t count);
/*!
 * \brief Get the interface for the user-defined key at index.
 *
 * \param index Index of user-defined key
 * \return Interface, or NULL if out of range.
 */
MCR_API const struct mcr_Interface *mcr_DispatchMap_key_iface(struct
	mcr_DispatchMap *dmPt, size_t index);
/*!
 * \brief After setting the number of interfaces, this will fill the key
 * interface at the given index.
 *
 * \return \ref reterr, Will always be -EFAULT if the array is not already
 * allocated with enough elements for the index.
 */
MCR_API int mcr_DispatchMap_set_key_iface(struct mcr_DispatchMap *dmPt,
	size_t index, mcr_compare_fnc compare, mcr_copy_fnc copy,
	size_t data_size, mcr_data_fnc init, mcr_data_fnc free);
MCR_API int mcr_DispatchMap_set_key_compare(struct mcr_DispatchMap *dmPt,
	size_t index, mcr_compare_fnc compare, size_t dataSize);

MCR_API int mcr_DispatchMap_add(struct mcr_DispatchMap *dmPt, void **keyPtArr,
	void *receiver, mcr_Dispatcher_receive_fnc receiveFnc);
MCR_API void mcr_DispatchMap_remove(struct mcr_DispatchMap *dmPt, void *ref);
MCR_API void mcr_DispatchMap_remove_empties(struct mcr_DispatchMap *dmPt,
	bool trimFlag);
MCR_API struct mcr_Array *mcr_DispatchMap_receivers(struct mcr_DispatchMap
	*dmPt, void **keyPtArr);
MCR_API struct mcr_Array *mcr_DispatchMap_receivers_ensured(struct
	mcr_DispatchMap *dmPt, void **keyPtArr);
MCR_API bool mcr_DispatchMap_dispatch(struct mcr_DispatchMap *dmPt,
	struct mcr_Signal *interceptPt, unsigned int mods,
	const void **keyPtArr, const void **genPtArr,
	struct mcr_Map **mapHolderArray, bool * specHandledHolderArr,
	bool * genHandledHolderArr, bool * useGenOnlyArr);
MCR_API void mcr_DispatchMap_clear(struct mcr_DispatchMap *dmPt);
MCR_API void mcr_DispatchMap_trim(struct mcr_DispatchMap *dmPt);

#endif
