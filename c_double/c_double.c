// ================================================================================
// ================================================================================
// - File:    c_double.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 04, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <immintrin.h>  // AVX/SSE
#include "c_double.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b;
// ================================================================================
// ================================================================================ 

double_v* init_double_vector(size_t buff) {
    if (buff == 0) {
        errno = EINVAL;
        return NULL;
    }
    double_v* struct_ptr = malloc(sizeof(double_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    double* data_ptr = malloc(buff * sizeof(double));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL; 
    }
   
    // Initialize all elements
    memset(data_ptr, 0, buff * sizeof(double));
   
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    struct_ptr->alloc_type = DYNAMIC;
    return struct_ptr;
}
// -------------------------------------------------------------------------------- 

double* c_double_ptr(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    return vec->data;
}
// --------------------------------------------------------------------------------

void free_double_vector(double_v* vec) {
   if (!vec || !vec->alloc_type || vec->alloc_type == STATIC) {
       errno = EINVAL;
       return;
   }
   if (vec->data) free(vec->data);
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_double_vector(double_v** vec) {
    if (vec && *vec) {
        free_double_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_double_vector(double_v* vec, const double value) {
    if (vec == NULL|| vec->data == NULL) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_double_vector(double_v* vec, const double value) {
    if (vec == NULL || vec->data == NULL) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        // Check for size_t overflow
        if (new_alloc > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return false;
        }
       
        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Check for length overflow
    if (vec->len > SIZE_MAX - 1) {
        errno = ERANGE;
        return false;
    }
    
    // Move existing elements right if there are any
    if (vec->len > 0) {
        memmove(vec->data + 1, vec->data, vec->len * sizeof(double));
    }
    
    vec->data[0] = value;    
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_double_vector(double_v* vec, double value, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
    if (vec->len >= SIZE_MAX) {  // Check length overflow
        errno = ERANGE;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        // Check allocation size overflow
        if (new_alloc > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return false;
        }
       
        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right
    if (index < vec->len) {  // Only move if not appending
        // Check for size_t overflow in move operation
        if (vec->len - index > SIZE_MAX - 1) {
            errno = ERANGE;
            return false;
        }
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(double));
    }
    
    vec->data[index] = value;
    vec->len++;
    return true;
}
// -------------------------------------------------------------------------------- 


double pop_back_double_vector(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
    
    // Create copy of last element
    double temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(double));
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

double pop_front_double_vector(double_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(double)) {
        errno = ERANGE;
        return DBL_MAX;
    }
   
    // Create copy of first element
    double temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(double));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(double));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

double pop_any_double_vector(double_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return DBL_MAX;
    }
    
    // Create copy of element to pop
    double temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return DBL_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(double));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(double));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

const double double_vector_index(const double_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return DBL_MAX;
    }
    return vec->data[index];
}
// // --------------------------------------------------------------------------------
//
const size_t double_vector_size(const double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

const size_t double_vector_alloc(const double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_double_vector(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return;
    }

    size_t i = 0;
    size_t j = vec->len - 1;
    while (i < j) {
       swap_double(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_double(double* a, double* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    double temp = *a;
    *a = *b;
    *b = temp;
}
// -------------------------------------------------------------------------------- 

static double* _median_of_three(double* a, double* b, double* c, iter_dir direction) {
    if ((direction == FORWARD && *a < *b) ||
        (direction == REVERSE && *a > *b)) {
        if ((direction == FORWARD && *b < *c) ||
            (direction == REVERSE && *b > *c))
            return b;
        if ((direction == FORWARD && *a < *c) ||
            (direction == REVERSE && *a > *c))
            return c;
        return a;
    }
    if ((direction == FORWARD && *a < *c) ||
        (direction == REVERSE && *a > *c))
        return a;
    if ((direction == FORWARD && *b < *c) ||
        (direction == REVERSE && *b > *c))
        return c;
    return b;
}
// -------------------------------------------------------------------------------- 

static void _insertion_sort(double* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        double key = vec[i];
        int j = i - 1;
        while (j >= low && ((direction == FORWARD && vec[j] > key) ||
                           (direction == REVERSE && vec[j] < key))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}
// --------------------------------------------------------------------------------

static int _partition_double(double* vec, int low, int high, iter_dir direction) {
    int mid = low + (high - low) / 2;
    double* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    if (pivot_ptr != &vec[high])
        swap_double(pivot_ptr, &vec[high]);
    
    double pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_double(&vec[i], &vec[j]);
        }
    }
    swap_double(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// -------------------------------------------------------------------------------- 

static void _quicksort_double(double* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_double(vec, low, high, direction);
        
        if (pi - low < high - pi) {
            _quicksort_double(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_double(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// -------------------------------------------------------------------------------- 

void sort_double_vector(double_v* vec, iter_dir direction) {
    if (!vec) {
        errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_double(vec->data, 0, vec->len - 1, direction);
}
// -------------------------------------------------------------------------------- 

void trim_double_vector(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    
    if (vec->alloc_type == STATIC || vec->len == vec->alloc) {
        return;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return;
    }

    // Check for overflow
    if (vec->len > SIZE_MAX / sizeof(double)) {
        errno = ERANGE;
        return;
    }
    
    double* ptr = realloc(vec->data, sizeof(double) * vec->len);
    if (ptr == NULL) {
        errno = ENOMEM;
        return;
    }
    
    vec->data = ptr;
    vec->alloc = vec->len;
}
// --------------------------------------------------------------------------------

size_t binary_search_double_vector(double_v* vec, double value, double tolerance, bool sort_first) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return LONG_MAX;
    }

    if (tolerance < 0) {
        errno = EINVAL;
        return LONG_MAX;
    }

    if (isnan(value) || isnan(tolerance)) {
        errno = EINVAL;
        return LONG_MAX;
    }
    
    // Sort if requested and vector has more than 1 element
    if (sort_first && vec->len > 1) {
        sort_double_vector(vec, FORWARD);
    }
    
    size_t left = 0;
    size_t right = vec->len - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        double diff = vec->data[mid] - value;
        
        // Check if we found a match within tolerance
        if (fabs(diff) <= tolerance) {
            return mid;
        }
        
        if (diff < 0) {
            left = mid + 1;
        } else {
            // Handle case where mid is 0 to prevent underflow
            if (mid == 0) {
                break;
            }
            right = mid - 1;
        }
    }
    // The value does not exist in the array 
    return LONG_MAX;
}
// -------------------------------------------------------------------------------- 

void update_double_vector(double_v* vec, size_t index, double replacement_value) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return;
    }
    if (index > vec->len -1) {
        errno = ERANGE;
        return;
    }
    vec->data[index] = replacement_value;
}
// -------------------------------------------------------------------------------- 

double min_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }

    double min_val = DBL_MAX;

#if defined(__AVX__)
    __m256d vmin = _mm256_set1_pd(min_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m256d v = _mm256_loadu_pd(&vec->data[i]);
        vmin = _mm256_min_pd(vmin, v);
    }

    __m128d low = _mm256_castpd256_pd128(vmin);
    __m128d high = _mm256_extractf128_pd(vmin, 1);
    __m128d min128 = _mm_min_pd(low, high);
    min128 = _mm_min_pd(min128, _mm_unpackhi_pd(min128, min128));
    min_val = _mm_cvtsd_f64(min128);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#elif defined(__SSE2__)
    __m128d vmin = _mm_set1_pd(min_val);
    size_t i = 0;

    for (; i + 1 < vec->len; i += 2) {
        __m128d v = _mm_loadu_pd(&vec->data[i]);
        vmin = _mm_min_pd(vmin, v);
    }

    vmin = _mm_min_pd(vmin, _mm_unpackhi_pd(vmin, vmin));
    min_val = _mm_cvtsd_f64(vmin);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#else
    for (size_t i = 0; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];
#endif

    return min_val;
}
// -------------------------------------------------------------------------------- 

double max_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return -DBL_MAX;
    }

    double max_val = -DBL_MAX;

#if defined(__AVX__)
    __m256d vmax = _mm256_set1_pd(max_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m256d v = _mm256_loadu_pd(&vec->data[i]);
        vmax = _mm256_max_pd(vmax, v);
    }

    // Reduce 4 values to 1 max
    __m128d low = _mm256_castpd256_pd128(vmax);
    __m128d high = _mm256_extractf128_pd(vmax, 1);
    __m128d max128 = _mm_max_pd(low, high);
    max128 = _mm_max_pd(max128, _mm_unpackhi_pd(max128, max128));
    max_val = _mm_cvtsd_f64(max128);

    // Scalar fallback for remainder
    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#elif defined(__SSE2__)
    __m128d vmax = _mm_set1_pd(max_val);
    size_t i = 0;

    for (; i + 1 < vec->len; i += 2) {
        __m128d v = _mm_loadu_pd(&vec->data[i]);
        vmax = _mm_max_pd(vmax, v);
    }

    // Reduce 2 values to 1
    vmax = _mm_max_pd(vmax, _mm_unpackhi_pd(vmax, vmax));
    max_val = _mm_cvtsd_f64(vmax);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#else
    // Pure scalar fallback
    for (size_t i = 0; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];
#endif

    return max_val;
}
// -------------------------------------------------------------------------------- 

double sum_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }

    const size_t len = vec->len;
    const double* data = vec->data;

    double sum = 0.0;

#if defined(__AVX__)
    __m256d vsum = _mm256_setzero_pd();
    size_t i = 0;

    for (; i + 3 < len; i += 4) {
        __m256d chunk = _mm256_loadu_pd(&data[i]);
        vsum = _mm256_add_pd(vsum, chunk);
    }

    // Horizontal sum of 4 doubles
    __m128d low  = _mm256_castpd256_pd128(vsum);        // lower 2 doubles
    __m128d high = _mm256_extractf128_pd(vsum, 1);      // upper 2 doubles
    __m128d sum128 = _mm_add_pd(low, high);             // add pairs
    sum128 = _mm_add_pd(sum128, _mm_unpackhi_pd(sum128, sum128)); // final sum
    sum += _mm_cvtsd_f64(sum128);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += data[i];
    }

#elif defined(__SSE2__)
    __m128d vsum = _mm_setzero_pd();
    size_t i = 0;

    for (; i + 1 < len; i += 2) {
        __m128d chunk = _mm_loadu_pd(&data[i]);
        vsum = _mm_add_pd(vsum, chunk);
    }

    vsum = _mm_add_pd(vsum, _mm_unpackhi_pd(vsum, vsum)); // sum 2 doubles
    sum += _mm_cvtsd_f64(vsum);

    for (; i < len; ++i) {
        sum += data[i];
    }

#else
    // Fallback to scalar sum
    for (size_t i = 0; i < len; ++i) {
        sum += data[i];
    }
#endif

    return sum;
}
// -------------------------------------------------------------------------------- 

double average_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }

    double sum = sum_double_vector(vec);
    if (errno != 0) return DBL_MAX;
    return sum / vec->len;
}

// -------------------------------------------------------------------------------- 

double stdev_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len < 2) {
        errno = ENODATA;
        return DBL_MAX;
    }

    double mean = average_double_vector(vec);
    if (errno != 0) return DBL_MAX;

    double sum_sq_diff = 0.0;

#if defined(__AVX__)
    __m256d vmean = _mm256_set1_pd(mean);
    __m256d vsum = _mm256_setzero_pd();
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m256d v = _mm256_loadu_pd(&vec->data[i]);
        __m256d diff = _mm256_sub_pd(v, vmean);
        __m256d sq = _mm256_mul_pd(diff, diff);

        // Check for infinity in any of the 4 values
        for (int j = 0; j < 4; ++j) {
            if (isinf(vec->data[i + j])) return INFINITY;
        }

        vsum = _mm256_add_pd(vsum, sq);
    }

    // Horizontal sum of vsum
    __m128d low  = _mm256_castpd256_pd128(vsum);
    __m128d high = _mm256_extractf128_pd(vsum, 1);
    __m128d sum128 = _mm_add_pd(low, high);
    sum128 = _mm_add_pd(sum128, _mm_unpackhi_pd(sum128, sum128));
    sum_sq_diff += _mm_cvtsd_f64(sum128);

    // Remainder (scalar)
    for (; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        double diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }

#elif defined(__SSE2__)
    __m128d vmean = _mm_set1_pd(mean);
    __m128d vsum = _mm_setzero_pd();
    size_t i = 0;

    for (; i + 1 < vec->len; i += 2) {
        __m128d v = _mm_loadu_pd(&vec->data[i]);
        __m128d diff = _mm_sub_pd(v, vmean);
        __m128d sq = _mm_mul_pd(diff, diff);

        for (int j = 0; j < 2; ++j) {
            if (isinf(vec->data[i + j])) return INFINITY;
        }

        vsum = _mm_add_pd(vsum, sq);
    }

    vsum = _mm_add_pd(vsum, _mm_unpackhi_pd(vsum, vsum));
    sum_sq_diff += _mm_cvtsd_f64(vsum);

    for (; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        double diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }

#else
    for (size_t i = 0; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        double diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }
#endif

    return sqrt(sum_sq_diff / vec->len);
}
// -------------------------------------------------------------------------------- 

double_v* cum_sum_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    double_v* new_vec = init_double_vector(vec->len);
    if (!new_vec) {
        errno = ENOMEM;
        return NULL;
    }

    double sum = 0.0f;
    for (size_t i = 0; i < vec->len; ++i) {
        double val = vec->data[i];
        if (isnan(val)) {
            errno = EINVAL;
            free_double_vector(new_vec);
            return NULL;
        }

        sum += val;

        if (isinf(sum)) {
            // Fill rest with infinity
            for (; i < vec->len; ++i) {
                push_back_double_vector(new_vec, INFINITY);
            }
            return new_vec;
        }

        if (!push_back_double_vector(new_vec, sum)) {
            free_double_vector(new_vec);
            return NULL;
        }
    }

    return new_vec;
}
// -------------------------------------------------------------------------------- 

double_v* copy_double_vector(const double_v* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    double_v* copy = init_double_vector(original->alloc);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < original->len; ++i) {
        if (!push_back_double_vector(copy, original->data[i])) {
            free_double_vector(copy);
            return NULL;
        }
    }

    return copy;
}
// ================================================================================ 
// ================================================================================ 

// DICTIONARY IMPLEMENTATION

typedef struct ddictNode {
    char* key;
    double value;
    struct ddictNode* next;
} ddictNode;
// --------------------------------------------------------------------------------

struct dict_d {
    ddictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

/**
 * @brief MurmurHash3-inspired hash function for strings
 * 
 * @param key The string key to hash
 * @param seed Optional seed for hash randomization (helps prevent hash flooding)
 * @return size_t The computed hash value
 */
static size_t hash_function(const char* key, const uint32_t seed) {
    if (!key) {
        return 0;
    }

    // Constants for mixing
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    uint32_t h1 = seed;

    // Process key in 4-byte chunks
    const unsigned char* data = (const unsigned char*)key;
    size_t len = strlen(key);
    const size_t nblocks = len / 4;

    // Body
    const uint32_t* blocks = (const uint32_t*)(data);
    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1, 15)
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);  // ROTL32(h1, 13)
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail
    const unsigned char* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
            /* fallthrough */
        case 2:
            k1 ^= tail[1] << 8;
            /* fallthrough */
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1, 15)
            k1 *= c2;
            h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return (size_t)h1;
}
// --------------------------------------------------------------------------------

/**
 * @brief Resizes the dictionary's hash table
 * 
 * @param dict Pointer to the dictionary
 * @param new_size Desired new size for the hash table
 * @return bool true if resize successful, false otherwise
 */
static bool resize_dict(dict_d* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    ddictNode* new_table = calloc(new_size, sizeof(ddictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table in case we need to recover
    ddictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash existing entries
    for (size_t i = 0; i < old_size; i++) {
        ddictNode* current = old_table[i].next;
        
        while (current) {
            ddictNode* next = current->next;  // Save next pointer before modifying node

            // Calculate new index using enhanced hash function
            size_t new_index = hash_function(current->key, HASH_SEED) % new_size;

            // Insert at the beginning of the new chain
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            rehashed_count++;
            current = next;
        }
    }

    // Verify all entries were rehashed successfully
    if (rehashed_count != dict->hash_size) {
        // Cleanup and restore old state
        for (size_t i = 0; i < new_size; i++) {
            ddictNode* current = new_table[i].next;
            while (current) {
                ddictNode* next = current->next;
                current->next = NULL;
                current = next;
            }
        }
        free(new_table);
        errno = EAGAIN;
        return false;
    }

    // Update the dictionary only after successful rehashing
    dict->keyValues = new_table;
    dict->alloc = new_size;

    // Clean up old table (but not the nodes, as they were moved)
    free(old_table);

    return true;
}
// --------------------------------------------------------------------------------

dict_d* init_double_dict(void) {
    // Allocate the dictionary structure
    dict_d* dict = calloc(1, sizeof(dict_d));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(ddictNode));
    if (!dict->keyValues) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate hash table array\n");
        free(dict);
        return NULL;
    }

    // Initialize dictionary metadata
    dict->hash_size = 0;   // No items yet
    dict->len = 0;         // No occupied buckets
    dict->alloc = hashSize;

    return dict;
}
// --------------------------------------------------------------------------------

bool insert_double_dict(dict_d* dict, const char* key, double value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    // Check load factor and resize if necessary using adaptive growth strategy
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size;
        if (dict->alloc < VEC_THRESHOLD) {
            new_size = dict->alloc * 2;  // Double when small
        } else {
            new_size = dict->alloc + VEC_FIXED_AMOUNT;  // Linear growth when large
        }
        
        if (!resize_dict(dict, new_size)) {
            return false;  // resize_dict sets appropriate errno
        }
    }

    // Rest of the function remains the same as previous improvement...
    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    // Check for existing key
    for (ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    ddictNode* new_node = malloc(sizeof(ddictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// --------------------------------------------------------------------------------

double pop_double_dict(dict_d* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    ddictNode* prev = &dict->keyValues[index];
    ddictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Save value and unlink node
            double value = current->value;
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free(current->key);
            free(current);
            
            return value;
        }
        prev = current;
        current = current->next;
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

double get_double_dict_value(const dict_d* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

void free_double_dict(dict_d* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        ddictNode* current = dict->keyValues[i].next;
        while (current) {      
            ddictNode* next = current->next;  // Save next pointer before freeing
            free(current->key);
            free(current);
            current = next;
        }
    }

    // Free the hash table and dictionary struct
    free(dict->keyValues);
    free(dict);
}
// -------------------------------------------------------------------------------- 

void _free_double_dict(dict_d** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_double_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// --------------------------------------------------------------------------------

bool update_double_dict(dict_d* dict, const char* key, double value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return true;
        }
    }

    errno = ENOENT;  // More specific error code for missing key
    return false;
}
// --------------------------------------------------------------------------------

size_t double_dict_size(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t double_dict_alloc(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// --------------------------------------------------------------------------------

size_t double_dict_hash_size(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

bool has_key_double_dict(const dict_d* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

dict_d* copy_double_dict(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with same capacity
    dict_d* new_dict = malloc(sizeof(dict_d));
    if (!new_dict) {
        errno = ENOMEM;
        return NULL;
    }

    new_dict->keyValues = calloc(dict->alloc, sizeof(ddictNode));
    if (!new_dict->keyValues) {
        free(new_dict);
        errno = ENOMEM;
        return NULL;
    }

    // Copy metadata
    new_dict->alloc = dict->alloc;
    new_dict->hash_size = 0;  // Will be incremented during insertion
    new_dict->len = 0;        // Will be incremented during insertion

    // Copy all entries
    for (size_t i = 0; i < dict->alloc; i++) {
        ddictNode* current = dict->keyValues[i].next;
        while (current) {
            // Insert will handle incrementing hash_size and len
            if (!insert_double_dict(new_dict, current->key, current->value)) {
                free_double_dict(new_dict);  // Clean up on failure
                return NULL;
            }
            current = current->next;
        }
    }

    return new_dict;
}
// -------------------------------------------------------------------------------- 

bool clear_double_dict(dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return false;
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        ddictNode* current = dict->keyValues[i].next;
        while (current) {
            ddictNode* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
        dict->keyValues[i].next = NULL;  // Reset bucket head
    }

    // Reset dictionary metadata
    dict->hash_size = 0;
    dict->len = 0;

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_double_dict(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    string_v* vec = init_str_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    for (size_t i = 0; i < dict->alloc; i++) {
        for (ddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_str_vector(vec, current->key)) {
                free_str_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

double_v* get_values_double_dict(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    double_v* vec = init_double_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    // Iterate through all buckets
    for (size_t i = 0; i < dict->alloc; i++) {
        for (ddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_double_vector(vec, current->value)) {
                free_double_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

dict_d* merge_double_dict(const dict_d* dict1, const dict_d* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with capacity for all items
    //size_t initial_size = dict1->hash_size + dict2->hash_size;
    dict_d* merged = init_double_dict();
    if (!merged) {
        return NULL;  // errno set by init_double_dict
    }

    // First, copy all entries from dict1
    for (size_t i = 0; i < dict1->alloc; i++) {
        for (ddictNode* current = dict1->keyValues[i].next; current; current = current->next) {
            if (!insert_double_dict(merged, current->key, current->value)) {
                free_double_dict(merged);
                return NULL;
            }
        }
    }

    // Then handle dict2 entries
    for (size_t i = 0; i < dict2->alloc; i++) {
        for (ddictNode* current = dict2->keyValues[i].next; current; current = current->next) {
            double existing_value;
            // Check if key exists in merged dictionary
            if ((existing_value = get_double_dict_value(merged, current->key)) != FLT_MAX) {
                if (overwrite) {
                    // Update existing value if overwrite is true
                    if (!update_double_dict(merged, current->key, current->value)) {
                        free_double_dict(merged);
                        return NULL;
                    }
                }
                // If overwrite is false, keep original value
            } else {
                // Key doesn't exist, insert new entry
                if (!insert_double_dict(merged, current->key, current->value)) {
                    free_double_dict(merged);
                    return NULL;
                }
            }
        }
    }

    return merged;
}
// --------------------------------------------------------------------------------

bool foreach_double_dict(const dict_d* dict, dict_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; i++) {
        for (ddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            iter(current->key, current->value, user_data);
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 

typedef struct dvdictNode {
    char* key;
    double_v* value;
    struct dvdictNode* next;
} dvdictNode;
// --------------------------------------------------------------------------------

struct dict_dv {
    dvdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

dict_dv* init_doublev_dict(void) {
    // Allocate the dictionary structure
    dict_dv* dict = calloc(1, sizeof(dict_d));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate vector dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(dvdictNode));
    if (!dict->keyValues) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate hash table array\n");
        free(dict);
        return NULL;
    }

    // Initialize dictionary metadata
    dict->hash_size = 0;   // No items yet
    dict->len = 0;         // No occupied buckets
    dict->alloc = hashSize;

    return dict;
}
// -------------------------------------------------------------------------------- 

static bool resize_dictv(dict_dv* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better hash distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    dvdictNode* new_table = calloc(new_size, sizeof(dvdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table for cleanup if something fails
    dvdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash all existing entries into the new table
    for (size_t i = 0; i < old_size; ++i) {
        dvdictNode* current = old_table[i].next;
        while (current) {
            dvdictNode* next = current->next;

            size_t new_index = hash_function(current->key, HASH_SEED) % new_size;

            // Reinsert into the new hash bucket (head insertion)
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;

            rehashed_count++;
            current = next;
        }
    }

    // Validate rehash count matches the number of entries
    if (rehashed_count != dict->hash_size) {
        // Rollback: disconnect moved nodes so they aren't double-freed
        for (size_t i = 0; i < new_size; ++i) {
            dvdictNode* current = new_table[i].next;
            while (current) {
                dvdictNode* next = current->next;
                current->next = NULL;
                current = next;
            }
        }
        free(new_table);
        errno = EAGAIN;
        return false;
    }

    // Replace table on success
    dict->keyValues = new_table;
    dict->alloc = new_size;

    // Free old hash bucket array (nodes were moved, not freed)
    free(old_table);

    return true;
}
// --------------------------------------------------------------------------------

bool create_doublev_dict(dict_dv* dict, char* key, size_t size) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    // Resize if load factor exceeded
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size = (dict->alloc < VEC_THRESHOLD)
                          ? dict->alloc * 2
                          : dict->alloc + VEC_FIXED_AMOUNT;

        if (!resize_dictv(dict, new_size)) {
            return false;
        }
    }

    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    // Check for key collision
    for (dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    dvdictNode* new_node = malloc(sizeof(dvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    double_v* value = NULL;
    value = init_double_vector(size);
    if (!value) {
        free(new_key);
        free(new_node);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// --------------------------------------------------------------------------------

bool pop_doublev_dict(dict_dv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    dvdictNode* prev = &dict->keyValues[index];
    dvdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free_double_vector(current->value);
            free(current->key);
            free(current);
            
            return true;
        }
        prev = current;
        current = current->next;
    }

    errno = ENOENT;  // Set errno when key not found
    return false;
}
// -------------------------------------------------------------------------------- 

double_v* return_doublev_pointer(dict_dv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return NULL;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return NULL;
}
// -------------------------------------------------------------------------------- 

void free_doublev_dict(dict_dv* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        dvdictNode* current = dict->keyValues[i].next;
        while (current) {      
            dvdictNode* next = current->next;

            free_double_vector(current->value);
            free(current->key);
            free(current);

            current = next;
        }
    }

    // Free the hash table and dictionary struct
    free(dict->keyValues);
    free(dict);
}
// -------------------------------------------------------------------------------- 

void _free_doublev_dict(dict_dv** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_doublev_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// -------------------------------------------------------------------------------- 

bool has_key_doublev_dict(const dict_dv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_doublev_dict(dict_dv* dict, const char* key, double_v* value) {
    if (!dict || !key || !value) {
        errno = EINVAL;
        return false;
    }

    // Only allow insertion of DYNAMIC vectors
    if (value->alloc_type != DYNAMIC) {
        errno = EPERM;  // Operation not permitted
        return false;
    }

    // Resize if load factor exceeded
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size = (dict->alloc < VEC_THRESHOLD)
                          ? dict->alloc * 2
                          : dict->alloc + VEC_FIXED_AMOUNT;

        if (!resize_dictv(dict, new_size)) {
            return false;
        }
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    // Check for existing key
    for (dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    // Allocate new key string
    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    // Allocate node
    dvdictNode* new_node = malloc(sizeof(dvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

size_t double_dictv_size(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t double_dictv_alloc(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// -------------------------------------------------------------------------------- 

size_t double_dictv_hash_size(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

dict_dv* copy_doublev_dict(const dict_dv* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    dict_dv* copy = init_doublev_dict();
    if (!copy) {
        return NULL;  // errno already set
    }

    for (size_t i = 0; i < original->alloc; ++i) {
        dvdictNode* current = original->keyValues[i].next;
        while (current) {
            // if (current->value->alloc_type != DYNAMIC) {
            //     free_doublev_dict(copy);
            //     errno = EPERM;
            //     return NULL;
            // }

            double_v* vec_copy = copy_double_vector(current->value);
            if (!vec_copy) {
                free_doublev_dict(copy);
                return NULL;
            }

            if (!insert_doublev_dict(copy, current->key, vec_copy)) {
                free_double_vector(vec_copy);
                free_doublev_dict(copy);
                return NULL;
            }

            current = current->next;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

dict_dv* merge_doublev_dict(const dict_dv* dict1, const dict_dv* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Start by deep copying dict1
    dict_dv* merged = copy_doublev_dict(dict1);
    if (!merged) {
        return NULL;
    }

    // Now process dict2 entries
    for (size_t i = 0; i < dict2->alloc; ++i) {
        dvdictNode* current = dict2->keyValues[i].next;
        while (current) {
            if (!current->key || !current->value || current->value->alloc_type != DYNAMIC) {
                free_doublev_dict(merged);
                errno = EPERM;
                return NULL;
            }

            bool exists = has_key_doublev_dict(merged, current->key);
            if (exists && !overwrite) {
                current = current->next;
                continue;
            }

            double_v* vec_copy = copy_double_vector(current->value);
            if (!vec_copy) {
                free_doublev_dict(merged);
                return NULL; // errno set by copy_double_vector
            }

            if (exists) {
                pop_doublev_dict(merged, current->key);
            }

            if (!insert_doublev_dict(merged, current->key, vec_copy)) {
                free_double_vector(vec_copy);
                free_doublev_dict(merged);
                return NULL;
            }

            current = current->next;
        }
    }

    return merged;
}
// -------------------------------------------------------------------------------- 

void clear_doublev_dict(dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        dvdictNode* current = dict->keyValues[i].next;
        dict->keyValues[i].next = NULL;

        while (current) {
            dvdictNode* next = current->next;

            if (current->value) {
                if (current->value->alloc_type == STATIC) {
                    free(current->value);
                } else {
                    free_double_vector(current->value);
                }
            }

            free(current->key);
            free(current);
            current = next;
        }
    }

    dict->hash_size = 0;
    dict->len = 0;
}
// -------------------------------------------------------------------------------- 

bool foreach_doublev_dict(const dict_dv* dict, dict_dv_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        dvdictNode* current = dict->keyValues[i].next;
        while (current) {
            iter(current->key, current->value, user_data);
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_doublev_dict(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    string_v* vec = init_str_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        for (dvdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_str_vector(vec, current->key)) {
                free_str_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }

    return vec;
}
// ================================================================================
// ================================================================================
// eof
