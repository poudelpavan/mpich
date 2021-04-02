/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef SHM_AM_FALLBACK_RECV_H_INCLUDED
#define SHM_AM_FALLBACK_RECV_H_INCLUDED

#include "mpidig_am.h"
#include "ch4_impl.h"

/* vni mapping */
/* NOTE: concerned by the modulo? If we restrict num_vnis to power of 2,
 * we may get away with bit mask */
MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_get_vni_recv(int flag, MPIR_Comm * comm_ptr,
                                               int src_rank, int dst_rank, int tag)
{
    return MPIDI_get_vci(flag, comm_ptr, src_rank, dst_rank, tag);
}

/* Common macro used by all MPIDI_POSIX_mpi_recv routines to compute vnis */
#define MPIDI_SHM_RECV_VNIS(vni_src_, vni_dst_) \
    do { \
        if (*request != NULL) { \
            /* workq path */ \
            vni_src_ = 0; \
            vni_dst_ = 0; \
        } else { \
            vni_src_ = MPIDI_SHM_get_vni_recv(SRC_VCI_FROM_SENDER, comm, comm->rank, rank, tag); \
            vni_dst_ = MPIDI_SHM_get_vni_recv(DST_VCI_FROM_SENDER, comm, comm->rank, rank, tag); \
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
    fprintf(stdout, "%ld, MPIDI_SHM_mpi_irecv, fallback, comm=%x, context=%d, vci=%d\n", pthread_self(), comm->handle, comm->context_id, vni_dst);
    return MPIDIG_mpi_irecv(buf, count, datatype, rank, tag, comm, context_offset, vni_src, vni_dst, request, 1,
                            NULL);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_cancel_recv(MPIR_Request * rreq)
{
    return MPIDIG_mpi_cancel_recv(rreq);
}

#endif /* SHM_AM_FALLBACK_RECV_H_INCLUDED */
