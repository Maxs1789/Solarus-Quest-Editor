/*
 * Copyright (C) 2014-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef EXPORT_TO_GIF_DIALOG_H
#define EXPORT_TO_GIF_DIALOG_H

#include "ui_export_to_gif_dialog.h"
#include "sprite_model.h"
#include <QDialog>

namespace SolarusEditor {

/**
 * @brief A dialog to export sprite animation direction to gif.
 */
class ExportToGifDialog : public QDialog {
  Q_OBJECT

public:

  ExportToGifDialog(QWidget* parent = nullptr);

  void set_sprite_direction(
    SpriteModel* model, const SpriteModel::Index& index);

public slots:

  void update();

  void change_file_name_requested();

  void change_exact_color_match_requested();

  void update_use_transparency();
  void change_use_transparency_requested();

  void update_transparent_color();
  void change_transparent_color_requested();

  void update_preview();

  void done(int result) override;

private slots:

  bool export_gif();

private:

  void rebuild_image();
  void rebuild_color_list();

  Ui::ExportToGifDialog ui;     /**< The widgets. */
  SpriteModel* model;           /**< The current sprite model. */
  SpriteModel::Index index;     /**< The current direction index. */
  QImage source_image;          /**< The source image. */
  QImage image;                 /**< The image. */
  QList<QRect> frames;          /**< The frames rect. */
  bool loop;                    /**< Whether the animation loop. */
  int frame_delay;              /**< The frame delay. */
  bool use_transparency;        /**< Whether the animation use transparency. */
  int transparent_color;        /**< The transparent color. */
  QGraphicsPixmapItem*
    preview_item;               /**< The preview item. */

};

}

#endif
