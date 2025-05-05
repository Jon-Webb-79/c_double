// ================================================================================
// ================================================================================
// - File:    c_double.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 04, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_double_H
#define c_double_H
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
#include "c_string.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

#ifndef ITER_DIR_H
#define ITER_DIR_H
    /**
     * @brief An enum containing keywords for an iterator 
     *
     * This enum contains keywords that are used to describe the order of iteration
     * within an iterator 
     *
     * @attribute FORWARD Keyword to command a forward iteration 
     * @attribute REVERSE Keyword to command a reverese iteration
     */
    typedef enum {
        FORWARD,
        REVERSE
    }iter_dir;
#endif /* ITER_DIR_H*/
// --------------------------------------------------------------------------------    

#ifndef ALLOC_H 
#define ALLOC_H 

    /**
     * @enum alloc_t 
     * @brief An enum to discern if an array is statically or allocated 
     */
    typedef enum {
        STATIC,
        DYNAMIC
    } alloc_t;

#endif /*ALLOC_H*/

/**
* @struct float_v
* @brief Dynamic array (vector) container for float objects
*
* This structure manages a resizable array of double objects with automatic
* memory management and capacity handling.
*/
typedef struct {
    double* data;
    size_t len;
    size_t alloc;
    alloc_t alloc_type;
} double_v;
// --------------------------------------------------------------------------------

/**
* @function init_double_vector
* @brief Initializes a new dynamically allocated double vector with specified initial capacity
*
* @param buffer Initial capacity to allocate
* @return Pointer to new double_v object, or NULL on allocation failure
*         Sets errno to ENOMEM if memory allocation fails
*/
double_v* init_double_vector(size_t buffer);
// -------------------------------------------------------------------------------- 

/**
 * @function init_double_static_array
 * @brief Creates a stack-based double vector with static array
 *
 * @param size Size of the array
 */
#define init_double_array(size) \
    ((double_v){.data = (double[size]){0}, .len = 0, .alloc = size, .alloc_type = STATIC})
// --------------------------------------------------------------------------------

/**
* @function push_back_double_vector
* @brief Adds a double value to the end of the vector
*
* Automatically resizes the vector if necessary.
*
* @param vec Target double vector
* @param value Double value to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_back_double_vector(double_v* vec, const double value);
// --------------------------------------------------------------------------------

/**
* @function push_front_double_vector
* @brief Adds a double value to the beginning of the vector
*
* Shifts existing elements right and automatically resizes if necessary.
*
* @param vec Target double vector
* @param value Double value to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_front_double_vector(double_v* vec, const double value);
// --------------------------------------------------------------------------------

/**
* @function insert_double_vector
* @brief Inserts a double value at specified index in the vector
*
* Shifts elements right starting at index and resizes if necessary.
*
* @param vec Target double vector
* @param value Double value to insert
* @param index Position to insert at
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or index out of bounds, ENOMEM on allocation failure
*/
bool insert_double_vector(double_v* vec, const double value, size_t index);
// --------------------------------------------------------------------------------

/**
* @function double_vector_index
* @brief Retrieves pointer to string_t at specified index
*
* @param vec Source double vector
* @param index Position to access
* @return Pointer to string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
const double double_vector_index(const double_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
* @function double_vector_size
* @brief Returns current number of strings in vector
*
* @param vec Double vector to query
* @return Number of strings in vector, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t double_vector_size(const double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function double_vector_alloc
* @brief Returns current allocation size of vector
*
* @param vec Double vector to query
* @return Current allocation capacity, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t double_vector_alloc(const double_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pop_back_double_vector
* @brief Removes and returns last double value in vector
*
* @param vec Source double vector
* @return Pointer to removed double object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
double pop_back_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function pop_front_double_vector
* @brief Removes and returns first double value in vector
*
* Shifts remaining elements left.
*
* @param vec Source string vector
* @return Pointer to removed double object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
double pop_front_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pup_any_double_vector
* @brief Removes and returns double value at specified index
*
* Shifts remaining elements left to fill gap.
*
* @param vec Source double vector
* @param index Position to remove from
* @return Pointer to removed double_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
double pop_any_double_vector(double_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function free_double_vector
* @brief Frees all memory associated with string vector
*
* Frees all contained strings and the vector itself.
*
* @param vec Double vector to free
* @return void
*         Sets errno to EINVAL for NULL input
*/
void free_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
* @function _free_double_vector
* @brief Helper function for garbage collection of double vectors
*
* Used with DBLEVEC_GBC macro for automatic cleanup.
*
* @param vec Double pointer to double vector to free
* @return void
*/
void _free_double_vector(double_v** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro FLTVEC_GBC
     * @brief A macro for enabling automatic cleanup of double vector objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_double_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DBLEVEC_GBC __attribute__((cleanup(_free_double_vector)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @function reverse_double_vector
 * @brief Reverses the order of elements in a double vector in place.
 *
 * The function reverses the order of elements by swapping elements from the
 * beginning and end of the vector until the middle is reached.
 *
 * @param vec double vector to reverse
 * @return void
 *         Sets errno to EINVAL if vec is NULL or invalid
 */
void reverse_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @function swap_double
 * @brief Swaps the contents of two double objects.
 *
 * Performs an in-place swap of double values
 *
 * @param a Pointer to first double object
 * @param b Pointer to second double object
 * @return void
 *         Sets errno to EINVAL if either input is NULL
 */
void swap_double(double* a, double* b);
// --------------------------------------------------------------------------------

/**
* @function sort_double_vector
* @brief Sorts a double vector in ascending or descending order.
*
* Uses an optimized QuickSort algorithm with median-of-three pivot selection
* and insertion sort for small subarrays. Sort direction is determined by
* the iter_dir parameter.
*
* @param vec double vector to sort
* @param direction FORWARD for ascending order, REVERSE for descending
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void sort_double_vector(double_v* vec, iter_dir direction);
// -------------------------------------------------------------------------------- 

/**
* @function trim_double_vector
* @brief Trims all un-necessary memory from a vector
*
* @param vec double vector to trim
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void trim_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function binary_search_double_vector
* @brief Searches a double vector to find the index where a value exists
*
* @param vec double vector object
* @param value The value to search for
* @param tolerance The double tolerance for finding a value 
* @param sort_first true if the vector or array needs to be sorted, false otherwise
* @return The index where a value exists, LONG_MAX if the value is not in the array.
*         Sets errno to EINVAL if vec is NULL or invalid, ENODATA if the array is 
*         not populated
*/
size_t binary_search_double_vector(double_v* vec, double value, double tolerance, bool sort_first);
// -------------------------------------------------------------------------------- 

/**
* @function update_double_vector
* @brief Replaces the value of a vector at a specific index
*
* @param vec double vector object
* @param index The index where data will be replaced
* @param replacement_value The replacement value
* @return void, Sets errno to EINVAL if vec does not exsist, or ERANGE 
*         if the index is out of bounds
*/
void update_double_vector(double_v* vec, size_t index, double replacement_value);
// -------------------------------------------------------------------------------- 

/**
 * @function min_double_vector 
 * @brief Returns the minimum value in a vector or array 
 *
 * @param vec A double vector or array object 
 * @return The minimum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double min_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function max_double_vector 
 * @brief Returns the maximum value in a vector or array 
 *
 * @param vec A double vector or array object 
 * @return The maximum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double max_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function sum_double_vector 
 * @brief Returns the summation of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The summation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double sum_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function average_double_vector 
 * @brief Returns the average of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The average of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double average_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function stdev_double_vector 
 * @brief Returns the standard deviation of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The standard deviation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double stdev_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function cum_sum_double_vector 
 * @brief Returns a dynamically allocated array containing the cumulative sum of all 
 *        values in vec
 *
 * @param vec A double vector or array object 
 * @return A double_v object with the cumulative sum of all values in vec.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double_v* cum_sum_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief creates a deep copy of a vector
 *
 * @param original A vector to be copied 
 * @return A copy of a double vector
 */
double_v* copy_double_vector(const double_v* original);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a float container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any float container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - float_v (calls float_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param d_struct Pointer to float container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_size(d_struct) _Generic((d_struct), \
    double_v*: double_vector_size) (d_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a float container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any float container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - float_v (calls float_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to float container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_alloc(d_struct) _Generic((d_struct), \
    double_v*: double_vector_alloc) (d_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_double_H */
// ================================================================================
// ================================================================================
// eof
