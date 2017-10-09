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
#ifndef SOLARUSEDITOR_WALL_H
#define SOLARUSEDITOR_WALL_H

#include "entity_model.h"

namespace SolarusEditor {

/**
 * @brief An editable wall.
 */
class Wall : public EntityModel {

public:

  Wall(MapModel& map, const EntityIndex& index);

protected:

  void set_initial_values() override;
  void notify_field_changed(const QString& key, const QVariant& value) override;

};

}

#endif
