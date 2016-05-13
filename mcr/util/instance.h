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
 * \brief Example and abstraction of an instance of an interface
 * implementation.  All instance implementations should begin with
 * \ref mcr_Interface *, and secondly a \ref mcr_Data structure.
 *
 * Typecasting is used throughout, so implementations of data sets
 * do not require the same name schemes.  Only defining with a pointer
 * and data structure are required.
 */

#ifndef MCR_INSTANCE_H
#define MCR_INSTANCE_H

#include "mcr/util/interface.h"

/*! \brief Example and contract of an instance of an interface. */
struct mcr_Instance {
	/*!
	 * \brief A reference to the common implementation of all objects of
	 * this type
	 */
	const struct mcr_Interface *iface;
	/*! \brief Actual data set for this instance */
	struct mcr_Data data;
};

/*! \ref mcr_Instance ctor */
MCR_API void mcr_inst_init(void *instDataPt);
/*!
 * \ref mcr_Instance dtor
 *
 * \ref mcr_ifree
 * \param inst \ref mcr_Instance * (opt)
 */
MCR_API void mcr_inst_free(void *instDataPt);
/*!
 * \brief Set interface and also \ref MCR_DATA_SET_ALL
 *
 * \param instPt \ref mcr_Instance *
 * \param iPt \ref mcr_Instance.iface
 * \param dataPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
MCR_API void mcr_inst_set_all(void *instDataPt, void *iDataPt,
	void *dataPt, mcr_data_fnc deallocate);
MCR_API void mcr_inst_set_iface(void *instDataPt, void *iDataPt);
/*!
 * \ref MCR_ISET
 *
 * \param instPt \ref mcr_instance *(opt)
 * \param dataPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
MCR_API void mcr_inst_set_data(void *instDataPt, void *dataPt,
	mcr_data_fnc deallocate);
/*!
 * \brief \ref MCR_DATA_IS_HEAP
 *
 * \param instPt \ref mcr_instance *(opt)
 * \return bool
 */
#define mcr_inst_is_heap(instPt) \
(instPt ? \
	MCR_DATA_IS_HEAP(((struct mcr_Instance *)instPt)->data) : \
false)
/*!
 * \brief Get \ref mcr_Data.data
 *
 * \return void *
 */
#define mcr_inst_data(instPt) \
(instPt ? \
	((struct mcr_Instance *)instPt)->data.data : \
NULL)

/* Interface */
#define mcr_inst_id(instPt) mcr_iref_id(instPt)
MCR_API void mcr_inst_reset(void *instDataPt);
/*!
 * \ref mcr_icmp
 *
 * \param lhsPt \ref mcr_Instance *
 * \param rhsPt \ref mcr_Instance *
 * \return \ref retcmp
 */
MCR_API int mcr_inst_compare(const void *lhsPt, const void *rhsPt);
/*!
 * \ref mcr_icpy
 *
 * \param dstPt \ref mcr_Instance *
 * \param srcPt \ref mcr_Instance *
 */
MCR_API int mcr_inst_copy(void *dstPt, void *srcPt);

/*!
 * \ref mcr_ifree
 *
 * \param inst \ref mcr_Instance
 */
#define MCR_INST_FREE(inst) \
if ((inst).iface) { \
	MCR_IFREE(*(inst).iface, (inst).data); \
} else { \
	MCR_DATA_FREE((inst).data); \
}
/*!
 * \brief Set interface and also \ref MCR_DATA_SET_ALL
 *
 * \param inst \ref mcr_Instance
 * \param iPt \ref mcr_Instance.iface
 * \param dataPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
#define MCR_INST_SET_ALL(inst, iPt, dataPt, deallocateFnc) \
(inst).iface = (struct mcr_Interface *)iPt; \
MCR_DATA_SET_ALL((inst).data, dataPt, deallocateFnc);
/*!
 * \ref MCR_DATA_IS_HEAP
 *
 * \param inst \ref mcr_instance
 * \return bool
 */
#define MCR_INST_IS_HEAP(inst) \
MCR_DATA_IS_HEAP((inst).data)
/*!
 * \ref MCR_ISET
 *
 * \param inst \ref mcr_instance
 * \param dataPt \ref mcr_Data.data(opt)
 * \param deallocateFnc \ref mcr_Data.deallocate(opt)
 */
#define MCR_INST_SET_DATA(inst, dataPt, deallocateFnc) \
if ((inst).iface) { \
	MCR_ISET_DATA(*(inst).iface, (inst).data, \
			dataPt, deallocateFnc); \
} else { \
	MCR_DATA_SET_ALL((inst).data, dataPt, deallocateFnc); \
}

#define MCR_INST_ID(inst) mcr_iid((inst).iface)
/* Can only reset with an interface */
#define MCR_INST_RESET(inst) \
if ((inst).iface) { \
	MCR_IRESET(*(inst).iface, (inst).data); \
}
/*!
 * \ref MCR_ICMP
 *
 * \param lhs \ref mcr_Instance Left side of comparison
 * \param rhs \ref mcr_Instance Right side of comparison
 * \return \ref retcmp
 */
#define MCR_INST_CMP(lhs, rhs) \
((rhs).iface ? \
	(lhs).iface ? \
		(lhs).iface == (rhs).iface ? \
			MCR_ICMP(*(lhs).iface, \
					(lhs).data, \
					(rhs).data) : \
		MCR_CMP_INTEGRAL(mcr_iid((lhs).iface), \
				mcr_iid((rhs).iface)) : \
	-1 : \
!!(lhs).iface)

#endif
