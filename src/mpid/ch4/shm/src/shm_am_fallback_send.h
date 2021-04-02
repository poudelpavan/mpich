/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef SHM_AM_FALLBACK_SEND_H_INCLUDED
#define SHM_AM_FALLBACK_SEND_H_INCLUDED

#include "mpidig_am.h"
#include "ch4_impl.h"

/* vni mapping */
/* NOTE: concerned by the modulo? If we restrict num_vnis to power of 2,
 * we may get away with bit mask */
MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_get_vni(int flag, MPIR_Comm * comm_ptr,
                                               int src_rank, int dst_rank, int tag)
{
    return MPIDI_get_vci(flag, comm_ptr, src_rank, dst_rank, tag) % MPIDI_global.n_vcis;
}

/* Common macro used by all MPIDI_POSIX_mpi_send routines to compute vnis */
#define MPIDI_SHM_SEND_VNIS(vni_src_, vni_dst_) \
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

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_isend(const void *buf,
                                                 MPI_Aint count,
                                                 MPI_Datatype datatype,
                                                 int rank,
                                                 int tag,
                                                 MPIR_Comm * comm, int context_offset,
                                                 MPIDI_av_entry_t * addr, MPIR_Request ** request)
{
    int vni_src = 0, vni_dst = 0;
    MPIDI_SHM_SEND_VNIS(vni_src, vni_dst);
    fprintf(stdout, "%ld, MPIDI_SHM_mpi_isend,fallback, comm=%x, context=%d, vci=%d\n", pthread_self(), comm->handle, comm->context_id, vni_dst);
    return MPIDIG_mpi_isend(buf, count, datatype, rank, tag, comm, context_offset, addr, vni_src, vni_dst, request);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_issend(const void *buf,
                                                  MPI_Aint count,
                                                  MPI_Datatype datatype,
                                                  int rank,
                                                  int tag,
                                                  MPIR_Comm * comm, int context_offset,
                                                  MPIDI_av_entry_t * addr, MPIR_Request ** request)
{
    return MPIDIG_mpi_issend(buf, count, datatype, rank, tag, comm, context_offset, addr, request);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_isend_coll(const void *buf, MPI_Aint count,
                                                  MPI_Datatype datatype, int rank, int tag,
                                                  MPIR_Comm * comm, int context_offset,
                                                  MPIDI_av_entry_t * addr, MPIR_Request ** request,
                                                  MPIR_Errflag_t * errflag)
{
    return MPIDIG_isend_coll(buf, count, datatype, rank, tag, comm, context_offset, addr,
                             request, errflag);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_SHM_mpi_cancel_send(MPIR_Request * sreq)
{
    return MPIDIG_mpi_cancel_send(sreq);
}

#endif /* SHM_AM_FALLBACK_SEND_H_INCLUDED */
