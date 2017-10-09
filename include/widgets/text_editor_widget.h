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
#ifndef SOLARUSEDITOR_TEXT_EDITOR_WIDGET_H
#define SOLARUSEDITOR_TEXT_EDITOR_WIDGET_H

#include <QPlainTextEdit>

namespace SolarusEditor {

class TextEditor;

/**
 * @brief Customization of QPlainTextEdit to add line numbers and undo/redo.
 *
 * The line number displaying system is highly inspired from the Qt
 * code editor example:
 * http://qt-project.org/doc/qt-4.8/widgets-codeeditor.html
 *
 * This implementation allows the undo/redo system of QPlainTextEdit to work
 * with an existing QUndoStack, and therefore with a QUndoGroup if wanted.
 * This is not the case by default, because QPlainTextEdit has its own
 * internal undo/redo implementation.
 *
 * Basically, these internal undo/redo commands are preserved, but the undo
 * and redo actions that trigger them are always under our control.
 * The context menu and the key event are replaced to suppress built-in
 * undo/redo actions and implement our own actions instead using the QUndoStack.
 */
class TextEditorWidget: public QPlainTextEdit {
  Q_OBJECT

public:

  TextEditorWidget(const QString& file_path, TextEditor& editor);

  void line_number_area_paint_event(QPaintEvent* event);
  int get_line_number_area_width();

  virtual void contextMenuEvent(QContextMenuEvent* event) override;
  virtual void keyPressEvent(QKeyEvent* event) override;
  virtual void resizeEvent(QResizeEvent* event) override;

  int get_tab_length() const;
  void set_tab_length(int length);

  bool get_replace_tab_by_spaces() const;
  void set_replace_tab_by_spaces(bool replace);

private slots:

  void undo_command_added();
  void update_line_number_area_width(int new_block_count);
  void highlight_current_line();
  void update_line_number_area(const QRect& rect, int dy);

private:

  void insert_tab();
  void remove_tab();

  QWidget* line_number_area;
  QUndoStack& undo_stack;       /**< The undo/redo history to use. */
  int tab_length;               /**< The tabulation length. */
  bool replace_tab_by_spaces;   /**< To replace tabulation by spaces. */

};

}

#endif
