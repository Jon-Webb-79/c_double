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
 * @brief returns a c style pointer to the beginning of an array 
 *
 * @param vec A pointer to a double_v data type 
 * @return A pointer to the beginning of a double array
 */
double* c_double_ptr(double_v* vec);
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
// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_d
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_d dict_d;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_d* init_double_dict();
// --------------------------------------------------------------------------------

/**
 * @brief Inserts a key-value pair into the dictionary.
 *
 * Adds a new key-value pair to the dictionary. If the key already exists, the function
 * does nothing and returns false. If the dictionary's load factor exceeds a threshold,
 * it automatically resizes.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to insert.
 * @param value The value associated with the key.
 * @return true if the key-value pair was inserted successfully, false otherwise.
 */
bool insert_double_dict(dict_d* dict, const char* key, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a key-value pair from the dictionary.
 *
 * Finds the specified key in the dictionary, removes the associated key-value pair,
 * and returns the value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to remove.
 * @return The value associated with the key if it was found and removed; FLT_MAX otherwise.
 */
double pop_double_dict(dict_d* dict,  const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Retrieves the value associated with a key.
 *
 * Searches the dictionary for the specified key and returns the corresponding value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to search for.
 * @return The value associated with the key, or FLT_MAX if the key is not found.
 */
double get_double_dict_value(const dict_d* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_double_dict(dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_double_dict(dict_d** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_d objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DDICT_GBC __attribute__((cleanup(_free_double_dict)))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Updates the value associated with a key in the dictionary.
 *
 * Searches for the specified key in the dictionary and updates its value.
 * If the key does not exist, the function takes no action.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to update.
 * @param value The new value to associate with the key.
 */
bool update_double_dict(dict_d* dict, const char* key, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t double_dict_size(const dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t double_dict_alloc(const dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t double_dict_hash_size(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_double_dict(const dict_d* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_d* New dictionary containing copies of all entries, NULL on error
 */
dict_d* copy_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_double_dict(dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return double* Array containing all values, NULL on error
 */
double_v* get_values_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_d* New dictionary containing merged entries, NULL on error
 */
dict_d* merge_double_dict(const dict_d* dict1, const dict_d* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*dict_iterator)(const char* key, double value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_double_dict(const dict_d* dict, dict_iterator iter, void* user_data);
// ================================================================================ 
// ================================================================================ 
// VECTOR DICTIONARY PROTOTYPES 

/**
 * @typedef dict_fv
 * @brief Opaque struct representing a dictionary for vector data types.
 *
 * This structure encapsulates a hash table that maps string keys to a vector of double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_dv dict_dv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of double vectors
 *
 * @returns a dictionary of double vectors
 */
dict_dv* init_doublev_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_doublev_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_doublev_dict(dict_dv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_doublev_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_doublev_dict(dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_doublev_dict 
* @brief Returns a double_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a double_v pointer so it can be used in vector and array functions
*/
double_v* return_doublev_pointer(dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_doublev_dict 
* @brief Returns a double_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_doublev_dict(dict_dv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_doublev_dict(dict_dv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_dv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DDICTV_GBC __attribute__((cleanup(_free_doublev_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The double vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_doublev_dict(const dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The double vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type double_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_doublev_dict(dict_dv* dict, const char* key, double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t double_dictv_size(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t double_dictv_alloc(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t double_dictv_hash_size(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector double dictionary 
 *
 * @param original A double vector dictionary 
 * @return A copy of a dictionary
 */
dict_dv* copy_doublev_dict(const dict_dv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to double vector dictionaries
 *
 * Returns a merged double vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_dv* merge_doublev_dict(const dict_dv* dict1, const dict_dv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_doublev_dict(dict_dv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for double vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated double_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_dv_iterator)(const char* key, const double_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a double_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_doublev_dict(const dict_dv* dict, dict_dv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_doublev_dict(const dict_dv* dict);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a double container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any double container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - double_v (calls double_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param d_struct Pointer to double container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_size(d_struct) _Generic((d_struct), \
    double_v*: double_vector_size, \
    dict_d*: double_dict_size, \
    dict_dv*: double_dictv_size) (d_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a double container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any double container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - double_v (calls double_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to double container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_alloc(d_struct) _Generic((d_struct), \
    double_v*: double_vector_alloc, \
    dict_d*: double_dict_alloc, \
    dict_dv*: double_dictv_alloc) (d_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_double_H */
// ================================================================================
// ================================================================================
// eof
