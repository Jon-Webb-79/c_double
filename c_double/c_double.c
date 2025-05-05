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
        return FLT_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
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
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(double)) {
        errno = ERANGE;
        return FLT_MAX;
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
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return FLT_MAX;
    }
    
    // Create copy of element to pop
    double temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return FLT_MAX;
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
        return FLT_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return FLT_MAX;
    }
    return vec->data[index];
}
// --------------------------------------------------------------------------------

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
        return FLT_MAX;
    }

    double min_val = FLT_MAX;

#if defined(__AVX__)
    __m256 vmin = _mm256_set1_ps(min_val);
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        vmin = _mm256_min_ps(vmin, v);
    }

    __m128 low = _mm256_castps256_ps128(vmin);
    __m128 high = _mm256_extractf128_ps(vmin, 1);
    __m128 min128 = _mm_min_ps(low, high);
    min128 = _mm_min_ps(min128, _mm_movehl_ps(min128, min128));
    min128 = _mm_min_ps(min128, _mm_shuffle_ps(min128, min128, 0x1));
    min_val = _mm_cvtss_f32(min128);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#elif defined(__SSE__)
    __m128 vmin = _mm_set1_ps(min_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        vmin = _mm_min_ps(vmin, v);
    }

    vmin = _mm_min_ps(vmin, _mm_movehl_ps(vmin, vmin));
    vmin = _mm_min_ps(vmin, _mm_shuffle_ps(vmin, vmin, 0x1));
    min_val = _mm_cvtss_f32(vmin);

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
        return FLT_MAX;
    }

    double max_val = -FLT_MAX;

#if defined(__AVX__)
    __m256 vmax = _mm256_set1_ps(max_val);
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        vmax = _mm256_max_ps(vmax, v);
    }

    __m128 low = _mm256_castps256_ps128(vmax);
    __m128 high = _mm256_extractf128_ps(vmax, 1);
    __m128 max128 = _mm_max_ps(low, high);
    max128 = _mm_max_ps(max128, _mm_movehl_ps(max128, max128));
    max128 = _mm_max_ps(max128, _mm_shuffle_ps(max128, max128, 0x1));
    max_val = _mm_cvtss_f32(max128);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#elif defined(__SSE__)
    __m128 vmax = _mm_set1_ps(max_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        vmax = _mm_max_ps(vmax, v);
    }

    vmax = _mm_max_ps(vmax, _mm_movehl_ps(vmax, vmax));
    vmax = _mm_max_ps(vmax, _mm_shuffle_ps(vmax, vmax, 0x1));
    max_val = _mm_cvtss_f32(vmax);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#else
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
        return FLT_MAX;
    }

    const size_t len = vec->len;
    const double* data = vec->data;

    double sum = 0.0f;

#if defined(__AVX__)
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 7 < len; i += 8) {
        __m256 chunk = _mm256_loadu_ps(&data[i]);
        vsum = _mm256_add_ps(vsum, chunk);
    }

    // Horizontal sum of vsum
    __m128 low  = _mm256_castps256_ps128(vsum);         // lower 128
    __m128 high = _mm256_extractf128_ps(vsum, 1);       // upper 128
    __m128 sum128 = _mm_add_ps(low, high);              // add lower + upper

    // Shuffle and reduce
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum += _mm_cvtss_f32(sum128);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += data[i];
    }

#elif defined(__SSE__)
    __m128 vsum = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 3 < len; i += 4) {
        __m128 chunk = _mm_loadu_ps(&data[i]);
        vsum = _mm_add_ps(vsum, chunk);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);
    sum += _mm_cvtss_f32(vsum);

    for (; i < len; ++i) {
        sum += data[i];
    }

#else
    // Fallback to scalar
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
        return FLT_MAX;
    }

    double sum = sum_double_vector(vec);
    if (errno != 0) return FLT_MAX;
    return sum / vec->len;
}

// -------------------------------------------------------------------------------- 

double stdev_double_vector(double_v* vec) {
    if (!vec || !vec->data) {
        errno = ENODATA;
        return FLT_MAX;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }

    if (vec->len == 1) {
        errno = ENODATA;
        return FLT_MAX;
    }

    double mean = average_double_vector(vec);
    if (errno != 0) return FLT_MAX;

    double sum_sq_diff = 0.0f;

#if defined(__AVX__)
    __m256 vmean = _mm256_set1_ps(mean);
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        __m256 diff = _mm256_sub_ps(v, vmean);
        __m256 sq = _mm256_mul_ps(diff, diff);
        if (!_mm256_testz_ps(sq, sq)) { // Skip all-zero check — we need isinf
            for (int j = 0; j < 8; ++j) {
                double x = vec->data[i + j];
                if (isinf(x)) return INFINITY;
            }
        }
        vsum = _mm256_add_ps(vsum, sq);
    }

    __m128 low = _mm256_castps256_ps128(vsum);
    __m128 high = _mm256_extractf128_ps(vsum, 1);
    __m128 sum128 = _mm_add_ps(low, high);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum_sq_diff += _mm_cvtss_f32(sum128);

    for (; i < vec->len; ++i) {
        double diff = vec->data[i] - mean;
        double sq = diff * diff;
        if (isinf(vec->data[i])) return INFINITY;
        sum_sq_diff += sq;
    }

#elif defined(__SSE__)
    __m128 vmean = _mm_set1_ps(mean);
    __m128 vsum = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        __m128 diff = _mm_sub_ps(v, vmean);
        __m128 sq = _mm_mul_ps(diff, diff);
        for (int j = 0; j < 4; ++j) {
            if (isinf(vec->data[i + j])) return INFINITY;
        }
        vsum = _mm_add_ps(vsum, sq);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);
    sum_sq_diff += _mm_cvtss_f32(vsum);

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

    return sqrtf(sum_sq_diff / vec->len);
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
// eof
