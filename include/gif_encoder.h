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
#ifndef SOLARUSEDITOR_GIF_ENCODER_H
#define SOLARUSEDITOR_GIF_ENCODER_H

#include "sprite_model.h"

class GifFileType;

namespace SolarusEditor {

/**
 * @brief Gif export utility functions.
 */
class GifEncoder {

public:

  static void encode_sprite_direction(
    const QString& filename, QImage& image, const QList<QRect>& frames,
    bool loop, int frame_delay, int transparent_color = -1);

private:

  static void put_global_data(
    GifFileType* file, const QImage& image, int width, int height, bool loop);

  static void put_frames_data(
    GifFileType* file, QImage& image, const QList<QRect>& frames,
    int frame_delay, int transparent_color);

};

}

#endif
