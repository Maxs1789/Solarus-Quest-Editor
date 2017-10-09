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
#ifndef SOLARUSEDITOR_OBSOLETE_EDITOR_EXCEPTION_H
#define SOLARUSEDITOR_OBSOLETE_EDITOR_EXCEPTION_H

#include "editor_exception.h"

namespace SolarusEditor {

/**
 * @brief Exception thrown if the quest has a too recent format for the editor.
 */
class ObsoleteEditorException : public EditorException {

public:

  ObsoleteEditorException(const QString& quest_format);

  QString get_quest_format() const;

private:

  QString quest_format;

};

}

#endif
