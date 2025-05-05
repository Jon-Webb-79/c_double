.. _vector_file:

******
Vector
******

Double Vector Overview
======================

A double vector provides both dynamic and static array containers for double values, offering
efficient management of numerical data with automatic memory handling for dynamic arrays
and safe access for static arrays. This implementation combines the flexibility of dynamic
arrays with the safety of bounds checking and proper memory management.

Key Features
------------

* Dynamic and static array support: Choose between automatic resizing or fixed-size arrays
* Memory safety: Proper encapsulation and memory management
* Bounds checking: Safe access to array elements
* Efficient access: O(1) access to any double in the vector
* Automatic cleanup: Optional garbage collection support with FLTVEC_GBC

When to Use Double Vectors
--------------------------

Double vectors are particularly useful when:

* Managing collections of double values that may grow or shrink
* Working with an unknown number of double values at compile time
* Requiring sequential access to multiple double values
* Needing efficient random access to double values by index
* Managing related numerical data as a single unit
* Implementing numerical algorithms or data processing pipelines

Performance Characteristics
---------------------------

* Access time: O(1) for index-based access
* Insertion time: O(1) amortized for push_back operations
* Memory efficiency: Dynamic vectors grow geometrically to minimize reallocations
* Memory overhead: Minimal per-element overhead

Data Types
==========

The following are derived data types used in the implementation of double vectors.

double_v
--------
The double vector type that can manage either dynamically or statically allocated double arrays
with automatic bounds checking and memory management.

.. code-block:: c

   typedef struct {
       double* data;
       size_t len;
       size_t alloc;
       alloc_t alloc_type;
   } double_v;

Core Functions
==============

The c_double library provides both dynamically and statically allocated double arrays through
the double_v type. All of the functions described in this section can be found in the
``c_double.h`` header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section are used to control the creation,
memory allocation, and specific destruction of ``double_v`` data types.

init_double_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: double_v* init_double_vector(size_t buffer)

   Initializes a new dynamically allocated double vector with specified initial capacity.
   The vector will automatically grow if needed when adding elements.

   :param buffer: Initial capacity (number of doubles) to allocate
   :returns: Pointer to new double_v object, or NULL on allocation failure
   :raises: Sets errno to EINVAL if buffer is 0, ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      // Create vector with initial capacity for 10 doubles
      double_v* vec = init_double_vector(10);
      if (!vec) {
          fprintf(stderr, "Failed to initialize vector\\n");
          return 1;
      }
      
      // Free when done
      free_double_vector(vec);

init_double_array
~~~~~~~~~~~~~~~~~
.. c:macro:: init_double_array(size)

   Creates a statically allocated double array wrapped in a double_v structure.
   Must be used at variable declaration.

   :param size: Size of the static array to create
   :returns: double_v structure containing the static array

   Example:

   .. code-block:: c

      // Create static array of 10 doubles
      double_v arr = init_double_array(10);
      
      // No need to free - automatically cleaned up when out of scope

free_double_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: void free_double_vector(double_v* vec)

   Frees all memory associated with a dynamically allocated double vector.
   This function should only be used with vectors created by init_double_vector().
   For vectors created with init_double_array(), this function will return an error.
   If using a ``gcc`` or ``clang`` compiler, consider using the FLTVEC_GBC macro
   instead.

   :param vec: Double vector to free
   :raises: Sets errno to EINVAL if vec is NULL or if attempting to free a static array

   Example:

   .. code-block:: c

      double_v* vec = init_double_vector(5);
      
      // Use the vector...
      
      // Free the vector when done
      free_double_vector(vec);
      vec = NULL;  // Good practice to avoid dangling pointer

.. warning::

   The free_double_vector() function should only be used with dynamically allocated
   vectors created by init_double_vector(). Attempting to free a static

trim_double_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: void trim_double_vector(double_v* vec)

   Reduces the allocated memory of a double vector to match its current size,
   eliminating any unused capacity. This operation has no effect on static arrays
   or vectors that are already at optimal capacity.

   :param vec: Target double vector
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty,
           ERANGE for size_t overflow, ENOMEM if reallocation fails

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec FLTVEC_GBC = init_double_vector(10);  // Allocate space for 10 doubles
      
      // Add 3 values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      
      printf("Before trim: size = %zu, capacity = %zu\n", 
             f_size(vec), f_alloc(vec));
      
      trim_double_vector(vec);
      
      printf("After trim:  size = %zu, capacity = %zu\n", 
             f_size(vec), f_alloc(vec));
      
   Output::

      Before trim: size = 3, capacity = 10
      After trim:  size = 3, capacity = 3

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(5);
      
      // Add some values
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 2.0f);
      
      printf("Before trim: size = %zu, capacity = %zu\n", 
             f_size(&arr), f_alloc(&arr));
      
      trim_double_vector(&arr);  // No effect on static arrays
      
      printf("After trim:  size = %zu, capacity = %zu\n", 
             f_size(&arr), f_alloc(&arr));

   Output::

      Before trim: size = 2, capacity = 5
      After trim:  size = 2, capacity = 5

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Sets errno to EINVAL
     - Returns without modification
   
   * If vector is empty:
     - Sets errno to ENODATA
     - Returns without modification
   
   * If memory reallocation fails:
     - Sets errno to ENOMEM
     - Returns without modification
     - Original vector remains unchanged

   The following conditions result in no modification and no error:

   * Static arrays (alloc_type == STATIC)
   * Vectors where capacity equals size
   
   .. note::

      This function is useful for reclaiming unused memory in vectors that have
      shrunk significantly from their peak size. However, frequent trimming
      can be counterproductive if the vector size fluctuates often, as it
      may lead to repeated allocations when the vector grows again.

Automatic Cleanup
-----------------

The C language does not generally allow automated garbage collection of memory that
is out of scope. This section describes a poor man's garbage collection within the
C language, for the ``double_v`` data type, that can only be enabled if compiled
with ``gcc`` or ``clang``.

DBLEVEC_GBC
~~~~~~~~~~~
.. c:macro:: DBLEVEC_GBC

   Macro that enables automatic cleanup of dynamically allocated double vectors when they
   go out of scope. Only available when using GCC or Clang compilers. Uses the cleanup
   attribute to automatically call _free_double_vector.

   Example:

   .. code-block:: c

      void process_doubles(void) {
          // Vector will be automatically freed when function returns
          DBLEVEC_GBC double_v* vec = init_double_vector(10);
          
          // Use the vector...
          
          // No need to call free_double_vector
      }  // vec is automatically freed here

   .. note::

      This macro should only be used with dynamically allocated vectors created by
      init_double_vector(). It is not needed for static arrays created with
      init_double_array() as they are automatically cleaned up when going out of scope.

Data Addition 
-------------
The functions below can be used to add data to a dynamically allocated array 
or a statically allocated vector.

push_back_double_vector
~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_back_double_vector(double_v* vec, const double value)

   Adds a double value to the end of the vector. If needed, the vector automatically
   resizes to accommodate the new value. For vectors smaller than VEC_THRESHOLD,
   capacity doubles when full. For larger vectors, a fixed amount is added.
   This is the most efficient method for adding data to a double vector with
   a time efficiency of :math:`O(1)`. If the structure passed is for a statically allocated 
   array, the function will return ``false``, if the user tries to enter data to 
   an out of bounds index and will set ``errno`` to ``EINVAL``

   :param vec: Target double vector
   :param value: Double value to add to vector
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure

   Vector Example:

   .. code-block:: c

      double_v* vec = init_double_vector(2);
      
      // Add some values
      push_back_double_vector(vec, 3.14f);
      push_back_double_vector(vec, 2.718f);
      
      // Vector will automatically resize
      push_back_double_vector(vec, 1.414f);
      
      printf("Vector size: %zu\n", f_size(vec));
      printf("[ ");
      for (size_t i = 0; i < f_size(vec) - 1; i++) 
          printf("%f, ", double_vector_index(vec, 1));
      printf("%f ]\n", double_vector_index(vec, f_size(vec) - 1))
      
      free_double_vector(vec);

   Output::

      Vector size: 3
      [ 3.140000, 2.718000, 1.414000 ]

   Array Example:

   .. code-block:: c

      double_v* vec = init_double_array(2);
      
      // Add some values
      push_back_double_vector(vec, 3.14f);
      push_back_double_vector(vec, 2.718f);
      
      // Array will refuse third element because it is out of bounds 
      
      if (!push_back_double_vector(vec, 1.414f))
          printf("push back failed\n");
      
      printf("Array size: %zu\n", f_size(vec));
      printf("[ ");
      for (size_t i = 0; i < f_size(vec) - 1; i++) 
          printf("%f, ", double_vector_index(vec, 1));
      printf("%f ]\n", double_vector_index(vec, f_size(vec) - 1))
      
      free_double_vector(vec);

   Output::

      push back failed
      Array size: 2
      [ 3.140000, 2.718000 ]

   The following should be considered when using this function:

   * The vector must be properly initialized using init_double_vector() or init_double_array()
   * For static arrays (created with init_double_array), attempts to exceed capacity will fail
   * If reallocation fails for dynamic vectors, the original vector remains unchanged
   * Any double value can be stored, including zero, infinities, and NaN
   * The operation requires enough contiguous memory for the entire resized array in dynamic case

   .. note::

      When resizing is needed, the vector grows either by doubling (when size < VEC_THRESHOLD)
      or by adding a fixed amount (when size >= VEC_THRESHOLD). This provides efficient
      amortized performance for both small and large vectors.

push_front_double_vector
~~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_front_double_vector(double_v* vec, const double value)

   Adds a double value to the beginning of the vector, shifting all existing elements
   to the right. Automatically resizes the vector if needed when using dynamic allocation.
   This is the least efficient method for adding data to a double vector with
   a time efficiency of :math:`O(n)`.

   :param vec: Target double vector
   :param value: Double value to add at front
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or if static array is full,
           ENOMEM on allocation failure, ERANGE on size_t overflow

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(3);
      
      // Add some values from the back
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      
      // Add value at the front
      push_front_double_vector(vec, 1.0f);
      
      // Print all values
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      // Vector will grow automatically if needed
      push_front_double_vector(vec, 0.0f);
      
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
   Output::

      1.0 2.0 3.0
      0.0 1.0 2.0 3.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(3);
      
      // Add values to static array
      push_front_double_vector(&arr, 3.0f);
      printf("After first:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      push_front_double_vector(&arr, 2.0f);
      printf("After second: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      push_front_double_vector(&arr, 1.0f);
      printf("After third:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Array is now full - this will fail
      if (!push_front_double_vector(&arr, 0.0f)) {
          printf("Cannot add to full static array\n");
      }

   Output::

      After first:  3.0
      After second: 2.0 3.0
      After third:  1.0 2.0 3.0
      Cannot add to full static array

   The following should be considered when using this function:

   * For static arrays (created with init_double_array):
     - Attempts to exceed capacity will fail with errno set to EINVAL
     - No automatic resizing occurs
   
   * For dynamic vectors (created with init_double_vector):
     - Vector will automatically resize when full
     - Growth follows the doubling strategy for small vectors
     - Growth adds fixed amount for vectors larger than VEC_THRESHOLD
   
   * Performance considerations:
     - All existing elements must be moved right by one position
     - More expensive than push_back_double_vector for large vectors
     - Memory reallocation may occur for dynamic vectors

   .. note::

      When resizing is needed in dynamic vectors, the vector grows either by
      doubling (when size < VEC_THRESHOLD) or by adding a fixed amount
      (when size >= VEC_THRESHOLD). This provides efficient amortized
      performance while preventing excessive memory usage in large vectors.

insert_double_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_double_vector(double_v* vec, double value, size_t index)

   Inserts a double value at any valid position in the vector, shifting subsequent
   elements to the right. Automatically resizes the vector if needed when using dynamic
   allocation. The time complexity of this function varies from :math:`O(1)` 
   to :math:`O(n)` depending on the insertion position.

   :param vec: Target double vector
   :param value: Double value to insert
   :param index: Position at which to insert (0 to vec->len)
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or if static array is full,
           ERANGE for invalid index or on size_t overflow,
           ENOMEM on allocation failure

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(4);
      
      // Create initial vector
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 3.0f);
      
      // Initial state
      printf("Initial:     ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      // Insert 2.0 between them
      insert_double_vector(vec, 2.0f, 1);
      
      printf("After insert: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
   Output::

      Initial:     1.0 3.0
      After insert: 1.0 2.0 3.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(3);
      
      // Insert values at different positions
      insert_double_vector(&arr, 3.0f, 0);  // First insertion
      printf("First insert:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      insert_double_vector(&arr, 1.0f, 0);  // At beginning
      printf("Second insert: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      insert_double_vector(&arr, 2.0f, 1);  // In middle
      printf("Third insert:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Array is now full - this will fail
      if (!insert_double_vector(&arr, 4.0f, 1)) {
          printf("Cannot insert into full static array\n");
      }

   Output::

      First insert:  3.0
      Second insert: 1.0 3.0
      Third insert:  1.0 2.0 3.0
      Cannot insert into full static array

   The following should be considered when using this function:

   * For static arrays (created with init_double_array):
     - Attempts to exceed capacity will fail with errno set to EINVAL
     - No automatic resizing occurs
     - Must be careful not to exceed fixed size
   
   * For dynamic vectors (created with init_double_vector):
     - Vector will automatically resize when full
     - Growth follows the doubling strategy for small vectors
     - Growth adds fixed amount for vectors larger than VEC_THRESHOLD
   
   * Performance considerations:
     - Inserting at the beginning requires moving all elements (most expensive)
     - Inserting at the end is equivalent to push_back (least expensive)
     - Cost increases with number of elements that must be shifted
     - Memory reallocation may occur for dynamic vectors

   .. note::

      The valid range for index is [0, length]. An index equal to the length
      performs an append operation. Any index greater than the length will
      result in ERANGE error.

update_double_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: void update_double_vector(double_v* vec, size_t index, double replacement_value)

   Updates a single element in a double vector at the specified index with a new value.
   Works with both dynamic vectors and static arrays.

   :param vec: Target double vector
   :param index: Position of element to update (0 to len-1)
   :param replacement_value: New value to store at the specified index
   :raises: Sets errno to EINVAL for NULL input or empty vector,
           ERANGE for index out of bounds

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(3);
      
      // Add initial values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      
      printf("Before update: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      // Update middle value
      update_double_vector(vec, 1, 5.0f);
      
      printf("After update:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");

   Output::

      Before update: 1.0 2.0 3.0
      After update:  1.0 5.0 3.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(3);
      
      // Add values
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 2.0f);
      push_back_double_vector(&arr, 3.0f);
      
      printf("Before update: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Update first and last values
      update_double_vector(&arr, 0, 10.0f);
      update_double_vector(&arr, 2, 30.0f);
      
      printf("After update:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before update: 1.0 2.0 3.0
      After update:  10.0 2.0 30.0

   Error Handling:

   * If vec is NULL, has invalid data pointer, or is empty:
     - Sets errno to EINVAL
     - Returns without modification
   
   * If index is out of bounds:
     - Sets errno to ERANGE
     - Returns without modification

   .. note::

      This function provides direct element access for updating values. Unlike
      some other operations, it works identically for both dynamic vectors
      and static arrays since it doesn't modify the container's size or
      capacity.

Data Removal
------------
The following functions can be used to remove ``double`` data from a dynamically 
allocated vector or statically allocated array.

pop_back_double_vector
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: double pop_back_double_vector(double_v* vec)

   Removes and returns the last element from the vector or array. This is the most
   efficient removal operation as it requires no element shifting. The time 
   complexity of this function is :math:`O(1)`.

   :param vec: Target double vector
   :returns: The removed double value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(3);
      
      // Add some values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      double popped = pop_back_double_vector(vec);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining size: %zu\n", f_size(vec));
      }

   Output::

      Initial values: 1.0 2.0 3.0
      Popped value: 3.0
      Remaining size: 2

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(2);
      
      // Add values to static array
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 2.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop values
      double first_pop = pop_back_double_vector(&arr);
      printf("First pop: %.1f\n", first_pop);
      
      double second_pop = pop_back_double_vector(&arr);
      printf("Second pop: %.1f\n", second_pop);
      
      // Try to pop from empty array
      double result = pop_back_double_vector(&arr);
      if (errno == ENODATA) {
          printf("Cannot pop from empty array\n");
      }

   Output::

      Initial values: 1.0 2.0
      First pop: 2.0
      Second pop: 1.0
      Cannot pop from empty array

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

   Example with error checking:

   .. code-block:: c

      double_v* vec = init_double_vector(2);
      push_back_double_vector(vec, FLT_MAX);  // Store actual FLT_MAX
      
      errno = 0;
      double value = pop_back_double_vector(vec);
      if (errno == 0) {
          // This was a valid FLT_MAX stored in the vector
          printf("Valid FLT_MAX popped\n");
      } else if (errno == ENODATA) {
          printf("Vector is empty\n");
      } else if (errno == EINVAL) {
          printf("Invalid vector\n");
      }
      
      free_double_vector(vec);

   Output::

      Valid FLT_MAX popped

pop_front_double_vector
~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: double pop_front_double_vector(double_v* vec)

   Removes and returns the first element from the vector or array, shifting all
   remaining elements left by one position. This operation requires moving all
   remaining elements and is therefore less efficient than pop_back_double_vector.
   This function has a time complexity of :math:`O(n)`.

   :param vec: Target double vector
   :returns: The removed double value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(3);
      
      // Add some values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      double popped = pop_front_double_vector(vec);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining values: ");
          for (size_t i = 0; i < f_size(vec); i++) {
              printf("%.1f ", double_vector_index(vec, i));
          }
          printf("\n");
      }
      
   Output::

      Initial values: 1.0 2.0 3.0
      Popped value: 1.0
      Remaining values: 2.0 3.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(2);
      
      // Add values to static array
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 2.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop first value
      double first_pop = pop_front_double_vector(&arr);
      printf("First pop: %.1f\n", first_pop);
      printf("After first pop: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop remaining value
      double second_pop = pop_front_double_vector(&arr);
      printf("Second pop: %.1f\n", second_pop);
      
      // Try to pop from empty array
      double result = pop_front_double_vector(&arr);
      if (errno == ENODATA) {
          printf("Cannot pop from empty array\n");
      }

   Output::

      Initial values: 1.0 2.0
      First pop: 1.0
      After first pop: 2.0
      Second pop: 2.0
      Cannot pop from empty array

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

pop_any_double_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: double pop_any_double_vector(double_v* vec, size_t index)

   Removes and returns the element at the specified index in the vector or array,
   shifting any subsequent elements to the left. Performance varies based on the
   removal position - removing from the end is fast, while removing from the start
   or middle requires shifting elements.  This function has a time complexity that 
   range from :math:`O(n)` to :math:`O(1)` depending on the index from which 
   data is popped.

   :param vec: Target double vector
   :param index: Position of element to remove (0 to len-1)
   :returns: The removed double value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty,
           ERANGE for invalid index or on size_t overflow

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(4);
      
      // Add values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 4.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      // Pop middle value (index 1)
      double popped = pop_any_double_vector(vec, 1);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining values: ");
          for (size_t i = 0; i < f_size(vec); i++) {
              printf("%.1f ", double_vector_index(vec, i));
          }
          printf("\n");
      }

   Output::

      Initial values: 1.0 2.0 3.0 4.0
      Popped value: 2.0
      Remaining values: 1.0 3.0 4.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(3);
      
      // Add values
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 2.0f);
      push_back_double_vector(&arr, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop first value (index 0)
      double first = pop_any_double_vector(&arr, 0);
      printf("After pop first: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop last value (index 1)
      double last = pop_any_double_vector(&arr, 1);
      printf("After pop last: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Initial values: 1.0 2.0 3.0
      After pop first: 2.0 3.0
      After pop last: 2.0

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA
   
   * If index is out of bounds:
     - Returns FLT_MAX
     - Sets errno to ERANGE

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

   Performance Considerations:

   * Removing from the last position (index == len-1) is O(1)
   * Removing from the beginning requires shifting all elements left: O(n)
   * Removing from position i requires shifting n-i elements: O(n-i)
   * For frequent removals from the front, consider using pop_front_double_vector()
   * For frequent removals from the back, consider using pop_back_double_vector()

Utility Functions
=================
The following functions and macros can be used to retrieve basic information from
double containers.

Vector Size and Access Functions
--------------------------------
The following functions and macros are used to determine the allocated size and populated
length of double containers.

.. _double-size-func:

c_double_ptr 
~~~~~~~~~~~~
.. c:function:: double* c_double_ptr(double_v* vec)

   Returns a pointer to the beginning of the double precision array.  Generally 
   speaking, it is frowned upon to directly access data within the vector; 
   however, this supports integration with many legacy C libraries where the 
   array is passed as a pointer and the bounds is controlled by a size variable.

   :param vec: A pointer to a double_v data type 
   :returns: A pointer to a s style raw array 
   :raises: EINVAL for NULL input pointer or NULL data pointer.

   Example:

   .. code-block:: c 

      void print_data(double* array, size_t len) {
          printf("[ ");
          for (size_t i = 0; i < len - 1; i++) {
              printf("%lf, ", array[i]);
          }
          printf("%lf ]");
      }

      int main() {
          double_v* vec = init_double_vector(3);
          push_back_double_vector(vec, 1.0);
          push_back_double_vector(vec, 2.0);
          push_back_double_vector(vec, 3.0);
          print_data(vec, double_vector_size(vec));
          free_double_vector(vec);
          return 0;
      }

   .. code-block:: bash 

      [ 1.0000, 2.0000, 3.0000 ]

double_vector_size
~~~~~~~~~~~~~~~~~~
.. c:function:: const size_t double_vector_size(const double_v* vec)

   Returns the current number of elements in the vector. This represents the
   actual number of doubles stored, not the allocated capacity. 
   The :ref:`f_alloc <f-alloc-macro>` generic macro can be used in place of this 
   function. 

   :param vec: Double vector to query
   :returns: Number of elements in vector, or LONG_MAX on error
   :raises: Sets errno to EINVAL for NULL input or invalid data pointer

   Example:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(5);  // Capacity of 5
      push_back_double_vector(vec, 1);
      push_back_double_vector(vec, 2);
      push_back_double_vector(vec, 3);
      printf("Vector size: %zu\n", double_vector_size(vec));

   Result 

   .. code-block:: bash 

      Vector size: 3

.. _double-alloc-func:

double_vector_alloc
~~~~~~~~~~~~~~~~~~~
.. c:function:: const size_t double_vector_alloc(const double_v* vec)

   Returns the current allocation size (capacity) of the vector. This represents
   the number of elements that can be stored without requiring reallocation.
   The :ref:`f_alloc <f-alloc-macro>` generic macro can be used in place of this 
   function.

   :param vec: Double vector to query
   :returns: Current allocation size, or LONG_MAX on error
   :raises: Sets errno to EINVAL for NULL input or invalid data pointer

   Example:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(5);
      printf("Allocation size: %zu\n", double_vector_alloc(vec));

   Result 

   .. code-block:: bash 

      Allocation size: 5

double_vector_index
~~~~~~~~~~~~~~~~~~~
.. c:function:: const double double_vector_index(const double_v* vec, size_t index)

   Safely retrieves the value at the specified index in a double vector. Works with
   both dynamic vectors and static arrays.

   :param vec: Target double vector
   :param index: Position of element to retrieve (0 to len-1)
   :returns: Value at specified index, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ERANGE for index out of bounds

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec = init_double_vector(3);
      
      // Add values
      push_back_double_vector(vec, 1.5f);
      push_back_double_vector(vec, 2.5f);
      push_back_double_vector(vec, 3.5f);
      
      // Access values
      printf("First value:  %.1f\n", double_vector_index(vec, 0));
      printf("Second value: %.1f\n", double_vector_index(vec, 1));
      printf("Third value:  %.1f\n", double_vector_index(vec, 2));
      
      // Demonstrate error handling
      double result = double_vector_index(vec, 3);  // Invalid index
      if (result == FLT_MAX && errno == ERANGE) {
          printf("Error: Index out of bounds\n");
      }
      
      free_double_vector(vec);

   Output::

      First value:  1.5
      Second value: 2.5
      Third value:  3.5
      Error: Index out of bounds

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(2);
      
      // Add values
      push_back_double_vector(&arr, 10.0f);
      push_back_double_vector(&arr, 20.0f);
      
      // Safe access
      errno = 0;
      double first = double_vector_index(&arr, 0);
      if (errno == 0) {
          printf("First element: %.1f\n", first);
      }
      
      // Boundary check
      errno = 0;
      double invalid = double_vector_index(&arr, 5);
      if (errno == ERANGE) {
          printf("Attempted access beyond array bounds\n");
      }

   Output::

      First element: 10.0
      Attempted access beyond array bounds

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If index is out of bounds:
     - Returns FLT_MAX
     - Sets errno to ERANGE

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between
      an error condition and a valid FLT_MAX value that was stored in the
      vector. This function provides bounds-checked access to prevent
      buffer overflows and undefined behavior.

Re-Order and Sort Data 
----------------------
These functions can be used to change the order of data in a dynamically allocated 
or statically allocated ``double`` array.

reverse_double_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: void reverse_double_vector(double_v* vec)

   Reverses the order of elements in a double vector or array. The operation is performed
   in place without allocating additional memory.

   :param vec: Target double vector
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec = init_double_vector(4);
      
      // Add some values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      
      printf("Before reverse: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      reverse_double_vector(vec);
      
      printf("After reverse:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      free_double_vector(vec);

   Output::

      Before reverse: 1.0 2.0 3.0
      After reverse:  3.0 2.0 1.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(3);
      
      // Add values
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 2.0f);
      push_back_double_vector(&arr, 3.0f);
      
      printf("Before reverse: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      reverse_double_vector(&arr);
      
      printf("After reverse:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before reverse: 1.0 2.0 3.0
      After reverse:  3.0 2.0 1.0

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Sets errno to EINVAL
     - Returns without modifying data
   
   * If vector or array is empty:
     - Sets errno to ENODATA
     - Returns without modifying data

   Performance Characteristics:

   * Time complexity: O(n) where n is the number of elements
   * Space complexity: O(1) as reversal is performed in place
   * Uses constant extra space regardless of vector size
   
   .. note::

      The function performs the reversal in place by swapping pairs of elements
      from the ends toward the middle. This approach minimizes memory usage and
      maintains efficiency for both small and large vectors.

sort_double_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: void sort_double_vector(double_v* vec, iter_dir direction)

   Sorts a double vector or array in either ascending (FORWARD) or descending (REVERSE) order
   using an optimized QuickSort algorithm with median-of-three pivot selection and
   insertion sort for small subarrays.

   :param vec: Target double vector
   :param direction: FORWARD for ascending, REVERSE for descending order
   :raises: Sets errno to EINVAL if vec is NULL

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec = init_double_vector(5);
      
      // Add some unsorted values
      push_back_double_vector(vec, 5.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      
      printf("Before sort: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      // Sort in ascending order
      sort_double_vector(vec, FORWARD);
      
      printf("Ascending:   ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      // Sort in descending order
      sort_double_vector(vec, REVERSE);
      
      printf("Descending:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      free_double_vector(vec);

   Output::

      Before sort: 5.0 3.0 4.0 1.0 2.0
      Ascending:   1.0 2.0 3.0 4.0 5.0
      Descending:  5.0 4.0 3.0 2.0 1.0

   Example with static array:

   .. code-block:: c

      double_v arr = init_double_array(4);
      
      // Add unsorted values
      push_back_double_vector(&arr, 4.0f);
      push_back_double_vector(&arr, 1.0f);
      push_back_double_vector(&arr, 3.0f);
      push_back_double_vector(&arr, 2.0f);
      
      printf("Before sort: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      sort_double_vector(&arr, FORWARD);
      
      printf("After sort:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before sort: 4.0 1.0 3.0 2.0
      After sort:  1.0 2.0 3.0 4.0

   Implementation Details:

   The sorting algorithm uses a hybrid approach combining QuickSort with
   Insertion Sort for optimal performance:

   * QuickSort with median-of-three pivot selection for large partitions
   * Insertion Sort for small partitions (less than 10 elements)
   * Tail-call optimization to reduce stack usage
   * Special handling for duplicate elements and special values (NaN, infinities)

   Performance Characteristics:

   * Average time complexity: O(n log n)
   * Worst case time complexity: O(n²) (rare due to median-of-three)
   * Space complexity: O(log n) for recursion stack
   * In-place sorting: No additional memory allocation
   * Stable: No, equal elements may be reordered
   * Adaptive: Yes, performs better on partially sorted arrays

   Special Value Handling:

   * NaN values are moved to the end of the array
   * Infinities are properly ordered (-∞ < finite numbers < +∞)
   * Zero values (both -0.0 and +0.0) are treated as equal

   .. note::

      For very small arrays (n < 10), the function automatically uses Insertion Sort
      instead of QuickSort, as this is more efficient for small datasets.

Search Vector 
-------------

binary_search_double_vector
~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: size_t binary_search_double_vector(double_v* vec, double value, double tolerance, bool sort_first)

   Performs a binary search on a double vector to find a value within the specified
   tolerance. Can optionally sort the vector before searching. Returns the index of
   the first matching value found, or LONG_MAX if not found.

   :param vec: Target double vector
   :param value: Double value to search for
   :param tolerance: Maximum allowed difference between values to consider a match
   :param sort_first: If true, sorts the vector before searching
   :returns: Index of found value, or LONG_MAX if not found
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with ordered vector:

   .. code-block:: c

      double_v* vec = init_double_vector(5);
      
      // Add sorted values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, 5.0f);
      
      // Search for exact value
      size_t index = binary_search_double_vector(vec, 3.0f, 0.0001f, false);
      if (index != LONG_MAX) {
          printf("Found 3.0 at index %zu\n", index);
      }
      
      // Search with tolerance
      index = binary_search_double_vector(vec, 2.95f, 0.1f, false);
      if (index != LONG_MAX) {
          printf("Found value near 2.95 at index %zu\n", index);
      }
      
      free_double_vector(vec);

   Output::

      Found 3.0 at index 2
      Found value near 2.95 at index 2

   Example with unordered vector:

   .. code-block:: c

      double_v* vec = init_double_vector(5);
      
      // Add unsorted values
      push_back_double_vector(vec, 5.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 3.0f);
      
      // Search with auto-sort
      size_t index = binary_search_double_vector(vec, 4.0f, 0.0001f, true);
      if (index != LONG_MAX) {
          printf("Found 4.0 at index %zu\n", index);
      }
      
      // Vector is now sorted for subsequent searches
      index = binary_search_double_vector(vec, 2.0f, 0.0001f, false);
      if (index != LONG_MAX) {
          printf("Found 2.0 at index %zu\n", index);
      }
      
      free_double_vector(vec);

   Output::

      Found 4.0 at index 3
      Found 2.0 at index 1

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns LONG_MAX
     - Sets errno to EINVAL
   
   * If vector is empty:
     - Returns LONG_MAX
     - Sets errno to ENODATA
   
   * If value is not found within tolerance:
     - Returns LONG_MAX
     - Does not set errno

   Performance Characteristics:

   * Time Complexity:
     - O(log n) if vector is sorted and sort_first is false
     - O(n log n) if sort_first is true due to sorting overhead
   * Space Complexity: O(1)

   .. note::

      The tolerance parameter allows for approximate matches, which is useful when
      working with doubleing-point values that may have small representation
      errors. Setting tolerance to 0.0f requires an exact match.

Min and Max Values 
------------------
The following functions can be used to find the maximum and minimum values 
in a dynamically allocated vector or a statically allocated array.

Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
such as AVX or SSE where supported, enabling high-performance parallel summation
of double values. Falls back to scalar implementation on platforms where SIMD is 
not available.

.. note:: 

   If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of double arrays.

min_double_vector
~~~~~~~~~~~~~~~~~
.. c:function:: double min_double_vector(double_v* vec)

   Returns the minimum value in a double vector. Works with both dynamic vectors
   and static arrays.

   :param vec: Target double vector
   :returns: Minimum value in vector, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or invalid data pointer

   Example:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(5);
      
      // Add values
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, -2.0f);
      push_back_double_vector(vec, 5.0f);
      
      double min_val = min_double_vector(vec);
      if (errno == 0) {
          printf("Minimum value: %.1f\n", min_val);
      }
      
   Output::

      Minimum value: -2.0

max_double_vector
~~~~~~~~~~~~~~~~~
.. c:function:: double max_double_vector(double_v* vec)

   Returns the maximum value in a double vector. Works with both dynamic vectors
   and static arrays.

   :param vec: Target double vector
   :returns: Maximum value in vector, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or invalid data pointer

   Example:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(5);
      
      // Add values
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, -2.0f);
      push_back_double_vector(vec, 5.0f);
      
      double max_val = max_double_vector(vec);
      if (errno == 0) {
          printf("Maximum value: %.1f\n", max_val);
      }

   Output::

      Maximum value: 5.0

Example using both functions with static array:

   .. code-block:: c

      double_v arr = init_double_array(4);
      
      // Add values
      push_back_double_vector(&arr, 3.14f);
      push_back_double_vector(&arr, -1.5f);
      push_back_double_vector(&arr, 2.718f);
      push_back_double_vector(&arr, 0.0f);
      
      printf("Values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.3f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      printf("Min: %.3f\n", min_double_vector(&arr));
      printf("Max: %.3f\n", max_double_vector(&arr));

   Output::

      Values: 3.140 -1.500 2.718 0.000
      Min: -1.500
      Max: 3.140

Error Handling for Both Functions:

* If vec is NULL, has invalid data pointer, or is empty:
  - Returns FLT_MAX
  - Sets errno to EINVAL

Special Value Handling:

* NaN values are ignored
* Infinities are properly compared
* Both positive and negative zeros are treated as equal

.. note::

   When FLT_MAX is returned, always check errno to distinguish between
   an error condition and a valid FLT_MAX value that was actually present
   in the vector.

Vector Statistics
------------------
These functions can be used to determine basic statistical parameters of a 
vector or array.

sum_double_vector
~~~~~~~~~~~~~~~~~
.. c:function:: double sum_double_vector(double_v* vec)

   Calculates the sum of all elements in a double vector. Works with both dynamic
   vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of double values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target double vector
   :returns: Sum of all elements, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or NaN values

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of double arrays. 

   Example:

   .. code-block:: c

      double_v* vec = init_double_vector(4);
      
      // Add values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 4.0f);
      
      double sum = sum_double_vector(vec);
      if (errno == 0) {
          printf("Sum: %.1f\n", sum);
      }
      
      free_double_vector(vec);

   Output::

      Sum: 10.0

average_double_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: double average_double_vector(double_v* vec)

   Calculates the arithmetic mean (average) of all elements in a double vector.
   Works with both dynamic vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of double values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target double vector
   :returns: Average of all elements, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or NaN values

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of double arrays.

   Example:

   .. code-block:: c

      double_v* vec = init_double_vector(4);
      
      // Add values
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, 6.0f);
      push_back_double_vector(vec, 8.0f);
      
      double avg = average_double_vector(vec);
      if (errno == 0) {
          printf("Average: %.1f\n", avg);
      }
      
      free_double_vector(vec);

   Output::

      Average: 5.0

Example using both functions with static array:

   .. code-block:: c

      double_v arr = init_double_array(3);
      
      // Add values
      push_back_double_vector(&arr, 1.5f);
      push_back_double_vector(&arr, 2.5f);
      push_back_double_vector(&arr, 3.5f);
      
      printf("Values:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", double_vector_index(&arr, i));
      }
      printf("\n");
      
      printf("Sum:     %.1f\n", sum_double_vector(&arr));
      printf("Average: %.1f\n", average_double_vector(&arr));

   Output::

      Values:  1.5 2.5 3.5
      Sum:     7.5
      Average: 2.5

Special Value Handling:

* Infinity values are allowed and propagate through calculations
* NaN values will cause the functions to return FLT_MAX and set errno to EINVAL
* Both positive and negative zeros are handled correctly

Error Handling for Both Functions:

* If vec is NULL, has invalid data pointer, or is empty:
  - Returns FLT_MAX
  - Sets errno to EINVAL
* If any value in the vector is NaN:
  - Returns FLT_MAX
  - Sets errno to EINVAL

.. note::

   When FLT_MAX is returned, always check errno to distinguish between
   an error condition and a valid calculation that resulted in FLT_MAX.

stdev_double_vector
~~~~~~~~~~~~~~~~~~~
.. c:function:: double stdev_double_vector(double_v* vec)

   Calculates the population standard deviation of elements in a double vector.
   Works with both dynamic vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of double values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target double vector
   :returns: Standard deviation of elements, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input or empty vector

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of double arrays.

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(4);
      
      // Add values
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, 4.0f);
      push_back_double_vector(vec, 6.0f);
      
      printf("Values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      double stdev = stdev_double_vector(vec);
      if (errno == 0) {
          printf("Standard Deviation: %.3f\n", stdev);
      }

   Output::

      Values: 2.0 4.0 4.0 6.0
      Standard Deviation: 1.414

Cummulative Distribution Function (CDF)
---------------------------------------

cum_sum_double_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: double_v* cum_sum_double_vector(double_v* vec)

   Creates a new vector containing the cumulative sum of elements from the input vector.
   Each element in the output vector is the sum of all elements up to and including
   that position in the input vector. Works with both dynamic vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of double values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target double vector
   :returns: New vector containing cumulative sums, or NULL on error
   :raises: Sets errno to EINVAL for NULL input or empty vector, ENODATA for failed push operations

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of double arrays.

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(4);
      
      // Add values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 4.0f);
      
      printf("Original values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      double_v* cum_sum = cum_sum_double_vector(vec);
      if (cum_sum != NULL) {
          printf("Cumulative sums: ");
          for (size_t i = 0; i < f_size(cum_sum); i++) {
              printf("%.1f ", double_vector_index(cum_sum, i));
          }
          printf("\n");
          
          free_double_vector(cum_sum);
      }

   Output::

      Original values: 1.0 2.0 3.0 4.0
      Cumulative sums: 1.0 3.0 6.0 10.0

Example using both functions with negative values:

   .. code-block:: c

      double_v* vec = init_double_vector(4);
      
      // Add values including negatives
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, -2.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, -4.0f);
      
      printf("Values:          ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");
      
      double stdev = stdev_double_vector(vec);
      if (errno == 0) {
          printf("Std Deviation:   %.3f\n", stdev);
      }
      
      double_v* cum_sum = cum_sum_double_vector(vec);
      if (cum_sum != NULL) {
          printf("Running totals:  ");
          for (size_t i = 0; i < f_size(cum_sum); i++) {
              printf("%.1f ", double_vector_index(cum_sum, i));
          }
          printf("\n");
          
          free_double_vector(cum_sum);
      }
      
      free_double_vector(vec);

   Output::

      Values:          1.0 -2.0 3.0 -4.0
      Std Deviation:   2.944
      Running totals:  1.0 -1.0 2.0 -2.0

Error Handling:

* If vec is NULL, has invalid data pointer, or is empty:
  - stdev_double_vector returns FLT_MAX and sets errno to EINVAL
  - cum_sum_double_vector returns NULL and sets errno to EINVAL

* If memory allocation fails in cum_sum_double_vector:
  - Returns NULL
  - Sets errno to ENODATA

Special Value Handling:

* Infinity values propagate through calculations
* Result will be infinite if any calculations overflow
* Both functions handle negative values correctly

.. note::

   The standard deviation calculation uses a population standard deviation
   formula (dividing by n), not a sample standard deviation formula
   (dividing by n-1).

Copy Vector 
~~~~~~~~~~~
.. c:function:: double_v* copy_double_vector(double_v* vec)

   Creates a deep copy of a vector or array.

   :param vec: The vector or array to be copied
   :returns: New vector containing a copy of the input vector
   :raises: Sets errno to EINVAL for NULL input or empty vector, ENOMEM for failure to create new vector

   .. note:: 

      This method will always output a dynamically allocated array even if the input array is statically allocated.

   Example with dynamic vector:

   .. code-block:: c

      double_v* vec DBLEVEC_GBC = init_double_vector(4);
      
      // Add values
      push_back_double_vector(vec, 1.0f);
      push_back_double_vector(vec, 2.0f);
      push_back_double_vector(vec, 3.0f);
      push_back_double_vector(vec, 4.0f);
      
      printf("Original values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", double_vector_index(vec, i));
      }
      printf("\n");

      double_v* DBLEVEC_GBC new_vec = copy_double_vector(vec);
      
      if (new_vec != NULL) {
          printf("New values: ");
          for (size_t i = 0; i < f_size(new_vec); i++) {
              printf("%.1f ", double_vector_index(new_vec, i));
          }
          printf("\n");
      }

   Output::

      Original values: 1.0 2.0 3.0 4.0
      New values: 1.0 2.0 3.0 4.0
