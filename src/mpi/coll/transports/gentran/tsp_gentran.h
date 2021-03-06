/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef TSP_GENTRAN_H_INCLUDED
#define TSP_GENTRAN_H_INCLUDED

#include "mpiimpl.h"
#include "tsp_gentran_types.h"

/* Undefine the previous definitions to avoid redefinition warnings */
#undef MPIR_TSP_TRANSPORT_NAME
#undef MPIR_TSP_sched_t
#undef MPIR_TSP_sched_create
#undef MPIR_TSP_sched_isend
#undef MPIR_TSP_sched_irecv
#undef MPIR_TSP_sched_imcast
#undef MPIR_TSP_sched_issend
#undef MPIR_TSP_sched_reduce_local
#undef MPIR_TSP_sched_localcopy
#undef MPIR_TSP_sched_selective_sink
#undef MPIR_TSP_sched_sink
#undef MPIR_TSP_sched_fence
#undef MPIR_TSP_sched_new_type
#undef MPIR_TSP_sched_generic
#undef MPIR_TSP_sched_malloc
#undef MPIR_TSP_sched_start
#undef MPIR_TSP_sched_free
#undef MPIR_TSP_sched_optimize
#undef MPIR_TSP_sched_reset

#define MPIR_TSP_TRANSPORT_NAME           Gentran_

/* Transport data structures */
#define MPIR_TSP_sched_t                  MPII_Genutil_sched_t

/* General transport API */
#define MPIR_TSP_sched_create              MPII_Genutil_sched_create
#define MPIR_TSP_sched_isend               MPII_Genutil_sched_isend
#define MPIR_TSP_sched_irecv               MPII_Genutil_sched_irecv
#define MPIR_TSP_sched_imcast              MPII_Genutil_sched_imcast
#define MPIR_TSP_sched_issend              MPII_Genutil_sched_issend
#define MPIR_TSP_sched_reduce_local        MPII_Genutil_sched_reduce_local
#define MPIR_TSP_sched_localcopy           MPII_Genutil_sched_localcopy
#define MPIR_TSP_sched_selective_sink      MPII_Genutil_sched_selective_sink
#define MPIR_TSP_sched_sink                MPII_Genutil_sched_sink
#define MPIR_TSP_sched_fence               MPII_Genutil_sched_fence
#define MPIR_TSP_sched_new_type            MPII_Genutil_sched_new_type
#define MPIR_TSP_sched_generic             MPII_Genutil_sched_generic
#define MPIR_TSP_sched_malloc              MPII_Genutil_sched_malloc
#define MPIR_TSP_sched_start               MPII_Genutil_sched_start
#define MPIR_TSP_sched_free                MPII_Genutil_sched_free
#define MPIR_TSP_sched_reset               MPII_Genutil_sched_reset

extern MPII_Coll_queue_t MPII_coll_queue;
extern int MPII_Genutil_progress_hook_id;

/* Transport function to initialize a new schedule */
int MPII_Genutil_sched_create(MPII_Genutil_sched_t * sched, bool is_persistent);

/* Transport function to free a schedule */
void MPII_Genutil_sched_free(MPII_Genutil_sched_t * sched);

int MPII_Genutil_sched_new_type(MPII_Genutil_sched_t * sched, MPII_Genutil_sched_issue_fn issue_fn,
                                MPII_Genutil_sched_complete_fn complete_fn,
                                MPII_Genutil_sched_free_fn free_fn);

int MPII_Genutil_sched_generic(int type_id, void *data,
                               MPII_Genutil_sched_t * sched, int n_in_vtcs, int *in_vtcs,
                               int *vtx_id);

/* Transport function to schedule an isend vertex */
int MPII_Genutil_sched_isend(const void *buf,
                             int count,
                             MPI_Datatype dt,
                             int dest,
                             int tag,
                             MPIR_Comm * comm_ptr,
                             MPII_Genutil_sched_t * sched, int n_in_vtcs, int *in_vtcs);

/* Transport function to schedule a issend vertex */
int MPII_Genutil_sched_issend(const void *buf,
                              int count,
                              MPI_Datatype dt,
                              int dest,
                              int tag,
                              MPIR_Comm * comm_ptr,
                              MPII_Genutil_sched_t * sched, int n_in_vtcs, int *in_vtcs);

/* Transport function to schedule an irecv vertex */
int MPII_Genutil_sched_irecv(void *buf,
                             int count,
                             MPI_Datatype dt,
                             int source,
                             int tag,
                             MPIR_Comm * comm_ptr,
                             MPII_Genutil_sched_t * sched, int n_in_vtcs, int *in_vtcs);

/* Transport function to schedule an imcast vertex */
int MPII_Genutil_sched_imcast(const void *buf,
                              int count,
                              MPI_Datatype dt,
                              UT_array * dests,
                              int num_dests,
                              int tag,
                              MPIR_Comm * comm_ptr,
                              MPII_Genutil_sched_t * sched, int n_in_vtcs, int *in_vtcs);


/* Transport function to schedule a local reduce vertex */
int MPII_Genutil_sched_reduce_local(const void *inbuf, void *inoutbuf, int count,
                                    MPI_Datatype datatype, MPI_Op op, MPII_Genutil_sched_t * sched,
                                    int n_in_vtcs, int *in_vtcs);

/* Transport function to schedule a local data copy */
int MPII_Genutil_sched_localcopy(const void *sendbuf, MPI_Aint sendcount, MPI_Datatype sendtype,
                                 void *recvbuf, MPI_Aint recvcount, MPI_Datatype recvtype,
                                 MPII_Genutil_sched_t * sched, int n_in_vtcs, int *in_vtcs);

/* Transport function to schedule a vertex that completes when all the incoming vertices have
 * completed */
int MPII_Genutil_sched_selective_sink(MPII_Genutil_sched_t * sched, int n_in_vtcs, int *invtcs);

/* Transport function to allocate memory required for schedule execution */
void *MPII_Genutil_sched_malloc(size_t size, MPII_Genutil_sched_t * sched);

/* Transport function to enqueue and kick start a non-blocking
 * collective */
int MPII_Genutil_sched_start(MPII_Genutil_sched_t * sched, MPIR_Comm * comm,
                             MPIR_Request ** request);


/* Transport function to schedule a sink */
int MPII_Genutil_sched_sink(MPII_Genutil_sched_t * sched);

/* Transport function to schedule a fence */
void MPII_Genutil_sched_fence(MPII_Genutil_sched_t * sched);

#endif /* TSP_GENTRAN_H_INCLUDED */
