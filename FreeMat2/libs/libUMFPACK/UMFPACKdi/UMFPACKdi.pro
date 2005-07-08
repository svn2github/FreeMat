TEMPLATE = lib

TARGET = UMFPACKdi 

CONFIG += staticlib

INCLUDEPATH += ../AMD/Source ../UMFPACK/Include ../UMFPACK/Source ../AMD/Include

DEFINES += DINT

SOURCES += ../UMFPACK/Source/umf_2by2.c \
../UMFPACK/Source/umf_analyze.c \
../UMFPACK/Source/umf_apply_order.c \
../UMFPACK/Source/umf_blas3_update.c \
../UMFPACK/Source/umf_build_tuples.c \
../UMFPACK/Source/umf_colamd.c \
../UMFPACK/Source/umf_create_element.c \
../UMFPACK/Source/umf_dump.c \
../UMFPACK/Source/umf_extend_front.c \
../UMFPACK/Source/umf_free.c \
../UMFPACK/Source/umf_fsize.c \
../UMFPACK/Source/umf_garbage_collection.c \
../UMFPACK/Source/umf_get_memory.c \
../UMFPACK/Source/umf_grow_front.c \
../UMFPACK/Source/umf_init_front.c \
../UMFPACK/Source/umf_is_permutation.c \
../UMFPACK/Source/umf_kernel.c \
../UMFPACK/Source/umf_kernel_init.c \
../UMFPACK/Source/umf_kernel_wrapup.c \
../UMFPACK/Source/umf_local_search.c \
../UMFPACK/Source/umf_lsolve.c \
../UMFPACK/Source/umf_malloc.c \
../UMFPACK/Source/umf_mem_alloc_element.c \
../UMFPACK/Source/umf_mem_alloc_head_block.c \
../UMFPACK/Source/umf_mem_alloc_tail_block.c \
../UMFPACK/Source/umf_mem_free_tail_block.c \
../UMFPACK/Source/umf_mem_init_memoryspace.c \
../UMFPACK/Source/umf_multicompile.c \
../UMFPACK/Source/umfpack_col_to_triplet.c \
../UMFPACK/Source/umfpack_defaults.c \
../UMFPACK/Source/umfpack_free_numeric.c \
../UMFPACK/Source/umfpack_free_symbolic.c \
../UMFPACK/Source/umfpack_get_lunz.c \
../UMFPACK/Source/umfpack_get_numeric.c \
../UMFPACK/Source/umfpack_get_symbolic.c \
../UMFPACK/Source/umfpack_load_numeric.c \
../UMFPACK/Source/umfpack_load_symbolic.c \
../UMFPACK/Source/umfpack_numeric.c \
../UMFPACK/Source/umfpack_qsymbolic.c \
../UMFPACK/Source/umfpack_report_control.c \
../UMFPACK/Source/umfpack_report_info.c \
../UMFPACK/Source/umfpack_report_matrix.c \
../UMFPACK/Source/umfpack_report_numeric.c \
../UMFPACK/Source/umfpack_report_perm.c \
../UMFPACK/Source/umfpack_report_status.c \
../UMFPACK/Source/umfpack_report_symbolic.c \
../UMFPACK/Source/umfpack_report_triplet.c \
../UMFPACK/Source/umfpack_report_vector.c \
../UMFPACK/Source/umfpack_save_numeric.c \
../UMFPACK/Source/umfpack_save_symbolic.c \
../UMFPACK/Source/umfpack_scale.c \
../UMFPACK/Source/umfpack_solve.c \
../UMFPACK/Source/umfpack_symbolic.c \
../UMFPACK/Source/umfpack_tictoc.c \
../UMFPACK/Source/umfpack_timer.c \
../UMFPACK/Source/umfpack_transpose.c \
../UMFPACK/Source/umfpack_triplet_to_col.c \
../UMFPACK/Source/umf_realloc.c \
../UMFPACK/Source/umf_report_perm.c \
../UMFPACK/Source/umf_report_vector.c \
../UMFPACK/Source/umf_row_search.c \
../UMFPACK/Source/umf_scale.c \
../UMFPACK/Source/umf_scale_column.c \
../UMFPACK/Source/umf_set_stats.c \
../UMFPACK/Source/umf_singletons.c \
../UMFPACK/Source/umf_solve.c \
../UMFPACK/Source/umf_start_front.c \
../UMFPACK/Source/umf_symbolic_usage.c \
../UMFPACK/Source/umf_transpose.c \
../UMFPACK/Source/umf_tuple_lengths.c \
../UMFPACK/Source/umf_usolve.c \
../UMFPACK/Source/umf_valid_numeric.c \
../UMFPACK/Source/umf_valid_symbolic.c \
../UMFPACK/Source/umf_utsolve.c \
../UMFPACK/Source/umf_ltsolve.c \
../UMFPACK/Source/umf_triplet.c \
../UMFPACK/Source/umf_assemble2.c \
../UMFPACK/Source/umf_store_lu.c

HEADERS += ../UMFPACK/Include/umfpack_col_to_triplet.h \
../UMFPACK/Include/umfpack_defaults.h \
../UMFPACK/Include/umfpack_free_numeric.h \
../UMFPACK/Include/umfpack_free_symbolic.h \
../UMFPACK/Include/umfpack_get_lunz.h \
../UMFPACK/Include/umfpack_get_numeric.h \
../UMFPACK/Include/umfpack_get_symbolic.h \
../UMFPACK/Include/umfpack.h \
../UMFPACK/Include/umfpack_load_numeric.h \
../UMFPACK/Include/umfpack_load_symbolic.h \
../UMFPACK/Include/umfpack_numeric.h \
../UMFPACK/Include/umfpack_qsymbolic.h \
../UMFPACK/Include/umfpack_report_control.h \
../UMFPACK/Include/umfpack_report_info.h \
../UMFPACK/Include/umfpack_report_matrix.h \
../UMFPACK/Include/umfpack_report_numeric.h \
../UMFPACK/Include/umfpack_report_perm.h \
../UMFPACK/Include/umfpack_report_status.h \
../UMFPACK/Include/umfpack_report_symbolic.h \
../UMFPACK/Include/umfpack_report_triplet.h \
../UMFPACK/Include/umfpack_report_vector.h \
../UMFPACK/Include/umfpack_save_numeric.h \
../UMFPACK/Include/umfpack_save_symbolic.h \
../UMFPACK/Include/umfpack_scale.h \
../UMFPACK/Include/umfpack_solve.h \
../UMFPACK/Include/umfpack_symbolic.h \
../UMFPACK/Include/umfpack_tictoc.h \
../UMFPACK/Include/umfpack_timer.h \
../UMFPACK/Include/umfpack_transpose.h \
../UMFPACK/Include/umfpack_triplet_to_col.h \
../UMFPACK/Include/umfpack_wsolve.h \
../UMFPACK/Source/umf_2by2.h \
../UMFPACK/Source/umf_analyze.h \
../UMFPACK/Source/umf_apply_order.h \
../UMFPACK/Source/umf_assemble.h \
../UMFPACK/Source/umf_blas3_update.h \
../UMFPACK/Source/umf_build_tuples.h \
../UMFPACK/Source/umf_colamd.h \
../UMFPACK/Source/umf_config.h \
../UMFPACK/Source/umf_create_element.h \
../UMFPACK/Source/umf_dump.h \
../UMFPACK/Source/umf_extend_front.h \
../UMFPACK/Source/umf_free.h \
../UMFPACK/Source/umf_fsize.h \
../UMFPACK/Source/umf_garbage_collection.h \
../UMFPACK/Source/umf_get_memory.h \
../UMFPACK/Source/umf_grow_front.h \
../UMFPACK/Source/umf_init_front.h \
../UMFPACK/Source/umf_internal.h \
../UMFPACK/Source/umf_is_permutation.h \
../UMFPACK/Source/umf_kernel.h \
../UMFPACK/Source/umf_kernel_init.h \
../UMFPACK/Source/umf_kernel_wrapup.h \
../UMFPACK/Source/umf_local_search.h \
../UMFPACK/Source/umf_lsolve.h \
../UMFPACK/Source/umf_ltsolve.h \
../UMFPACK/Source/umf_malloc.h \
../UMFPACK/Source/umf_mem_alloc_element.h \
../UMFPACK/Source/umf_mem_alloc_head_block.h \
../UMFPACK/Source/umf_mem_alloc_tail_block.h \
../UMFPACK/Source/umf_mem_free_tail_block.h \
../UMFPACK/Source/umf_mem_init_memoryspace.h \
../UMFPACK/Source/umf_realloc.h \
../UMFPACK/Source/umf_report_perm.h \
../UMFPACK/Source/umf_report_vector.h \
../UMFPACK/Source/umf_row_search.h \
../UMFPACK/Source/umf_scale_column.h \
../UMFPACK/Source/umf_scale.h \
../UMFPACK/Source/umf_set_stats.h \
../UMFPACK/Source/umf_singletons.h \
../UMFPACK/Source/umf_solve.h \
../UMFPACK/Source/umf_start_front.h \
../UMFPACK/Source/umf_store_lu.h \
../UMFPACK/Source/umf_symbolic_usage.h \
../UMFPACK/Source/umf_transpose.h \
../UMFPACK/Source/umf_triplet.h \
../UMFPACK/Source/umf_tuple_lengths.h \
../UMFPACK/Source/umf_usolve.h \
../UMFPACK/Source/umf_utsolve.h \
../UMFPACK/Source/umf_valid_numeric.h \
../UMFPACK/Source/umf_valid_symbolic.h \
../UMFPACK/Source/umf_version.h \