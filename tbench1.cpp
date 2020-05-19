// tbench1.cpp. Simple benchmark of Organism class.
// g++ compile on Linux:
//   g++ -o tbench1 -std=c++11 -Wall -O3 tbench1.cpp
// MSVC 2017 compile on Windows, start a 64-bit compiler cmdline via:
//   Start > AllApps > Visual Studio 2017 > x64 Native Tools Command Prompt for VS2017
// then:
//   cl /W3 /MT /Ox /EHsc tbench1.cpp

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Organism.h"

static bool is_file_exist(const char *fname) {
   std::ifstream infile(fname);
   return infile.good();
}

static Cell read_cell(const std::string& str) {
   cell_coord_type x, y;
   std::istringstream iss(str);
   iss >> x; iss >> y;
   return Cell(x, y);
}

// Reads a Life 1.06 text file
static cell_list_type read_cells_106(const char* fname) {
   cell_list_type cells;
   std::ifstream cellfile(fname);
   if (!cellfile) {
      std::cerr << "Error opening '" << fname << "'\n";
      return cells;
   }
   for (std::string line; std::getline(cellfile, line); ) {
      if (line.empty())   continue;    // ignore blank lines
      if (line[0] == '#') continue;    // ignore comment lines
      cells.push_back(read_cell(line));
   }
   return cells;
}

static void RunTest(const char* fname, int nticks)
{
   Organism org;
   {
      cell_list_type cells = read_cells_106(fname);
      org.insert_cells(cells);
      size_t ncells = org.count();
      std::cout << "cell count at start = " << ncells << "\n";
      if (ncells != cells.size()) { std::cerr << "oops cell count mismatch" << "\n"; }
   }
   std::cerr << "run benchmark for " << nticks << " ticks\n";
   time_t tstart = ::time(NULL);
   for (int i = 1; i <= nticks; ++i ) {
      org.tick();
   }
   time_t tend = ::time(NULL);
   long taken  = static_cast<long>(::difftime(tend, tstart) + 0.5);
   size_t ncells = org.count();
   std::cout << "cell count at end = " << ncells << "\n";
   std::cerr << "time taken " << taken << " secs\n";
}

int main(int argc, char* argv[])
{
   if (argc != 3) {
      std::cerr << "usage: tbench1 file nticks\n";
      return 1;
   }
   const char* fname = argv[1];
   if (!is_file_exist(fname)) {
      std::cerr << "File '" << fname << "' does not exist\n";
      return 1;
   }
   int nticks = ::atoi(argv[2]);
   if (nticks <= 0)
   {
      std::cerr << "'" << argv[2] << "'" << " invalid nticks\n";
      return 1;
   }
   RunTest(fname, nticks);
   return 0;
}
