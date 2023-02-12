/**
 * @file matrix.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Real matrix operations. A matrix is represented as a 1D array of floating points.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

/**
 * @brief Zeros out a matrix.
 *
 * @param[out] m Matrix to zero out.
 * @param r Number of rows.
 * @param c Number of columns.
 */
void matrix_zero(float *m, uint16_t r, uint16_t c);

/**
 * @brief Calculates the transpose of a matrix.
 *
 * @param[in] m Matrix to transpose.
 * @param r Number of rows.
 * @param c Number of columns.
 * @param[out] res Transpose of the matrix.
 */
void matrix_transpose(const float *m, uint16_t r, uint16_t c, float *res);

/**
 * @brief Calculates the Cholesky decomposition of a symmetric matrix.
 *
 * @param[in] m Symmetric matrix to decompose.
 * @param r Number of rows.
 * @param[out] res Cholesky decomposition of the matrix.
 */
void matrix_cholesky(const float *m, uint16_t r, float *res);

/**
 * @brief Calculates the inverse of a symmetric matrix.
 *
 * @param[in] m Symmetric matrix to invert.
 * @param r Number of rows.
 * @param[out] res Inverse of the matrix.
 */
void matrix_inv_sym(const float *m, uint16_t r, float *res);

/**
 * @brief Adds a scalar to a matrix.
 *
 * @param m Matrix to add the scalar to.
 * @param t Scalar to add.
 * @param r Number of rows.
 * @param c Number of columns.
 * @param res Result of the addition.
 */
void matrix_add_scalar(const float *m, float t, uint16_t r, uint16_t c, float *res);

/**
 * @brief Adds a scalar multiple of an identity matrix to a square matrix.
 *
 * @param m Square matrix to add the scalar multiple of the identity matrix to.
 * @param t Scalar to multiply the identity matrix with.
 * @param r Number of rows.
 * @param res Result of the addition.
 */
void matrix_add_eye(const float *m, float t, uint16_t r, float *res);

/**
 * @brief Multiplies a matrix with a scalar.
 *
 * @param m Matrix to multiply.
 * @param t Scalar to multiply with.
 * @param r Number of rows.
 * @param c Number of columns.
 * @param res Result of the multiplication.
 */
void matrix_mul_scalar(const float *m, float t, uint16_t r, uint16_t c, float *res);

/**
 * @brief Adds two matrices.
 *
 * @param a First matrix.
 * @param b Second matrix.
 * @param r Number of rows.
 * @param c Number of columns.
 * @param res Result of the addition.
 */
void matrix_add(const float *a, const float *b, uint16_t r, uint16_t c, float *res);

/**
 * @brief Subtracts two matrices.
 *
 * @param a First matrix.
 * @param b Second matrix.
 * @param r Number of rows.
 * @param c Number of columns.
 * @param res Result of the subtraction.
 */
void matrix_sub(const float *a, const float *b, uint16_t r, uint16_t c, float *res);

/**
 * @brief Multiplies two matrices.
 *
 * @param a First matrix.
 * @param b Second matrix.
 * @param r Number of rows of the first matrix.
 * @param c Number of columns of the first matrix.
 * @param n Number of columns of the second matrix.
 * @param res Result of the multiplication.
 */
void matrix_mul(const float *a, const float *b, uint16_t r, uint16_t c, uint16_t n, float *res);

/**
 * @brief Gets the upper triangular part of a matrix.
 *
 * @param m Matrix.
 * @param r Number of rows.
 * @param c Number of columns.
 * @param res Upper triangular part of the matrix in row-major order.
 */
void matrix_get_upper_triangular(const float *m, uint16_t r, uint16_t c, float *res);

#endif
