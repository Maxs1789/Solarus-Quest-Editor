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
#include "entities/entity_traits.h"
#include <solarus/entities/EntityTypeInfo.h>
#include <QApplication>

namespace SolarusEditor {

/**
 * @brief Returns all values.
 * @return The existing values.
 */
QList<EntityType> EnumTraits<EntityType>::get_values() {

  return QList<EntityType>::fromStdList(
      Solarus::EnumInfo<EntityType>::enums()
  );
}

/**
 * @brief Returns a user-friendly name describing a value.
 * @param value A value.
 * @return The human-readable name of this value in the current language.
 */
QString EnumTraits<EntityType>::get_friendly_name(EntityType value) {

  // Use a switch to ensure we don't forget a value,
  // and also to translate names dynamically.
  switch (value) {

  case EntityType::ARROW:
    return QApplication::tr("Arrow");

  case EntityType::BLOCK:
    return QApplication::tr("Block");

  case EntityType::BOMB:
    return QApplication::tr("Bomb");

  case EntityType::BOOMERANG:
    return QApplication::tr("Boomerang");

  case EntityType::CAMERA:
    return QApplication::tr("Camera");

  case EntityType::CARRIED_OBJECT:
    return QApplication::tr("Carried object");

  case EntityType::CHEST:
    return QApplication::tr("Chest");

  case EntityType::CRYSTAL:
    return QApplication::tr("Crystal");

  case EntityType::CRYSTAL_BLOCK:
    return QApplication::tr("Crystal block");

  case EntityType::CUSTOM:
    return QApplication::tr("Custom entity");

  case EntityType::DESTINATION:
    return QApplication::tr("Destination");

  case EntityType::DESTRUCTIBLE:
    return QApplication::tr("Destructible object");

  case EntityType::DOOR:
    return QApplication::tr("Door");

  case EntityType::DYNAMIC_TILE:
    return QApplication::tr("Dynamic tile");

  case EntityType::ENEMY:
    return QApplication::tr("Enemy");

  case EntityType::EXPLOSION:
    return QApplication::tr("Explosion");

  case EntityType::FIRE:
    return QApplication::tr("Fire");

  case EntityType::HERO:
    return QApplication::tr("Hero");

  case EntityType::HOOKSHOT:
    return QApplication::tr("Hookshot");

  case EntityType::JUMPER:
    return QApplication::tr("Jumper");

  case EntityType::NPC:
    return QApplication::tr("NPC");

  case EntityType::PICKABLE:
    return QApplication::tr("Pickable treasure");

  case EntityType::SENSOR:
    return QApplication::tr("Sensor");

  case EntityType::SEPARATOR:
    return QApplication::tr("Separator");

  case EntityType::SHOP_TREASURE:
    return QApplication::tr("Shop treasure");

  case EntityType::STAIRS:
    return QApplication::tr("Stairs");

  case EntityType::STREAM:
    return QApplication::tr("Stream");

  case EntityType::SWITCH:
    return QApplication::tr("Switch");

  case EntityType::TELETRANSPORTER:
    return QApplication::tr("Teletransporter");

  case EntityType::TILE:
    return QApplication::tr("Tile");

  case EntityType::WALL:
    return QApplication::tr("Wall");

  }

  return "";
}

/**
 * @brief Returns an icon representing a value.
 * @param value A value.
 * @return The corresponding icon.
 */
QIcon EnumTraits<EntityType>::get_icon(EntityType value) {
  return QIcon(":/images/entity_" + get_lua_name(value) + ".png");
}

/**
 * @brief Returns the Lua name of a value.
 * @param value A value.
 * @return The corresponding Lua name.
 */
QString EnumTraits<EntityType>::get_lua_name(EntityType value) {
  return QString::fromStdString(Solarus::enum_to_name(value));
}

/**
 * @brief Returns whether entities of the specified type can be stored in map files.
 * @param type A type of entity.
 * @return @c true if this type can be stored.
 */
bool EnumTraits<EntityType>::can_be_stored_in_map_file(EntityType type) {

  return Solarus::EntityTypeInfo::can_be_stored_in_map_file(type);
}

/**
 * @brief Returns the keyboard shortcut to show or hide entities to a type.
 * @param type A type of entity.
 * @return The keyboard shortcut to use.
 */
QString EnumTraits<EntityType>::get_show_hide_shortcut(EntityType type) {

  // Use a switch to ensure we don't forget a value,
  // and also to translate names dynamically.
  switch (type) {

  case EntityType::ARROW:
  case EntityType::BOMB:
  case EntityType::BOOMERANG:
  case EntityType::CAMERA:
  case EntityType::CARRIED_OBJECT:
  case EntityType::EXPLOSION:
  case EntityType::HERO:
  case EntityType::HOOKSHOT:
  case EntityType::FIRE:
    return QString();

  case EntityType::BLOCK:
    return QApplication::tr("Ctrl+E,Ctrl+B");

  case EntityType::CHEST:
    return QApplication::tr("Ctrl+E,Ctrl+C");

  case EntityType::CRYSTAL:
    return QApplication::tr("Ctrl+E,Ctrl+L");

  case EntityType::CRYSTAL_BLOCK:
    return QApplication::tr("Ctrl+E,Ctrl+K");

  case EntityType::CUSTOM:
    return QApplication::tr("Ctrl+E,Ctrl+Y");

  case EntityType::DESTINATION:
    return QApplication::tr("Ctrl+E,Ctrl+I");

  case EntityType::DESTRUCTIBLE:
    return QApplication::tr("Ctrl+E,Ctrl+D");

  case EntityType::DOOR:
    return QApplication::tr("Ctrl+E,Ctrl+O");

  case EntityType::DYNAMIC_TILE:
    return QApplication::tr("Ctrl+E,Ctrl+2");

  case EntityType::ENEMY:
    return QApplication::tr("Ctrl+E,Ctrl+E");

  case EntityType::JUMPER:
    return QApplication::tr("Ctrl+E,Ctrl+J");

  case EntityType::NPC:
    return QApplication::tr("Ctrl+E,Ctrl+N");

  case EntityType::PICKABLE:
    return QApplication::tr("Ctrl+E,Ctrl+P");

  case EntityType::SENSOR:
    return QApplication::tr("Ctrl+E,Ctrl+S");

  case EntityType::SEPARATOR:
    return QApplication::tr("Ctrl+E,Ctrl+A");

  case EntityType::SHOP_TREASURE:
    return QApplication::tr("Ctrl+E,Ctrl+U");

  case EntityType::STAIRS:
    return QApplication::tr("Ctrl+E,Ctrl+R");

  case EntityType::STREAM:
    return QApplication::tr("Ctrl+E,Ctrl+M");

  case EntityType::SWITCH:
    return QApplication::tr("Ctrl+E,Ctrl+H");

  case EntityType::TELETRANSPORTER:
    return QApplication::tr("Ctrl+E,Ctrl+T");

  case EntityType::TILE:
    return QApplication::tr("Ctrl+E,Ctrl+1");

  case EntityType::WALL:
    return QApplication::tr("Ctrl+E,Ctrl+W");

  }

  return "";
}

}
