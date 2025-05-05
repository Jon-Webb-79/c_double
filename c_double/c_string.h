// ================================================================================
// ================================================================================
// - File:    c_string.h
// - Purpose: This file contains the prototypes for functions used in the 
//            c_string library/
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_string_H
#define c_string_H

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @struct string_t
 * @brief Forward declaration for a dynamic data structure for storing strings.
 *
 * Fields:
 *  - chart* str: Pointer to a string literal
 *  - size_t len: The current number of elements in the arrays.
 *  - size_t alloc: The total allocated capacity of the arrays.
 */
typedef struct string_t string_t;
// --------------------------------------------------------------------------------

/**
 * @function init_string
 * @brief Allocates and initializes a dynamically allocated string_t object.
 *
 * The function initializes the string_t structure with the contents of the provided
 * string, copying the string into dynamically allocated memory.
 *
 * @param str A null-terminated C string to initialize the string_t object with.
 * @return A pointer to the initialized string_t object, or NULL on failure.
 *         Sets errno to ENOMEM if memory allocation fails or EINVAL if `str` is NULL.
 */
string_t* init_string(const char* str);
// --------------------------------------------------------------------------------

/**
 * @function free_string
 * @brief Frees all memory associated with a string_t object.
 *
 * The function releases the memory for the internal string and the string_t structure itself.
 *
 * @param str A pointer to the string_t object to be freed.
 * @return void. Logs an error if `str` is NULL.
 */
void free_string(string_t* str);
// -------------------------------------------------------------------------------- 

/**
 * @function _free_string
 * @brief A helper function for use with cleanup attributes to free string_t objects.
 *
 * This function frees the memory of a string_t object and sets the pointer to NULL.
 *
 * @param str A double pointer to the string_t object to be freed.
 * @return void.
 */
void _free_string(string_t** str);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro STRING_GBC
     * @brief A macro for enabling automatic cleanup of string_t objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_string`
     * when the scope ends, ensuring proper memory management.
     */
    #define STRING_GBC __attribute__((cleanup(_free_string)))
#endif
// --------------------------------------------------------------------------------

/**
 * @function get_string
 * @brief Retrieves the C string stored in a string_t object.
 *
 * @param str A pointer to the string_t object.
 * @return A pointer to the null-terminated C string stored in the object,
 *         or NULL if `str` is NULL or invalid. Sets errno to EINVAL on error.
 */
const char* get_string(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function string_size
 * @brief Retrieves the current length of the string stored in a string_t object.
 *
 * @param str A pointer to the string_t object.
 * @return The length of the string in bytes (excluding the null terminator),
 *         or -1 on error. Sets errno to EINVAL if `str` is NULL.
 */
const size_t string_size(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function string_alloc
 * @brief Retrieves the total allocated capacity of the string in a string_t object.
 *
 * @param str A pointer to the string_t object.
 * @return The total allocated capacity in bytes, or -1 on error.
 *         Sets errno to EINVAL if `str` is NULL.
 */
const size_t string_alloc(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function string_string_concat
 * @brief Concatenates the contents of one string_t object to another.
 *
 * @param str1 A pointer to the destination string_t object.
 * @param str2 A pointer to the source string_t object.
 * @return true if successful, false on failure. Sets errno to ENOMEM if memory
 *         allocation fails or EINVAL if either input is NULL.
 */
bool string_string_concat(string_t* str, const string_t* string);
// -------------------------------------------------------------------------------- 

/**
 * @function string_lit_concat
 * @brief Concatenates a string literal to a string_t object.
 *
 * @param str1 A pointer to the destination string_t object.
 * @param literal A null-terminated C string to append to the string_t object.
 * @return true if successful, false on failure. Sets errno to ENOMEM if memory
 *         allocation fails or EINVAL if either input is NULL.
 */
bool string_lit_concat(string_t* str, const char* string);
// --------------------------------------------------------------------------------

/**
 * @macro string_concat
 * @brief A generic macro that selects the appropriate concatenation function
 *        based on the type of the second argument.
 *
 * If the second argument is a `char*`, it calls `string_lit_concat`.
 * Otherwise, it calls `string_string_concat`.
 */
#define string_concat(str_one, str_two) _Generic((str_two), \
    char*: string_lit_concat, \
    default: string_string_concat) (str_one, str_two)
// --------------------------------------------------------------------------------

/**
 * @function compare_strings_lit
 * @brief Compares a string_t object with a string literal.
 *
 * The comparison is lexicographical and case-sensitive.
 *
 * @param str_struct A pointer to the string_t object.
 * @param string A null-terminated C string to compare with.
 * @return An integer less than, equal to, or greater than zero if the string_t
 *         is found, respectively, to be less than, to match, or to be greater than `string`.
 *         Returns INT_MIN on error (sets errno to EINVAL).
 */
int compare_strings_lit(const string_t* str_struct, const char* string);
// --------------------------------------------------------------------------------

/**
 * @function compare_strings_string
 * @brief Compares two string_t objects.
 *
 * The comparison is lexicographical and case-sensitive.
 *
 * @param str_struct_one A pointer to the first string_t object.
 * @param str_struct_two A pointer to the second string_t object.
 * @return An integer less than, equal to, or greater than zero if the first string_t
 *         is found, respectively, to be less than, to match, or to be greater than the second string_t.
 *         Returns INT_MIN on error (sets errno to EINVAL).
 */
int compare_strings_string(const string_t* str_struct_one, string_t* str_struct_two);
// --------------------------------------------------------------------------------

/**
 * @macro compare_strings
 * @brief A generic macro that selects the appropriate string comparison function
 *        based on the type of the second argument.
 *
 * If the second argument is a `char*`, it calls `compare_strings_lit`.
 * Otherwise, it calls `compare_strings_string`.
 */
#define compare_strings(str_one, str_two) _Generic((str_two), \
    char*: compare_strings_lit, \
    default: compare_strings_string) (str_one, str_two)
// --------------------------------------------------------------------------------

/**
 * @function copy_string
 * @brief Creates a deep copy of a string data type
 *
 * @param str A string_t data type
 * @return A string_t data type with the exact contents of str
 */
string_t* copy_string(const string_t *str);
// --------------------------------------------------------------------------------

/**
 * @function reserve_string
 * @brief Reserves memory for the string_t data type to avoid constant memory allocations
 *
 * Will not allow the user to reserve less memory than exists at function call 
 *
 * @param str A string_t data type
 * @param len The buffer length to allocate in string
 * @return true of allocation is successful, false otherwise
 */
bool reserve_string(string_t* str, size_t len);
// -------------------------------------------------------------------------------- 

/**
 * @brief Tims the string memory to the minimum necessary size 
 *
 * THis function will determine the minimum memory allocation needed to fit 
 * the string and will resize to the minimum memory if oversized.
 *
 * @param str A string container of type string_t
 * @return true if operation is succesful, false otherwise with stderr printout
 */
bool trim_string(string_t* str);
// -------------------------------------------------------------------------------- 

/**
 * @brief Finds the first occurance of a char between two pointers
 *
 * This function deterimines the first appearance of a char value in 
 * a string literal.
 *
 * @param str A pointer to the string_t data type
 * @param value The char value being search for in the string_t data type
 * @return A char pointer to character value or a NULL pointer.
 */
char* first_char_occurance(string_t* str, char value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Finds the last occurance of a char between two pointers
 *
 * This function deterimines the first appearance of a char value in 
 * a string literal.
 *
 * @param str A pointer to the string_t data type
 * @param value The char value being search for in the string_t data type
 * @return A char pointer to character value or a NULL pointer.
 */
char* last_char_occurance(string_t* str, char value);
// -------------------------------------------------------------------------------- 

/**
* @function first_lit_substr_occurance
* @brief Finds the first occurrence of a C string literal substring within a string_t object.
*
* @param str The string_t object to search within
* @param sub_str The C string literal to search for
* @return Pointer to the beginning of the first occurrence of sub_str, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* first_lit_substr_occurrence(string_t* str, char* sub_str);
// --------------------------------------------------------------------------------

/**
* @function first_string_substr_occurance
* @brief Finds the first occurrence of a string_t substring within another string_t object.
*
* @param str_1 The string_t object to search within
* @param str_2 The string_t substring to search for
* @return Pointer to the beginning of the first occurrence of str_2, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* first_string_substr_occurrence(string_t* str_1, string_t* str_2);
// -------------------------------------------------------------------------------- 

/**
* @macro first_substr_occurance
* @brief A generic macro that selects the appropriate substring search function
*        based on the type of the second argument.
*
* If the second argument is a char*, calls first_lit_substr_occurance.
* If the second argument is a string_t*, calls first_string_substr_occurance.
*
* Example usage:
*     first_substr_occurance(str, "substring")     // Uses literal version
*     first_substr_occurance(str1, str2)          // Uses string_t version
*/
#define first_substr_occurrence(str1, str2) _Generic((str2), \
    char*: first_lit_substr_occurrence, \
    string_t*: first_string_substr_occurrence) (str1, str2)
// -------------------------------------------------------------------------------- 

/**
* @function last_lit_substr_occurance
* @brief Finds the last occurrence of a C string literal substring within a string_t object.
*
* @param str The string_t object to search within
* @param sub_str The C string literal to search for
* @return Pointer to the beginning of the last occurrence of sub_str, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* last_lit_substr_occurrence(string_t* str, char* sub_str);
// --------------------------------------------------------------------------------

/**
* @function last_string_substr_occurance
* @brief Finds the last occurrence of a string_t substring within another string_t object.
*
* @param str_1 The string_t object to search within
* @param str_2 The string_t substring to search for
* @return Pointer to the beginning of the last occurrence of str_2, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* last_string_substr_occurrence(string_t* str_1, string_t* str_2);
// -------------------------------------------------------------------------------- 

/**
* @macro last_substr_occurance
* @brief A generic macro that selects the appropriate substring search function
*        based on the type of the second argument.
*
* If the second argument is a char*, calls last_lit_substr_occurance.
* If the second argument is a string_t*, calls last_string_substr_occurance.
*
* Example usage:
*     last_substr_occurance(str, "substring")      // Uses literal version
*     last_substr_occurance(str1, str2)           // Uses string_t version
*/
#define last_substr_occurrence(str1, str2) _Generic((str2), \
    char*: last_lit_substr_occurrence, \
    string_t*: last_string_substr_occurrence) (str1, str2)
// --------------------------------------------------------------------------------

/**
* @function first_char
* @brief Returns a pointer to the beginning of a string
*
* @param str A string_t object
* @return Pointer to the beginning of the string NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* first_char(string_t* str);
// --------------------------------------------------------------------------------

/**
* @function last_char
* @brief Returns a pointer to the end of a string
*
* @param str A string_t object
* @return Pointer to the end of the string NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* last_char(string_t* str);
// --------------------------------------------------------------------------------

/**
* @function is_string_ptr
* @brief Deterimes if a pointer is within the bounds of a function
*
* @param str A string_t object
* @param ptr A char pointer 
* @return true if the pointer is within the string bounds, 
*         false otherwise.  If str, str->str, or ptr are NULL,
*         the function will return false and set errno to EINVAL
*/
bool is_string_ptr(string_t* str, char* ptr);
// --------------------------------------------------------------------------------

/**
* @function drop_lit_substr
* @brief Removes all occurrences of a C string literal substring between two pointers.
*
* Searches from end to beginning of the specified range and removes each occurrence
* of the substring, preserving existing spaces between words.
*
* @param string string_t object to modify
* @param substring C string literal to remove
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL or range invalid
*         Sets errno to ERANGE if pointers are out of bounds
*/
bool drop_lit_substr(string_t* string, const char* substring, char* min_ptr,
                     char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
* @function drop_string_substr
* @brief Removes all occurrences of a string_t substring between two pointers.
*
* Searches from end to beginning of the specified range and removes each occurrence
* of the substring, preserving existing spaces between words.
*
* @param string string_t object to modify
* @param substring string_t object containing substring to remove
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL or range invalid
*         Sets errno to ERANGE if pointers are out of bounds
*/
bool drop_string_substr(string_t* string, const string_t* substring, char* min_ptr,
                        char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
* @macro drop_substr
* @brief Generic macro that selects appropriate substring removal function based on type.
*
* If the substring is char*, calls drop_lit_substr.
* If the substring is string_t*, calls drop_string_substr.
*
* Example usage:
*     drop_substr(str, "hello", start, end)      // Uses literal version
*     drop_substr(str1, str2, start, end)        // Uses string_t version
*/
#define drop_substr(string, substr, min_ptr, max_ptr) _Generic((substr), \
    char*: drop_lit_substr, \
    string_t*: drop_string_substr) (string, substr, min_ptr, max_ptr)
// -------------------------------------------------------------------------------- 

/**
* @function replace_lit_substring
* @brief Replaces all occurrences of a C string literal pattern with a replacement string
*        between two specified pointers in a string_t object.
*
* If the replacement string is longer than the pattern, memory will be reallocated as needed.
* If shorter, the string will be compacted. The function maintains proper null termination.
*
* @param string string_t object to modify
* @param pattern const C string literal to search for and replace
* @param replace_string const C string literal to replace pattern with
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL
*         Sets errno to ERANGE if pointers are out of bounds
*         Sets errno to ENOMEM if memory reallocation fails
*/
bool replace_lit_substr(string_t* string, const char* pattern, const char* replace_string,
                        char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
* @function replace_string_substring
* @brief Replaces all occurrences of a string_t pattern with another string_t
*        between two specified pointers in a string_t object.
*
* If the replacement string is longer than the pattern, memory will be reallocated as needed.
* If shorter, the string will be compacted. The function maintains proper null termination.
*
* @param string string_t object to modify
* @param pattern const string_t object containing pattern to search for and replace
* @param replace_string const string_t object containing string to replace pattern with
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL
*         Sets errno to ERANGE if pointers are out of bounds
*         Sets errno to ENOMEM if memory reallocation fails
*/
bool replace_string_substr(string_t* string, const string_t* pattern, const string_t* replace_string,
                           char* min_ptr, char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
* @macro replace_substr
* @brief Generic macro that selects appropriate substring replacement function based on type.
*
* If the pattern is char*, calls replace_lit_substring.
* If the pattern is string_t*, calls replace_string_substring.
*
* Example usage:
*     replace_substr(str, "old", "new", start, end)          // Uses literal version
*     replace_substr(str1, pattern, replacement, start, end)  // Uses string_t version
*/
#define replace_substr(string, pattern, replace_string, min_ptr, max_ptr) _Generic((pattern), \
    char*: replace_lit_substr, \
    string_t*: replace_string_substr) (string, pattern, replace_string, min_ptr, max_ptr)
// ================================================================================
// ================================================================================ 

/**
 * @func to_upper_char
 * @brief Transforms a char value to its uppercase form 
 *
 * Sets the value of errno to EINVAL if val points to a NULL value
 *
 * @param val A pointer to he char value to be transformed to its uppercase form
 */
void to_upper_char(char* val);
// --------------------------------------------------------------------------------

/**
 * @func to_lower_char
 * @brief Transforms a char value to its lowercase form 
 *
 * Sets the value of errno to EINVAL if val points to a NULL value
 *
 * @param val A pointer to he char value to be transformed to its lowercase form
 */
void to_lower_char(char* val);
// --------------------------------------------------------------------------------

/**
 * @func to_uppercase
 * @brief Transforms all values in a string to uppercase
 *
 * Sets the value of errno to EINVAL if val points to a NULL value or a null value 
 * of val->str
 *
 * @param val A pointer to a string_t data type
 */
void to_uppercase(string_t* val);
// --------------------------------------------------------------------------------

/**
 * @func to_lowercase
 * @brief Transforms all values in a string to lowercase
 *
 * Sets the value of errno to EINVAL if val points to a NULL value or a null value 
 * of val->str
 *
 * @param val A pointer to a string_t data type
 */
void to_lowercase(string_t* val);
// --------------------------------------------------------------------------------

/**
* @function pop_string_token
* @brief Splits a string at the rightmost occurrence of a token character.
*
* Returns the portion of the string after the token as a new string_t object and
* modifies the original string to contain only the portion before the token.
* The token character is removed from the original string.
*
* @param str_struct string_t object to split
* @param token Character to use as splitting token
* @return New string_t object containing portion after token, or NULL if token not found
*         Sets errno to EINVAL if str_struct is NULL
*/
string_t* pop_string_token(string_t* str_struct, char token);
// --------------------------------------------------------------------------------

/**
* @function token_count
* @brief Counts the number of tokens in a string separated by specified delimiter(s).
*
* Consecutive delimiters are treated as a single delimiter. Leading and trailing
* delimiters are ignored.
*
* @param str string_t object to analyze
* @param delim String containing delimiter character(s)
* @return Number of tokens found, or 0 if str is empty or on error
*         Sets errno to EINVAL if str or delim is NULL
*
* Example:
*     string_t* str = init_string("hello world there");
*     size_t count = token_count(str, " ");  // Returns 3
*
*     string_t* str2 = init_string("one,two;three");
*     size_t count2 = token_count(str2, ",;");  // Returns 3
*/
size_t token_count(const string_t* str, const char* delim);
// --------------------------------------------------------------------------------

/**
 * @function get_char 
 * @brief Returns a chare to the user from a string_t data structure.
 *
 * @param str A string_t data type 
 * @param index The index from within the string_t data type from which a char will 
 *              be retrieved 
 * @return A char value.  Sets errno to EINVAL if str or str->str is NULL, or
 *         ERANGE if index is out of range
 */
char get_char(string_t* str, size_t index);
// --------------------------------------------------------------------------------

/**
 * @function replace_char 
 * @brief Replaces an existing char value in a string_t data type with another 
 *
 * Sets errno to EINVAL if str or str->str is NULL or ERANGE if index is out
 * of range
 *
 * @param str A string_t data type 
 * @param index The index within str where a char value will be replaced 
 * @param value The char value to replace with 
 *
 */
void replace_char(string_t* str, size_t index, char value);
// --------------------------------------------------------------------------------

/**
 * @function trim_leading_whitespace 
 * @brief Removes any white space at the leading edge of a string 
 *
 * Sets errno to EINVAL if str or str->str is NULL
 *
 * @param str A string_t data type 
 */
void trim_leading_whitespace(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function trim_trailing_whitespace 
 * @brief Removes any white space at the trailing edge of a string 
 *
 * Sets errno to EINVAL if str or str->str is NULL
 *
 * @param str A string_t data type 
 */
void trim_trailing_whitespace(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function trim_all_whitespace 
 * @brief Removes all white space in a string 
 *
 * Sets errno to EINVAL if str or str->str is NULL
 *
 * @param str A string_t data type 
 */
void trim_all_whitespace(string_t* str);
// ================================================================================
// ================================================================================
// STRING ITERATOR

typedef struct str_iter {
    char* (*begin) (string_t *s);
    char* (*end) (string_t *s);
    void (*next) (char** current);
    void (*prev) (char** current);
    char (*get) (char** current);
} str_iter;
// -------------------------------------------------------------------------------- 

str_iter init_str_iter();
// ================================================================================ 
// ================================================================================ 

/**
* @struct string_v
* @brief Dynamic array (vector) container for string_t objects
*
* This structure manages a resizable array of string_t objects with automatic
* memory management and capacity handling.
*/
typedef struct string_v string_v;
// --------------------------------------------------------------------------------

/**
* @function init_str_vector
* @brief Initializes a new string vector with specified initial capacity
*
* @param buffer Initial capacity (number of strings) to allocate
* @return Pointer to new string_v object, or NULL on allocation failure
*         Sets errno to ENOMEM if memory allocation fails
*/
string_v* init_str_vector(size_t buffer);
// --------------------------------------------------------------------------------

/**
* @function push_back_str_vector
* @brief Adds a string to the end of the vector
*
* Automatically resizes the vector if necessary.
*
* @param vec Target string vector
* @param value String to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_back_str_vector(string_v* vec, const char* value);
// --------------------------------------------------------------------------------

/**
* @function push_front_str_vector
* @brief Adds a string to the beginning of the vector
*
* Shifts existing elements right and automatically resizes if necessary.
*
* @param vec Target string vector
* @param value String to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_front_str_vector(string_v* vec, const char* value);
// --------------------------------------------------------------------------------

/**
* @function insert_str_vector
* @brief Inserts a string at specified index in the vector
*
* Shifts elements right starting at index and resizes if necessary.
*
* @param vec Target string vector
* @param value String to insert
* @param index Position to insert at
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or index out of bounds, ENOMEM on allocation failure
*/
bool insert_str_vector(string_v* vec, const char* value, size_t index);
// --------------------------------------------------------------------------------

/**
* @function str_vector_index
* @brief Retrieves pointer to string_t at specified index
*
* @param vec Source string vector
* @param index Position to access
* @return Pointer to string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
const string_t* str_vector_index(const string_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
* @function str_vector_size
* @brief Returns current number of strings in vector
*
* @param vec String vector to query
* @return Number of strings in vector, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t str_vector_size(const string_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function str_vector_alloc
* @brief Returns current allocation size of vector
*
* @param vec String vector to query
* @return Current allocation capacity, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t str_vector_alloc(const string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pop_back_str_vector
* @brief Removes and returns last string in vector
*
* @param vec Source string vector
* @return Pointer to removed string_t object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
string_t* pop_back_str_vector(string_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function pop_front_str_vector
* @brief Removes and returns first string in vector
*
* Shifts remaining elements left.
*
* @param vec Source string vector
* @return Pointer to removed string_t object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
string_t* pop_front_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pup_any_str_vector
* @brief Removes and returns string at specified index
*
* Shifts remaining elements left to fill gap.
*
* @param vec Source string vector
* @param index Position to remove from
* @return Pointer to removed string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
string_t* pop_any_str_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function delete_back_str_vector
* @brief Removes the last string in a vector
*
* @param vec Source string vector
* @return true if successful, false otherwise, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
bool delete_back_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function delete_front_str_vector
* @brief Removes the first string in a vector
*
* @param vec Source string vector
* @return true if successful, false otherwise, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
bool delete_front_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function delete_any_str_vector
* @brief Removes any string in a vector
*
* @param vec Source string vector
* @param index The index to be deleted
* @return true if successful, false otherwise, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
bool delete_any_str_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function free_str_vector
* @brief Frees all memory associated with string vector
*
* Frees all contained strings and the vector itself.
*
* @param vec String vector to free
* @return void
*         Sets errno to EINVAL for NULL input
*/
void free_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function _free_str_vector
* @brief Helper function for garbage collection of string vectors
*
* Used with STRVEC_GBC macro for automatic cleanup.
*
* @param vec Double pointer to string vector to free
* @return void
*/
void _free_str_vector(string_v** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro STRVEC_GBC
     * @brief A macro for enabling automatic cleanup of str_vector objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_str_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define STRVEC_GBC __attribute__((cleanup(_free_str_vector)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @function reverse_str_vector
 * @brief Reverses the order of elements in a string vector in place.
 *
 * The function reverses the order of elements by swapping elements from the
 * beginning and end of the vector until the middle is reached.
 *
 * @param vec string vector to reverse
 * @return void
 *         Sets errno to EINVAL if vec is NULL or invalid
 */
void reverse_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @function swap_string
 * @brief Swaps the contents of two string_t objects.
 *
 * Performs an in-place swap of all attributes (str pointer, length, and allocation)
 * between two string_t objects. The operation maintains memory ownership and
 * allocation status.
 *
 * @param a Pointer to first string_t object
 * @param b Pointer to second string_t object
 * @return void
 *         Sets errno to EINVAL if either input is NULL
 */
void swap_string(string_t* a, string_t* b);
// ================================================================================
// ================================================================================ 
// DICTIONARY PROTOTYPES

/**
 * @typedef dict_t
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to int values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_t dict_t;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_t* init_dict();
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
bool insert_dict(dict_t* dict, const char* key, size_t value);
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
size_t pop_dict(dict_t* dict,  char* key);
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
const size_t get_dict_value(const dict_t* dict, char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_dict(dict_t* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_dict(dict_t** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_t objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DICT_GBC __attribute__((cleanup(_free_dict)))
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
bool update_dict(dict_t* dict, char* key, size_t value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
const size_t dict_size(const dict_t* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
const size_t dict_alloc(const dict_t* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
const size_t dict_hash_size(const dict_t* dict);
// --------------------------------------------------------------------------------

/**
* @function is_key_value
* @brief returns true if a key value pair exists in dictionary
*
* @param str dict_t object to tokenize
* @param key A string literal representing a key word
* @return returns true of key value pair exists, false otherwise.  if char 
*         pointer or dict_t data type is a null pointer will sett errno 
*         to ENOMEM and return false
*/
bool is_key_value(const dict_t* dict, const char* key);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS 

/**
* @macro s_size
* @brief Generic macro to return the populated length of a string data type such as
*        string_t and string_v
*
* @param dat A pointer to a string_t or strinv_v data type
* @return The populated length of a string data type
*/
#define s_size(dat) _Generic((dat), \
    string_t*: string_size, \
    string_v*: str_vector_size, \
    dict_t*: dict_size) (dat)
// --------------------------------------------------------------------------------

/**
* @macro s_alloc
* @brief Generic macro to return the allocated size of a string data type such as
*        string_t and string_v
*
* @param dat A pointer to a string_t or strinv_v data type
* @return The allocated length of a string data type
*/
#define s_alloc(dat) _Generic((dat), \
    string_t*: string_alloc, \
    string_v*: str_vector_alloc, \
    dict_t*: dict_alloc) (dat)
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

/**
* @function sort_str_vector
* @brief Sorts a string vector in ascending or descending order.
*
* Uses an optimized QuickSort algorithm with median-of-three pivot selection
* and insertion sort for small subarrays. Sort direction is determined by
* the iter_dir parameter.
*
* @param vec string vector to sort
* @param direction FORWARD for ascending order, REVERSE for descending
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void sort_str_vector(string_v* vec, iter_dir direction);
// --------------------------------------------------------------------------------

/**
* @function tokenize_string
* @brief Splits a string into tokens based on delimiter characters.
*
* Creates a vector of strings containing each token found in the input string.
* Multiple consecutive delimiters are treated as a single delimiter, and
* the original string is preserved.
*
* @param str string_t object to tokenize
* @param delim string containing delimiter characters (e.g., " ,;")
* @return string vector containing tokens, or NULL on error
*         Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure
*
* Example:
*     string_t* str = init_string("hello,world;test");
*     string_v* tokens = tokenize_string(str, ",;");
*     // tokens now contains ["hello", "world", "test"]
*/
string_v* tokenize_string(const string_t* str, const char* delim);
// -------------------------------------------------------------------------------- 

/**
* @function get_dict_keys
* @brief Returns a string vector of dictionary keys
*
* @param dict A dict_t type
* @return string vector containing dictionary keys, or NULL on error
*         Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure
*/
string_v* get_dict_keys(const dict_t* dict);
// --------------------------------------------------------------------------------

/**
* @function count_words
* @brief Returns a dictionary of words that occur in a string and the number 
*        of their occurrances
*
* @param dict A dict_t type
* @param delim A stirng literal of delimters used to parse a string
* @return A dictionary containing dictionary keys, or NULL on error
*         Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure
*
*/
dict_t* count_words(const string_t* str, const char* delim);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
// ================================================================================
// ================================================================================ 
#endif /* cplusplus */
#endif /* c_string_H */
// ================================================================================
// ================================================================================
// eof
