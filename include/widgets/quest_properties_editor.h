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
#ifndef QUEST_PROPERTIES_EDITOR_H
#define QUEST_PROPERTIES_EDITOR_H

#include "widgets/editor.h"
#include "ui_quest_properties_editor.h"
#include "quest_properties.h"

namespace SolarusEditor {

/**
 * @brief A dialog to change properties of the quest.
 */
class QuestPropertiesEditor : public Editor {
  Q_OBJECT

public:

  QuestPropertiesEditor(Quest& quest, QWidget* parent = nullptr);

  QuestProperties& get_model();

  virtual void save() override;

private slots:

  void update();

  void update_write_dir_field();
  void change_write_dir_requested();
  void update_title_field();
  void change_title_requested();
  void update_short_description_field();
  void change_short_description_requested();
  void update_long_description_field();
  void change_long_description_requested();
  void update_author_field();
  void change_author_requested();
  void update_quest_version_field();
  void change_quest_version_requested();
  void update_release_date_field();
  void change_release_date_requested();
  void update_website_field();
  void change_website_requested();
  void update_normal_size_field();
  void change_normal_size_requested();
  void update_min_size_field();
  void change_min_size_requested();
  void update_max_size_field();
  void change_max_size_requested();

private:

  void set_min_size_values();
  void set_max_size_values();

  Ui::QuestPropertiesEditor ui;   /**< The widgets. */
  QuestProperties model;          /**< The model. */

};

}

#endif
