//------------------------------------------------------------------------------
// Copyright 2011 Bryan St. Amour
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// This file is part of Grid_scheduler.
//
// Grid_scheduler is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Grid_scheduler is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Grid_scheduler.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#ifndef BST_UTILS_MATRIX_HPP_
#define BST_UTILS_MATRIX_HPP_

#include <array>
#include <iostream>

template <typename T>
struct matrix
{
  typedef T value_type;

  //
  // ctors
  //
  matrix() = delete;

  matrix(std::size_t m, std::size_t n)
    : rows_(m), cols_(n), data_(new T[m * n])
  {
  }

  matrix(matrix<T> const& rhs)
    : rows_(rhs.rows_), cols_(rhs.cols_), data_(new T[rows_ * cols_])
  {
    std::copy(rhs.data_, rhs.data_ + (rows_ * cols_), data_);
  }

  matrix(matrix<T>&& rhs)
    : rows_(rhs.rows_), cols_(rhs.cols_), data_(std::move(rhs.data_))
  {
    rhs.data_ = nullptr;
  }

  ~matrix()
  {
    delete [] data_;
  }

  //
  // Assignment and swap function.
  //
  friend void swap(matrix<T>& lhs, matrix<T>& rhs)
  {
    using std::swap;
    swap(lhs.rows_, rhs.rows_);
    swap(lhs.cols_, rhs.cols_);
    swap(lhs.data_, rhs.data_);
  }

  matrix<T>& operator=(matrix<T> rhs)
  {
    swap(*this, rhs);
    return *this;
  }

  //
  // Accessor functions.
  //
  std::size_t const rows() const { return rows_; }
  std::size_t const cols() const { return cols_; }

  value_type& operator()(std::size_t i, std::size_t j)
  {
    std::size_t index = i * cols_ + j;
    return data_[index];
  }

  value_type operator()(std::size_t i, std::size_t j) const
  {
    std::size_t index = i * cols_ + j;
    return data_[index];
  }

private:
  std::size_t const rows_;
  std::size_t const cols_;
  T* data_;
};

#endif