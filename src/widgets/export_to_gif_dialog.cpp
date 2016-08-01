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
#include "widgets/export_to_gif_dialog.h"
#include "editor_exception.h"
#include "gif_encoder.h"
#include <QBitmap>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMessageBox>

namespace SolarusEditor {

/**
 * @brief Creates a export to gif dialog.
 * @param parent Parent object or nullptr.
 */
ExportToGifDialog::ExportToGifDialog(QWidget* parent) :
  QDialog(parent),
  model(nullptr),
  loop(false),
  frame_delay(-1),
  use_transparency(false),
  transparent_color(0),
  preview_item(new QGraphicsPixmapItem()) {

  ui.setupUi(this);

  ui.color_list_widget->setFlow(QListView::LeftToRight);
  ui.color_list_widget->setWrapping(true);
  ui.color_list_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui.color_list_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui.color_list_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  ui.color_list_widget->setFixedSize(259, 259);

  ui.preview_graphics_view->setScene(new QGraphicsScene());
  ui.preview_graphics_view->scene()->addItem(preview_item);

  update();

  connect(ui.browse_button, SIGNAL(clicked(bool)),
          this, SLOT(change_file_name_requested()));
  connect(ui.use_transparency_field, SIGNAL(toggled(bool)),
          this, SLOT(change_use_transparency_requested()));
  connect(ui.color_list_widget, SIGNAL(currentRowChanged(int)),
          this, SLOT(change_transparent_color_requested()));
}

/**
 * @brief Changes the current sprite animation direction to convert.
 * @param model The sprite model.
 * @param index The index of the direction.
 * @throw EditorException If the gived direction doesn't exists.
 */
void ExportToGifDialog::set_sprite_direction(
  SpriteModel* model, const SpriteModel::Index& index) {

  if (model == this->model &&
      index.animation_name == this->index.animation_name &&
      index.direction_nb == this->index.direction_nb) {
    // No change.
    return;
  }

  // Check the model.
  if (model == nullptr) {
    throw EditorException(tr("Invalid sprite"));
  }

  // Check the direction.
  if (!index.is_direction_index() || !model->direction_exists(index)) {
    throw EditorException(
      tr("The direction '%1' doesn't exists in animation '%2'").arg(
      QString::number(index.direction_nb), index.animation_name));
  }

  this->model = model;
  this->index = index;

  // Prepare the loop.
  int loop_on_frame = model->get_animation_loop_on_frame(index);
  frames = model->get_direction_frames(index);
  loop = loop_on_frame >= 0 && frames.length() > 1;
  frame_delay = model->get_animation_frame_delay(index);

  if (loop) {
    // Remove the frames that aren't in the loop.
    while (loop_on_frame-- > 0 && frames.length() > 1) {
      frames.removeFirst();
    }
    loop = frames.length() > 1;
  }

  // Get the useful part of the image.
  QRect rect = model->get_direction_all_frames_rect(index);
  image = model->get_animation_image(index).copy(rect);

  // Convert to indexed image.
  if (image.format() != QImage::Format_Indexed8) {
    image = image.convertToFormat(
      QImage::Format_Indexed8, Qt::ThresholdDither | Qt::PreferDither);
  }

  // Remove the alpha channel.
  QVector<QRgb> colors = image.colorTable();
  for (QRgb& color: colors) {
    color = qRgb(qRed(color), qGreen(color), qBlue(color));
  }
  image.setColorTable(colors);

  // Translate the frame to the subimage.
  for (QRect& frame: frames) {
    frame.translate(-rect.topLeft());
  }

  // Update.
  rebuild_color_list();
  update();
}

/**
 * @brief Updates the dialog.
 */
void ExportToGifDialog::update() {

  update_use_transparency();
  update_transparent_color();
  update_preview();
}

/**
 * @brief Slot called when the user wants change the file name field.
 */
void ExportToGifDialog::change_file_name_requested() {

  QString filename = ui.file_name_field->text();
  filename = QFileDialog::getSaveFileName(
    this, tr("Export to GIF"), filename, tr("GIF (*.gif)"), nullptr,
    QFileDialog::DontConfirmOverwrite);

  if (!filename.isEmpty()) {
    ui.file_name_field->setText(filename);
  }
}

/**
 * @brief Updates the use transparency field.
 */
void ExportToGifDialog::update_use_transparency() {

  ui.use_transparency_field->setChecked(use_transparency);
  ui.color_list_widget->setEnabled(use_transparency);
}

/**
 * @brief Slot called when the user wants change the use transparency field.
 */
void ExportToGifDialog::change_use_transparency_requested() {

  bool use_transparency = ui.use_transparency_field->isChecked();

  if (use_transparency != this->use_transparency) {
    this->use_transparency = use_transparency;
    update_use_transparency();
    update_preview();
  }
}

/**
 * @brief Updates the transparent color field.
 */
void ExportToGifDialog::update_transparent_color() {

  ui.color_list_widget->setCurrentRow(transparent_color);
}

/**
 * @brief Slot called when the user wants change the transparent color field.
 */
void ExportToGifDialog::change_transparent_color_requested() {

  int transparent_color = ui.color_list_widget->currentRow();

  if (transparent_color != this->transparent_color) {
    this->transparent_color = transparent_color;
    update_preview();
  }
}

/**
 * @brief Updates the preview graphics view.
 */
void ExportToGifDialog::update_preview() {

  QPixmap pixmap = QPixmap::fromImage(image);

  // Apply transparency.
  if (use_transparency) {
    QImage mask(image.size(), QImage::Format_Mono);
    for (int x = 0; x < image.width(); ++x) {
      for (int y = 0; y < image.height(); ++y) {
        int pixel = image.pixelIndex(x, y) == transparent_color ? 1 : 0;
        mask.setPixel(x, y, pixel);
      }
    }
    pixmap.setMask(QBitmap::fromImage(mask));
  }

  preview_item->setPixmap(pixmap);
}

/**
 * @brief Closes the dialog unless the user tries to set invalid data.
 * @param result Result code of the dialog.
 */
void ExportToGifDialog::done(int result) {

  if (result == QDialog::Accepted) {
    if (!export_gif()) {
      return;
    }
  }

  QDialog::done(result);
}

/**
 * @brief Exports the current sprite animation direction into the gif file.
 * @return Whether the gif has been exported with success.
 */
bool ExportToGifDialog::export_gif() {

  QString filename = ui.file_name_field->text();

  if (!filename.endsWith(".gif", Qt::CaseInsensitive)) {
    filename += ".gif";
  }

  if (QFile(filename).exists()) {
    int res = QMessageBox::question(
      this, tr("Overwrite the file"),
      tr("The file '%1' already exists."
         " Do you want to overwrite it?").arg(filename));
    if (res != QMessageBox::Yes) {
      return false;
    }
  }

  try {
    int transparent_color = use_transparency ? this->transparent_color : -1;
    GifEncoder::encode_sprite_direction(
      filename, image, frames, loop, frame_delay, transparent_color);
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
    return false;
  }

  return true;
}

/**
 * @brief Rebuilds the color list.
 */
void ExportToGifDialog::rebuild_color_list() {

  ui.color_list_widget->clear();
  transparent_color = 0;

  QVector<QRgb> colors = image.colorTable();
  for (const QRgb& color: colors) {
    QListWidgetItem* item = new QListWidgetItem("");
    item->setSizeHint(QSize(16, 16));
    item->setBackground(QColor(color));
    ui.color_list_widget->addItem(item);
  }
}

}
