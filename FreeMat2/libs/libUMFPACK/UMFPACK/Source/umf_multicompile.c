#define CONJUGATE_SOLVE
#include "umf_utsolve.c"
#include "umf_ltsolve.c"
#undef CONJUGATE_SOLVE

#define DO_MAP
#define DO_VALUES
#include "umf_triplet.c"
#undef DO_VALUES
#undef DO_MAP

#define DO_MAP
#include "umf_triplet.c"
#undef DO_MAP

#define DO_VALUES
#include "umf_triplet.c"
#undef DO_VALUES

#define FIXQ
#include "umf_assemble.c"
#undef FIXQ

#define DROP
#include "umf_store_lu.c"
#undef DROP

#define WSOLVE
#include "umfpack_solve.c"
#undef WSOLVE
