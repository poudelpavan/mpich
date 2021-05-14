/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef NETMOD_AM_FALLBACK_RECV_H_INCLUDED
#define NETMOD_AM_FALLBACK_RECV_H_INCLUDED

MPL_STATIC_INLINE_PREFIX int MPIDI_NM_mpi_imrecv(void *buf,
                                                 MPI_Aint count, MPI_Datatype datatype,
                                                 MPIR_Request * message)
{
    return MPIDIG_mpi_imrecv(buf, count, datatype, message);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_NM_mpi_irecv(void *buf,
                                                MPI_Aint count,
                                                MPI_Datatype datatype,
                                                int rank,
                                                int tag,
                                                MPIR_Comm * comm, int context_offset,
                                                MPIDI_av_entry_t * addr, MPIR_Request ** request,
                                                MPIR_Request * partner)
{
    int vni_src = comm->seq % MPIDI_CH4_MAX_VCIS;
    int vni_dst = comm->seq % MPIDI_CH4_MAX_VCIS;
    return MPIDIG_mpi_irecv(buf, count, datatype, rank, tag, comm, context_offset, request, 0,
                            partner, vni_src, vni_dst);
}

MPL_STATIC_INLINE_PREFIX int MPIDI_NM_mpi_cancel_recv(MPIR_Request * rreq)
{
    return MPIDIG_mpi_cancel_recv(rreq);
}

#endif /* NETMOD_AM_FALLBACK_RECV_H_INCLUDED */
