#include "lcd_lib.h"

//-------------variables-----------------------------------------------------
LCD_STRUCT lcdStruct = { 0x00, 0, 0, LCD_WriteSymbolToBuffer, LCD_WriteStringToBuffer, 
                         LCD_DisplayFullUpdate, 0, LCD_ClearOrFillDisplay};
const uint8_t LIBRARY_SYMBOL[] = 	           // �������� ������� ASCII
{
  // 0x00 - ������ ����������.
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  // 0x01 - ���� � �������.
  0x00, 0x3e, 0x45, 0x51, 0x45, 0x3e,

  // 0x02 - ���� � ������� �����������.
  0x00, 0x3e, 0x6b, 0x6f, 0x6b, 0x3e,

  // 0x03 - ����� (������).
  0x00, 0x1c, 0x3e, 0x7c, 0x3e, 0x1c,

  // 0x04 - �����.
  0x00, 0x18, 0x3c, 0x7e, 0x3c, 0x18,

  // 0x05 - ������.
  0x00, 0x30, 0x36, 0x7f, 0x36, 0x30,

  // 0x06 - ����.
  0x00, 0x18, 0x5c, 0x7e, 0x5c, 0x18,

  // 0x07 - ����������� ���� �� ������.
  0x00, 0x00, 0x18, 0x18, 0x00, 0x00,

  // 0x08 - ����������� ���� �� ������ � ��������.
  0xff, 0xff, 0xe7, 0xe7, 0xff, 0xff,

  // 0x09 - ������������� ���� �� ������.
  0x00, 0x3c, 0x24, 0x24, 0x3c, 0x00,

  // 0x0a - ������������� ���� �� ������ � �������� (������).
  0xff, 0xc3, 0xdb, 0xdb, 0xc3, 0xff,

  // 0x0b - ������� ������ (���� �� �������� �����).
  0x00, 0x30, 0x48, 0x4a, 0x36, 0x0e,

  // 0x0c - ������� ������ (���� � ������� �����).
  0x00, 0x06, 0x29, 0x79, 0x29, 0x06,

  // 0x0d - ���� I.
  0x00, 0x60, 0x70, 0x3f, 0x02, 0x04,

  // 0x0e - ���� II.
  0x00, 0x60, 0x7e, 0x0a, 0x35, 0x3f,

  // 0x0f - ������ (���� � ������������� ������).
  0x00, 0x2a, 0x1c, 0x36, 0x1c, 0x2a,

  // 0x10 - ������� ������� ������.
  0x00, 0x00, 0x7f, 0x3e, 0x1c, 0x08,

  // 0x11 - ������� ������� �����.
  0x00, 0x08, 0x1c, 0x3e, 0x7f, 0x00,

  // 0x12 - ������ ������� �����-����.
  0x00, 0x14, 0x36, 0x7f, 0x36, 0x14,

  // 0x13 - ��� ��������������� �����.
  0x00, 0x00, 0x5f, 0x00, 0x5f, 0x00,

  // 0x14 - ������ "��".
  0x00, 0x06, 0x09, 0x7f, 0x01, 0x7f,

  // 0x15 - ������ ���������.
  0x00, 0x22, 0x4d, 0x55, 0x59, 0x22,

  // 0x16 - ������� �������������.
  0x00, 0x60, 0x60, 0x60, 0x60, 0x00,

  // 0x17 - ������ ������� �����-���� � ��������������.
  0x00, 0x14, 0xb6, 0xff, 0xb6, 0x14,

  // 0x18 - ������ ������� �����.
  0x00, 0x04, 0x06, 0x7f, 0x06, 0x04,

  // 0x19 - ������ ������� ����.
  0x00, 0x10, 0x30, 0x7f, 0x30, 0x10,

  // 0x1a - ������ ������� ������.
  0x00, 0x08, 0x08, 0x3e, 0x1c, 0x08,

  // 0x1b - ������ ������� �����.
  0x00, 0x08, 0x1c, 0x3e, 0x08, 0x08,

  // 0x1c - ������ �������.
  0x00, 0x78, 0x40, 0x40, 0x40, 0x40,

  // 0x1d - ������ ������� �����-������.
  0x00, 0x08, 0x3e, 0x08, 0x3e, 0x08,

  // 0x1e - ������� ������� �����.
  0x00, 0x30, 0x3c, 0x3f, 0x3c, 0x30,

  // 0x1f - ������� ������� ����.
  0x00, 0x03, 0x0f, 0x3f, 0x0f, 0x03,

  // 0x20 - ������ (������ ����������).
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  // 0x21 - ��������������� ����.
  0x00, 0x00, 0x06, 0x5f, 0x06, 0x00,

  // 0x22 - ������� �������.
  0x00, 0x07, 0x03, 0x00, 0x07, 0x03,

  // 0x23 - �������.
  0x00, 0x24, 0x7e, 0x24, 0x7e, 0x24,

  // 0x24 - ������.
  0x00, 0x24, 0x2b, 0x6a, 0x12, 0x00,

  // 0x25 - �������.
  0x00, 0x63, 0x13, 0x08, 0x64, 0x63,

  // 0x26 - ���������.
  0x00, 0x36, 0x49, 0x56, 0x20, 0x50,

  // 0x27 - ��������.
  0x00, 0x00, 0x07, 0x03, 0x00, 0x00,

  // 0x28 - ����������� ������.
  0x00, 0x00, 0x3e, 0x41, 0x00, 0x00,

  // 0x29 - ����������� ������.
  0x00, 0x00, 0x41, 0x3e, 0x00, 0x00,

  // 0x2a - ��������� (���������).
  0x00, 0x08, 0x3e, 0x1c, 0x3e, 0x08,

  // 0x2b - ����.
  0x00, 0x08, 0x08, 0x3e, 0x08, 0x08,

  // 0x2c - �������.
  0x00, 0x00, 0xe0, 0x60, 0x00, 0x00,

  // 0x2d - ����.
  0x00, 0x08, 0x08, 0x08, 0x08, 0x08,

  // 0x2e - �����.
  0x00, 0x00, 0x60, 0x60, 0x00, 0x00,

  // 0x2f - ���� �����-������� ('/').
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02,

  // 0x30 - '0'.
  0x00, 0x3e, 0x41, 0x41, 0x41, 0x3e,

  // 0x31 - '1'.
  0x00, 0x00, 0x42, 0x7f, 0x40, 0x00,

  // 0x32 - '2'.
  0x00, 0x62, 0x51, 0x49, 0x49, 0x46,

  // 0x33 - '3'.
  0x00, 0x22, 0x49, 0x49, 0x49, 0x36,

  // 0x34 - '4'.
  0x00, 0x18, 0x14, 0x12, 0x7f, 0x10,

  // 0x35 - '5'.
  0x00, 0x2f, 0x49, 0x49, 0x49, 0x31,

  // 0x36 - '6'.
  0x00, 0x3c, 0x4a, 0x49, 0x49, 0x30,

  // 0x37 - '7'.
  0x00, 0x01, 0x71, 0x09, 0x05, 0x03,

  // 0x38 - '8'.
  0x00, 0x36, 0x49, 0x49, 0x49, 0x36,

  // 0x39 - '9'.
  0x00, 0x06, 0x49, 0x49, 0x29, 0x1e,

  // 0x3a - ���������.
  0x00, 0x00, 0x6c, 0x6c, 0x00, 0x00,

  // 0x3b - ����� � �������.
  0x00, 0x00, 0xec, 0x6c, 0x00, 0x00,

  // 0x3c - ������.
  0x00, 0x08, 0x14, 0x22, 0x41, 0x00,

  // 0x3d - �����.
  0x00, 0x24, 0x24, 0x24, 0x24, 0x24,

  // 0x3e - ������.
  0x00, 0x00, 0x41, 0x22, 0x14, 0x08,

  // 0x3f - �������������� ����.
  0x00, 0x02, 0x01, 0x59, 0x09, 0x06,

  // 0x40 - "������" ('@').
  0x00, 0x3e, 0x41, 0x5d, 0x55, 0x1e,

  // 0x41 - 'A'.
  0x00, 0x7e, 0x11, 0x11, 0x11, 0x7e,

  // 0x42 - 'B'.
  0x00, 0x7f, 0x49, 0x49, 0x49, 0x36,

  // 0x43 - 'C'.
  0x00, 0x3e, 0x41, 0x41, 0x41, 0x22,

  // 0x44 - 'D'.
  0x00, 0x7f, 0x41, 0x41, 0x41, 0x3e,

  // 0x45 - 'E'.
  0x00, 0x7f, 0x49, 0x49, 0x49, 0x41,

  // 0x46 - 'F'.
  0x00, 0x7f, 0x09, 0x09, 0x09, 0x01,

  // 0x47 - 'G'.
  0x00, 0x3e, 0x41, 0x49, 0x49, 0x7a,

  // 0x48 - 'H'.
  0x00, 0x7f, 0x08, 0x08, 0x08, 0x7f,

  // 0x49 - 'I'.
  0x00, 0x00, 0x41, 0x7f, 0x41, 0x00,

  // 0x4a - 'J'.
  0x00, 0x30, 0x40, 0x40, 0x40, 0x3f,

  // 0x4b - 'K'.
  0x00, 0x7f, 0x08, 0x14, 0x22, 0x41,

  // 0x4c - 'L'.
  0x00, 0x7f, 0x40, 0x40, 0x40, 0x40,

  // 0x4d - 'M'.
  0x00, 0x7f, 0x02, 0x04, 0x02, 0x7f,

  // 0x4e - 'N'.
  0x00, 0x7f, 0x02, 0x04, 0x08, 0x7f,

  // 0x4f - 'O'.
  0x00, 0x3e, 0x41, 0x41, 0x41, 0x3e,

  // 0x50 - 'P'.
  0x00, 0x7f, 0x09, 0x09, 0x09, 0x06,

  // 0x51 - 'Q'.
  0x00, 0x3e, 0x41, 0x51, 0x21, 0x5e,

  // 0x52 - 'R'.
  0x00, 0x7f, 0x09, 0x09, 0x19, 0x66,

  // 0x53 - 'S'.
  0x00, 0x26, 0x49, 0x49, 0x49, 0x32,

  // 0x54 - 'T'.
  0x00, 0x01, 0x01, 0x7f, 0x01, 0x01,

  // 0x55 - 'U'.
  0x00, 0x3f, 0x40, 0x40, 0x40, 0x3f,

  // 0x56 - 'V'.
  0x00, 0x1f, 0x20, 0x40, 0x20, 0x1f,

  // 0x57 - 'W'.
  0x00, 0x3f, 0x40, 0x3c, 0x40, 0x3f,

  // 0x58 - 'X'.
  0x00, 0x63, 0x14, 0x08, 0x14, 0x63,

  // 0x59 - 'Y'.
  0x00, 0x07, 0x08, 0x70, 0x08, 0x07,

  // 0x5a - 'Z'.
  0x00, 0x71, 0x49, 0x45, 0x43, 0x00,

  // 0x5b - '['.
  0x00, 0x00, 0x7f, 0x41, 0x41, 0x00,

  // 0x5c - '\'.
  0x00, 0x02, 0x04, 0x08, 0x10, 0x20,

  // 0x5d - ']'.
  0x00, 0x00, 0x41, 0x41, 0x7f, 0x00,

  // 0x5e - '^'.
  0x00, 0x04, 0x02, 0x01, 0x02, 0x04,

  // 0x5f - '_'.
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80,

  // 0x60 - �������� ��������.
  0x00, 0x00, 0x03, 0x07, 0x00, 0x00,

  // 0x61 - 'a'.
  0x00, 0x20, 0x54, 0x54, 0x54, 0x78,

  // 0x62 - 'b'.
  0x00, 0x7f, 0x44, 0x44, 0x44, 0x38,

  // 0x63 - 'c'.
  0x00, 0x38, 0x44, 0x44, 0x44, 0x28,

  // 0x64 - 'd'.
  0x00, 0x38, 0x44, 0x44, 0x44, 0x7f,

  // 0x65 - 'e'.
  0x00, 0x38, 0x54, 0x54, 0x54, 0x08,

  // 0x66 - 'f'.
  0x00, 0x08, 0x7e, 0x09, 0x09, 0x00,

  // 0x67 - 'g'.
  0x00, 0x18, 0xa4, 0xa4, 0xa4, 0x7c,

  // 0x68 - 'h'.
  0x00, 0x7f, 0x04, 0x04, 0x78, 0x00,

  // 0x69 - 'i'.
  0x00, 0x00, 0x00, 0x7d, 0x40, 0x00,

  // 0x6a - 'j'.
  0x00, 0x40, 0x80, 0x84, 0x7d, 0x00,

  // 0x6b - 'k'.
  0x00, 0x7f, 0x10, 0x28, 0x44, 0x00,

  // 0x6c - 'l'.
  0x00, 0x00, 0x00, 0x7f, 0x40, 0x00,

  // 0x6d - 'm'.
  0x00, 0x7c, 0x04, 0x18, 0x04, 0x78,

  // 0x6e - 'n'.
  0x00, 0x7c, 0x04, 0x04, 0x78, 0x00,

  // 0x6f - 'o'.
  0x00, 0x38, 0x44, 0x44, 0x44, 0x38,

  // 0x70 - 'p'.
  0x00, 0xfc, 0x44, 0x44, 0x44, 0x38,

  // 0x71 - 'q'.
  0x00, 0x38, 0x44, 0x44, 0x44, 0xfc,

  // 0x72 - 'r'.
  0x00, 0x44, 0x78, 0x44, 0x04, 0x08,

  // 0x73 - 's'.
  0x00, 0x08, 0x54, 0x54, 0x54, 0x20,

  // 0x74 - 't'.
  0x00, 0x04, 0x3e, 0x44, 0x24, 0x00,

  // 0x75 - 'u'.
  0x00, 0x3c, 0x40, 0x20, 0x7c, 0x00,

  // 0x76 - 'v'.
  0x00, 0x1c, 0x20, 0x40, 0x20, 0x1c,

  // 0x77 - 'w'.
  0x00, 0x3c, 0x60, 0x30, 0x60, 0x3c,

  // 0x78 - 'x'.
  0x00, 0x6c, 0x10, 0x10, 0x6c, 0x00,

  // 0x79 - 'y'.
  0x00, 0x9c, 0xa0, 0x60, 0x3c, 0x00,

  // 0x7a - 'z'.
  0x00, 0x64, 0x54, 0x54, 0x4c, 0x00,

  // 0x7b - '{'.
  0x00, 0x08, 0x3e, 0x41, 0x41, 0x00,

  // 0x7c - '|'.
  0x00, 0x00, 0x00, 0x77, 0x00, 0x00,

  // 0x7d - '}'.
  0x00, 0x00, 0x41, 0x41, 0x3e, 0x08,

  // 0x7e - '~'.
  0x00, 0x02, 0x01, 0x02, 0x01, 0x00,

  // 0x7f - "�����".
  0x00, 0x3c, 0x26, 0x23, 0x26, 0x3c,

  // 0x80 - ����� �� ����� �����������.
  0x44, 0x11, 0x44, 0x11, 0x44, 0x11,

  // 0x81 - ����� �� ����� ������.
  0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,

  // 0x82 - ����� �� ��������.
  0xbb, 0xee, 0xbb, 0xee, 0xbb, 0xee,

  // 0x83 - ������� ����� ������������� - ������������ �����.
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00,

  // 0x84 - ������������� - ������������ ����� � ������� �� ������ �����.
  0x08, 0x08, 0x08, 0xff, 0x00, 0x00,

  // 0x85 - ������������� - ������������ ����� � ������� ������� �� ������ �����.
  0x0a, 0x0a, 0x0a, 0xff, 0x00, 0x00,

  // 0x86 - ������������� - ������� ������������ ����� � ������� �� ������ �����.
  0x08, 0xff, 0x00, 0xff, 0x00, 0x00,

  // 0x87 - ������������� - ������� ������ ���� � ������� ������������ ������.
  0x08, 0xf8, 0x08, 0xf8, 0x00, 0x00,

  // 0x88 - ������������� - ������� ������ ���� � ������� �������������� ������.
  0x0a, 0x0a, 0x0a, 0xfe, 0x00, 0x00,

  // 0x89 - ������������� - ������� ������������ ����� � ������� ������� �� ������ �����.
  0x0a, 0xfb, 0x00, 0xff, 0x00, 0x00,

  // 0x8a - ������������� - ������� ������������ �����.
  0x00, 0xff, 0x00, 0xff, 0x00, 0x00,

  // 0x8b - ������������� - ������� ������� ������ ����.
  0x0a, 0xfa, 0x02, 0xfe, 0x00, 0x00,

  // 0x8c - ������������� - ������� ������ ������ ����.
  0x0a, 0x0b, 0x08, 0x0f, 0x00, 0x00,

  // 0x8d - ������������� - ������ ������ ���� � ������� ������������ ������.
  0x08, 0x0f, 0x08, 0x0f, 0x00, 0x00,

  // 0x8e - ������������� - ������ ������ ���� � ������� �������������� ������.
  0x0a, 0x0a, 0x0a, 0x0f, 0x00, 0x00,

  // 0x8f - ������������� - ������� ������ ����.
  0x08, 0x08, 0x08, 0xf8, 0x00, 0x00,

  // 0x90 - ������������� - ������ ����� ����.
  0x00, 0x00, 0x00, 0x0f, 0x08, 0x08,

  // 0x91 - ������������� - �������������� ����� � ������� �� ������ �����.
  0x08, 0x08, 0x08, 0x0f, 0x08, 0x08,

  // 0x92 - ������������� - �������������� ����� � ������� �� ������ ����.
  0x08, 0x08, 0x08, 0xf8, 0x08, 0x08,

  // 0x93 - ������������� - ������������ ����� � ������� �� ������ ������.
  0x00, 0x00, 0x00, 0xff, 0x08, 0x08,

  // 0x94 - ������������� - �������������� ����� �� ������.
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08,

  // 0x95 - ������������� - �����������.
  0x08, 0x08, 0x08, 0xff, 0x08, 0x08,

  // 0x96 - ������������� - ������������ ����� � ������� ������� �� ������ ������.
  0x00, 0x00, 0x00, 0xff, 0x0a, 0x0a,

  // 0x97 - ������������� - ������� ������������ ����� � ������� �� ������ ������.
  0x00, 0xff, 0x00, 0xff, 0x08, 0x08,

  // 0x98 - ������������� - ������� ������ ����� ����.
  0x00, 0x0f, 0x08, 0x0b, 0x0a, 0x0a,

  // 0x99 - ������������� - ������� ������� ����� ����.
  0x00, 0xfe, 0x02, 0xfa, 0x0a, 0x0a,

  // 0x9a - ������������� - ������� �������������� ����� � ������� ������� �� ������ �����.
  0x0a, 0x0b, 0x08, 0x0b, 0x0a, 0x0a,

  // 0x9b - ������������� - ������� �������������� ����� � ������� ������� �� ������ ����.
  0x0a, 0xfa, 0x02, 0xfa, 0x0a, 0x0a,

  // 0x9c - ������������� - ������� ������������ ����� � ������� ������� �� ������ ������.
  0x00, 0xff, 0x00, 0xfb, 0x0a, 0x0a,

  // 0x9d - ������������� - ������� �������������� ����� �� ������.
  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,

  // 0x9e - ������������� - ������� �����������.
  0x0a, 0xfb, 0x00, 0xfb, 0x0a, 0x0a,

  // 0x9f - ������������� - ������� �������������� ����� � ������� �� ������ �����.
  0x0a, 0x0a, 0x0a, 0x0b, 0x0a, 0x0a,

  // 0xa0 - ������������� - �������������� ����� � ������� ������� �� ������ �����.
  0x08, 0x0f, 0x08, 0x0f, 0x08, 0x08,

  // 0xa1 - ������������� - ������� �������������� ����� � ������� �� ������ ����.
  0x0a, 0x0a, 0x0a, 0xfa, 0x0a, 0x0a,

  // 0xa2 - ������������� - �������������� ����� � ������� ������� �� ������ ����.
  0x08, 0xf8, 0x08, 0xf8, 0x08, 0x08,

  // 0xa3 - ������������� - ������ ����� ����, ������� ������������ �����.
  0x00, 0x0f, 0x08, 0x0f, 0x08, 0x08,

  // 0xa4 - ������������� - ������ ����� ����, ������� �������������� �����.
  0x00, 0x00, 0x00, 0x0f, 0x0a, 0x0a,

  // 0xa5 - ������������� - ������� ����� ����, ������� �������������� �����.
  0x00, 0x00, 0x00, 0xfe, 0x0a, 0x0a,

  // 0xa6 - ������������� - ������� ����� ����, ������� ������������ �����.
  0x00, 0xf8, 0x08, 0xf8, 0x08, 0x08,

  // 0xa7 - ������������� - ������� ������������ ����� �� ������ � �������� ������ � �����.
  0x08, 0xff, 0x08, 0xff, 0x08, 0x08,

  // 0xa8 - '�'.
  0x00, 0x7e, 0x4b, 0x4a, 0x4b, 0x42,

  // 0xa9 - ������������� - ������� �������������� ����� �� ������ � �������� ����� � ����.
  0x0a, 0x0a, 0x0a, 0xff, 0x0a, 0x0a,

  // 0xaa - ������������� - ������ ������ ����.
  0x08, 0x08, 0x08, 0x0f, 0x00, 0x00,

  // 0xab - ������������� - ������� ����� ����.
  0x00, 0x00, 0x00, 0xf8, 0x08, 0x08,

  // 0xac - ������������� - ����������� ����������.
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

  // 0xad - ������������� - ����������� ������ �������� ����������.
  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,

  // 0xae - ������������� - ����������� ����� �������� ����������.
  0xff, 0xff, 0xff, 0x00, 0x00, 0x00,

  // 0xaf - ������������� - ����������� ������ �������� ����������.
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff,

  // 0xb0 - ������������� - ����������� ������� �������� ����������.
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,

  // 0xb1 - ���������� '�'.
  0x00, 0x3e, 0x49, 0x49, 0x41, 0x22,

  // 0xb2 - ���������� '�'.
  0x00, 0x38, 0x54, 0x54, 0x44, 0x28,

  // 0xb3 - 'I' � ����� ������� ������.
  0x00, 0x01, 0x40, 0x7e, 0x40, 0x01,

  // 0xb4 - 'i' � ����� ������� ������.
  0x00, 0x00, 0x01, 0x7c, 0x41, 0x00,

  // 0xb5 - 'Y' � ������� ������ ('~').
  0x00, 0x27, 0x48, 0x4b, 0x48, 0x3f,

  // 0xb6 - 'y' � ������� ������ ('~').
  0x00, 0x9d, 0xa2, 0x62, 0x3d, 0x00,

  // 0xb7 - ��������� ������ ������.
  0x00, 0x06, 0x09, 0x09, 0x06, 0x00,

  // 0xb8 - '�'.
  0x00, 0x38, 0x55, 0x54, 0x55, 0x08,

  // 0xb9 - ������� ����������� ���� �� ������.
  0x00, 0x00, 0x18, 0x18, 0x00, 0x00,

  // 0xba - ��������� ����������� ���� �� ������.
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00,

  // 0xbb - ������ ����� �����������.
  0x00, 0x30, 0x40, 0x3e, 0x02, 0x02,

  
  0x7f, 0x06, 0x18, 0x7f, 0x13, 0x13, // 0xbc - '�'.
  0x2a, 0x3e, 0x14, 0x14, 0x3e, 0x2a, // 0xbd - "������".
  0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, // 0xbe - ����������� ������� �� ������.
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xbf - ������ ����������.
  0x00, 0x7e, 0x11, 0x11, 0x11, 0x7e, // 0xc0 - '�'.
  0x00, 0x7f, 0x49, 0x49, 0x49, 0x31, // 0xc1 - '�'.
  0x00, 0x7f, 0x49, 0x49, 0x49, 0x36, // 0xc2 - '�'.
  0x00, 0x7f, 0x01, 0x01, 0x01, 0x01, // 0xc3 - '�'.
  0xc0, 0x7e, 0x41, 0x41, 0x7f, 0xc0, // 0xc4 - '�'.
  0x00, 0x7f, 0x49, 0x49, 0x49, 0x41, // 0xc5 - '�'.
  0x00, 0x77, 0x08, 0x7f, 0x08, 0x77, // 0xc6 - '�'.
  0x22, 0x49, 0x49, 0x49, 0x36, 0x00, // 0xc7 - '�'.
  0x00, 0x7f, 0x20, 0x10, 0x08, 0x7f, // 0xc8 - '�'.
  0x00, 0x7e, 0x21, 0x11, 0x09, 0x7e, // 0xc9 - '�'.
  0x00, 0x7f, 0x08, 0x14, 0x22, 0x41, // 0xca - '�'.
  0x00, 0x40, 0x7e, 0x01, 0x01, 0x7f, // 0xcb - '�'.
  0x00, 0x7f, 0x02, 0x04, 0x02, 0x7f, // 0xcc - '�'.
  0x00, 0x7f, 0x08, 0x08, 0x08, 0x7f, // 0xcd - '�'.
  0x00, 0x3e, 0x41, 0x41, 0x41, 0x3e, // 0xce - '�'.
  0x00, 0x7f, 0x01, 0x01, 0x01, 0x7f, // 0xcf - '�'.
  0x00, 0x7f, 0x09, 0x09, 0x09, 0x06, // 0xd0 - '�'.
  0x00, 0x3e, 0x41, 0x41, 0x41, 0x22, // 0xd1 - '�'.
  0x00, 0x01, 0x01, 0x7f, 0x01, 0x01, // 0xd2 - '�'.
  0x00, 0x27, 0x48, 0x48, 0x48, 0x3f, // 0xd3 - '�'.
  0x00, 0x0e, 0x11, 0x7f, 0x11, 0x0e, // 0xd4 - '�'.
  0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // 0xd5 - '�'.
  0x00, 0x7f, 0x40, 0x40, 0x7f, 0xc0, // 0xd6 - '�'.
  0x00, 0x07, 0x08, 0x08, 0x08, 0x7f, // 0xd7 - '�'.
  0x00, 0x7f, 0x40, 0x7f, 0x40, 0x7f, // 0xd8 - '�'.
  0x00, 0x7f, 0x40, 0x7f, 0x40, 0xff, // 0xd9 - '�'.
  0x03, 0x01, 0x7f, 0x48, 0x48, 0x30, // 0xda - '�'.
  0x00, 0x7f, 0x48, 0x48, 0x30, 0x7f, // 0xdb - '�'.
  0x00, 0x7f, 0x48, 0x48, 0x48, 0x30, // 0xdc - '�'.
  0x00, 0x22, 0x41, 0x49, 0x49, 0x3e, // 0xdd - '�'.
  0x00, 0x7f, 0x08, 0x3e, 0x41, 0x3e, // 0xde - '�'.
  0x00, 0x66, 0x19, 0x09, 0x09, 0x7f, // 0xdf - '�'.
  0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // 0xe0 - '�'.
  0x00, 0x3c, 0x4a, 0x4a, 0x4a, 0x31, // 0xe1 - '�'.
  0x00, 0x7c, 0x54, 0x54, 0x54, 0x28, // 0xe2 - '�'.
  0x00, 0x7c, 0x04, 0x04, 0x0c, 0x00, // 0xe3 - '�'.
  0xc0, 0x78, 0x44, 0x44, 0x7c, 0xc0, // 0xe4 - '�'.
  0x00, 0x38, 0x54, 0x54, 0x54, 0x08, // 0xe5 - '�'.
  0x00, 0x6c, 0x10, 0x7c, 0x10, 0x6c, // 0xe6 - '�'.
  0x00, 0x28, 0x44, 0x54, 0x54, 0x28, // 0xe7 - '�'.
  0x00, 0x7c, 0x20, 0x10, 0x08, 0x7c, // 0xe8 - '�'.
  0x00, 0x7c, 0x20, 0x12, 0x0a, 0x7c, // 0xe9 - '�'.
  0x00, 0x7c, 0x10, 0x28, 0x44, 0x00, // 0xea - '�'.
  0x40, 0x38, 0x04, 0x04, 0x7c, 0x00, // 0xeb - '�'.
  0x00, 0x7c, 0x08, 0x10, 0x08, 0x7c, // 0xec - '�'.
  0x00, 0x7c, 0x10, 0x10, 0x10, 0x7c, // 0xed - '�'.
  0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // 0xee - '�'.
  0x00, 0x7c, 0x04, 0x04, 0x04, 0x7c, // 0xef - '�'.
  0x00, 0xfc, 0x44, 0x44, 0x44, 0x38, // 0xf0 - '�'.
  0x00, 0x38, 0x44, 0x44, 0x44, 0x28, // 0xf1 - '�'.
  0x00, 0x04, 0x04, 0x7c, 0x04, 0x04, // 0xf2 - '�'.
  0x00, 0x9c, 0xa0, 0x60, 0x3c, 0x00, // 0xf3 - '�'.
  0x00, 0x18, 0x24, 0x7c, 0x24, 0x18, // 0xf4 - '�'.
  0x00, 0x6c, 0x10, 0x10, 0x6c, 0x00, // 0xf5 - '�'.
  0x00, 0x7c, 0x40, 0x40, 0x7c, 0xc0, // 0xf6 - '�'.
  0x00, 0x0c, 0x10, 0x10, 0x10, 0x7c, // 0xf7 - '�'.
  0x00, 0x7c, 0x40, 0x7c, 0x40, 0x7c, // 0xf8 - '�'.
  0x00, 0x7c, 0x40, 0x7c, 0x40, 0xfc, // 0xf9 - '�'.
  0x0c, 0x04, 0x7c, 0x50, 0x50, 0x20, // 0xfa - '�'.
  0x00, 0x7c, 0x50, 0x50, 0x20, 0x7c, // 0xfb - '�'.
  0x00, 0x7c, 0x50, 0x50, 0x50, 0x20, // 0xfc - '�'.
  0x00, 0x28, 0x44, 0x54, 0x54, 0x38, // 0xfd - '�'.
  0x00, 0x7c, 0x10, 0x38, 0x44, 0x38, // 0xfe - '�'.
  0x00, 0x48, 0x34, 0x14, 0x14, 0x7c  // 0xff - '�'.
};

//
//-----init functions--------------------------------------------------------------
void LCD_PortsInit(void){
    // enable port clocking
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOCEN, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIODEN, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOFEN, ENABLE);
    
    // Port F init (PF6  - R/W; PF7  - E)
    GPIO_InitTypeDef GPIOF_ini = {0};
    GPIOF_ini.GPIO_Pin = (RW_PIN | E_PIN);
    GPIOF_ini.GPIO_Mode = GPIO_Mode_OUT;                     // Otput push-pull mode
    GPIOF_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOF_ini.GPIO_OType = GPIO_OType_PP;
    GPIOF_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOF, &GPIOF_ini);
    
    // Port A init (PA12 - RS)
    GPIO_InitTypeDef GPIOA_ini;
    GPIOA_ini.GPIO_Pin = RS_PIN;
    GPIOA_ini.GPIO_Mode = GPIO_Mode_OUT;                     // Otput push-pull mode
    GPIOA_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOA_ini.GPIO_OType = GPIO_OType_PP;
    GPIOA_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOA, &GPIOA_ini);
    
    // Port C init (PC10 - DB0;  PC11 - DB1;  PC12 - DB2)
    GPIO_InitTypeDef GPIOC_ini = {0};
    GPIOC_ini.GPIO_Pin = (DB0_PIN | DB1_PIN | DB2_PIN);
    GPIOC_ini.GPIO_Mode = GPIO_Mode_OUT;                     // Otput push-pull mode
    GPIOC_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOC_ini.GPIO_OType = GPIO_OType_PP;
    GPIOC_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOC, &GPIOC_ini);
    
    // Port D init (PD2  - DB3)
    GPIO_InitTypeDef GPIOD_ini = {0};
    GPIOD_ini.GPIO_Pin = DB3_PIN;
    GPIOD_ini.GPIO_Mode = GPIO_Mode_OUT;                     // Otput push-pull mode
    GPIOD_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOD_ini.GPIO_OType = GPIO_OType_PP;
    GPIOD_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOD, &GPIOD_ini);
    
    // Port B init (PB3  - DB4;  PB4  - DB5; PB5 - DB6; PB6 - DB7; PB7  - RST)
    GPIO_InitTypeDef GPIOB_ini = {0};
    GPIOB_ini.GPIO_Pin = (DB4_PIN | DB5_PIN | DB6_PIN | DB7_PIN | RST_PIN);
    GPIOB_ini.GPIO_Mode = GPIO_Mode_OUT;                  // Otput push-pull mode
    GPIOB_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOB_ini.GPIO_OType = GPIO_OType_PP;
    GPIOB_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOB, &GPIOB_ini);
    
    // Pins reset
	GPIO_WriteBit(GPIOA, RS_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOF, RW_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOF, E_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOB, RST_PIN, Bit_RESET);
}

void LCD_Init(void){
    // wait > 40 ms after power on
    delay(40000);
    
    // reset system
    RST(0);
    delay(10000);
    // not reset system
    RST(1);
    
    // function set
    LCD_SendCommand(FUNCTION_SET_BASE); // 0011x0xx (RW = 0, RS = 0)
    // wait > 100 mks
    delay(300);
    
    // function set again
    LCD_SendCommand(FUNCTION_SET_BASE); // 0011x0xx
    // wait > 37 mks
    delay(100);
    
    // display status
    LCD_SendCommand(DISPLAY_ON); // display on
    // wait > 100 mks
    delay(200);
    
    LCD_SendCommand(CLEAR_DISPLAY); // clear display
    // wait > 10 ms
    delay(20000);
    
    LCD_SendCommand(ENTRY_MODE_RS); // cursor move right and no shift
    delay(10); 
    
    // enable grafical mode
    LCD_SendCommand(FUNCTION_SET_EXT);  // 0x30 + RE = 1. Go to extended instruction.
    delay(10);
	LCD_SendCommand(ENABLE_GRAFICAL_MODE);  // Enable graphical mode
	delay(10);
}

//
//-----send functions-----------------------------------------------------------
void LCD_SendCommand(uint8_t byte){
    RS(0);
    RW(0);
    
    E(1);
    DB7((byte >> 7) & 0x01);
    DB6((byte >> 6) & 0x01);
    DB5((byte >> 5) & 0x01);
    DB4((byte >> 4) & 0x01);
    DB3((byte >> 3) & 0x01);
    DB2((byte >> 2) & 0x01);
    DB1((byte >> 1) & 0x01);
    DB0(byte & 0x01);
    
    delay(8);
    E(0);
    
    // delay for command execution (or data send)
    delay(72);
}
void LCD_SendByteData(uint8_t byte){
    RS(1);
    RW(0);
    
    E(1);
    DB7((byte >> 7) & 0x01);
    DB6((byte >> 6) & 0x01);
    DB5((byte >> 5) & 0x01);
    DB4((byte >> 4) & 0x01);
    DB3((byte >> 3) & 0x01);
    DB2((byte >> 2) & 0x01);
    DB1((byte >> 1) & 0x01);
    DB0(byte & 0x01);
    
    delay(8);
    E(0);
    
    // delay for command execution (or data send)
    delay(72);
}

void LCD_SetAddr(uint8_t vertAddr, uint8_t horizAddr){
    // address auto increase after set y (0x3F (63) max) and x (0x0F (15) max) address
    
    // set y address [0, 63]
    LCD_SendCommand((SET_GDRAM_ADDR | (vertAddr & 0x3F)));
  
    // set x address [0, 15]
    LCD_SendCommand((SET_GDRAM_ADDR | (horizAddr & 0x0F)));
}
//
//-----functions with displayBuffer-------------------------------------------
uint8_t LCD_GetHorizontalByte(uint8_t *pBuff, uint8_t Row, uint8_t Col){ // how is it work?????
  uint8_t Byte = 0;
  
  // calculation start byte index
  uint16_t ByteIdx = (Row >> 3) * DISPLAY_WIDTH;         // Index of row start in displayBuffer
  ByteIdx += (Col << 3);
  
  // Bit mask, which selects a row of eight px in displayBuffer
  uint8_t BitMask = Row % 8;
  BitMask = (1 << BitMask);
  
  // Selects 8 bits
  for (uint8_t Bit = 0; Bit < 8; Bit++)
  {
    if (pBuff[ByteIdx + Bit] & BitMask)
      Byte |= (1 << (7 - Bit));
  }
  
  return Byte;
}

void LCD_BufferShiftPage(uint8_t *pBuff, uint8_t page, uint8_t direct, uint8_t isCyclic){
    // remember first or last element if it is a cyclic shift
    uint8_t temp;
    if(isCyclic == CYCLIC_SHIFT){
        temp = (direct == RIGHT_SHIFT ? pBuff[(page+1)*DISPLAY_WIDTH-1] : pBuff[page*DISPLAY_WIDTH]);
    }
    
    if(direct == RIGHT_SHIFT){
        // shift array right
        int index;
        for(index = (page+1)*DISPLAY_WIDTH-1; index > page*DISPLAY_WIDTH; index--){
            pBuff[index] = pBuff[index-1];
        }
        pBuff[page*DISPLAY_WIDTH] = (isCyclic == CYCLIC_SHIFT ? temp : 0x00);
    } else{
        // shift array left
        int index;
        for(index = page*DISPLAY_WIDTH; index < (page+1)*DISPLAY_WIDTH - 1; index++){
            pBuff[index] = pBuff[index+1];
        }
        pBuff[(page+1)*DISPLAY_WIDTH-1] = (isCyclic == CYCLIC_SHIFT ? temp : 0x00);
    }
}

void LCD_WriteSymbolToBuffer(uint8_t *pBuff, char sumbol){	
	// max symbols in one line = 21, max on display = 84;
    if(lcdStruct.byteIndex > (LCD_BUFFER_LENGTH - 6)) lcdStruct.byteIndex = 0;
    
    if((DISPLAY_WIDTH - lcdStruct.byteIndex%DISPLAY_WIDTH < 6)){
        lcdStruct.byteIndex += DISPLAY_WIDTH - lcdStruct.byteIndex%DISPLAY_WIDTH;
    }
	
	// write one symbol to displayBuffer
	int i;
	for(i = 0; i < 6; i++)
	{
		pBuff[lcdStruct.byteIndex + i] = (uint8_t) LIBRARY_SYMBOL[ (sumbol * 6) + i ];
	}
    lcdStruct.byteIndex += 6;
}

void LCD_WriteStringToBuffer(uint8_t *pBuff, const char* str){
	int i;
	for(i = 0; str[i] != 0; i++)
	{
		LCD_WriteSymbolToBuffer(pBuff, str[i]);
	}
}

void LCD_DrawPageFromBuffer(uint8_t *pBuff, uint8_t page){
    for (uint8_t Row = (page << 3); Row < ((page+1) << 3); Row++)
    {
        LCD_SetAddr(Row, 0);
        for (uint8_t Col = 0; Col < 16; Col++){
            LCD_SendByteData(LCD_GetHorizontalByte(pBuff, Row, Col));
        }
    }
}

void LCD_DisplayFullUpdate(uint8_t *pBuff){
    for (uint8_t Row = 0; Row < 32; Row++)
    {
        // Select a new line
        LCD_SetAddr(Row, 0);
        
        // Draw a line one pixel high in the upper of the display
        for (uint8_t Col = 0; Col < 16; Col++)
            LCD_SendByteData(LCD_GetHorizontalByte(pBuff, Row, Col));
        // Draw a line one pixel high in the lower of the display
        for (uint8_t Col = 0; Col < 16; Col++)
            LCD_SendByteData(LCD_GetHorizontalByte(pBuff, Row + 32, Col));
    }
}

//
//----draw functions-----------------------------------------------------
void LCD_ClearOrFillDisplay(uint8_t isClear){
    uint8_t data = (isClear == CLEAR ? 0x00 : 0xFF);
    uint8_t Row, Col;
    for (Row = 0; Row < 32; Row++)
    {
        // Stay on start of new row in display buffer
        LCD_SetAddr(Row, 0);
    
        // Draw a line one pixel high in the upper of the display
        for (Col = 0; Col < 16; Col++)
            LCD_SendByteData(data);
        
        // Draw a line one pixel high in the lower of the display
        for (Col = 0; Col < 16; Col++)
            LCD_SendByteData(data);
    }
}

void LCD_RunLine(const char *str, uint8_t charCount, uint8_t page, uint8_t direct, uint8_t isCyclic){
    uint8_t runLineBuffer[LCD_BUFFER_LENGTH] = {0x00};
    
    long runLineDelay = 40000, shiftCount = 0;
    int i, j;
    // start filling with string
    if(direct == RIGHT_SHIFT){
        // right shift
        for(i = charCount-1; i >= 0; i--){
            for(j = 6; j > 0; j--){
                LCD_BufferShiftPage(runLineBuffer, page, direct, isCyclic);
                shiftCount++;
                runLineBuffer[page*DISPLAY_WIDTH] = (uint8_t) LIBRARY_SYMBOL[ (str[i] * 6) + j ];
                LCD_DrawPageFromBuffer(runLineBuffer, page);
                delay(runLineDelay); 
            }
        }
    } else{
        // left shift
        for(i = 0; i < charCount; i++){
            for(j = 0; j < 6; j++){
                LCD_BufferShiftPage(runLineBuffer, page, direct, isCyclic);
                shiftCount++;
                runLineBuffer[(page+1)*DISPLAY_WIDTH-1] = (uint8_t) LIBRARY_SYMBOL[ (str[i] * 6) + j ];
                LCD_DrawPageFromBuffer(runLineBuffer, page);
                delay(runLineDelay); 
            }
        }
    }
    
    // running line
    while(shiftCount < DISPLAY_WIDTH+charCount*6){
        LCD_BufferShiftPage(runLineBuffer, page, direct, isCyclic);
        shiftCount += (isCyclic == CYCLIC_SHIFT ? 0 : 1);
        LCD_DrawPageFromBuffer(runLineBuffer, page);
        delay(runLineDelay); 
    }
}
