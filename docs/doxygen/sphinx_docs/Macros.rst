****************************
Macros and Utility Functions
****************************

Generic Macro Overview
======================

The C language's ``_Generic`` keyword, introduced in C11, enables type-safe generic programming
through compile-time type selection. This feature allows the creation of macros that can
adapt their behavior based on argument types while maintaining type safety and avoiding the
pitfalls of traditional C macros.

The macros in this library leverage ``_Generic`` to provide intuitive interfaces that work
seamlessly with both string literals (char*) and string_t objects. This type-safe approach
ensures proper function selection at compile time, reducing runtime overhead and preventing
type-related errors.

Benefits of Generic Macros
--------------------------
- Type safety: Compiler catches type mismatches at compile time
- Code clarity: Single interface for multiple types
- Performance: No runtime overhead for type checking
- Maintainability: Centralized type handling logic

Size Inspection Macros
----------------------
These macros provide several advantages:

- **Consistency**: Same syntax for both string types
- **Type Safety**: Compiler ensures correct function selection
- **Code Clarity**: Eliminates need to remember type-specific function names
- **Maintainability**: Changes to underlying functions only need updates in one place

.. _f-size-macro:

d_size
~~~~~~
A generic macro that provides a uniform interface for checking the current populated
size of both double_v and xxxx objects.  This macro works with dynamically allocated 
vectors and statically allocated arrays.  It automatically selects:

- For double_v*: Uses double_vector_size to get string length

This macro simplifies size queries by providing a consistent interface regardless of
the underlying type. This Macro may be safely used in place of the 
:ref:`double_vector_size() <double-size-func>`, :ref:`double_dict_size() <double-dict-size-func>` 
and :ref:`double_dictv_size() <doublev-dict-size-func>` functions.

Example:

.. code-block:: c

   double_v* vec DBLEVEC_GBC = init_double_vector(5);  // Capacity of 5
   push_back_double_vector(vec, 1);
   push_back_double_vector(vec, 2);
   push_back_double_vector(vec, 3);
   printf("Vector size: %zu\n", f_size(vec));

Result

.. code-block:: bash 

   Vector size: 5

.. _f-alloc-macro:

d_alloc
~~~~~~~
A generic macro that provides a uniform interface for checking the current allocation
size of both double_v xxxx objects. This macro works with dynamically allocated vectors 
and statically allocated arrays. It selects:

- For double_v*: Uses double_vector_alloc to get allocated vector capacity

This macro is particularly useful for capacity planning and debugging memory usage.
This Macro may be safely used in place of the :ref:`double_vector_alloc() <double-alloc-func>`
:ref:`double_dict_alloc() <double-dict-alloc-func>` and 
:ref:`double_dictv_alloc() <doublev-dict-alloc-func>` functions.

Example:

.. code-block:: c

    double_v* vec = init_double_array(5);  // Capacity of 5
    push_back_double_vector(vec, 1);
    push_back_double_vector(vec, 2);
    push_back_double_vector(vec, 3);
    printf("Array Length: %zu\n", d_size(vec));
    printf("Array Allocated Size: %zu\n", d_alloc(vec));

Result 

.. code-block:: bash 

   Array Length: 3
   Array Allocated Size: 5

Utility Functions 
=================
The functions listed below can be used to augment ``double`` object handling.

swap_double
-----------
.. c:function:: void swap_double(double* a, double* b)

   Swaps the values of two double variables. Uses a temporary variable to ensure
   safe exchange of values.

   :param a: Pointer to first double value
   :param b: Pointer to second double value
   :raises: Sets errno to EINVAL for NULL input pointers

   Example:

   .. code-block:: c

      double x = 1.0f;
      double y = 2.0f;
      
      printf("Before swap: x = %.1f, y = %.1f\n", x, y);
      swap_double(&x, &y);
      printf("After swap:  x = %.1f, y = %.1f\n", x, y);

   Output::

      Before swap: x = 1.0, y = 2.0
      After swap:  x = 2.0, y = 1.0

   .. note::

      This utility function serves as a helper for other double manipulation functions
      but can also be used independently for general double value swapping operations.
