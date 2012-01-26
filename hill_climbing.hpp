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

#ifndef BST_HEURISTICS_HILL_CLIMBING_HILL_CLIMBER_HPP_
#define BST_HEURISTICS_HILL_CLIMBING_HILL_CLIMBER_HPP_

#include "matrix.hpp"
#include "options.hpp"
#include "schedule_type.hpp"
#include "random.hpp"

#include "min_min.hpp"

#include <ctime>
#include <algorithm>
#include <iterator>
#include <utility>
#include <random>
#include <chrono>
#include <vector>
#include <tuple>

namespace hc {

std::vector<schedule_type> successors(
  schedule_type const& schedule,
  options const& opts
)
{
  std::vector<schedule_type> next;
  std::uniform_int_distribution<std::size_t> dist(0, schedule.tasks() - 1);

  for (
    std::size_t i = 0;
    i < std::ceil(schedule.tasks() / opts.hc_space_size);
    ++i
  )
  {
    std::size_t t = dist(utils::generator());
    auto current_machine = schedule.assigned_machine(t);

    for (std::size_t m = 0; m < schedule.machines(); ++m)
    {
      if (m == current_machine) continue;

      schedule_type temp = schedule;
      temp.assign_task(t, m);
      next.push_back(temp);
    }
  }

  return next;
}

schedule_type succ_picker(
  std::vector<schedule_type> const& schedules,
  matrix<double> const& etc_matrix
)
{
  return *std::min_element(
    std::begin(schedules), std::end(schedules),
    [&etc_matrix](schedule_type const& v1, schedule_type const& v2)
    {
      return makespan(v1, etc_matrix) < makespan(v2, etc_matrix);
    }
  );
}

schedule_type hill_climbing_impl(
  schedule_type schedule,
  matrix<double> const& etc_matrix,
  options const& opts
)
{
  schedule_type current = schedule;
  schedule_type best_successor = schedule;
  do
  {
    current = best_successor;
    auto succs = successors(best_successor, opts);
    best_successor = succ_picker(
      succs,
      etc_matrix
    );
  } while (makespan(best_successor, etc_matrix) < makespan(current, etc_matrix));
  return current;
}

std::tuple<double, std::size_t>
hill_climbing(matrix<double> const& etc_matrix, options const& opts)
{
  auto t0 = std::chrono::high_resolution_clock::now();

  auto seed = mm::min_min_impl(etc_matrix);
  schedule_type result = hill_climbing_impl(seed, etc_matrix, opts);

  auto t1 = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  return std::make_tuple(makespan(result, etc_matrix), dur.count());
}

schedule_type rr_hill_climbing_impl(
  schedule_type schedule,
  matrix<double> const& etc_matrix,
  options const& opts
)
{
  schedule_type current = schedule;
  schedule_type best = hill_climbing_impl(current, etc_matrix, opts);

  for (std::size_t r = 0; r < opts.hc_restarts; ++r)
  {
    current = make_random_schedule(etc_matrix.rows(), etc_matrix.cols());
    best = std::min(
      best,
      hill_climbing_impl(current, etc_matrix, opts),
      [&etc_matrix](schedule_type const& v1, schedule_type const& v2)
      {
        return makespan(v1, etc_matrix) < makespan(v2, etc_matrix);
      }
    );
  }

  return best;
}

std::tuple<double, std::size_t>
rr_hill_climbing(matrix<double> const& etc_matrix, options const& opts)
{
  auto t0 = std::chrono::high_resolution_clock::now();

  auto seed = mm::min_min_impl(etc_matrix);
  schedule_type result = rr_hill_climbing_impl(seed, etc_matrix, opts);

  auto t1 = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  return std::make_tuple(makespan(result, etc_matrix), dur.count());
}

} // namespace hc

#endif