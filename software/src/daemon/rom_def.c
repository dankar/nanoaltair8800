#include "rom_def.h"

const uint8_t TURMON_DATA[] = {
    0x3e, 0x03, 0xd3, 0x10, 0x3e, 0x11, 0xd3, 0x10, 0x31, 0x00, 0xf8, 0xcd,
    0x9d, 0xfd, 0x3e, 0x2e, 0xcd, 0xf2, 0xfd, 0xcd, 0xe8, 0xfd, 0xfe, 0x4d,
    0xca, 0x29, 0xfd, 0xfe, 0x44, 0xcc, 0x4f, 0xfd, 0xfe, 0x4a, 0xc2, 0x08,
    0xfd, 0xcd, 0xa7, 0xfd, 0xe9, 0xcd, 0xa7, 0xfd, 0x3e, 0x23, 0xcd, 0x9d,
    0xfd, 0x54, 0x5d, 0xcd, 0xc9, 0xfd, 0x1a, 0x67, 0xcd, 0xcf, 0xfd, 0xcd,
    0xa8, 0xfd, 0xeb, 0xda, 0x2d, 0xfd, 0x77, 0xbe, 0xca, 0x2d, 0xfd, 0x3e,
    0x3f, 0xcd, 0xf2, 0xfd, 0xc3, 0x08, 0xfd, 0xcd, 0xa7, 0xfd, 0xeb, 0xd4,
    0xe3, 0xfd, 0xcd, 0xa7, 0xfd, 0x3e, 0x0d, 0x06, 0x3c, 0xcd, 0xf2, 0xfd,
    0x05, 0xc2, 0x5d, 0xfd, 0xb8, 0x78, 0xc2, 0x5b, 0xfd, 0x7d, 0x93, 0x6f,
    0x7c, 0x9a, 0x67, 0x23, 0x05, 0x7c, 0xb7, 0xc2, 0x77, 0xfd, 0x45, 0x3e,
    0x3c, 0xcd, 0xf2, 0xfd, 0x78, 0xcd, 0xf2, 0xfd, 0x0e, 0x00, 0x7b, 0xcd,
    0xf2, 0xfd, 0x7a, 0xcd, 0xf2, 0xfd, 0x1a, 0xcd, 0xf2, 0xfd, 0x13, 0x2b,
    0x05, 0xc2, 0x8a, 0xfd, 0x79, 0xcd, 0xf2, 0xfd, 0x7c, 0xb5, 0xc2, 0x70,
    0xfd, 0x3e, 0x0d, 0xcd, 0xf2, 0xfd, 0x3e, 0x0a, 0xc3, 0xf2, 0xfd, 0x06,
    0x06, 0x03, 0x21, 0x00, 0x00, 0xcd, 0xe8, 0xfd, 0x4f, 0xfe, 0x20, 0x37,
    0xc8, 0xe6, 0xb8, 0xee, 0x30, 0xc2, 0x47, 0xfd, 0x79, 0xe6, 0x07, 0x29,
    0x29, 0x29, 0x85, 0x6f, 0x05, 0xc2, 0xad, 0xfd, 0xc9, 0x06, 0x06, 0xaf,
    0xc3, 0xd6, 0xfd, 0x06, 0x03, 0xe6, 0x29, 0x17, 0x29, 0x17, 0x29, 0x17,
    0xe6, 0x07, 0xf6, 0x30, 0xcd, 0xf2, 0xfd, 0x05, 0xc2, 0xd2, 0xfd, 0x3e,
    0x20, 0xc3, 0xf2, 0xfd, 0xdb, 0x10, 0x0f, 0xd2, 0xe8, 0xfd, 0xdb, 0x11,
    0xe6, 0x7f, 0xf5, 0x81, 0x4f, 0xdb, 0x10, 0x0f, 0x0f, 0xd2, 0xf5, 0xfd,
    0xf1, 0xd3, 0x11, 0xc9};
const uint8_t TURMONH_DATA[] = {
    0x3e, 0x03, 0xd3, 0x10, 0x3e, 0x11, 0xd3, 0x10, 0x31, 0x00, 0x80, 0xcd,
    0x9f, 0xfd, 0x3e, 0x2e, 0xcd, 0xf4, 0xfd, 0xcd, 0xec, 0xfd, 0xfe, 0x4d,
    0xca, 0x29, 0xfd, 0xfe, 0x48, 0xcc, 0x4f, 0xfd, 0xfe, 0x4a, 0xc2, 0x08,
    0xfd, 0xcd, 0xa9, 0xfd, 0xe9, 0xcd, 0xa9, 0xfd, 0x3e, 0x23, 0xcd, 0x9f,
    0xfd, 0x54, 0x5d, 0xcd, 0xcd, 0xfd, 0x1a, 0x67, 0xcd, 0xd3, 0xfd, 0xcd,
    0xaa, 0xfd, 0xeb, 0xda, 0x2d, 0xfd, 0x77, 0xbe, 0xca, 0x2d, 0xfd, 0x3e,
    0x3f, 0xcd, 0xf4, 0xfd, 0xc3, 0x08, 0xfd, 0xcd, 0x8c, 0xfd, 0xc2, 0x4f,
    0xfd, 0xcd, 0xf4, 0xfd, 0xcd, 0x7f, 0xfd, 0xca, 0x76, 0xfd, 0x57, 0xcd,
    0x7f, 0xfd, 0x67, 0xcd, 0x7f, 0xfd, 0x6f, 0xcd, 0x7f, 0xfd, 0xcd, 0x7f,
    0xfd, 0x77, 0x23, 0x15, 0xc2, 0x6a, 0xfd, 0xc3, 0x4f, 0xfd, 0xdb, 0x11,
    0x1b, 0x7a, 0xb3, 0xc2, 0x76, 0xfd, 0xc9, 0xcd, 0x8c, 0xfd, 0x87, 0x87,
    0x87, 0x87, 0x5f, 0xcd, 0x8c, 0xfd, 0x83, 0xc9, 0xdb, 0x10, 0x0f, 0xd2,
    0x8c, 0xfd, 0xdb, 0x11, 0xfe, 0x3a, 0xc8, 0xd6, 0x30, 0xfe, 0x0a, 0xd8,
    0xd6, 0x07, 0xc9, 0x3e, 0x0d, 0xcd, 0xf4, 0xfd, 0x3e, 0x0a, 0xc3, 0xf4,
    0xfd, 0x06, 0x06, 0x03, 0x21, 0x00, 0x00, 0x0e, 0x01, 0xcd, 0xec, 0xfd,
    0x4f, 0xfe, 0x20, 0x37, 0xc8, 0xe6, 0xb8, 0xee, 0x30, 0xc2, 0x47, 0xfd,
    0x79, 0xe6, 0x07, 0x29, 0x29, 0x29, 0x85, 0x6f, 0x05, 0xc2, 0xaf, 0xfd,
    0xc9, 0x06, 0x06, 0xaf, 0xc3, 0xda, 0xfd, 0x06, 0x03, 0xe6, 0x29, 0x17,
    0x29, 0x17, 0x29, 0x17, 0xe6, 0x07, 0xf6, 0x30, 0xcd, 0xf4, 0xfd, 0x05,
    0xc2, 0xd6, 0xfd, 0x3e, 0x20, 0xc3, 0xf4, 0xfd, 0xdb, 0x10, 0x0f, 0xd2,
    0xec, 0xfd, 0xdb, 0x11, 0xf5, 0xdb, 0x10, 0x0f, 0x0f, 0xd2, 0xf5, 0xfd,
    0xf1, 0xd3, 0x11, 0xc9};
const uint8_t MBL_DATA[] = {
    0xf3, 0x21, 0xff, 0xff, 0x23, 0x7e, 0x47, 0x2f, 0x77, 0xbe, 0x70, 0xca,
    0x04, 0xfe, 0x7d, 0xb7, 0xc2, 0xbb, 0xfe, 0x16, 0xfe, 0x2b, 0x5d, 0x1a,
    0x77, 0x7b, 0xfe, 0x41, 0xc2, 0x15, 0xfe, 0xf9, 0xe5, 0x1d, 0x1a, 0x6f,
    0x74, 0xd6, 0x74, 0xc2, 0x21, 0xfe, 0x54, 0xc9, 0x74, 0x7a, 0x7d, 0x82,
    0x85, 0x89, 0x8d, 0x91, 0x98, 0x9d, 0xa3, 0xa8, 0xad, 0xb5, 0xb8, 0xbf,
    0xc4, 0xc7, 0xdc, 0xdf, 0xe7, 0xd3, 0x20, 0xd3, 0x21, 0xd3, 0x22, 0x2f,
    0xd3, 0x23, 0x3e, 0x2c, 0xd3, 0x20, 0xd3, 0x22, 0x3e, 0x03, 0xd3, 0x10,
    0xd3, 0x12, 0x3e, 0x11, 0xd3, 0x10, 0xd3, 0x12, 0xdb, 0xff, 0xe6, 0x07,
    0x87, 0xc6, 0xf0, 0x5f, 0x1a, 0x2e, 0xe9, 0x1f, 0x77, 0x2e, 0xe2, 0x3d,
    0x77, 0x1c, 0x1a, 0x2e, 0xe4, 0x77, 0xd2, 0x78, 0xfe, 0x2c, 0x36, 0xc2,
    0xcd, 0xe8, 0xfe, 0xcd, 0xe1, 0xfe, 0x4f, 0xb7, 0xca, 0x92, 0xfe, 0xcd,
    0xe1, 0xfe, 0xb9, 0xca, 0x83, 0xfe, 0x0d, 0xcd, 0xe1, 0xfe, 0x0d, 0xc2,
    0x8b, 0xfe, 0x62, 0x2e, 0x92, 0xe5, 0xcd, 0xe1, 0xfe, 0xfe, 0x3c, 0xca,
    0xa6, 0xfe, 0xfe, 0x78, 0xc0, 0xcd, 0xdd, 0xfe, 0x67, 0xe9, 0xcd, 0xe1,
    0xfe, 0x41, 0x4f, 0xcd, 0xdd, 0xfe, 0x67, 0x7a, 0xbc, 0x3e, 0x4f, 0xca,
    0xcb, 0xfe, 0xcd, 0xe1, 0xfe, 0x77, 0xbe, 0x3e, 0x4d, 0xc2, 0xcb, 0xfe,
    0x23, 0x0d, 0xc2, 0xaf, 0xfe, 0xcd, 0xe1, 0xfe, 0xc8, 0x3e, 0x43, 0x32,
    0x00, 0x00, 0x22, 0x01, 0x00, 0xfb, 0xd3, 0x01, 0xd3, 0x11, 0xd3, 0x05,
    0xd3, 0x23, 0xc3, 0xd2, 0xfe, 0xcd, 0xe1, 0xfe, 0x6f, 0xdb, 0x00, 0xe6,
    0x00, 0xca, 0xe1, 0xfe, 0xdb, 0x00, 0xb8, 0xf5, 0x80, 0x47, 0xf1, 0xc9,
    0x22, 0x01, 0x22, 0x01, 0x03, 0x01, 0x0f, 0x01, 0x42, 0x80, 0x0a, 0x02,
    0x26, 0x01, 0x26, 0x01};
const uint8_t DBL_DATA[] = {
    0x21, 0x13, 0xff, 0x11, 0x00, 0x2c, 0x0e, 0xeb, 0x7e, 0x12, 0x23, 0x13,
    0x0d, 0xc2, 0x08, 0xff, 0xc3, 0x00, 0x2c, 0xf3, 0xaf, 0xd3, 0x22, 0x2f,
    0xd3, 0x23, 0x3e, 0x2c, 0xd3, 0x22, 0x3e, 0x03, 0xd3, 0x10, 0xdb, 0xff,
    0xe6, 0x10, 0x0f, 0x0f, 0xc6, 0x10, 0xd3, 0x10, 0x31, 0x79, 0x2d, 0xaf,
    0xd3, 0x08, 0xdb, 0x08, 0xe6, 0x08, 0xc2, 0x1c, 0x2c, 0x3e, 0x04, 0xd3,
    0x09, 0xc3, 0x38, 0x2c, 0xdb, 0x08, 0xe6, 0x02, 0xc2, 0x2d, 0x2c, 0x3e,
    0x02, 0xd3, 0x09, 0xdb, 0x08, 0xe6, 0x40, 0xc2, 0x2d, 0x2c, 0x11, 0x00,
    0x00, 0x06, 0x00, 0x3e, 0x10, 0xf5, 0xd5, 0xc5, 0xd5, 0x11, 0x86, 0x80,
    0x21, 0xeb, 0x2c, 0xdb, 0x09, 0x1f, 0xda, 0x50, 0x2c, 0xe6, 0x1f, 0xb8,
    0xc2, 0x50, 0x2c, 0xdb, 0x08, 0xb7, 0xfa, 0x5c, 0x2c, 0xdb, 0x0a, 0x77,
    0x23, 0x1d, 0xca, 0x72, 0x2c, 0x1d, 0xdb, 0x0a, 0x77, 0x23, 0xc2, 0x5c,
    0x2c, 0xe1, 0x11, 0xee, 0x2c, 0x01, 0x80, 0x00, 0x1a, 0x77, 0xbe, 0xc2,
    0xcb, 0x2c, 0x80, 0x47, 0x13, 0x23, 0x0d, 0xc2, 0x79, 0x2c, 0x1a, 0xfe,
    0xff, 0xc2, 0x90, 0x2c, 0x13, 0x1a, 0xb8, 0xc1, 0xeb, 0xc2, 0xc2, 0x2c,
    0xf1, 0xf1, 0x2a, 0xec, 0x2c, 0xcd, 0xe5, 0x2c, 0xd2, 0xbb, 0x2c, 0x04,
    0x04, 0x78, 0xfe, 0x20, 0xda, 0x44, 0x2c, 0x06, 0x01, 0xca, 0x44, 0x2c,
    0xdb, 0x08, 0xe6, 0x02, 0xc2, 0xad, 0x2c, 0x3e, 0x01, 0xd3, 0x09, 0xc3,
    0x42, 0x2c, 0x3e, 0x80, 0xd3, 0x08, 0xc3, 0x00, 0x00, 0xd1, 0xf1, 0x3d,
    0xc2, 0x46, 0x2c, 0x3e, 0x43, 0x01, 0x3e, 0x4d, 0xfb, 0x32, 0x00, 0x00,
    0x22, 0x01, 0x00, 0x47, 0x3e, 0x80, 0xd3, 0x08, 0x78, 0xd3, 0x01, 0xd3,
    0x11, 0xd3, 0x05, 0xd3, 0x23, 0xc3, 0xda, 0x2c, 0x7a, 0xbc, 0xc0, 0x7b,
    0xbd, 0xc9, 0x00, 0x00};

const rom_t TURMON =   {0xfd00, sizeof(TURMON_DATA), TURMON_DATA};
// H replaces DUMP functionality with Intel HEX load
const rom_t TURMONH =   {0xfd00, sizeof(TURMONH_DATA), TURMONH_DATA};
const rom_t MBL =      {0xfe00, sizeof(MBL_DATA), MBL_DATA};
const rom_t DBL =      {0xff00, sizeof(DBL_DATA), DBL_DATA};