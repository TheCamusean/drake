#ifndef DRAKEGRADIENTUTIL_H_
#define DRAKEGRADIENTUTIL_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <cmath>
#include <vector>
#include <array>
#include <assert.h>

template<unsigned int Size>
std::array<int, Size> intRange(int start)
{
  std::array<int, Size> ret;
  for (unsigned int i = 0; i < Size; i++) {
    ret[i] = i + start;
  }
  return ret;
}

/*
 * Recursively defined template specifying a matrix type of the correct size for a gradient of a matrix function with respect to Nq variables, of any order.
 */
template<typename Derived, int Nq, int DerivativeOrder = 1>
struct Gradient {
  typedef Eigen::Matrix<typename Derived::Scalar, (Derived::SizeAtCompileTime == Eigen::Dynamic || Nq == Eigen::Dynamic) ? Eigen::Dynamic : Gradient<Derived, Nq, DerivativeOrder - 1>::type::SizeAtCompileTime, Nq> type;
};

/*
 * Base case for recursively defined gradient template.
 */
template<typename Derived, int Nq>
struct Gradient<Derived, Nq, 1> {
  typedef Eigen::Matrix<typename Derived::Scalar, Derived::SizeAtCompileTime, Nq> type;
};

#define matGradMultMatNumRows(rows_A, cols_B) (((rows_A) == Eigen::Dynamic || (cols_B) == Eigen::Dynamic) ? Eigen::Dynamic : ((rows_A) * (cols_B)))

#define matGradMultNumRows(rows_dA, rows_b) (((rows_dA) == Eigen::Dynamic || (rows_b) == Eigen::Dynamic) ? Eigen::Dynamic : ((rows_dA) / (rows_b)))

#define getRowBlockGradientNumRows(M_cols, block_rows) ((M_cols) == Eigen::Dynamic ? Eigen::Dynamic : (M_cols) * (block_rows))

/*
 * Profile results: looks like return value optimization works; a version that sets a reference
 * instead of returning a value is just as fast and this is cleaner.
 */
template <typename Derived>
typename Derived::PlainObject transposeGrad(
    const Eigen::MatrixBase<Derived>& dX, int rows_X);

template <typename DerivedA, typename DerivedB, typename DerivedDA, typename DerivedDB>
Eigen::Matrix<typename DerivedA::Scalar, matGradMultMatNumRows(DerivedA::RowsAtCompileTime, DerivedB::ColsAtCompileTime), DerivedDA::ColsAtCompileTime>
matGradMultMat(
    const Eigen::MatrixBase<DerivedA>& A,
    const Eigen::MatrixBase<DerivedB>& B,
    const Eigen::MatrixBase<DerivedDA>& dA,
    const Eigen::MatrixBase<DerivedDB>& dB);


template<typename DerivedDA, typename Derivedb>
Eigen::Matrix<typename DerivedDA::Scalar, matGradMultNumRows(DerivedDA::RowsAtCompileTime, Derivedb::RowsAtCompileTime), DerivedDA::ColsAtCompileTime>
matGradMult(const Eigen::MatrixBase<DerivedDA>& dA, const Eigen::MatrixBase<Derivedb>& b);

// TODO: could save copies once http://eigen.tuxfamily.org/bz/show_bug.cgi?id=329 is fixed
template<typename Derived>
Eigen::Matrix<typename Derived::Scalar, Eigen::Dynamic, Eigen::Dynamic> getSubMatrixGradient(
    const Eigen::MatrixBase<Derived>& dM, const std::vector<int>& rows, const std::vector<int>& cols,
    int M_rows, int q_start = 0, int q_subvector_size = -1);

template<int QSubvectorSize = -1, typename Derived, std::size_t NRows, std::size_t NCols>
Eigen::Matrix<typename Derived::Scalar, (int) (NRows * NCols), QSubvectorSize == Eigen::Dynamic ? Derived::ColsAtCompileTime : QSubvectorSize>
getSubMatrixGradient(const Eigen::MatrixBase<Derived>& dM,
    const std::array<int, NRows>& rows, const std::array<int, NCols>& cols, int M_rows, int q_start = 0, int q_subvector_size = QSubvectorSize);

template<typename Derived>
Eigen::Matrix<typename Derived::Scalar, 1, Derived::ColsAtCompileTime>
getSubMatrixGradient(const Eigen::MatrixBase<Derived>& dM, int row, int col, int M_rows);

template<typename DerivedA, typename DerivedB>
void setSubMatrixGradient(Eigen::MatrixBase<DerivedA>& dM, const Eigen::MatrixBase<DerivedB>& dM_submatrix,
    const std::vector<int>& rows, const std::vector<int>& cols, int M_rows, int q_start = 0, int q_subvector_size = -1);

template<int QSubvectorSize = -1, typename DerivedA, typename DerivedB, std::size_t NRows, std::size_t NCols>
void setSubMatrixGradient(Eigen::MatrixBase<DerivedA>& dM, const Eigen::MatrixBase<DerivedB>& dM_submatrix,
    const std::array<int, NRows>& rows, const std::array<int, NCols>& cols, int M_rows, int q_start = 0, int q_subvector_size = QSubvectorSize);

#endif /* DRAKEGRADIENTUTIL_H_ */
