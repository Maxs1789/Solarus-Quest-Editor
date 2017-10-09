/*
 * Copyright (C) 2014-2017 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "auto_tiler.h"
#include "tileset_model.h"
#include <QDebug>
#include <iostream>
#include <iomanip>

namespace SolarusEditor {

/**
 * @brief Creates an autotiler.
 * @param map The map.
 * @param entity_indexes Indexes of entities where to create a border.
 */
AutoTiler::AutoTiler(
    MapModel& map,
    const EntityIndexes& entity_indexes,
    const QString& border_set_id) :
  map(map),
  entity_indexes(entity_indexes),
  border_set_id(border_set_id) {

  for (const EntityIndex& index : entity_indexes) {
    entity_rectangles.append(map.get_entity_bounding_box(index));
  }
}

/**
 * @brief Returns the number of cells in the 8x8 grid.
 * @return The number of cells.
 */
int AutoTiler::get_num_cells() const {

  return grid_size.width() * grid_size.height();
}

/**
 * @brief Converts map coordinates to an index in the bounding box 8x8 grid.
 * @param xy Coordinates on the map.
 * @return The corresponding grid index.
 */
int AutoTiler::to_grid_index(const QPoint& xy) const {

  int x = xy.x() - bounding_box.x();
  int y = xy.y() - bounding_box.y();
  return (y / 8) * grid_size.width() + (x / 8);
}

/**
 * @brief Converts an index in the bounding box 8x8 grid to map coordinates.
 * @param grid_index A grid index.
 * @return The corresponding map coordinates.
 */
QPoint AutoTiler::to_map_xy(int grid_index) const {

  int grid_x = grid_index % grid_size.width();
  int grid_y = grid_index / grid_size.width();
  return QPoint(grid_x * 8 + bounding_box.x(), grid_y * 8 + bounding_box.y());
}

/**
 * @brief Returns whether a cell of the 8x8 grid is occupied by an entity.
 * @param grid_index An index in the 8x8 grid.
 * @return @c true if this cell is occupied.
 */
bool AutoTiler::is_cell_occupied(int grid_index) const {

  Q_ASSERT(grid_index >= 0);
  Q_ASSERT(grid_index < get_num_cells());

  return occupied_squares[grid_index];
}

/**
 * @brief Returns a bit field indicating the occupied state of 4 cells.
 * @param cell_0 the top-left cell of the 4 cells.
 * @return The occupied state of the 4 cells.
 */
int AutoTiler::get_four_cells_mask(int cell_0) const {

  int cell_1 = cell_0 + 1;
  int cell_2 = cell_0 + grid_size.width();
  int cell_3 = cell_2 + 1;

  int bit_0 = is_cell_occupied(cell_0) ? 1 : 0;
  int bit_1 = is_cell_occupied(cell_1) ? 1 : 0;
  int bit_2 = is_cell_occupied(cell_2) ? 1 : 0;
  int bit_3 = is_cell_occupied(cell_3) ? 1 : 0;

  return bit_3 | (bit_2 << 1) | (bit_1 << 2) | (bit_0 << 3);
}

/**
 * @brief Returns whether a border type is a side.
 * @param which_border A border type.
 * @return @c true if this is a side.
 */
bool AutoTiler::is_side_border(BorderKind which_border) const {

  return which_border == BorderKind::RIGHT ||
      which_border == BorderKind::TOP ||
      which_border == BorderKind::LEFT||
      which_border == BorderKind::BOTTOM;
}

/**
 * @brief Returns whether a border type is a corner (convex or concave).
 * @param which_border A border type.
 * @return @c true if this is a corner.
 */
bool AutoTiler::is_corner_border(BorderKind which_border) const {

  return which_border != BorderKind::NONE && !is_side_border(which_border);
}

/**
 * @brief Returns whether a border type is a convex corner.
 * @param which_border A border type.
 * @return @c true if this is a convex corner.
 */
bool AutoTiler::is_convex_corner_border(BorderKind which_border) const {

  return which_border == BorderKind::TOP_RIGHT_CONVEX ||
      which_border == BorderKind::TOP_LEFT_CONVEX ||
      which_border == BorderKind::BOTTOM_LEFT_CONVEX ||
      which_border == BorderKind::BOTTOM_RIGHT_CONVEX;
}

/**
 * @brief Returns whether a border type is a concave corner.
 * @param which_border A border type.
 * @return @c true if this is a concave corner.
 */
bool AutoTiler::is_concave_corner_border(BorderKind which_border) const {

  return which_border == BorderKind::TOP_RIGHT_CONCAVE ||
      which_border == BorderKind::TOP_LEFT_CONCAVE ||
      which_border == BorderKind::BOTTOM_LEFT_CONCAVE ||
      which_border == BorderKind::BOTTOM_RIGHT_CONCAVE;
}
/**
 * @brief Returns whether a square of the 8x8 grid is marked with a border value.
 * @param grid_index An index in the 8x8 grid.
 * @return @c true if there is a border.
 */
bool AutoTiler::has_border(int grid_index) const {

  return get_which_border(grid_index) != BorderKind::NONE;
}

/**
 * @brief Returns the kind of border in a cell of the 8x8 grid.
 * @param grid_index An index in the 8x8 grid.
 * @return The kind of border in this cell (-1 means none).
 */
BorderKind AutoTiler::get_which_border(int grid_index) const {

  Q_ASSERT(grid_index >= 0);
  Q_ASSERT(grid_index < get_num_cells());

  const auto& it = which_borders.find(grid_index);
  if (it == which_borders.end()) {
    return BorderKind::NONE;
  }
  return it->second;
}

/**
 * @brief Sets the kind of border in a cell of the 8x8 grid.
 * @param grid_index An index in the 8x8 grid.
 * @param which_border The kind of border in this cell.
 */
void AutoTiler::set_which_border(int grid_index, BorderKind which_border) {

  Q_ASSERT(grid_index >= 0);
  Q_ASSERT(grid_index < get_num_cells());

  which_borders[grid_index] = which_border;
}

/**
 * @brief Marks squares of the 8x8 grid with their border info.
 *
 * When there is already a border value in a cell, corners are prioritary.
 *
 * @param cell_0 Index of the top-left cell of a four square mask in the 8x8 grid.
 */
void AutoTiler::detect_border_info(int cell_0) {

  if (get_tileset().is_border_set_inner(border_set_id)) {
    detect_border_info_inner(cell_0);
  }
  else {
    detect_border_info_outer(cell_0);
  }
}

/**
 * @brief Marks squares of the 8x8 grid with their border info (inner border case).
 *
 * When there is already a border value in a cell, corners are prioritary.
 *
 * @param cell_0 Index of the top-left cell of a four square mask in the 8x8 grid.
 */
void AutoTiler::detect_border_info_inner(int cell_0) {

  int cell_1 = cell_0 + 1;
  int cell_2 = cell_0 + grid_size.width();
  int cell_3 = cell_2 + 1;

  int mask = get_four_cells_mask(cell_0);

  switch (mask) {

  // 0 0
  // 0 0
  case 0:
    break;

  // 0 0
  // 0 1
  case 1:
    set_which_border(cell_3, BorderKind::TOP_LEFT_CONVEX);
    break;

  // 0 0
  // 1 0
  case 2:
    set_which_border(cell_2, BorderKind::TOP_RIGHT_CONVEX);
    break;

  // 0 0
  // 1 1
  case 3:
    if (!has_border(cell_2)) {
      set_which_border(cell_2, BorderKind::TOP);
    }
    if (!has_border(cell_3)) {
      set_which_border(cell_3, BorderKind::TOP);
    }
    break;

  // 0 1
  // 0 0
  case 4:
    set_which_border(cell_1, BorderKind::BOTTOM_LEFT_CONVEX);
    break;

  // 0 1
  // 0 1
  case 5:
    if (!has_border(cell_1)) {
      set_which_border(cell_1, BorderKind::LEFT);
    }
    if (!has_border(cell_3)) {
      set_which_border(cell_3, BorderKind::LEFT);
    }
    break;

  // 0 1
  // 1 0
  case 6:
    set_which_border(cell_1, BorderKind::BOTTOM_LEFT_CONVEX);
    set_which_border(cell_2, BorderKind::TOP_RIGHT_CONVEX);
    break;

  // 0 1
  // 1 1
  case 7:
    set_which_border(cell_3, BorderKind::TOP_LEFT_CONCAVE);
    break;

  // 1 0
  // 0 0
  case 8:
    set_which_border(cell_0, BorderKind::BOTTOM_RIGHT_CONVEX);
    break;

  // 1 0
  // 0 1
  case 9:
    set_which_border(cell_0, BorderKind::BOTTOM_RIGHT_CONVEX);
    set_which_border(cell_3, BorderKind::TOP_LEFT_CONVEX);
    break;

  // 1 0
  // 1 0
  case 10:
    if (!has_border(cell_0)) {
      set_which_border(cell_0, BorderKind::RIGHT);
    }
    if (!has_border(cell_2)) {
      set_which_border(cell_2, BorderKind::RIGHT);
    }
    break;

  // 1 0
  // 1 1
  case 11:
    set_which_border(cell_2, BorderKind::TOP_RIGHT_CONCAVE);
    break;

  // 1 1
  // 0 0
  case 12:
    if (!has_border(cell_0)) {
      set_which_border(cell_0, BorderKind::BOTTOM);
    }
    if (!has_border(cell_1)) {
      set_which_border(cell_1, BorderKind::BOTTOM);
    }
    break;

  // 1 1
  // 0 1
  case 13:
    set_which_border(cell_1, BorderKind::BOTTOM_LEFT_CONCAVE);
    break;

  // 1 1
  // 1 0
  case 14:
    set_which_border(cell_0, BorderKind::BOTTOM_RIGHT_CONCAVE);
    break;

  // 1 1
  // 1 1
  case 15:
    break;

  }

}

/**
 * @brief Marks squares of the 8x8 grid with their border info (outer border case).
 *
 * When there is already a border value in a cell, corners are prioritary.
 *
 * @param cell_0 Index of the top-left cell of a four square mask in the 8x8 grid.
 */
void AutoTiler::detect_border_info_outer(int cell_0) {

  int cell_1 = cell_0 + 1;
  int cell_2 = cell_0 + grid_size.width();
  int cell_3 = cell_2 + 1;

  int mask = get_four_cells_mask(cell_0);

  switch (mask) {

  // 0 0
  // 0 0
  case 0:
    break;

  // 0 0
  // 0 1
  case 1:
    set_which_border(cell_0, BorderKind::TOP_LEFT_CONVEX);
    break;

  // 0 0
  // 1 0
  case 2:
    set_which_border(cell_1, BorderKind::TOP_RIGHT_CONVEX);
    break;

  // 0 0
  // 1 1
  case 3:
    if (!has_border(cell_0)) {
      set_which_border(cell_0, BorderKind::TOP);
    }
    if (!has_border(cell_1)) {
      set_which_border(cell_1, BorderKind::TOP);
    }
    break;

  // 0 1
  // 0 0
  case 4:
    set_which_border(cell_2, BorderKind::BOTTOM_LEFT_CONVEX);
    break;

  // 0 1
  // 0 1
  case 5:
    if (!has_border(cell_0)) {
      set_which_border(cell_0, BorderKind::LEFT);
    }
    if (!has_border(cell_2)) {
      set_which_border(cell_2, BorderKind::LEFT);
    }
    break;

  // 0 1
  // 1 0
  case 6:
    set_which_border(cell_0, BorderKind::TOP_LEFT_CONCAVE);
    set_which_border(cell_3, BorderKind::BOTTOM_RIGHT_CONCAVE);
    break;

  // 0 1
  // 1 1
  case 7:
    set_which_border(cell_0, BorderKind::TOP_LEFT_CONCAVE);
    break;

  // 1 0
  // 0 0
  case 8:
    set_which_border(cell_3, BorderKind::BOTTOM_RIGHT_CONVEX);
    break;

  // 1 0
  // 0 1
  case 9:
    set_which_border(cell_1, BorderKind::TOP_RIGHT_CONCAVE);
    set_which_border(cell_2, BorderKind::BOTTOM_LEFT_CONCAVE);
    break;

  // 1 0
  // 1 0
  case 10:
    if (!has_border(cell_1)) {
      set_which_border(cell_1, BorderKind::RIGHT);
    }
    if (!has_border(cell_3)) {
      set_which_border(cell_3, BorderKind::RIGHT);
    }
    break;

  // 1 0
  // 1 1
  case 11:
    set_which_border(cell_1, BorderKind::TOP_RIGHT_CONCAVE);
    break;

  // 1 1
  // 0 0
  case 12:
    if (!has_border(cell_2)) {
      set_which_border(cell_2, BorderKind::BOTTOM);
    }
    if (!has_border(cell_3)) {
      set_which_border(cell_3, BorderKind::BOTTOM);
    }
    break;

  // 1 1
  // 0 1
  case 13:
    set_which_border(cell_2, BorderKind::BOTTOM_LEFT_CONCAVE);
    break;

  // 1 1
  // 1 0
  case 14:
    set_which_border(cell_3, BorderKind::BOTTOM_RIGHT_CONCAVE);
    break;

  // 1 1
  // 1 1
  case 15:
    break;

  }

}

/**
 * @brief Creates a tile with the given position in the 8x8 grid.
 * @param which_border Kind of border to create.
 * @param grid_index Index in the 8x8 grid of the first cell occupied by the tile.
 * @param num_cells_repeat On how many cells of the 8x8 grid the pattern should be repeated
 * (ignored for corners).
 */
void AutoTiler::make_tile(BorderKind which_border, int grid_index, int num_cells_repeat) {

  if (which_border == BorderKind::NONE) {
    return;
  }
  Q_ASSERT(num_cells_repeat > 0);

  QPoint xy = to_map_xy(grid_index);
  QSize size;
  const TilesetModel& tileset = get_tileset();
  const QString& pattern_id = tileset.get_border_set_pattern(border_set_id, which_border);

  if (!tileset.pattern_exists(pattern_id)) {
    // No tile to create for this border.
    return;
  }
  const QSize& pattern_size = tileset.get_pattern_frame(tileset.id_to_index(pattern_id)).size();
  Q_ASSERT(!pattern_size.isEmpty());

  int size_repeated = num_cells_repeat * 8;

  switch (which_border) {

  case BorderKind::RIGHT:
  case BorderKind::LEFT:
    size = { pattern_size.width(), size_repeated };
    break;

  case BorderKind::TOP:
  case BorderKind::BOTTOM:
    size = { size_repeated, pattern_size.height() };
    break;

  default:
    // Corner.
    size = pattern_size;
    break;
  }

  Q_ASSERT(!size.isEmpty());

  const EntityIndex& first_entity_index = entity_indexes.first();
  int layer = first_entity_index.layer;  // TODO choose the lowest layer.

  EntityModelPtr tile = EntityModel::create(map, EntityType::TILE);
  tile->set_field("pattern", pattern_id);
  tile->set_xy(xy);
  tile->set_size(size);
  tile->set_layer(layer);

  tiles.emplace_back(std::move(tile));
}

/**
 * @brief Returns the current tileset.
 * @return The tileset.
 */
const TilesetModel& AutoTiler::get_tileset() const {

  return *map.get_tileset_model();
}

/**
 * @brief Returns the base size of a border pattern.
 * @param which_border A type of border.
 * @return The corresponding size.
 */
const QSize& AutoTiler::get_pattern_size(BorderKind which_border) const {

  return pattern_sizes[static_cast<int>(which_border)];
}

/**
 * @brief Determines the base size of border patterns.
 */
void AutoTiler::compute_pattern_sizes() {

  pattern_sizes.clear();

  const TilesetModel& tileset = get_tileset();

  for (int i = 0; i < 12; ++i) {
    const QString& pattern_id = tileset.get_border_set_pattern(border_set_id, static_cast<BorderKind>(i));
    QSize pattern_size;
    if (tileset.pattern_exists(pattern_id)) {
      pattern_size = tileset.get_pattern_frame(tileset.id_to_index(pattern_id)).size();
    }
    pattern_sizes.append(pattern_size);
  }
}

/**
 * @brief Determines the bounding box of the entities and extends it of 8 pixels.
 */
void AutoTiler::compute_bounding_box() {

  bounding_box = QRect();
  for (const QRect& rectangle : entity_rectangles) {
    bounding_box |= rectangle;
  }

  QSize max_pattern_size;
  for (const QSize& pattern_size : pattern_sizes) {
    max_pattern_size = max_pattern_size.expandedTo(pattern_size);
  }

  // Add a margin.
  bounding_box.translate(-max_pattern_size.width(), -max_pattern_size.height());
  bounding_box.setSize(bounding_box.size() + max_pattern_size * 2);

  grid_size = bounding_box.size() / 8;
}

/**
 * @brief Determines the 8x8 squares that are overlapped by entities.
 */
void AutoTiler::compute_occupied_squares() {

  occupied_squares.clear();
  occupied_squares.assign(get_num_cells(), false);

  for (const QRect& rectangle : entity_rectangles) {

    for (int y = rectangle.y(); y < rectangle.y() + rectangle.height(); y += 8) {
      for (int x = rectangle.x(); x < rectangle.x() + rectangle.width(); x += 8) {
        int grid_index = to_grid_index(QPoint(x, y));
        occupied_squares[grid_index] = true;
      }
    }
  }
}

/**
 * @brief Detect the borders.
 */
void AutoTiler::compute_borders() {

  which_borders.clear();

  for (const QRect& rectangle : entity_rectangles) {

    int num_cells_x = rectangle.width() / 8;
    int num_cells_y = rectangle.height() / 8;

    // Top side.
    int rectangle_top_left_cell = to_grid_index(rectangle.topLeft());
    int initial_position = rectangle_top_left_cell - 1 - grid_size.width();  // 1 cell above and to the left.
    int cell_0 = initial_position;

    for (int i = 0; i < num_cells_x; ++i) {
      detect_border_info(cell_0);
      ++cell_0;
    }

    // Right side.
    for (int i = 0; i < num_cells_y; ++i) {
      detect_border_info(cell_0);
      cell_0 += grid_size.width();
    }

    // Bottom side.
    for (int i = 0; i < num_cells_x; ++i) {
      detect_border_info(cell_0);
      --cell_0;
    }

    // Left side.
    for (int i = 0; i < num_cells_y; ++i) {
      detect_border_info(cell_0);
      cell_0 -= grid_size.width();
    }

  }
}

/**
 * @brief Outputs the grid of border types for debugging.
 */
void AutoTiler::print_which_borders() const {

  int index = 0;
  for (int i = 0; i < grid_size.height(); ++i) {
    for (int j = 0; j < grid_size.width(); ++j) {
      BorderKind which_border = get_which_border(index);
      if (which_border != BorderKind::NONE) {
        std::cout << std::setw(2) << static_cast<int>(which_border) << " ";
      }
      else {
        std::cout << "   ";
      }
      ++index;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

/**
 * @brief Creates the border tiles from the border info previously detected.
 */
void AutoTiler::compute_tiles() {

  if (get_tileset().is_border_set_inner(border_set_id)) {
    compute_tiles_inner();
  }
  else {
    compute_tiles_outer();
  }
}

/**
 * @brief Creates the border tiles from the border info previously detected.
 *
 * Inner border case.
 */
void AutoTiler::compute_tiles_inner() {

  // Generate sides first.
  for (const auto& it : which_borders) {

    int start_index = it.first;
    BorderKind which_border = it.second;

    int grid_x = start_index % grid_size.width();
    int grid_y = start_index / grid_size.width();
    int num_cells_repeat = 1;
    int current_index = start_index;

    if (which_border == BorderKind::NONE) {
      continue;
    }

    if (!is_side_border(which_border)) {
      continue;
    }

    set_which_border(start_index, BorderKind::NONE);  // Mark visited.

    if (!get_tileset().has_border_set_pattern(border_set_id, which_border)) {
      continue;
    }

    if (which_border == BorderKind::RIGHT ||
        which_border == BorderKind::LEFT) {

      // Right or left vertical border.
      BorderKind corner_1 = get_which_border(start_index - grid_size.width());

      if (which_border == BorderKind::RIGHT) {
        // Right border: translate to the left because of the thickness.
        int width = get_pattern_size(which_border).width();
        start_index -= width / 8 - 1;
      }

      // Count how many cells the border occupies vertically.
      for (int i = grid_y + 1; i < grid_size.height(); ++i) {
        current_index += grid_size.width();
        if (get_which_border(current_index) != which_border) {
          break;
        }
        ++num_cells_repeat;
        set_which_border(current_index, BorderKind::NONE);
      }
      BorderKind corner_2 = get_which_border(current_index);

      Q_ASSERT(is_corner_border(corner_1));
      Q_ASSERT(is_corner_border(corner_2));

      // Remove from the cell count the thickness of convex corners.
      if (is_convex_corner_border(corner_1)) {
        const QSize& corner_size = get_pattern_size(corner_1);
        int corner_additional_num_cells = corner_size.height() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
        start_index += corner_additional_num_cells * grid_size.width();
      }
      if (is_convex_corner_border(corner_2)) {
        const QSize& corner_size = get_pattern_size(corner_2);
        int corner_additional_num_cells = corner_size.height() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
      }

      // Finally create the tile.
      if (num_cells_repeat > 0) {
        // Check that the tile size is a multiple of the pattern base size.
        int base_height = get_pattern_size(which_border).height() / 8;
        int rest = num_cells_repeat % base_height;
        if (rest != 0) {
          // Illegal size! Round it to a multiple of the pattern size.
          int num_cells_fixed = base_height - rest;
          num_cells_repeat += base_height - rest;
          if (is_concave_corner_border(corner_1)) {
            start_index -= num_cells_fixed * grid_size.width();
          }
        }

        make_tile(which_border, start_index, num_cells_repeat);
      }
    }

    else {
      // Top or bottom horizontal border.
      BorderKind corner_1 = get_which_border(start_index - 1);

      if (which_border == BorderKind::BOTTOM) {
        // Bottom border: translate to the top because of the thickness.
        int height = get_pattern_size(which_border).height();
        start_index -= (height / 8 - 1) * grid_size.width();
      }

      // Count how many cells the border occupies horizontally.
      for (int j = grid_x + 1; j < grid_size.width(); ++j) {
        ++current_index;
        if (get_which_border(current_index) != which_border) {
          break;
        }
        ++num_cells_repeat;
        set_which_border(current_index, BorderKind::NONE);
      }
      BorderKind corner_2 = get_which_border(current_index);

      Q_ASSERT(is_corner_border(corner_1));
      Q_ASSERT(is_corner_border(corner_2));

      // Remove from the cell count the thickness of convex corners.
      if (is_convex_corner_border(corner_1)) {
        const QSize& corner_size = get_pattern_size(corner_1);
        int corner_additional_num_cells = corner_size.width() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
        start_index += corner_additional_num_cells;
      }
      if (is_convex_corner_border(corner_2)) {
        const QSize& corner_size = get_pattern_size(corner_2);
        int corner_additional_num_cells = corner_size.width() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
      }

      // Finally create the tile.
      if (num_cells_repeat > 0) {
        // Check that the tile size is a multiple of the pattern base size.
        int base_width = get_pattern_size(which_border).width() / 8;
        int rest = num_cells_repeat % base_width;
        if (rest != 0) {
          // Illegal size! Round it to a multiple of the pattern size.
          int num_cells_fixed = base_width - rest;
          num_cells_repeat += base_width - rest;
          if (is_concave_corner_border(corner_1)) {
            start_index -= num_cells_fixed;
          }
        }

        make_tile(which_border, start_index, num_cells_repeat);
      }
    }
  }

  // Generate corners.
  for (const auto& it : which_borders) {
    int start_index = it.first;
    BorderKind which_border = it.second;

    if (!get_tileset().has_border_set_pattern(border_set_id, which_border)) {
      continue;
    }

    if (
        which_border == BorderKind::TOP_RIGHT_CONVEX ||
        which_border == BorderKind::TOP_RIGHT_CONCAVE ||
        which_border == BorderKind::BOTTOM_RIGHT_CONVEX ||
        which_border == BorderKind::BOTTOM_RIGHT_CONCAVE
    ) {
      int width = get_pattern_size(which_border).width();
      start_index -= width / 8 - 1;
    }

    if (
        which_border == BorderKind::BOTTOM_RIGHT_CONVEX ||
        which_border == BorderKind::BOTTOM_RIGHT_CONCAVE ||
        which_border == BorderKind::BOTTOM_LEFT_CONVEX ||
        which_border == BorderKind::BOTTOM_LEFT_CONCAVE
    ) {
      int height = get_pattern_size(which_border).height();
      start_index -= (height / 8 - 1) * grid_size.width();
    }

    make_tile(which_border, start_index, 1);
  }

  which_borders.clear();
}

/**
 * @brief Creates the border tiles from the border info previously detected.
 *
 * Outer border case.
 */
void AutoTiler::compute_tiles_outer() {

  // Generate sides first.
  for (const auto& it : which_borders) {

    int start_index = it.first;
    BorderKind which_border = it.second;

    int grid_x = start_index % grid_size.width();
    int grid_y = start_index / grid_size.width();
    int num_cells_repeat = 1;
    int current_index = start_index;

    if (which_border == BorderKind::NONE) {
      continue;
    }

    if (!is_side_border(which_border)) {
      continue;
    }

    set_which_border(start_index, BorderKind::NONE);  // Mark visited.

    if (!get_tileset().has_border_set_pattern(border_set_id, which_border)) {
      continue;
    }

    if (which_border == BorderKind::RIGHT ||
        which_border == BorderKind::LEFT) {

      // Right or left vertical border.
      BorderKind corner_1 = get_which_border(start_index - grid_size.width());

      if (which_border == BorderKind::LEFT) {
        // Left border: translate to the left because of the thickness.
        int width = get_pattern_size(which_border).width();
        start_index -= width / 8 - 1;
      }

      // Count how many cells the border occupies vertically.
      for (int i = grid_y + 1; i < grid_size.height(); ++i) {
        current_index += grid_size.width();
        if (get_which_border(current_index) != which_border) {
          break;
        }
        ++num_cells_repeat;
        set_which_border(current_index, BorderKind::NONE);
      }
      BorderKind corner_2 = get_which_border(current_index);

      Q_ASSERT(is_corner_border(corner_1));
      Q_ASSERT(is_corner_border(corner_2));

      // Remove from the cell count the thickness of concave corners.
      if (is_concave_corner_border(corner_1)) {
        const QSize& corner_size = get_pattern_size(corner_1);
        int corner_additional_num_cells = corner_size.height() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
        start_index += corner_additional_num_cells * grid_size.width();
      }
      if (is_concave_corner_border(corner_2)) {
        const QSize& corner_size = get_pattern_size(corner_2);
        int corner_additional_num_cells = corner_size.height() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
      }

      // Finally create the tile.
      if (num_cells_repeat > 0) {
        // Check that the tile size is a multiple of the pattern base size.
        int base_height = get_pattern_size(which_border).height() / 8;
        int rest = num_cells_repeat % base_height;
        if (rest != 0) {
          // Illegal size! Round it to a multiple of the pattern size.
          int num_cells_fixed = base_height - rest;
          num_cells_repeat += base_height - rest;
          if (is_concave_corner_border(corner_1)) {
            start_index -= num_cells_fixed * grid_size.width();
          }
        }

        make_tile(which_border, start_index, num_cells_repeat);
      }
    }

    else {
      // Top or bottom horizontal border.
      BorderKind corner_1 = get_which_border(start_index - 1);

      if (which_border == BorderKind::TOP) {
        // Top border: translate to the top because of the thickness.
        int height = get_pattern_size(which_border).height();
        start_index -= (height / 8 - 1) * grid_size.width();
      }

      // Count how many cells the border occupies horizontally.
      for (int j = grid_x + 1; j < grid_size.width(); ++j) {
        ++current_index;
        if (get_which_border(current_index) != which_border) {
          break;
        }
        ++num_cells_repeat;
        set_which_border(current_index, BorderKind::NONE);
      }
      BorderKind corner_2 = get_which_border(current_index);

      Q_ASSERT(is_corner_border(corner_1));
      Q_ASSERT(is_corner_border(corner_2));

      // Remove from the cell count the thickness of concave corners.
      if (is_concave_corner_border(corner_1)) {
        const QSize& corner_size = get_pattern_size(corner_1);
        int corner_additional_num_cells = corner_size.width() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
        start_index += corner_additional_num_cells;
      }
      if (is_concave_corner_border(corner_2)) {
        const QSize& corner_size = get_pattern_size(corner_2);
        int corner_additional_num_cells = corner_size.width() / 8 - 1;
        num_cells_repeat -= corner_additional_num_cells;
      }

      // Finally create the tile.
      if (num_cells_repeat > 0) {
        // Check that the tile size is a multiple of the pattern base size.
        int base_width = get_pattern_size(which_border).width() / 8;
        int rest = num_cells_repeat % base_width;
        if (rest != 0) {
          // Illegal size! Round it to a multiple of the pattern size.
          int num_cells_fixed = base_width - rest;
          num_cells_repeat += base_width - rest;
          if (is_concave_corner_border(corner_1)) {
            start_index -= num_cells_fixed;
          }
        }

        make_tile(which_border, start_index, num_cells_repeat);
      }
    }
  }

  // Generate corners.
  for (const auto& it : which_borders) {
    int start_index = it.first;
    BorderKind which_border = it.second;

    if (!get_tileset().has_border_set_pattern(border_set_id, which_border)) {
      continue;
    }

    if (
        which_border == BorderKind::TOP_LEFT_CONVEX ||
        which_border == BorderKind::TOP_LEFT_CONCAVE ||
        which_border == BorderKind::BOTTOM_LEFT_CONVEX ||
        which_border == BorderKind::BOTTOM_LEFT_CONCAVE
    ) {
      int width = get_pattern_size(which_border).width();
      start_index -= width / 8 - 1;
    }
    if (
        which_border == BorderKind::TOP_RIGHT_CONVEX ||
        which_border == BorderKind::TOP_RIGHT_CONCAVE ||
        which_border == BorderKind::TOP_LEFT_CONVEX ||
        which_border == BorderKind::TOP_LEFT_CONCAVE
    ) {
      int height = get_pattern_size(which_border).height();
      start_index -= (height / 8 - 1) * grid_size.width();
    }

    make_tile(which_border, start_index, 1);
  }

  which_borders.clear();
}

/**
 * @brief Creates border tiles around the given entities.
 * @return The border tiles ready to be added to the map.  1 1
 */
AddableEntities AutoTiler::generate_border_tiles() {

  if (entity_rectangles.empty()) {
    return AddableEntities();
  }

  // Determine the 8x8 grid.
  compute_pattern_sizes();
  compute_bounding_box();

  // Create a list indicating which 8x8 squares are inside the selection.
  compute_occupied_squares();

  // Detect the borders.
  compute_borders();

  // Create the corresponding tiles.
  compute_tiles();
  if (tiles.empty()) {
    return AddableEntities();
  }

  const EntityModelPtr& first_tile = *tiles.begin();
  int layer = first_tile->get_layer();
  int order = map.get_num_tiles(layer);
  AddableEntities addable_tiles;
  for (EntityModelPtr& tile : tiles) {
    EntityIndex index = { layer, order };
    addable_tiles.emplace_back(std::move(tile), index);
    ++order;
  }

  return addable_tiles;
}

}
