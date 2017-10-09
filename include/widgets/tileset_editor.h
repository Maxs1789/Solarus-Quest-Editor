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
#ifndef SOLARUSEDITOR_TILESET_EDITOR_H
#define SOLARUSEDITOR_TILESET_EDITOR_H

#include "widgets/editor.h"
#include "ui_tileset_editor.h"

namespace SolarusEditor {

class TilesetModel;

/**
 * \brief A widget to edit graphically a tileset file.
 */
class TilesetEditor : public Editor {
  Q_OBJECT

public:

  TilesetEditor(Quest& quest, const QString& path, QWidget* parent = nullptr);

  TilesetModel& get_model();

  void save() override;
  void select_all() override;
  void unselect_all() override;
  void reload_settings() override;

public slots:

  void update();

  void update_tileset_id_field();
  void update_background_color();
  void change_background_color();

  void update_description_to_gui();
  void set_description_from_gui();

  void update_pattern_view();
  void tileset_image_changed();
  void update_pattern_id_field();
  void change_selected_patterns_position_requested(const QPoint& delta);
  void update_ground_field();
  void ground_selector_activated();
  void change_selected_patterns_ground_requested(Ground ground);
  void update_animation_type_field();
  void animation_type_selector_activated();
  void change_selected_patterns_animation_requested(PatternAnimation animation);
  void update_animation_separation_field();
  void animation_separation_selector_activated();
  void change_selected_patterns_separation_requested(PatternSeparation separation);
  void update_default_layer_field();
  void change_selected_patterns_default_layer_requested(int default_layer);
  void update_repeat_mode_field();
  void repeat_mode_selector_activated();
  void change_selected_patterns_repeat_mode_requested(TilePatternRepeatMode repeat_mode);

  void create_pattern_requested(
      const QString& pattern_id, const QRect& frame, Ground ground);
  void duplicate_selected_patterns_requested(const QPoint& delta);
  void delete_selected_patterns_requested();
  void change_selected_pattern_id_requested();

  void create_border_set_requested();
  void delete_border_set_selection_requested();
  void delete_border_sets_requested(const QStringList& border_set_ids);
  void delete_border_set_patterns_requested(const QList<QPair<QString, BorderKind>>& patterns);
  void change_border_set_patterns_requested(
      const QString& border_set_id,
      const QStringList& pattern_ids
  );

protected:

  void editor_made_visible() override;

private:

  void set_model(TilesetModel* model);
  QStringList change_pattern_id_in_maps(
      const QString& old_pattern_id, const QString& new_pattern_id);
  bool change_pattern_id_in_map(
      const QString& map_id, const QString& old_pattern_id, const QString& new_pattern_id);
  void load_settings();

private:

  Ui::TilesetEditor ui;         /**< The tileset editor widgets. */
  QString tileset_id;           /**< Id of the tileset being edited. */
  TilesetModel* model;          /**< Tileset model being edited. */
  bool tileset_image_dirty;     /**< Whether the PNG image has changed externally. */

};

}

#endif
