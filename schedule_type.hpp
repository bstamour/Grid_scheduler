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

#ifndef BST_SCHEDULE_TYPE_HPP_
#define BST_SCHEDULE_TYPE_HPP_

#include "random.hpp"

#include <cstring>
#include <unordered_map>
#include <list>
#include <utility>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <random>

struct schedule_type
{
  schedule_type() : tasks_(0), machines_(0), tasks_to_machines_()
  {}

  schedule_type(std::size_t t, std::size_t m)
    : tasks_(t), machines_(m), tasks_to_machines_(tasks_)
  {}

  schedule_type(schedule_type const& rhs)
    : tasks_(rhs.tasks_),
      machines_(rhs.machines_),
      tasks_to_machines_(rhs.tasks_to_machines_)
  {}

  schedule_type(schedule_type&& rhs)
    : tasks_(std::move(rhs.tasks_)),
      machines_(std::move(rhs.machines_)),
      tasks_to_machines_(std::move(rhs.tasks_to_machines_))
  {}

  friend void swap(schedule_type& lhs, schedule_type& rhs)
  {
    using std::swap;
    swap(lhs.tasks_, rhs.tasks_);
    swap(lhs.machines_, rhs.machines_);
    swap(lhs.tasks_to_machines_, rhs.tasks_to_machines_);
  }

  schedule_type& operator=(schedule_type rhs)
  {
    swap(*this, rhs);
    return *this;
  }

  std::size_t tasks() const { return tasks_; }
  std::size_t machines() const { return machines_; }

  void assign_task(std::size_t t, std::size_t m)
  {
    tasks_to_machines_[t] = m;
  }

  std::size_t assigned_machine(std::size_t t) const
  {
    return tasks_to_machines_[t];
  }

private:
  std::size_t tasks_;
  std::size_t machines_;
  std::vector<std::size_t> tasks_to_machines_;
};

template <typename Matrix>
double availability_time(
  schedule_type const& schedule,
  Matrix const& etc_matrix,
  std::size_t m
)
{
  double result = 0.0;
  for (std::size_t t = 0; t < schedule.tasks(); ++t)
    if (schedule.assigned_machine(t) == m)
      result += etc_matrix(t, m);
  return result;
}

template <typename Matrix>
double completion_time(
  schedule_type const& schedule,
  Matrix const& etc_matrix,
  std::size_t t,
  std::size_t m
)
{
  return availability_time(schedule, etc_matrix, m) + etc_matrix(t, m);
}

template <typename Matrix>
double makespan(
  schedule_type const& schedule,
  Matrix const& etc_matrix
)
{
  std::vector<double> c_times(schedule.machines(), 0.0);
  std::size_t const tasks = schedule.tasks();
  for (std::size_t i = 0; i < tasks; ++i)
  {
    std::size_t m = schedule.assigned_machine(i);
    c_times[m] += etc_matrix(i, m);
  }

  return *std::max_element(std::begin(c_times), std::end(c_times));
}

//========================================================================
// Generate a random schedule.
//========================================================================
schedule_type make_random_schedule(std::size_t tasks, std::size_t machines)
{
  std::uniform_int_distribution<std::size_t> dist(0, machines - 1);
  schedule_type schedule(tasks, machines);

  for (std::size_t t = 0; t < tasks; ++t)
    schedule.assign_task(t, dist(utils::generator()));

  return schedule;
}

#endif