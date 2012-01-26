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

#include "hill_climbing.hpp"
#include "simulated_annealing.hpp"
#include "min_min.hpp"

#include "matrix.hpp"
#include "etc_matrix.hpp"
#include "schedule_type.hpp"
#include "options.hpp"
#include "random.hpp"

#include <tuple>
#include <random>
#include <iostream>

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, std::tuple<T1, T2> const& t)
{
  os << std::get<0>(t) << '\t' << std::get<1>(t);
  return os;
}

int main(int argc, char* argv[])
{
  options opts(argc, argv);

  std::cout << "mm\t\thc\t\trr-hc\t\tsa" << std::endl;

  for (std::size_t i = 0; i < opts.tests; ++i)
  {
    auto etc_matrix = make_etc_matrix(opts);

    auto mm_result = mm::min_min(etc_matrix);
    auto hc_result = hc::hill_climbing(etc_matrix, opts);
    auto rr_hc_result = hc::rr_hill_climbing(etc_matrix, opts);
    auto sa_result = sa::simulated_annealing(etc_matrix);

    std::cout << mm_result << '\t'
              << hc_result << '\t'
              << rr_hc_result << '\t'
              << sa_result << std::endl;
  }
}