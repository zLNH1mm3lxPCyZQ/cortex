#pragma once

#include "buffer.h"

#define CX_TENSOR_MAX_N_DIM 6

typedef enum {
    CX_TENSOR_OK,
    CX_TENSOR_NULL_POINTER,
    CX_TENSOR_ALLOCATION_FAILED,
    CX_TENSOR_ALREADY_INITIALIZED,
    CX_TENSOR_INVALID_ARGUMENT,
    CX_TENSOR_INVALID_SHAPE,
    CX_TENSOR_SHAPE_MISMATCH,
    CX_TENSOR_INVALID_OPERATION,
    CX_TENSOR_OUT_OF_BOUNDS,
    CX_TENSOR_ZERO_DIVISION,
    CX_TENSOR_BROADCASTING_FAILED,
} cxTensorStatus;

typedef struct {
    size_t dims[CX_TENSOR_MAX_N_DIM];
    size_t n_dim;
} cxTensorShape;

typedef struct {
    cxBuffer* buffer;
    cxTensorShape shape;
} cxTensor;


// tensor creation and destruction

cxTensorStatus cx_tensor_create(cxTensor** tensor, const cxTensorShape shape);
cxTensorStatus cx_tensor_destroy(cxTensor** tensor);
cxTensorStatus cx_tensor_from_array(cxTensor** tensor, const float* array, const cxTensorShape shape);
cxTensorStatus cx_tensor_to_array(const cxTensor* tensor, float* array);
cxTensorStatus cx_tensor_clone(cxTensor** result, const cxTensor* tensor);

// tensor initialization

cxTensorStatus cx_tensor_zeros(cxTensor** tensor, const cxTensorShape shape);
cxTensorStatus cx_tensor_zeros_like(cxTensor** tensor, const cxTensor* other);
cxTensorStatus cx_tensor_ones(cxTensor** tensor, const cxTensorShape shape);
cxTensorStatus cx_tensor_ones_like(cxTensor** tensor, const cxTensor* other);
cxTensorStatus cx_tensor_full(cxTensor** tensor, float value, const cxTensorShape shape);
cxTensorStatus cx_tensor_full_like(cxTensor** tensor, float value, const cxTensor* other);
cxTensorStatus cx_tensor_eye(cxTensor** tensor, const cxTensorShape shape);

// tensor manipulation

bool cx_tensor_is_scalar(const cxTensor* tensor);
bool cx_tensor_is_vector(const cxTensor* tensor);
bool cx_tensor_is_matrix(const cxTensor* tensor);
bool cx_tensor_is_square(const cxTensor* tensor);
bool cx_tensor_has_shape(const cxTensor* tensor, const cxTensorShape shape);

bool cx_tensor_can_reshape(const cxTensor* tensor, const cxTensorShape shape);
bool cx_tensor_can_broadcast(const cxTensor* tensor, const cxTensorShape shape);
bool cx_tensor_can_dot(const cxTensor* a, const cxTensor* b);
bool cx_tensor_can_cross(const cxTensor* a, const cxTensor* b);
bool cx_tensor_can_matmul(const cxTensor* a, const cxTensor* b);
bool cx_tensor_can_tensordot(const cxTensor* a, const cxTensor* b, const size_t* a_axes, const size_t* b_axes, size_t num_axes);
bool cx_tensor_can_kronecker(const cxTensor* a, const cxTensor* b);

cxTensorStatus cx_tensor_reshape(cxTensor** result, cxTensor* tensor, const cxTensorShape shape);
cxTensorStatus cx_tensor_reshape_in_place(cxTensor* tensor, const cxTensorShape shape);
cxTensorStatus cx_tensor_broadcast(cxTensor** result, const cxTensor* a, const cxTensorShape shape);
cxTensorStatus cx_tensor_flatten(cxTensor** result, cxTensor* tensor);
cxTensorStatus cx_tensor_flatten_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_permute(cxTensor** result, cxTensor* tensor, const size_t axes[], const size_t n_axes);
cxTensorStatus cx_tensor_permute_in_place(cxTensor* tensor, const size_t axes[], const size_t n_axes);

// scalar ops

cxTensorStatus cx_tensor_scalar_add(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_add_in_place(cxTensor* tensor, float scalar);
cxTensorStatus cx_tensor_scalar_sub(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_sub_in_place(cxTensor* tensor, float scalar);
cxTensorStatus cx_tensor_scalar_mult(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_mult_in_place(cxTensor* tensor, float scalar);
cxTensorStatus cx_tensor_scalar_div(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_div_in_place(cxTensor* tensor, float scalar);
cxTensorStatus cx_tensor_scalar_pow(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_pow_in_place(cxTensor* tensor, float scalar);
cxTensorStatus cx_tensor_scalar_log(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_log_in_place(cxTensor* tensor, float scalar);
cxTensorStatus cx_tensor_scalar_mod(cxTensor** result, const cxTensor* a, float scalar);
cxTensorStatus cx_tensor_scalar_mod_in_place(cxTensor* tensor, float scalar);

// unary ops

cxTensorStatus cx_tensor_unary_abs(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_abs_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_sign(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_sign_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_sin(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_sin_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_cos(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_cos_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_tan(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_tan_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_negate(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_negate_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_reciprocal(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_reciprocal_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_floor(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_floor_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_ceil(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_ceil_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_round(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_round_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_trunc(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_trunc_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_clip(cxTensor** result, const cxTensor* a, float min_value, float max_value);
cxTensorStatus cx_tensor_unary_clip_in_place(cxTensor* tensor, float min_value, float max_value);
cxTensorStatus cx_tensor_unary_trace(cxTensor** result, const cxTensor* a, const size_t axis_1, const size_t axis_2);

cxTensorStatus cx_tensor_unary_exp(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_exp_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_log(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_log_in_place(cxTensor* tensor);
cxTensorStatus cx_tensor_unary_sqrt(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_sqrt_in_place(cxTensor* tensor);

// binary ops

cxTensorStatus cx_tensor_binary_add(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_add_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_sub(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_sub_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_div(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_div_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_mult(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_mult_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_mod(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_mod_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_pow(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_pow_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_log(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_log_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_min(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_min_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_binary_max(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_max_in_place(cxTensor* tensor, const cxTensor* other);

// need to add a boolean tensor typedef ??

cxTensorStatus cx_tensor_binary_equal(cxTensor** result, const cxTensor* a, const cxTensor* b, const float tol);
cxTensorStatus cx_tensor_binary_not_equal(cxTensor** result, const cxTensor* a, const cxTensor* b, const float tol);
cxTensorStatus cx_tensor_binary_greater(cxTensor** result, const cxTensor* a, const cxTensor* b, const float tol);
cxTensorStatus cx_tensor_binary_greater_equal(cxTensor** result, const cxTensor* a, const cxTensor* b, const float tol);
cxTensorStatus cx_tensor_binary_less(cxTensor** result, const cxTensor* a, const cxTensor* b, const float tol);
cxTensorStatus cx_tensor_binary_less_equal(cxTensor** result, const cxTensor* a, const cxTensor* b, const float tol);

// tensor products

cxTensorStatus cx_tensor_product_tensordot(cxTensor** result, const cxTensor* a, const cxTensor* b, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_product_hadamard(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_product_hadamard_in_place(cxTensor* tensor, const cxTensor* other);
cxTensorStatus cx_tensor_product_matrix(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_product_inner(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_product_outer(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_product_kronecker(cxTensor** result, const cxTensor* a, const cxTensor* b);

// reductions

cxTensorStatus cx_tensor_reduction_sum(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_product(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_min(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_max(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_mean(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_var(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_std(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_argmin(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_reduction_argmax(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes);
cxTensorStatus cx_tensor_linalg_norm(cxTensor** result, const cxTensor* a, const size_t* axes, const size_t num_axes, const float ord);




// logical operations maybe should only be applied to boolean tensors ??
// boolean ops may need its own file decls
cxTensorStatus cx_tensor_unary_equal(cxTensor** result, const cxTensor* a, const float value, const float tol);
cxTensorStatus cx_tensor_unary_not_equal(cxTensor** result, const cxTensor* a, const float value, const float tol);
cxTensorStatus cx_tensor_unary_greater(cxTensor** result, const cxTensor* a, const float value);
cxTensorStatus cx_tensor_unary_greater_equal(cxTensor** result, const cxTensor* a, const float value, const float tol);
cxTensorStatus cx_tensor_unary_less(cxTensor** result, const cxTensor* a, const float value);
cxTensorStatus cx_tensor_unary_less_equal(cxTensor** result, const cxTensor* a, const float value, const float tol);

// apply only to boolean tensors ?
cxTensorStatus cx_tensor_unary_logical_not(cxTensor** result, const cxTensor* a);
cxTensorStatus cx_tensor_unary_logical_and(cxTensor** result, const cxTensor* a, const float value);
cxTensorStatus cx_tensor_unary_logical_or(cxTensor** result, const cxTensor* a, const float value);
cxTensorStatus cx_tensor_unary_logical_xor(cxTensor** result, const cxTensor* a, const float value);

cxTensorStatus cx_tensor_binary_logical_and(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_logical_or(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_binary_logical_xor(cxTensor** result, const cxTensor* a, const cxTensor* b);


// specific linalg ops (2D, 3D)
float cx_tensor_linalg_scalarize(const cxTensor* tensor);
float cx_tensor_linalg_dot(const cxTensor* a, const cxTensor* b);
float cx_tensor_linalg_norm(const cxTensor* a);
float cx_tensor_linalg_trace(const cxTensor* a);
float cx_tensor_linalg_determinant(const cxTensor* a);
float cx_tensor_linalg_inverse(const cxTensor* a);
float cx_tensor_linalg_rank(const cxTensor* a);
float cx_tensor_linalg_diag(const cxTensor* a);

cxTensorStatus cx_tensor_linalg_cross(cxTensor** result, const cxTensor* a, const cxTensor* b);
cxTensorStatus cx_tensor_linalg_matmul(cxTensor** result, const cxTensor* a, const cxTensor* b);
