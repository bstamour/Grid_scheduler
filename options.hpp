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

#ifndef BST_PROG_OPTIONS_HPP_
#define BST_PROG_OPTIONS_HPP_

#include <cstring>
#include <boost/program_options.hpp>

//=============================================================================
// This struct stores a bunch of program options for the simulation.
//=============================================================================
struct options
{
  options(int argc, char** argv)
  {
    namespace po = boost::program_options;

    //--------------------------------------------------------------------------
    // Create a list of available options.
    //--------------------------------------------------------------------------

    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      (
        "tests",
        po::value<std::size_t>(&tests)->default_value(10),
        "set the number of tests to run"
      )
      (
        "tasks",
        po::value<std::size_t>(&tasks)->default_value(10),
        "set number of tasks"
      )
      (
        "machines",
        po::value<std::size_t>(&machines)->default_value(10),
        "set number of machines"
      )
      (
        "task-variance",
        po::value<std::size_t>(&task_variance)->default_value(3000),
        "set task variance (higher = higher variance)"
      )
      (
        "machine-variance",
        po::value<std::size_t>(&machine_variance)->default_value(1000),
        "set machine variance (higher = higher variance)"
      )
      (
        "consistent",
        po::value<bool>(&consistent)->default_value(false),
        "set whether to use consistent matrices or not"
      )
      (
        "partially-consistent",
        po::value<bool>(&partially_consistent)->default_value(false),
        "set whether to use partially consistent matrices or not. Overrides --consistent"
      )

      (
        "hc-space-size",
        po::value<std::size_t>(&hc_space_size)->default_value(5),
        "set k, where 1 / k is the search space size"
      )
      (
        "hc-restarts",
        po::value<std::size_t>(&hc_restarts)->default_value(500),
        "set number of restarts for hill-climber"
      )
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }

  std::size_t tests;

  std::size_t tasks;
  std::size_t machines;

  std::size_t task_variance;
  std::size_t machine_variance;

  bool consistent;
  bool partially_consistent;

  std::size_t hc_space_size;
  std::size_t hc_restarts;
};

#endif