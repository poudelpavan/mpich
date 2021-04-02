/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPI_Recv */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Recv = PMPI_Recv
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Recv  MPI_Recv
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Recv as PMPI_Recv
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) __attribute__ ((weak, alias("PMPI_Recv")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Recv
#define MPI_Recv PMPI_Recv

#endif

/*@
   MPI_Recv - Blocking receive for a message

Input Parameters:
+ count - number of elements in receive buffer (non-negative integer)
. datatype - datatype of each receive buffer element (handle)
. source - rank of source or MPI_ANY_SOURCE (integer)
. tag - message tag or MPI_ANY_TAG (integer)
- comm - communicator (handle)

Output Parameters:
+ buf - initial address of receive buffer (choice)
- status - status object (Status)

Notes:
The 'count' argument indicates the maximum length of a message; the actual
length of the message can be determined with 'MPI_Get_count'.
.N ThreadSafe

.N Fortran

.N Errors
.N MPI_SUCCESS

.N MPI_ERR_ARG
.N MPI_ERR_COMM
.N MPI_ERR_RANK
.N MPI_ERR_TAG
@*/

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_Comm *comm_ptr = NULL;
    MPIR_FUNC_TERSE_STATE_DECL(MPID_STATE_MPI_RECV);

    MPIR_ERRTEST_INITIALIZED_ORDIE();

    MPID_THREAD_CS_ENTER(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    MPIR_FUNC_TERSE_ENTER(MPID_STATE_MPI_RECV);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_ERRTEST_COMM(comm, mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    MPIR_Comm_get_ptr(comm, comm_ptr);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_Comm_valid_ptr(comm_ptr, mpi_errno, FALSE);
            if (mpi_errno) {
                goto fn_fail;
            }
            MPIR_ERRTEST_COUNT(count, mpi_errno);
            if (count > 0) {
                MPIR_ERRTEST_DATATYPE(datatype, "datatype", mpi_errno);
                if (!HANDLE_IS_BUILTIN(datatype)) {
                    MPIR_Datatype *datatype_ptr = NULL;
                    MPIR_Datatype_get_ptr(datatype, datatype_ptr);
                    MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                    MPIR_Datatype_committed_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                }
                MPIR_ERRTEST_USERBUFFER(buf, count, datatype, mpi_errno);
            }
            MPIR_ERRTEST_RECV_RANK(comm_ptr, source, mpi_errno);
            MPIR_ERRTEST_RECV_TAG(tag, mpi_errno);
            MPIR_ERRTEST_ARGNULL(status, "status", mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    /* Return immediately for dummy process */
    if (unlikely(source == MPI_PROC_NULL)) {
        MPIR_Status_set_procnull(status);
        goto fn_exit;
    }

    /* ... body of routine ... */
    MPIR_Request *request_ptr = NULL;

    /* MT: Note that MPID_Recv may release the SINGLE_CS if it
     * decides to block internally.  MPID_Recv in that case will
     * re-aquire the SINGLE_CS before returnning */
    mpi_errno = MPID_Recv(buf, count, datatype, source, tag, comm_ptr,
                          MPIR_CONTEXT_INTRA_PT2PT, status, &request_ptr);
    if (mpi_errno != MPI_SUCCESS)
        goto fn_fail;

    if (request_ptr == NULL) {
        goto fn_exit;
    }

    mpi_errno = MPID_Wait(request_ptr, MPI_STATUS_IGNORE);
    if (mpi_errno != MPI_SUCCESS)
        goto fn_fail;

    mpi_errno = request_ptr->status.MPI_ERROR;
    MPIR_Request_extract_status(request_ptr, status);
    MPIR_Request_free(request_ptr);

    if (mpi_errno != MPI_SUCCESS)
        goto fn_fail;
    /* ... end of body of routine ... */

  fn_exit:
    MPIR_FUNC_TERSE_EXIT(MPID_STATE_MPI_RECV);
    MPID_THREAD_CS_EXIT(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLINE-- */
#ifdef HAVE_ERROR_CHECKING
    mpi_errno = MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE, __func__, __LINE__, MPI_ERR_OTHER, "**mpi_recv", "**mpi_recv %p %d %D %i %t %C %p", buf, count, datatype, source, tag, comm, status);
#endif
    mpi_errno = MPIR_Err_return_comm(comm_ptr, __func__, mpi_errno);
    /* --END ERROR HANDLING-- */
    goto fn_exit;
}
