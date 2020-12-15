/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef SHM_AM_FALLBACK_RECV_H_INCLUDED
#define SHM_AM_FALLBACK_RECV_H_INCLUDED

#include "ch4_impl.h"

/* Common macro used by all MPIDI_POSIX_mpi_recv routines to compute vnis */
#define MPIDI_SHM_RECV_VNIS(vni_src_, vni_dst_) \
    do { \
        if (*request != NULL) { \
            /* workq path */ \
            vni_src_ = 0; \
            vni_dst_ = 0; \
        } else { \
            vni_src_ = MPIDI_SHM_get_vni(SRC_VCI_FROM_SENDER, comm, comm->rank, rank, tag); \
            vni_dst_ = MPIDI_SHM_get_vni(DST_VCI_FROM_SENDER, comm, comm->rank, rank, tag); \
        } \
    } while (0)

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_imrecv(void *buf,
                                                  MPI_Aint count, MPI_Datatype datatype,
                                                  MPIR_Request * message)
{
    return MPIDIG_mpi_imrecv(buf, count, datatype, message);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_irecv(void *buf,
                                                 MPI_Aint count,
                                                 MPI_Datatype datatype,
                                                 int rank,
                                                 int tag,
                                                 MPIR_Comm * comm, int context_offset,
                                                 MPIR_Request ** request)
{
    int vni_src = 0, vni_dst = 0;
    MPIDI_SHM_RECV_VNIS(vni_src, vni_dst);
    return MPIDIG_mpi_irecv(buf, count, datatype, rank, tag, comm, context_offset, vni_src, vni_dst, request, 1,
                            NULL);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_cancel_recv(MPIR_Request * rreq)
{
    return MPIDIG_mpi_cancel_recv(rreq);
}

#endif /* SHM_AM_FALLBACK_RECV_H_INCLUDED */
