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

#ifndef BST_HEURISTICS_SIMULATED_ANNEALING_SIMULATED_ANNEALING_HPP_
#define BST_HEURISTICS_SIMULATED_ANNEALING_SIMULATED_ANNEALING_HPP_

#include "matrix.hpp"
#include "schedule_type.hpp"
#include "random.hpp"

#include "min_min.hpp"

#include <iostream>

#include <random>
#include <utility>
#include <ctime>
#include <cstring>
#include <list>
#include <chrono>
#include <tuple>

namespace sa {

std::list<double> make_temperature_schedule(double start, double scale)
{
  std::list<double> temps;
  while (start >= std::pow(10, -200))
  {
    temps.push_back(start);
    start *= scale;
  }
  return temps;
}

schedule_type modify(schedule_type schedule)
{
  std::uniform_int_distribution<std::size_t> t(0, schedule.tasks() - 1);
  std::uniform_int_distribution<std::size_t> m(0, schedule.machines() - 1);
  schedule.assign_task(t(utils::generator()), m(utils::generator()));
  return schedule;
}

schedule_type simulated_annealing_impl(
  schedule_type const& schedule,
  matrix<double> const& etc_matrix,
  std::list<double> temperatures
)
{
  schedule_type current = schedule;
  std::size_t same_schedule = 0;

  for (double& temp : temperatures)
  {
    schedule_type succ = modify(current);

    if (makespan(succ, etc_matrix) < makespan(current, etc_matrix))
    {
      // Found a better solution, so take it no questions asked.
      current = succ;
      same_schedule = 0;
    }
    else
    {
      double y = 1.0 / (
        1.0 + std::exp(
          (makespan(current, etc_matrix) - makespan(succ, etc_matrix) )
          / temp
        )
      );

      std::uniform_real_distribution<double> dist(0, 1);
      double z = dist(utils::generator());

      if (z > y)
      {
        current = succ;
        same_schedule = 0;
      }
      else
      {
        ++same_schedule;
      }

      if (same_schedule >= 200)
        break;
    }
  }
  return current;
}

std::tuple<double, std::size_t>
simulated_annealing(matrix<double> const& etc_matrix)
{
  schedule_type best_schedule(0, 0);
  double best_makespan = std::numeric_limits<double>::max();

  auto t0 = std::chrono::high_resolution_clock::now();

  // Seeded with min-min.
  {
    schedule_type min_schedule = mm::min_min_impl(etc_matrix);

    auto temp_90 = make_temperature_schedule(
      makespan(min_schedule, etc_matrix),
      0.9
    );

    auto temp_80 = make_temperature_schedule(
      makespan(min_schedule, etc_matrix),
      0.8
    );

    for (std::size_t i = 0; i < 8; ++i)
    {
      schedule_type result_90 =
        simulated_annealing_impl(min_schedule, etc_matrix, temp_90);
      schedule_type result_80 =
        simulated_annealing_impl(min_schedule, etc_matrix, temp_80);

      double makespan_90 = makespan(result_90, etc_matrix);
      double makespan_80 = makespan(result_80, etc_matrix);

      if (makespan_90 < best_makespan)
      {
        best_makespan = makespan_90;
        best_schedule = result_90;
      }

      if (makespan_80 < best_makespan)
      {
        best_makespan = makespan_80;
        best_schedule = result_80;
      }
    }
  }

  // Not seeded with min-min.
  for (std::size_t i = 0; i < 8; ++i)
  {
    schedule_type schedule =
      make_random_schedule(etc_matrix.rows(), etc_matrix.cols());

    auto temp_90 = make_temperature_schedule(
      makespan(schedule, etc_matrix),
      0.9
    );

    auto temp_80 = make_temperature_schedule(
      makespan(schedule, etc_matrix),
      0.8
    );

    schedule_type result_90 =
      simulated_annealing_impl(schedule, etc_matrix, temp_90);
    schedule_type result_80 =
      simulated_annealing_impl(schedule, etc_matrix, temp_80);

    double makespan_90 = makespan(result_90, etc_matrix);
    double makespan_80 = makespan(result_80, etc_matrix);

    if (makespan_90 < best_makespan)
    {
      best_makespan = makespan_90;
      best_schedule = result_90;
    }

    if (makespan_80 < best_makespan)
    {
      best_makespan = makespan_80;
      best_schedule = result_80;
    }
  }

  auto t1 = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  return std::make_tuple(best_makespan, dur.count());
}

} // namespace sa

#endif