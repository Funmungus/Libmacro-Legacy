/* Libmacro - A multi-platform, extendable macro and hotkey C library
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
 * \brief \ref mcr_Instance Example and abstraction of an
 * interface instance.
 *
 * All instance implementations should begin with
 * \ref mcr_Interface *, and then a \ref mcr_Data structure.\n
 *
 * Typecasting is used throughout, so implementations do not need
 * the same element names.
 */

#ifndef MCR_INSTANCE_H
#define MCR_INSTANCE_H

#include "mcr/util/interface.h"

/*! \brief Example of an interface instance. */
struct mcr_Instance {
	/*! \brief A reference to the interface */
	const struct mcr_Interface *interface;
	/*! \brief Actual data of this instance */
	struct mcr_Data data;
};

/*! \brief \ref mcr_Instance ctor
 *
 * Use \ref mcr_Instance_set_interface and \ref mcr_Instance_reset to
 * create new, empty data.
 * \param instPt \ref opt \ref mcr_Instance *
 * \return 0
 */
MCR_API int mcr_Instance_init(void *instPt);
/*! \brief \ref mcr_Instance dtor
 *
 * Free resources of an instance
 * \param instPt \ref opt \ref mcr_Instance *
 * \return \ref reterr
 */
MCR_API int mcr_Instance_deinit(void *instPt);
/*! \brief \ref mcr_Instance_set_interface and \ref mcr_Instance_set_data
 *
 * \param instPt \ref mcr_Instance *
 * \param iPt \ref opt \ref mcr_Instance.interface
 * \param dataPt \ref opt \ref mcr_Data.data
 * \param deallocate \ref opt \ref mcr_Data.deallocate
 * \return \ref reterr
 */
MCR_API int mcr_Instance_set_all(void *instPt, const void *iPt,
				 void *dataPt, void (*deallocate) (void *));

/*! \brief Get \ref mcr_Data.data
 *
 * \param instPt \ref opt \ref mcr_Instance *
 * \return void *
 */
#define mcr_Instance_data(instPt) \
((instPt) ? \
	((struct mcr_Instance *)(instPt))->data.data : \
NULL)

/*! \brief \ref mcr_Data_is_heap
 *
 * \param instPt \ref opt \ref mcr_Instance *
 * \return bool
 */
#define mcr_Instance_is_heap(instPt) \
((instPt) ? \
	MCR_DATA_IS_HEAP((instPt)->data) : \
false)

/*! \brief Get the id of an instance's interface
 *
 * \ref mcr_iid
 * \param instPt \ref opt \ref mcr_Instance *
 * \return \ref retid
 */
#define mcr_Instance_id(instPt) mcr_iref_id(instPt)

/*! \brief Change the interface of an instance
 *
 * Existing data will be deinitialized before setting the new interface.
 * \param instPt \ref mcr_Instance *
 * \param iPt \ref opt \ref mcr_Interface *
 * \return \ref reterr
 */
MCR_API int mcr_Instance_set_interface(void *instPt, const void *iPt);
/*! \ref Change current data of an instance
 *
 * Existing data will be deinitialized before setting the new data.
 * \ref mcr_iset_data
 * \param instPt \ref mcr_Instance *
 * \param dataPt \ref opt \ref mcr_Data.data
 * \param deallocate \ref opt \ref mcr_Data.deallocate
 * \return \ref reterr
 */
MCR_API int mcr_Instance_set_data(void *instPt, void *dataPt,
				  void (*deallocate) (void *));
/*! \brief Compare two instances
 *
 * \param lhsPt \ref opt \ref mcr_Instance *
 * \param rhsPt \ref opt \ref mcr_Instance *
 * \return \ref retcmp
 */
MCR_API int mcr_Instance_compare(const void *lhsPt, const void *rhsPt);
/*! \ref Copy from source instance to destination instance
 *
 * \param dstPt \ref mcr_Instance *
 * \param srcPt \ref opt \ref mcr_Instance *
 * \return \ref reterr
 */
MCR_API int mcr_Instance_copy(void *dstPt, void *srcPt);
/*! \brief Free existing data and reinitialize
 *
 * \ref mcr_ireset
 * \param instPt \ref mcr_Instance *
 * \return \ref reterr
 */
MCR_API int mcr_Instance_reset(void *instPt);

#endif
