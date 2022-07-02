#!/bin/bash
# Copyright (C) 2022 Arun007coder
# 
# This file is part of SectorOS-RE2.
# 
# SectorOS-RE2 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# SectorOS-RE2 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.


for i in files/sorfs/*; do
    echo Adding "$i"
    sudo ./src/mksorfs /dev/nbd0p2 "$i" 2> /dev/null
done