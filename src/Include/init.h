// Copyright (C) 2022 Arun007coder
// 
// This file is part of SectorOS-RE2.
// 
// SectorOS-RE2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SectorOS-RE2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

unsigned char initrd[] = {
  0xaa, 0x9b, 0xe0, 0x00, 0x69, 0x6e, 0x69, 0x74, 0x72, 0x64, 0x00, 0x87,
  0xe2, 0x7f, 0x00, 0x00, 0xb1, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x81, 0x69, 0x87, 0xe2, 0x7f, 0x00, 0x00, 0x00, 0x40, 0x88, 0x87,
  0x30, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
  0xff, 0x9c, 0xe0, 0x00, 0x69, 0x6e, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xdf, 0x00, 0x00, 0x00, 0xf2, 0x01, 0x00, 0x00, 0xff, 0x9c, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x9c, 0xe0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xff, 0x9c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0x9c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x9c, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x9c, 0xe0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xff, 0x9c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0x9c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x9c, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x63, 0x68, 0x6f, 0x20, 0x49,
  0x6e, 0x69, 0x74, 0x20, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x6c, 0x6f, 0x61,
  0x64, 0x65, 0x64, 0x0a, 0x65, 0x63, 0x68, 0x6f, 0x20, 0x4d, 0x6f, 0x75,
  0x6e, 0x74, 0x69, 0x6e, 0x67, 0x20, 0x2f, 0x64, 0x65, 0x76, 0x2f, 0x61,
  0x70, 0x69, 0x6f, 0x30, 0x6d, 0x20, 0x74, 0x6f, 0x20, 0x2f, 0x0a, 0x75,
  0x6d, 0x6f, 0x75, 0x6e, 0x74, 0x20, 0x2f, 0x0a, 0x6d, 0x6f, 0x75, 0x6e,
  0x74, 0x20, 0x45, 0x58, 0x54, 0x32, 0x20, 0x2f, 0x64, 0x65, 0x76, 0x2f,
  0x61, 0x70, 0x69, 0x6f, 0x30, 0x6d, 0x70, 0x30, 0x20, 0x2f, 0x0a, 0x23,
  0x20, 0x49, 0x66, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x77, 0x61, 0x6e, 0x74,
  0x20, 0x74, 0x6f, 0x20, 0x64, 0x69, 0x73, 0x61, 0x62, 0x6c, 0x65, 0x20,
  0x76, 0x69, 0x64, 0x65, 0x6f, 0x20, 0x6d, 0x6f, 0x64, 0x65, 0x20, 0x63,
  0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x74, 0x68, 0x65, 0x20, 0x33,
  0x20, 0x6c, 0x69, 0x6e, 0x65, 0x73, 0x20, 0x62, 0x65, 0x6c, 0x6f, 0x77,
  0x0a, 0x23, 0x20, 0x76, 0x69, 0x64, 0x65, 0x6f, 0x20, 0x38, 0x30, 0x30,
  0x78, 0x36, 0x30, 0x30, 0x78, 0x33, 0x32, 0x0a, 0x23, 0x20, 0x42, 0x4d,
  0x50, 0x20, 0x2f, 0x77, 0x61, 0x6c, 0x6c, 0x70, 0x61, 0x70, 0x65, 0x72,
  0x2e, 0x62, 0x6d, 0x70, 0x0a, 0x23, 0x20, 0x54, 0x47, 0x41, 0x20, 0x2f,
  0x43, 0x43, 0x2e, 0x74, 0x67, 0x61, 0x0a, 0x65, 0x63, 0x68, 0x6f, 0x20,
  0x49, 0x6e, 0x69, 0x74, 0x20, 0x63, 0x6f, 0x6d, 0x70, 0x6c, 0x65, 0x74,
  0x65
};
unsigned int initrd_len = 721;
