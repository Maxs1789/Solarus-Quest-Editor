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
#include "entities/enemy.h"

namespace SolarusEditor {

/**
 * @brief Constructor.
 * @param map The map containing the entity.
 * @param index Index of the entity in the map.
 */
Enemy::Enemy(MapModel& map, const EntityIndex& index) :
  EntityModel(map, index, EntityType::ENEMY) {

  set_origin(QPoint(8, 13));
  set_num_directions(4);
}

/**
 * @copydoc EntityModel::notify_field_changed
 */
void Enemy::notify_field_changed(const QString& key, const QVariant& value) {

  EntityModel::notify_field_changed(key, value);

  if (key == "breed") {
    update_breed();
  }
}

/**
 * @brief Updates the representation of the enemy.
 *
 * This function should be called when the breed changes.
 */
void Enemy::update_breed() {

  DrawSpriteInfo info;
  info.sprite_id = QString("enemies/") + get_field("breed").toString();
  info.animation = "stopped";
  set_draw_sprite_info(info);
}

}
