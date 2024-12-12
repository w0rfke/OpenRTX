/**
** The original 3x5 font is licensed under the 3-clause BSD license:
**
** Copyright 1999 Brian J. Swetland
** Copyright 1999 Vassilii Khachaturov
** Portions (of vt100.c/vt100.h) copyright Dan Marks
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions, and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions, and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the authors may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** Modifications to Tom Thumb for improved readability are from Robey Pointer,
** see:
** http://robey.lag.net/2010/01/23/tiny-monospace-font.html
**
** The original author does not have any objection to relicensing of Robey
** Pointer's modifications (in this file) in a more permissive license.  See
** the discussion at the above blog, and also here:
** http://opengameart.org/forumtopic/how-to-submit-art-using-the-3-clause-bsd-license
**
** TomThumbPlus - Modified by Tris
** No more monofont for readabilty (3px to 4px wide, space 2px)
** Some extra mods here and there for what I prefer a cleaner look (only matter of taste:) )
** No changes to the TOMTHUMB_USE_EXTENDED zone
**
** Edited with: GFX Font Editor
** + Some manual cleanup as the tool does not like the jump from 0x7E asciitilde to 0xA1 exclamdown
** https://github.com/ScottFerg56/GFXFontEditor
**
** 
*/


#pragma once

#include <stdint.h>
#include "gfxfont.h"

static const uint8_t TomThumbPlusBitmaps[] PROGMEM = {
					/* ' ' 0x20 */
    0xE8,              /* '!' 0x21 */
    0xB4,              /* '"' 0x22 */
    0x5F, 0x5F, 0x50,  /* '#' 0x23 */
    0x7A, 0x65, 0xE0,  /* '$' 0x24 */
    0x85, 0x42,        /* '%' 0x25 */
    0x4A, 0x4A, 0x50,  /* '&' 0x26 */
    0xC0,              /* ''' 0x27 */
    0x6A, 0x40,        /* '(' 0x28 */
    0x95, 0x80,        /* ')' 0x29 */
    0xAA, 0x80,        /* '*' 0x2A */
    0x5D, 0x00,        /* '+' 0x2B */
    0x60,              /* ',' 0x2C */
    0xE0,              /* '-' 0x2D */
    0x80,              /* '.' 0x2E */
    0x25, 0x48,        /* '/' 0x2F */
    0xF6, 0xDE,        /* '0' 0x30 */
    0xD5, 0x40,        /* '1' 0x31 */
    0xE7, 0xCE,        /* '2' 0x32 */
    0xE5, 0x9E,        /* '3' 0x33 */
    0xB7, 0x92,        /* '4' 0x34 */
    0xF3, 0x9E,        /* '5' 0x35 */
    0xF3, 0xDE,        /* '6' 0x36 */
    0xF4, 0x92,        /* '7' 0x37 */
    0xF7, 0xDE,        /* '8' 0x38 */
    0xF7, 0x9E,        /* '9' 0x39 */
    0xA0,              /* ':' 0x3A */
    0x46,              /* ';' 0x3B */
    0x2A, 0x22,        /* '<' 0x3C */
    0xE3, 0x80,        /* '=' 0x3D */
    0x88, 0xA8,        /* '>' 0x3E */
    0xE5, 0x04,        /* '?' 0x3F */
    0x69, 0xB8, 0x70,  /* '@' 0x40 */
    0x57, 0xDA,        /* 'A' 0x41 */
    0xD7, 0x5C,        /* 'B' 0x42 */
    0x72, 0x46,        /* 'C' 0x43 */
    0xD6, 0xDC,        /* 'D' 0x44 */
    0xF3, 0xCE,        /* 'E' 0x45 */
    0xF3, 0xC8,        /* 'F' 0x46 */
    0x73, 0xD6,        /* 'G' 0x47 */
    0xB7, 0xDA,        /* 'H' 0x48 */
    0xF8,              /* 'I' 0x49 */
    0x24, 0xD4,        /* 'J' 0x4A */
    0xB7, 0x5A,        /* 'K' 0x4B */
    0x92, 0x4E,        /* 'L' 0x4C */
    0x9F, 0x99, 0x90,  /* 'M' 0x4D */
    0x9D, 0xB9, 0x90,  /* 'N' 0x4E */
    0xF6, 0xDE,        /* 'O' 0x4F */
    0xF7, 0xC8,        /* 'P' 0x50 */
    0xF6, 0xF2,        /* 'Q' 0x51 */
    0xF7, 0xEA,        /* 'R' 0x52 */
    0xF1, 0x1E,        /* 'S' 0x53 */
    0xE9, 0x24,        /* 'T' 0x54 */
    0xB6, 0xD6,        /* 'U' 0x55 */
    0xB6, 0xA4,        /* 'V' 0x56 */
    0xB7, 0xFA,        /* 'W' 0x57 */
    0xB5, 0x5A,        /* 'X' 0x58 */
    0xB5, 0x24,        /* 'Y' 0x59 */
    0xE5, 0x4E,        /* 'Z' 0x5A */
    0xEA, 0xC0,        /* '[' 0x5B */
    0x88, 0x80,        /* '\' 0x5C */
    0xD5, 0xC0,        /* ']' 0x5D */
    0x54,              /* '^' 0x5E */
    0xE0,              /* '_' 0x5F */
    0x90,              /* '`' 0x60 */
    0xCE, 0xF0,        /* 'a' 0x61 */
    0x9A, 0xDC,        /* 'b' 0x62 */
    0x72, 0x30,        /* 'c' 0x63 */
    0x2E, 0xD6,        /* 'd' 0x64 */
    0x77, 0x30,        /* 'e' 0x65 */
    0x2B, 0xA4,        /* 'f' 0x66 */
    0x77, 0x94,        /* 'g' 0x67 */
    0x9A, 0xDA,        /* 'h' 0x68 */
    0xB8,              /* 'i' 0x69 */
    0x20, 0x9A, 0x80,  /* 'j' 0x6A */
    0x97, 0x6A,        /* 'k' 0x6B */
    0xC9, 0x2E,        /* 'l' 0x6C */
    0xFF, 0xD0,        /* 'm' 0x6D */
    0xD6, 0xD0,        /* 'n' 0x6E */
    0x56, 0xA0,        /* 'o' 0x6F */
    0xD6, 0xE8,        /* 'p' 0x70 */
    0x76, 0xB2,        /* 'q' 0x71 */
    0x72, 0x40,        /* 'r' 0x72 */
    0x79, 0xE0,        /* 's' 0x73 */
    0x5D, 0x26,        /* 't' 0x74 */
    0xB6, 0xB0,        /* 'u' 0x75 */
    0xB7, 0xA0,        /* 'v' 0x76 */
    0xBF, 0xF0,        /* 'w' 0x77 */
    0xA9, 0x50,        /* 'x' 0x78 */
    0xB5, 0x94,        /* 'y' 0x79 */
    0xEF, 0x70,        /* 'z' 0x7A */
    0x6A, 0x26,        /* '{' 0x7B */
    0xF8,              /* '|' 0x7C */
    0xC8, 0xAC,        /* '}' 0x7D */
    0x78,              /* '~' 0x7E */
#if (TOMTHUMB_USE_EXTENDED)	
    0xB8,              /* 0xA1 exclamdown */
    0x5E, 0x74,        /* 0xA2 cent */
    0x6B, 0xAE,        /* 0xA3 sterling */
    0xAB, 0xAA,        /* 0xA4 currency */
    0xB5, 0x74,        /* 0xA5 yen */
    0xD8,              /* 0xA6 brokenbar */
    0x6A, 0xAC,        /* 0xA7 section */
    0xA0,              /* 0xA8 dieresis */
    0x71, 0x80,        /* 0xA9 copyright */
    0x77, 0x8E,        /* 0xAA ordfeminine */
    0x64,              /* 0xAB guillemotleft */
    0xE4,              /* 0xAC logicalnot */
    0xC0,              /* 0xAD softhyphen */
    0xDA, 0x80,        /* 0xAE registered */
    0xE0,              /* 0xAF macron */
    0x55, 0x00,        /* 0xB0 degree */
    0x5D, 0x0E,        /* 0xB1 plusminus */
    0xC9, 0x80,        /* 0xB2 twosuperior */
    0xEF, 0x80,        /* 0xB3 threesuperior */
    0x60,              /* 0xB4 acute */
    0xB6, 0xE8,        /* 0xB5 mu */
    0x75, 0xB6,        /* 0xB6 paragraph */
    0xFF, 0x80,        /* 0xB7 periodcentered */
    0x47, 0x00,        /* 0xB8 cedilla */
    0xE0,              /* 0xB9 onesuperior */
    0x55, 0x0E,        /* 0xBA ordmasculine */
    0x98,              /* 0xBB guillemotright */
    0x90, 0x32,        /* 0xBC onequarter */
    0x90, 0x66,        /* 0xBD onehalf */
    0xD8, 0x32,        /* 0xBE threequarters */
    0x41, 0x4E,        /* 0xBF questiondown */
    0x45, 0x7A,        /* 0xC0 Agrave */
    0x51, 0x7A,        /* 0xC1 Aacute */
    0xE1, 0x7A,        /* 0xC2 Acircumflex */
    0x79, 0x7A,        /* 0xC3 Atilde */
    0xAA, 0xFA,        /* 0xC4 Adieresis */
    0xDA, 0xFA,        /* 0xC5 Aring */
    0x7B, 0xEE,        /* 0xC6 AE */
    0x72, 0x32, 0x80,  /* 0xC7 Ccedilla */
    0x47, 0xEE,        /* 0xC8 Egrave */
    0x53, 0xEE,        /* 0xC9 Eacute */
    0xE3, 0xEE,        /* 0xCA Ecircumflex */
    0xA3, 0xEE,        /* 0xCB Edieresis */
    0x47, 0xAE,        /* 0xCC Igrave */
    0x53, 0xAE,        /* 0xCD Iacute */
    0xE3, 0xAE,        /* 0xCE Icircumflex */
    0xA3, 0xAE,        /* 0xCF Idieresis */
    0xD7, 0xDC,        /* 0xD0 Eth */
    0xCE, 0xFA,        /* 0xD1 Ntilde */
    0x47, 0xDE,        /* 0xD2 Ograve */
    0x53, 0xDE,        /* 0xD3 Oacute */
    0xE3, 0xDE,        /* 0xD4 Ocircumflex */
    0xCF, 0xDE,        /* 0xD5 Otilde */
    0xA3, 0xDE,        /* 0xD6 Odieresis */
    0xAA, 0x80,        /* 0xD7 multiply */
    0x77, 0xDC,        /* 0xD8 Oslash */
    0x8A, 0xDE,        /* 0xD9 Ugrave */
    0x2A, 0xDE,        /* 0xDA Uacute */
    0xE2, 0xDE,        /* 0xDB Ucircumflex */
    0xA2, 0xDE,        /* 0xDC Udieresis */
    0x2A, 0xF4,        /* 0xDD Yacute */
    0x9E, 0xF8,        /* 0xDE Thorn */
    0x77, 0x5D, 0x00,  /* 0xDF germandbls */
    0x45, 0xDE,        /* 0xE0 agrave */
    0x51, 0xDE,        /* 0xE1 aacute */
    0xE1, 0xDE,        /* 0xE2 acircumflex */
    0x79, 0xDE,        /* 0xE3 atilde */
    0xA1, 0xDE,        /* 0xE4 adieresis */
    0x6D, 0xDE,        /* 0xE5 aring */
    0x7F, 0xE0,        /* 0xE6 ae */
    0x71, 0x94,        /* 0xE7 ccedilla */
    0x45, 0xF6,        /* 0xE8 egrave */
    0x51, 0xF6,        /* 0xE9 eacute */
    0xE1, 0xF6,        /* 0xEA ecircumflex */
    0xA1, 0xF6,        /* 0xEB edieresis */
    0x9A, 0x80,        /* 0xEC igrave */
    0x65, 0x40,        /* 0xED iacute */
    0xE1, 0x24,        /* 0xEE icircumflex */
    0xA1, 0x24,        /* 0xEF idieresis */
    0x79, 0xD6,        /* 0xF0 eth */
    0xCF, 0x5A,        /* 0xF1 ntilde */
    0x45, 0x54,        /* 0xF2 ograve */
    0x51, 0x54,        /* 0xF3 oacute */
    0xE1, 0x54,        /* 0xF4 ocircumflex */
    0xCD, 0x54,        /* 0xF5 otilde */
    0xA1, 0x54,        /* 0xF6 odieresis */
    0x43, 0x84,        /* 0xF7 divide */
    0x7E, 0xE0,        /* 0xF8 oslash */
    0x8A, 0xD6,        /* 0xF9 ugrave */
    0x2A, 0xD6,        /* 0xFA uacute */
    0xE2, 0xD6,        /* 0xFB ucircumflex */
    0xA2, 0xD6,        /* 0xFC udieresis */
    0x2A, 0xB2, 0x80,  /* 0xFD yacute */
    0x9A, 0xE8,        /* 0xFE thorn */
    0xA2, 0xB2, 0x80,  /* 0xFF ydieresis */
    0x00,              /* 0x11D gcircumflex */
    0x7B, 0xE6,        /* 0x152 OE */
    0x7F, 0x70,        /* 0x153 oe */
    0xAF, 0x3C,        /* 0x160 Scaron */
    0xAF, 0x3C,        /* 0x161 scaron */
    0xA2, 0xA4,        /* 0x178 Ydieresis */
    0xBD, 0xEE,        /* 0x17D Zcaron */
    0xBD, 0xEE,        /* 0x17E zcaron */
    0x00,              /* 0xEA4 uni0EA4 */
    0x00,              /* 0x13A0 uni13A0 */
    0x80,              /* 0x2022 bullet */
    0xA0,              /* 0x2026 ellipsis */
    0x7F, 0xE6,        /* 0x20AC Euro */
    0xEA, 0xAA, 0xE0,  /* 0xFFFD uniFFFD */
#endif       /* (TOMTHUMB_USE_EXTENDED)  */
};

static const GFXglyph TomThumbPlusGlyphs[] PROGMEM = {
    {  0, 0, 0, 2, 0,   0 },  /* ' ' 0x20 */
    {  0, 1, 5, 2, 0, -5 },   /* '!' 0x21 */
    {  1, 3, 2, 4, 0, -5 },   /* '"' 0x22 */
    {  2, 4, 5, 5, 0, -5 },   /* '#' 0x23 */
    {  5, 4, 5, 5, 0, -5 },   /* '$' 0x24 */
    {  8, 3, 5, 4, 0, -5 },   /* '%' 0x25 */
    { 10, 4, 5, 5, 0, -5 },   /* '&' 0x26 */
    { 13, 1, 2, 2, 0, -5 },   /* ''' 0x27 */
    { 14, 2, 5, 3, 0, -5 },   /* '(' 0x28 */
    { 16, 2, 5, 3, 0, -5 },   /* ')' 0x29 */
    { 18, 3, 3, 4, 0, -4 },   /* '*' 0x2A */
    { 20, 3, 3, 4, 0, -4 },   /* '+' 0x2B */
    { 22, 2, 2, 3, 0, -2 },   /* ',' 0x2C */
    { 23, 3, 1, 4, 0, -3 },   /* '-' 0x2D */
    { 24, 1, 1, 2, 0, -1 },   /* '.' 0x2E */
    { 25, 3, 5, 4, 0, -5 },   /* '/' 0x2F */
    { 27, 3, 5, 4, 0, -5 },   /* '0' 0x30 */
    { 29, 2, 5, 3, 0, -5 },   /* '1' 0x31 */
    { 31, 3, 5, 4, 0, -5 },   /* '2' 0x32 */
    { 33, 3, 5, 4, 0, -5 },   /* '3' 0x33 */
    { 35, 3, 5, 4, 0, -5 },   /* '4' 0x34 */
    { 37, 3, 5, 4, 0, -5 },   /* '5' 0x35 */
    { 39, 3, 5, 4, 0, -5 },   /* '6' 0x36 */
    { 41, 3, 5, 4, 0, -5 },   /* '7' 0x37 */
    { 43, 3, 5, 4, 0, -5 },   /* '8' 0x38 */
    { 45, 3, 5, 4, 0, -5 },   /* '9' 0x39 */
    { 47, 1, 3, 2, 0, -4 },   /* ':' 0x3A */
    { 48, 2, 4, 3, 0, -4 },   /* ';' 0x3B */
    { 49, 3, 5, 4, 0, -5 },   /* '<' 0x3C */
    { 51, 3, 3, 4, 0, -4 },   /* '=' 0x3D */
    { 53, 3, 5, 4, 0, -5 },   /* '>' 0x3E */
    { 55, 3, 5, 4, 0, -5 },   /* '?' 0x3F */
    { 57, 4, 5, 5, 0, -5 },   /* '@' 0x40 */
    { 60, 3, 5, 4, 0, -5 },   /* 'A' 0x41 */
    { 62, 3, 5, 4, 0, -5 },   /* 'B' 0x42 */
    { 64, 3, 5, 4, 0, -5 },   /* 'C' 0x43 */
    { 66, 3, 5, 4, 0, -5 },   /* 'D' 0x44 */
    { 68, 3, 5, 4, 0, -5 },   /* 'E' 0x45 */
    { 70, 3, 5, 4, 0, -5 },   /* 'F' 0x46 */
    { 72, 3, 5, 4, 0, -5 },   /* 'G' 0x47 */
    { 74, 3, 5, 4, 0, -5 },   /* 'H' 0x48 */
    { 76, 1, 5, 3, 1, -5 },   /* 'I' 0x49 */
    { 77, 3, 5, 4, 0, -5 },   /* 'J' 0x4A */
    { 79, 3, 5, 4, 0, -5 },   /* 'K' 0x4B */
    { 81, 3, 5, 4, 0, -5 },   /* 'L' 0x4C */
    { 83, 4, 5, 5, 0, -5 },   /* 'M' 0x4D */
    { 86, 4, 5, 5, 0, -5 },   /* 'N' 0x4E */
    { 89, 3, 5, 4, 0, -5 },   /* 'O' 0x4F */
    { 91, 3, 5, 4, 0, -5 },   /* 'P' 0x50 */
    { 93, 3, 5, 4, 0, -5 },   /* 'Q' 0x51 */
    { 95, 3, 5, 4, 0, -5 },   /* 'R' 0x52 */
    { 97, 3, 5, 4, 0, -5 },   /* 'S' 0x53 */
    { 99, 3, 5, 4, 0, -5 },   /* 'T' 0x54 */
    {101, 3, 5, 4, 0, -5 },   /* 'U' 0x55 */
    {103, 3, 5, 4, 0, -5 },   /* 'V' 0x56 */
    {105, 3, 5, 4, 0, -5 },   /* 'W' 0x57 */
    {107, 3, 5, 4, 0, -5 },   /* 'X' 0x58 */
    {109, 3, 5, 4, 0, -5 },   /* 'Y' 0x59 */
    {111, 3, 5, 4, 0, -5 },   /* 'Z' 0x5A */
    {113, 2, 5, 3, 0, -5 },   /* '[' 0x5B */
    {115, 3, 3, 4, 0, -4 },   /* '\' 0x5C */
    {117, 2, 5, 3, 0, -5 },   /* ']' 0x5D */
    {119, 3, 2, 4, 0, -5 },   /* '^' 0x5E */
    {120, 3, 1, 4, 0, -1 },   /* '_' 0x5F */
    {121, 2, 2, 3, 0, -5 },   /* '`' 0x60 */
    {122, 3, 4, 4, 0, -4 },   /* 'a' 0x61 */
    {124, 3, 5, 4, 0, -5 },   /* 'b' 0x62 */
    {126, 3, 4, 4, 0, -4 },   /* 'c' 0x63 */
    {128, 3, 5, 4, 0, -5 },   /* 'd' 0x64 */
    {130, 3, 4, 4, 0, -4 },   /* 'e' 0x65 */
    {132, 3, 5, 4, 0, -5 },   /* 'f' 0x66 */
    {134, 3, 5, 4, 0, -4 },   /* 'g' 0x67 */
    {136, 3, 5, 4, 0, -5 },   /* 'h' 0x68 */
    {138, 1, 5, 2, 0, -5 },   /* 'i' 0x69 */
    {139, 3, 6, 4, 0, -5 },   /* 'j' 0x6A */
    {142, 3, 5, 4, 0, -5 },   /* 'k' 0x6B */
    {144, 3, 5, 4, 0, -5 },   /* 'l' 0x6C */
    {146, 3, 4, 4, 0, -4 },   /* 'm' 0x6D */
    {148, 3, 4, 4, 0, -4 },   /* 'n' 0x6E */
    {150, 3, 4, 4, 0, -4 },   /* 'o' 0x6F */
    {152, 3, 5, 4, 0, -4 },   /* 'p' 0x70 */
    {154, 3, 5, 4, 0, -4 },   /* 'q' 0x71 */
    {156, 3, 4, 4, 0, -4 },   /* 'r' 0x72 */
    {158, 3, 4, 4, 0, -4 },   /* 's' 0x73 */
    {160, 3, 5, 4, 0, -5 },   /* 't' 0x74 */
    {162, 3, 4, 4, 0, -4 },   /* 'u' 0x75 */
    {164, 3, 4, 4, 0, -4 },   /* 'v' 0x76 */
    {166, 3, 4, 4, 0, -4 },   /* 'w' 0x77 */
    {168, 3, 4, 4, 0, -4 },   /* 'x' 0x78 */
    {170, 3, 5, 4, 0, -4 },   /* 'y' 0x79 */
    {172, 3, 4, 4, 0, -4 },   /* 'z' 0x7A */
    {174, 3, 5, 4, 0, -5 },   /* '{' 0x7B */
    {176, 1, 5, 2, 0, -5 },   /* '|' 0x7C */
    {177, 3, 5, 4, 0, -5 },   /* '}' 0x7D */
    {179, 3, 2, 4, 0, -5 },   /* '~' 0x7E */
#if (TOMTHUMB_USE_EXTENDED)
    {180, 1, 5, 2, 0, -5 },   /* 0xA1 exclamdown */
    {181, 3, 5, 4, 0, -5 },   /* 0xA2 cent */
    {183, 3, 5, 4, 0, -5 },   /* 0xA3 sterling */
    {185, 3, 5, 4, 0, -5 },   /* 0xA4 currency */
    {187, 3, 5, 4, 0, -5 },   /* 0xA5 yen */
    {189, 1, 5, 2, 0, -5 },   /* 0xA6 brokenbar */
    {190, 3, 5, 4, 0, -5 },   /* 0xA7 section */
    {192, 3, 1, 4, 0, -5 },   /* 0xA8 dieresis */
    {193, 3, 3, 4, 0, -5 },   /* 0xA9 copyright */
    {195, 3, 5, 4, 0, -5 },   /* 0xAA ordfeminine */
    {197, 2, 3, 3, 0, -5 },   /* 0xAB guillemotleft */
    {198, 3, 2, 4, 0, -4 },   /* 0xAC logicalnot */
    {199, 2, 1, 3, 0, -3 },   /* 0xAD softhyphen */
    {200, 3, 3, 4, 0, -5 },   /* 0xAE registered */
    {202, 3, 1, 4, 0, -5 },   /* 0xAF macron */
    {203, 3, 3, 4, 0, -5 },   /* 0xB0 degree */
    {205, 3, 5, 4, 0, -5 },   /* 0xB1 plusminus */
    {207, 3, 3, 4, 0, -5 },   /* 0xB2 twosuperior */
    {209, 3, 3, 4, 0, -5 },   /* 0xB3 threesuperior */
    {211, 2, 2, 3, 0, -5 },   /* 0xB4 acute */
    {212, 3, 5, 4, 0, -5 },   /* 0xB5 mu */
    {214, 3, 5, 4, 0, -5 },   /* 0xB6 paragraph */
    {216, 3, 3, 4, 0, -4 },   /* 0xB7 periodcentered */
    {218, 3, 3, 4, 0, -3 },   /* 0xB8 cedilla */
    {220, 1, 3, 2, 0, -5 },   /* 0xB9 onesuperior */
    {221, 3, 5, 4, 0, -5 },   /* 0xBA ordmasculine */
    {223, 2, 3, 3, 0, -5 },   /* 0xBB guillemotright */
    {224, 3, 5, 4, 0, -5 },   /* 0xBC onequarter */
    {226, 3, 5, 4, 0, -5 },   /* 0xBD onehalf */
    {228, 3, 5, 4, 0, -5 },   /* 0xBE threequarters */
    {230, 3, 5, 4, 0, -5 },   /* 0xBF questiondown */
    {232, 3, 5, 4, 0, -5 },   /* 0xC0 Agrave */
    {234, 3, 5, 4, 0, -5 },   /* 0xC1 Aacute */
    {236, 3, 5, 4, 0, -5 },   /* 0xC2 Acircumflex */
    {238, 3, 5, 4, 0, -5 },   /* 0xC3 Atilde */
    {240, 3, 5, 4, 0, -5 },   /* 0xC4 Adieresis */
    {242, 3, 5, 4, 0, -5 },   /* 0xC5 Aring */
    {244, 3, 5, 4, 0, -5 },   /* 0xC6 AE */
    {246, 3, 6, 4, 0, -5 },   /* 0xC7 Ccedilla */
    {249, 3, 5, 4, 0, -5 },   /* 0xC8 Egrave */
    {251, 3, 5, 4, 0, -5 },   /* 0xC9 Eacute */
    {253, 3, 5, 4, 0, -5 },   /* 0xCA Ecircumflex */
    {255, 3, 5, 4, 0, -5 },   /* 0xCB Edieresis */
    {257, 3, 5, 4, 0, -5 },   /* 0xCC Igrave */
    {259, 3, 5, 4, 0, -5 },   /* 0xCD Iacute */
    {261, 3, 5, 4, 0, -5 },   /* 0xCE Icircumflex */
    {263, 3, 5, 4, 0, -5 },   /* 0xCF Idieresis */
    {265, 3, 5, 4, 0, -5 },   /* 0xD0 Eth */
    {267, 3, 5, 4, 0, -5 },   /* 0xD1 Ntilde */
    {269, 3, 5, 4, 0, -5 },   /* 0xD2 Ograve */
    {271, 3, 5, 4, 0, -5 },   /* 0xD3 Oacute */
    {273, 3, 5, 4, 0, -5 },   /* 0xD4 Ocircumflex */
    {275, 3, 5, 4, 0, -5 },   /* 0xD5 Otilde */
    {277, 3, 5, 4, 0, -5 },   /* 0xD6 Odieresis */
    {279, 3, 3, 4, 0, -4 },   /* 0xD7 multiply */
    {281, 3, 5, 4, 0, -5 },   /* 0xD8 Oslash */
    {283, 3, 5, 4, 0, -5 },   /* 0xD9 Ugrave */
    {285, 3, 5, 4, 0, -5 },   /* 0xDA Uacute */
    {287, 3, 5, 4, 0, -5 },   /* 0xDB Ucircumflex */
    {289, 3, 5, 4, 0, -5 },   /* 0xDC Udieresis */
    {291, 3, 5, 4, 0, -5 },   /* 0xDD Yacute */
    {293, 3, 5, 4, 0, -5 },   /* 0xDE Thorn */
    {295, 3, 6, 4, 0, -5 },   /* 0xDF germandbls */
    {298, 3, 5, 4, 0, -5 },   /* 0xE0 agrave */
    {300, 3, 5, 4, 0, -5 },   /* 0xE1 aacute */
    {302, 3, 5, 4, 0, -5 },   /* 0xE2 acircumflex */
    {304, 3, 5, 4, 0, -5 },   /* 0xE3 atilde */
    {306, 3, 5, 4, 0, -5 },   /* 0xE4 adieresis */
    {308, 3, 5, 4, 0, -5 },   /* 0xE5 aring */
    {310, 3, 4, 4, 0, -4 },   /* 0xE6 ae */
    {312, 3, 5, 4, 0, -4 },   /* 0xE7 ccedilla */
    {314, 3, 5, 4, 0, -5 },   /* 0xE8 egrave */
    {316, 3, 5, 4, 0, -5 },   /* 0xE9 eacute */
    {318, 3, 5, 4, 0, -5 },   /* 0xEA ecircumflex */
    {320, 3, 5, 4, 0, -5 },   /* 0xEB edieresis */
    {322, 3, 5, 4, 0, -5 },   /* 0xEC igrave */
    {324, 3, 5, 4, 0, -5 },   /* 0xED iacute */
    {326, 3, 5, 4, 0, -5 },   /* 0xEE icircumflex */
    {328, 3, 5, 4, 0, -5 },   /* 0xEF idieresis */
    {330, 3, 5, 4, 0, -5 },   /* 0xF0 eth */
    {332, 3, 5, 4, 0, -5 },   /* 0xF1 ntilde */
    {334, 3, 5, 4, 0, -5 },   /* 0xF2 ograve */
    {336, 3, 5, 4, 0, -5 },   /* 0xF3 oacute */
    {338, 3, 5, 4, 0, -5 },   /* 0xF4 ocircumflex */
    {340, 3, 5, 4, 0, -5 },   /* 0xF5 otilde */
    {342, 3, 5, 4, 0, -5 },   /* 0xF6 odieresis */
    {344, 3, 3, 4, 0, -4 },   /* 0xF7 divide */
    {346, 3, 5, 4, 0, -5 },   /* 0xF8 oslash */
    {348, 3, 5, 4, 0, -5 },   /* 0xF9 ugrave */
    {350, 3, 5, 4, 0, -5 },   /* 0xFA uacute */
    {352, 3, 5, 4, 0, -5 },   /* 0xFB ucircumflex */
    {354, 3, 5, 4, 0, -5 },   /* 0xFC udieresis */
    {356, 3, 5, 4, 0, -5 },   /* 0xFD yacute */
    {359, 3, 5, 4, 0, -4 },   /* 0xFE thorn */
    {361, 3, 6, 4, 0, -5 },   /* 0xFF ydieresis */
    {364, 1, 1, 2, 0, -1 },   /* 0x11D gcircumflex */
    {365, 3, 5, 4, 0, -5 },   /* 0x152 OE */
    {367, 3, 4, 4, 0, -4 },   /* 0x153 oe */
    {369, 3, 5, 4, 0, -5 },   /* 0x160 Scaron */
    {371, 3, 5, 4, 0, -5 },   /* 0x161 scaron */
    {373, 3, 5, 4, 0, -5 },   /* 0x178 Ydieresis */
    {375, 3, 5, 4, 0, -5 },   /* 0x17D Zcaron */
    {377, 3, 5, 4, 0, -5 },   /* 0x17E zcaron */
    {379, 1, 1, 2, 0, -1 },   /* 0xEA4 uni0EA4 */
    {380, 1, 1, 2, 0, -1 },   /* 0x13A0 uni13A0 */
    {381, 1, 1, 2, 0, -3 },   /* 0x2022 bullet */
    {382, 3, 1, 4, 0, -1 },   /* 0x2026 ellipsis */
    {383, 3, 5, 4, 0, -5 },   /* 0x20AC Euro */
    {385, 4, 5, 5, 0, -5 },   /* 0xFFFD uniFFFD */
#endif                     /* (TOMTHUMB_USE_EXTENDED) */
};

static const GFXfont TomThumbPlus  = {(uint8_t *)TomThumbPlusBitmaps,
    (GFXglyph *)TomThumbPlusGlyphs,
    0x20,
    0x7E,
    6};
