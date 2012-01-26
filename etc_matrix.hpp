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

#ifndef BST_ETC_MATRIX_HPP_
#define BST_ETC_MATRIX_HPP_

#include "matrix.hpp"
#include "random.hpp"
#include "options.hpp"

#include <random>
#include <vector>
#include <algorithm>

void make_consistent(matrix<double>& etc_matrix)
{
  std::vector<double> row;

  for (std::size_t i = 0; i < etc_matrix.rows(); ++i)
  {
    row.clear();
    row.reserve(etc_matrix.cols());

    for (std::size_t j = 0; j < etc_matrix.cols(); ++j)
      row.push_back(etc_matrix(i, j));

    std::stable_sort(std::begin(row), std::end(row), std::greater<double>());

    for (std::size_t j = 0; j < etc_matrix.cols(); ++j)
      etc_matrix(i, j) = row[j];
  }
}


void make_partially_consistent(matrix<double>& etc_matrix)
{
  std::vector<double> row;

  for (std::size_t i = 0; i < etc_matrix.rows(); ++i)
  {
    row.clear();
    row.reserve(etc_matrix.cols());

    for (std::size_t j = 0; j < etc_matrix.cols(); j += 2)
      row.push_back(etc_matrix(i, j));

    std::stable_sort(std::begin(row), std::end(row), std::greater<double>());

    // Gross-ass loop. Fix me later.
    for (std::size_t j = 0, k = 0; j < etc_matrix.cols(); j += 2, ++k)
      etc_matrix(i, j) = row[k];
  }
}

matrix<double> make_etc_matrix(options const& opts)
{
  // Create the baseline vector.
  std::uniform_real_distribution<double> baseline_gen(1, opts.task_variance);
  std::vector<double> baseline;
  baseline.reserve(opts.tasks);
  for (std::size_t i = 0; i < opts.tasks; ++i)
    baseline.push_back(baseline_gen(utils::generator()));

  // Create the etc matrix.
  std::uniform_real_distribution<double> row_gen(1, opts.machine_variance);
  matrix<double> etc_matrix(opts.tasks, opts.machines);
  for (std::size_t i = 0; i < opts.tasks; ++i)
    for (std::size_t j = 0; j < opts.machines; ++j)
      etc_matrix(i, j) = baseline[i] * row_gen(utils::generator());

  if (opts.partially_consistent)
    make_partially_consistent(etc_matrix);
  else if (opts.consistent)
    make_consistent(etc_matrix);

  return etc_matrix;
}



#endif