/*
 * file: geom_outline.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: This class represents an outline which consists of
 * a list of segments. Once the segments form a closed loop no
 * more segments may be added and the internal segments are arranged
 * in a counterclockwise fashion.
 *
 * This file is part of libIGES.
 *
 * libIGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * NOTES:
 *
 * OPERATIONS:
 * + AddSegment: adds another segment to the list; the added segment
 *   must have a start point which coincides with the terminal point
 *   of the previous segment.
 */

#ifndef IGES_GEOM_OUTLINE_H
#define IGES_GEOM_OUTLINE_H

#endif  // IGES_GEOM_OUTLINE_H
