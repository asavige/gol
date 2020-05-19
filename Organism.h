// Organism.h
// Simple implementation of Conway game of life in standard C++11.

#ifndef ORGANISM_H
#define ORGANISM_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <algorithm>

static_assert(sizeof(size_t) == sizeof(int64_t), "size_t too small, need a 64-bit compile");

// CELL
typedef int32_t  cell_coord_type;
typedef uint64_t cell_whole_type;
typedef int64_t  cell_signed_whole_type;

#define XX(w)    ((cell_coord_type)(w))
#define YY(w)    ((cell_coord_type)(((w) >> 32) & 0xFFFFFFFF))
#define WW(x, y) (((cell_signed_whole_type)(y) << 32) | ((cell_signed_whole_type)(x) & 0xFFFFFFFF))

struct Cell {
   cell_coord_type x;
   cell_coord_type y;
   Cell(cell_coord_type x_, cell_coord_type y_) : x(x_), y(y_) {}
   Cell(cell_whole_type w) : x(XX(w)), y(YY(w)) {}
};
inline bool operator<(const Cell& lhs, const Cell& rhs) {
   if (lhs.x < rhs.x) return true;
   if (lhs.x > rhs.x) return false;
   return lhs.y < rhs.y;
   // or one-liner: return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
}
inline bool operator>( const Cell& lhs, const Cell& rhs) { return rhs < lhs; }
inline bool operator<=(const Cell& lhs, const Cell& rhs) { return !(lhs > rhs); }
inline bool operator>=(const Cell& lhs, const Cell& rhs) { return !(lhs < rhs); }
inline bool operator==(const Cell& lhs, const Cell& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(const Cell& lhs, const Cell& rhs) { return !(lhs == rhs); }

using cell_list_type = std::vector<Cell>;
struct CellHasher { size_t operator()(cell_whole_type w) const { return w; }  };
using cell_lookup_type = std::unordered_set<cell_whole_type, CellHasher>;

// ORGANISM
class Organism {
public:
   Organism(size_t ncell = 1000) { cells_m.reserve(ncell); }
   size_t count() const { return cells_m.size(); }
   size_t is_alive(cell_whole_type w) const { return cells_m.find(w) != cells_m.end(); }
   size_t is_dead(cell_whole_type w)  const { return cells_m.find(w) == cells_m.end(); }

   // Used to initialize the starting state of the organism
   size_t insert_cells(const cell_list_type& cells) {
      size_t n_inserted = 0;
      for (const auto& c : cells) {
         if (cells_m.insert(WW(c.x, c.y)).second) ++n_inserted;
      }
      return n_inserted;
   }

   // Used for verification and testing the state of the organism
   cell_list_type get_live_cells() const {
      cell_list_type vec_items(cells_m.cbegin(), cells_m.cend());
      std::sort(vec_items.begin(), vec_items.end());
      return vec_items;
   }

   // Get the number of live neighbours surrounding a cell
   size_t get_num_live(cell_coord_type x, cell_coord_type y) const {
      return is_alive( WW(x-1, y-1) )
           + is_alive( WW(x-1, y  ) )
           + is_alive( WW(x-1, y+1) )
           + is_alive( WW(x  , y-1) )
           + is_alive( WW(x  , y+1) )
           + is_alive( WW(x+1, y-1) )
           + is_alive( WW(x+1, y  ) )
           + is_alive( WW(x+1, y+1) );
   }

   // Return the number of dead cells surrounding a cell
   // The cells themselves are returned in z
   size_t get_dead_cells(cell_coord_type x, cell_coord_type y, cell_whole_type* z) const {
      size_t n = 0;
      if (is_dead(WW(x-1, y-1))) z[n++] = WW(x-1, y-1);
      if (is_dead(WW(x-1, y  ))) z[n++] = WW(x-1, y  );
      if (is_dead(WW(x-1, y+1))) z[n++] = WW(x-1, y+1);
      if (is_dead(WW(x  , y-1))) z[n++] = WW(  x, y-1);
      if (is_dead(WW(x  , y+1))) z[n++] = WW(  x, y+1);
      if (is_dead(WW(x+1, y-1))) z[n++] = WW(x+1, y-1);
      if (is_dead(WW(x+1, y  ))) z[n++] = WW(x+1, y  );
      if (is_dead(WW(x+1, y+1))) z[n++] = WW(x+1, y+1);
      return n;
   }

   void tick() {
      cell_lookup_type new_cells;
      size_t ncells = cells_m.size();
      new_cells.reserve(ncells + ncells/4);

      // Stores the (up to 8) dead neighbour cells surrounding a cell
      cell_whole_type zcells[8];

      for (const auto& c : cells_m) {
         // Get the (up to 8) dead cells surrounding the cell
         size_t ndead = get_dead_cells(XX(c), YY(c), zcells);

         // Note: next line equivalent to nlive == 2 || nlive == 3
         if (ndead == 5 || ndead == 6) new_cells.insert(c);

         for (size_t i = 0; i < ndead; ++i) {
            if (get_num_live(XX(zcells[i]), YY(zcells[i])) == 3) new_cells.insert(zcells[i]);
         }
      }

      cells_m = std::move(new_cells);
   }

private:
   cell_lookup_type cells_m;
};

#endif /* ORGANISM_H */
