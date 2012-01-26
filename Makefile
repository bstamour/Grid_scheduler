#------------------------------------------------------------------------------
# Copyright 2011 Bryan St. Amour
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# This file is part of Grid_scheduler.
#
# Grid_scheduler is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Grid_scheduler is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Grid_scheduler.  If not, see <http://www.gnu.org/licenses/>.
#------------------------------------------------------------------------------

CXX = g++-4.6.2
CXXFLAGS = -O2 -Wall -Werror -std=c++0x -g
CXXLNFLAGS = -lboost_program_options -Xlinker -R/usr/local/lib64

.SUFFIXES:
.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

SRCS = random.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = main

all: $(OBJS)
	$(CXX) $(OBJS) -o $(EXE) $(CXXLNFLAGS)

clean:
	rm -f $(OBJS) $(EXE)