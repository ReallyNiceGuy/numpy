#ifndef __NPY_NEW_ITERATOR__
#define __NPY_NEW_ITERATOR__

#include <Python.h>
#include <numpy/ndarraytypes.h>

/* The actual structure of the iterator is an internal detail */
typedef struct NpyIter_InternalOnly NpyIter;

/* Iterator function pointers that may be specialized */
typedef int (*NpyIter_IterNext_Fn )(NpyIter *iter);
typedef void (*NpyIter_GetCoords_Fn )(NpyIter *iter,
                                      npy_intp *outcoords);

/* For specifying allowed casting in operations which support it */
typedef enum {
        /* Only allow identical types */
        NPY_NO_CASTING=0,
        /* Allow identical and byte swapped types */
        NPY_EQUIV_CASTING=1,
        /* Only allow safe casts */
        NPY_SAFE_CASTING=2,
        /* Allow safe casts or casts within the same kind */
        NPY_SAME_KIND_CASTING=3,
        /* Allow any casts */
        NPY_UNSAFE_CASTING=4
} NPY_CASTING;


/* Allocate a new iterator for one array object */
NpyIter *
NpyIter_New(PyArrayObject* op, npy_uint32 flags,
                  NPY_ORDER order, NPY_CASTING casting,
                  PyArray_Descr* dtype,
                  npy_intp a_ndim, npy_intp *axes, npy_intp buffersize);

/* Allocate a new iterator for multiple array objects */
NpyIter *
NpyIter_MultiNew(npy_intp niter, PyArrayObject **op_in, npy_uint32 flags,
                 NPY_ORDER order, NPY_CASTING casting,
                 npy_uint32 *op_flags, PyArray_Descr **op_request_dtypes,
                 npy_intp oa_ndim, npy_intp **op_axes, npy_intp buffersize);

/* Makes a copy of the iterator */
NpyIter *
NpyIter_Copy(NpyIter *iter);

/* Deallocate an iterator */
int NpyIter_Deallocate(NpyIter* iter);

/* Whether the buffer allocation is being delayed */
int NpyIter_HasDelayedBufAlloc(NpyIter *iter);

/* Whether the iterator handles the inner loop */
int NpyIter_HasInnerLoop(NpyIter *iter);
/* Removes the inner loop handling (so HasInnerLoop returns false) */
int NpyIter_RemoveInnerLoop(NpyIter *iter);
/* Get the array of strides for the inner loop (when HasInnerLoop is false) */
npy_intp *NpyIter_GetInnerStrideArray(NpyIter *iter);
/* Get a pointer to the size of the inner loop  (when HasInnerLoop is false) */
npy_intp* NpyIter_GetInnerLoopSizePtr(NpyIter *iter);

/* Resets the iterator to its initial state */
int NpyIter_Reset(NpyIter *iter);
/* Resets the iterator to its initial state, with new base data pointers */
int NpyIter_ResetBasePointers(NpyIter *iter, char **baseptrs);
/* Resets the iterator to a new iterator index range */
int NpyIter_ResetToIterIndexRange(NpyIter *iter,
                                  npy_intp istart, npy_intp iend);

/* Gets the number of dimensions being iterated */
npy_intp NpyIter_GetNDim(NpyIter *iter);
/* Gets the number of objects being iterated */
npy_intp NpyIter_GetNIter(NpyIter *iter);

/* Compute the specialized iteration function for an iterator */
NpyIter_IterNext_Fn NpyIter_GetIterNext(NpyIter *iter);

/* Gets the number of elements being iterated */
npy_intp NpyIter_GetIterSize(NpyIter *iter);
/* Get the range of iteration indices being iterated */
void NpyIter_GetIterIndexRange(NpyIter *iter,
                               npy_intp *istart, npy_intp *iend);
/* Gets the current iteration index */
npy_intp NpyIter_GetIterIndex(NpyIter *iter);
/* Sets the iterator to point at the given iteration index */
int NpyIter_GotoIterIndex(NpyIter *iter, npy_intp iterindex);

/* Whether the iterator is tracking coordinates */
int NpyIter_HasCoords(NpyIter *iter);
/* Gets the broadcast shape (if coords are enabled) */
int NpyIter_GetShape(NpyIter *iter, npy_intp *outshape);
/* Compute a specialized getcoords function for the iterator */
NpyIter_GetCoords_Fn NpyIter_GetGetCoords(NpyIter *iter);
/* Sets the iterator to point at the coordinates in 'coords' */
int NpyIter_GotoCoords(NpyIter *iter, npy_intp *coords);
/* Removes coords support from an iterator */
int NpyIter_RemoveCoords(NpyIter *iter);

/* Whether the iterator is tracking an index */
int NpyIter_HasIndex(NpyIter *iter);
/* Get a pointer to the index, if it is being tracked */
npy_intp *NpyIter_GetIndexPtr(NpyIter *iter);
/* Sets the iterator to point at the given index */
int NpyIter_GotoIndex(NpyIter *iter, npy_intp index);

/* Get the array of data pointers (1 per object being iterated) */
char **NpyIter_GetDataPtrArray(NpyIter *iter);
/* Get the array of data type pointers (1 per object being iterated) */
PyArray_Descr **NpyIter_GetDescrArray(NpyIter *iter);
/* Get the array of objects being iterated */
PyArrayObject **NpyIter_GetObjectArray(NpyIter *iter);
/* Returns a view to the i-th object with the iterator's internal axes */
PyArrayObject *NpyIter_GetIterView(NpyIter *iter, npy_intp i);

/* Gets an array of read flags (1 per object being iterated) */
void NpyIter_GetReadFlags(NpyIter *iter, char *outreadflags);
/* Gets an array of write flags (1 per object being iterated) */
void NpyIter_GetWriteFlags(NpyIter *iter, char *outwriteflags);

/* For debugging */
NPY_NO_EXPORT void NpyIter_DebugPrint(NpyIter *iter);


/*** Global flags that may be passed to the iterator constructors ***/

/* Track an index representing C order */
#define NPY_ITER_C_INDEX                    0x00000001
/* Track an index representing Fortran order */
#define NPY_ITER_F_INDEX                    0x00000002
/* Track coordinates */
#define NPY_ITER_COORDS                     0x00000004
/* Let the caller handle the inner loop of iteration */
#define NPY_ITER_NO_INNER_ITERATION         0x00000008
/* Convert all the operands to a common data type */
#define NPY_ITER_COMMON_DTYPE               0x00000010
/* Enables sub-range iteration */
#define NPY_ITER_RANGED                     0x00000020
/* Enables buffering */
#define NPY_ITER_BUFFERED                   0x00000040
/* When buffering is enabled, grows the inner loop if possible */
#define NPY_ITER_GROWINNER                  0x00000080
/* Delay allocation of buffers until first Reset* call */
#define NPY_ITER_DELAY_BUFALLOC             0x00000100

/*** Per-operand flags that may be passed to the iterator constructors ***/

/* The operand will be read from and written to */
#define NPY_ITER_READWRITE                  0x00010000
/* The operand will only be read from */
#define NPY_ITER_READONLY                   0x00020000
/* The operand will only be written to */
#define NPY_ITER_WRITEONLY                  0x00040000
/* The operand's data must be in native byte order and aligned */
#define NPY_ITER_NBO_ALIGNED                0x00080000
/* The operand may be copied to satisfy requirements */
#define NPY_ITER_COPY                       0x00100000
/* The operand may be copied with UPDATEIFCOPY to satisfy requirements */
#define NPY_ITER_UPDATEIFCOPY               0x00200000
/* Allow writeable operands to have references or pointers */
#define NPY_ITER_WRITEABLE_REFERENCES       0x00400000
/* Allocate the operand if it is NULL */
#define NPY_ITER_ALLOCATE                   0x00800000
/* If an operand is allocated, don't use any subtype */
#define NPY_ITER_NO_SUBTYPE                 0x01000000
/* Require that the dimension match the iterator dimensions exactly */
#define NPY_ITER_NO_BROADCAST               0x02000000

#define NPY_ITER_GLOBAL_FLAGS               0x0000ffff
#define NPY_ITER_PER_OP_FLAGS               0xffff0000

#endif
