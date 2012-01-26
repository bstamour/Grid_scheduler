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

#ifndef BST_HEURISTICS_MIN_MIN_HPP_
#define BST_HEURISTICS_MIN_MIN_HPP_

#include "matrix.hpp"
#include "schedule_type.hpp"

#include <tuple>
#include <iostream>
#include <random>
#include <vector>
#include <limits>
#include <chrono>

namespace mm {

schedule_type min_min_impl(matrix<double> const& etc)
{
  std::vector<bool> assigned(etc.rows(), false);

  schedule_type schedule(etc.rows(), etc.cols());
  for (std::size_t i = 0; i < schedule.tasks(); ++i)
  {
    std::size_t min_task = 0, min_machine = 0;
    double min_time = std::numeric_limits<double>::max();

    for (std::size_t t = 0; t < schedule.tasks(); ++t)
    {
      if (!assigned[t])
      {
        for (std::size_t m = 0; m < schedule.machines(); ++m)
        {
          double ct = completion_time(schedule, etc, t, m);
          if (ct < min_time)
          {
            min_time = ct;
            min_task = t;
            min_machine = m;
          }
        }
      }
    }
    schedule.assign_task(min_task, min_machine);
    assigned[min_task] = true;
  }
  return schedule;
}

std::tuple<double, std::size_t>
min_min(matrix<double> const& etc_matrix)
{
  auto t0 = std::chrono::high_resolution_clock::now();

  schedule_type result = min_min_impl(etc_matrix);

  auto t1 = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  return std::make_tuple(makespan(result, etc_matrix), dur.count());
}

} // namespace mm

#endif