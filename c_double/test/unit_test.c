// ================================================================================
// ================================================================================
// - File:    unit_test.c
// - Purpose: This file implements a cmocka test environment for the c_float 
//            library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 0.1
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

//#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "test_vector.h"
// ================================================================================ 
// ================================================================================ 

const struct CMUnitTest test_vector[] = {
	cmocka_unit_test(test_init_double_vector_success),
    cmocka_unit_test(test_init_double_vector_zero_size),
    cmocka_unit_test(test_init_double_array_success),
    cmocka_unit_test(test_free_double_vector_null),
    cmocka_unit_test(test_free_double_vector_static),
    #if defined(__GNUC__) || defined(__clang__)
        cmocka_unit_test(test_double_vector_gbc),
    #endif
    cmocka_unit_test(test_push_back_basic),
    cmocka_unit_test(test_push_back_growth),
    cmocka_unit_test(test_push_back_static),
    cmocka_unit_test(test_generic_macros),
    cmocka_unit_test(test_double_vector_index),
    cmocka_unit_test(test_push_back_special_values),
    cmocka_unit_test(test_static_array_bounds),
    cmocka_unit_test(test_static_array_index_bounds),
    cmocka_unit_test(test_static_array_initialization),
    cmocka_unit_test(test_static_array_free),
    cmocka_unit_test(test_push_front_basic),
    cmocka_unit_test(test_push_front_growth),
    cmocka_unit_test(test_push_front_static),
    cmocka_unit_test(test_push_front_special_values),
    cmocka_unit_test(test_push_front_error_cases),
    cmocka_unit_test(test_insert_vector_basic),
    cmocka_unit_test(test_insert_vector_growth),
    cmocka_unit_test(test_insert_array_basic),
    cmocka_unit_test(test_insert_array_bounds),
    cmocka_unit_test(test_insert_error_cases),
    cmocka_unit_test(test_insert_special_values),
    cmocka_unit_test(test_pop_back_basic),
    cmocka_unit_test(test_pop_back_empty),
    cmocka_unit_test(test_pop_back_errors),
    cmocka_unit_test(test_pop_back_special_values),
    cmocka_unit_test(test_pop_back_static),
    cmocka_unit_test(test_pop_front_basic),
    cmocka_unit_test(test_pop_front_empty),
    cmocka_unit_test(test_pop_front_errors),
    cmocka_unit_test(test_pop_front_special_values),
    cmocka_unit_test(test_pop_front_static),
    cmocka_unit_test(test_pop_any_basic),
    cmocka_unit_test(test_pop_any_errors),
    cmocka_unit_test(test_pop_any_static),
    cmocka_unit_test(test_pop_any_special_values),
    cmocka_unit_test(test_reverse_basic),
    cmocka_unit_test(test_reverse_errors),
    cmocka_unit_test(test_reverse_static),
    cmocka_unit_test(test_reverse_special_values),
    cmocka_unit_test(test_sort_basic),
    cmocka_unit_test(test_sort_edge_cases),
    cmocka_unit_test(test_sort_duplicates),
    cmocka_unit_test(test_sort_special_values),
    cmocka_unit_test(test_sort_static_array),
    cmocka_unit_test(test_sort_errors),
    cmocka_unit_test(test_trim_basic),
    cmocka_unit_test(test_trim_empty_vector),
    cmocka_unit_test(test_trim_static_array),
    cmocka_unit_test(test_trim_already_optimal),
    cmocka_unit_test(test_trim_errors),
    cmocka_unit_test(test_binary_search_basic),
    cmocka_unit_test(test_binary_search_tolerance),
    cmocka_unit_test(test_binary_search_with_sort),
    cmocka_unit_test(test_binary_search_errors),
    cmocka_unit_test(test_binary_search_static),
    cmocka_unit_test(test_update_double_vector_nominal),
    cmocka_unit_test(test_update_double_vector_bad_index),
    cmocka_unit_test(test_update_double_vector_null),
    cmocka_unit_test(test_min_double_basic),
    cmocka_unit_test(test_max_double_basic),
    cmocka_unit_test(test_min_max_special_values),
    cmocka_unit_test(test_min_max_static_array),
    cmocka_unit_test(test_min_max_errors),
    cmocka_unit_test(test_sum_basic),
    cmocka_unit_test(test_average_basic),
    cmocka_unit_test(test_sum_average_special_values),
    cmocka_unit_test(test_sum_average_negative),
    cmocka_unit_test(test_sum_average_static),
    cmocka_unit_test(test_sum_average_errors),
    cmocka_unit_test(test_stdev_basic),
    cmocka_unit_test(test_stdev_single_value),
    cmocka_unit_test(test_stdev_same_values),
    cmocka_unit_test(test_cum_sum_basic),
    cmocka_unit_test(test_cum_sum_negative),
    cmocka_unit_test(test_stdev_cum_sum_special_values),
    cmocka_unit_test(test_stdev_cum_sum_errors)
};
// -------------------------------------------------------------------------------- 

const struct CMUnitTest test_dict[] = {
    cmocka_unit_test(test_dictionary_gbc),
    cmocka_unit_test_setup_teardown(test_insert_double_dict_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_insert_double_dict_duplicate, setup, teardown),
    cmocka_unit_test_setup_teardown(test_insert_double_dict_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_double_dict_value_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_double_dict_value_missing, setup, teardown),
    cmocka_unit_test_setup_teardown(test_update_double_dict_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_update_double_dict_missing, setup, teardown),
    cmocka_unit_test_setup_teardown(test_pop_double_dict_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_pop_double_dict_missing, setup, teardown),
    cmocka_unit_test_setup_teardown(test_resize_behavior, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_keys_double_dict, setup, teardown),
    cmocka_unit_test_setup_teardown(test_get_values_double_dict, setup, teardown),
    cmocka_unit_test_setup_teardown(test_foreach_double_dict_basic, setup, teardown),
    cmocka_unit_test_setup_teardown(test_foreach_double_dict_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_foreach_double_dict_null, setup, teardown),
    cmocka_unit_test(test_vector_dictionary),
    cmocka_unit_test(test_vector_dictionary_resize),
    cmocka_unit_test(test_vector_dictionary_gbc),
    cmocka_unit_test(test_pop_vector_dictionary),
    cmocka_unit_test(test_insert_doublev_dict_basic),
    cmocka_unit_test(test_doublev_size_macros),
    cmocka_unit_test(test_copy_doublev_dict_success),
    cmocka_unit_test(test_copy_doublev_dict_null_input),
    cmocka_unit_test(test_copy_doublev_dict_static_vector),
    cmocka_unit_test(test_copy_doublev_dict_multiple_entries),
    cmocka_unit_test(test_copy_doublev_dict_independence),
    cmocka_unit_test(test_merge_doublev_dict_unique_keys),
    cmocka_unit_test(test_merge_doublev_dict_no_overwrite),
    cmocka_unit_test(test_merge_doublev_dict_overwrite),
    cmocka_unit_test(test_merge_doublev_dict_reject_static),
    cmocka_unit_test(test_merge_doublev_dict_null_inputs),
    cmocka_unit_test(test_clear_doublev_dict_basic),
    cmocka_unit_test(test_clear_doublev_dict_empty),
    cmocka_unit_test(test_clear_doublev_dict_reuse_after_clear),
    cmocka_unit_test(test_foreach_doublev_dict_counts_keys),
    cmocka_unit_test(test_foreach_doublev_dict_with_null_dict),
    cmocka_unit_test(test_foreach_doublev_dict_with_null_callback),
    cmocka_unit_test(test_foreach_doublev_dict_accumulates_sum),
};
// ================================================================================ 
// ================================================================================ 
// Begin code
int main(int argc, const char * argv[]) {
    int status;
    status = cmocka_run_group_tests(test_vector, NULL, NULL);
    if (status != 0) 
        return status;	
    return cmocka_run_group_tests(test_dict, NULL, NULL);
}
// ================================================================================ 
// ================================================================================ 
// eof
