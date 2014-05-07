/***************************************************************************

    rgbutil.c

    Utility definitions for RGB manipulation. Allows RGB handling to be
    performed in an abstracted fashion and optimized with SIMD.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#include "emu.h"
#include "rgbutil.h"


/***************************************************************************
    SSE TABLES
***************************************************************************/

#if !defined(MAME_DEBUG) && (defined(__SSE2__) || defined(_MSC_VER)) && defined(PTR64)
const struct _rgbsse_statics rgbsse_statics =
{
	{ 0 },
	{ 255, 255, 255, 255, 255, 255, 255, 255 },
	{
		{   0, 256,   0, 256,   0, 256,   0, 256 }, {   1, 255,   1, 255,   1, 255,   1, 255 },
		{   2, 254,   2, 254,   2, 254,   2, 254 }, {   3, 253,   3, 253,   3, 253,   3, 253 },
		{   4, 252,   4, 252,   4, 252,   4, 252 }, {   5, 251,   5, 251,   5, 251,   5, 251 },
		{   6, 250,   6, 250,   6, 250,   6, 250 }, {   7, 249,   7, 249,   7, 249,   7, 249 },
		{   8, 248,   8, 248,   8, 248,   8, 248 }, {   9, 247,   9, 247,   9, 247,   9, 247 },
		{  10, 246,  10, 246,  10, 246,  10, 246 }, {  11, 245,  11, 245,  11, 245,  11, 245 },
		{  12, 244,  12, 244,  12, 244,  12, 244 }, {  13, 243,  13, 243,  13, 243,  13, 243 },
		{  14, 242,  14, 242,  14, 242,  14, 242 }, {  15, 241,  15, 241,  15, 241,  15, 241 },
		{  16, 240,  16, 240,  16, 240,  16, 240 }, {  17, 239,  17, 239,  17, 239,  17, 239 },
		{  18, 238,  18, 238,  18, 238,  18, 238 }, {  19, 237,  19, 237,  19, 237,  19, 237 },
		{  20, 236,  20, 236,  20, 236,  20, 236 }, {  21, 235,  21, 235,  21, 235,  21, 235 },
		{  22, 234,  22, 234,  22, 234,  22, 234 }, {  23, 233,  23, 233,  23, 233,  23, 233 },
		{  24, 232,  24, 232,  24, 232,  24, 232 }, {  25, 231,  25, 231,  25, 231,  25, 231 },
		{  26, 230,  26, 230,  26, 230,  26, 230 }, {  27, 229,  27, 229,  27, 229,  27, 229 },
		{  28, 228,  28, 228,  28, 228,  28, 228 }, {  29, 227,  29, 227,  29, 227,  29, 227 },
		{  30, 226,  30, 226,  30, 226,  30, 226 }, {  31, 225,  31, 225,  31, 225,  31, 225 },
		{  32, 224,  32, 224,  32, 224,  32, 224 }, {  33, 223,  33, 223,  33, 223,  33, 223 },
		{  34, 222,  34, 222,  34, 222,  34, 222 }, {  35, 221,  35, 221,  35, 221,  35, 221 },
		{  36, 220,  36, 220,  36, 220,  36, 220 }, {  37, 219,  37, 219,  37, 219,  37, 219 },
		{  38, 218,  38, 218,  38, 218,  38, 218 }, {  39, 217,  39, 217,  39, 217,  39, 217 },
		{  40, 216,  40, 216,  40, 216,  40, 216 }, {  41, 215,  41, 215,  41, 215,  41, 215 },
		{  42, 214,  42, 214,  42, 214,  42, 214 }, {  43, 213,  43, 213,  43, 213,  43, 213 },
		{  44, 212,  44, 212,  44, 212,  44, 212 }, {  45, 211,  45, 211,  45, 211,  45, 211 },
		{  46, 210,  46, 210,  46, 210,  46, 210 }, {  47, 209,  47, 209,  47, 209,  47, 209 },
		{  48, 208,  48, 208,  48, 208,  48, 208 }, {  49, 207,  49, 207,  49, 207,  49, 207 },
		{  50, 206,  50, 206,  50, 206,  50, 206 }, {  51, 205,  51, 205,  51, 205,  51, 205 },
		{  52, 204,  52, 204,  52, 204,  52, 204 }, {  53, 203,  53, 203,  53, 203,  53, 203 },
		{  54, 202,  54, 202,  54, 202,  54, 202 }, {  55, 201,  55, 201,  55, 201,  55, 201 },
		{  56, 200,  56, 200,  56, 200,  56, 200 }, {  57, 199,  57, 199,  57, 199,  57, 199 },
		{  58, 198,  58, 198,  58, 198,  58, 198 }, {  59, 197,  59, 197,  59, 197,  59, 197 },
		{  60, 196,  60, 196,  60, 196,  60, 196 }, {  61, 195,  61, 195,  61, 195,  61, 195 },
		{  62, 194,  62, 194,  62, 194,  62, 194 }, {  63, 193,  63, 193,  63, 193,  63, 193 },
		{  64, 192,  64, 192,  64, 192,  64, 192 }, {  65, 191,  65, 191,  65, 191,  65, 191 },
		{  66, 190,  66, 190,  66, 190,  66, 190 }, {  67, 189,  67, 189,  67, 189,  67, 189 },
		{  68, 188,  68, 188,  68, 188,  68, 188 }, {  69, 187,  69, 187,  69, 187,  69, 187 },
		{  70, 186,  70, 186,  70, 186,  70, 186 }, {  71, 185,  71, 185,  71, 185,  71, 185 },
		{  72, 184,  72, 184,  72, 184,  72, 184 }, {  73, 183,  73, 183,  73, 183,  73, 183 },
		{  74, 182,  74, 182,  74, 182,  74, 182 }, {  75, 181,  75, 181,  75, 181,  75, 181 },
		{  76, 180,  76, 180,  76, 180,  76, 180 }, {  77, 179,  77, 179,  77, 179,  77, 179 },
		{  78, 178,  78, 178,  78, 178,  78, 178 }, {  79, 177,  79, 177,  79, 177,  79, 177 },
		{  80, 176,  80, 176,  80, 176,  80, 176 }, {  81, 175,  81, 175,  81, 175,  81, 175 },
		{  82, 174,  82, 174,  82, 174,  82, 174 }, {  83, 173,  83, 173,  83, 173,  83, 173 },
		{  84, 172,  84, 172,  84, 172,  84, 172 }, {  85, 171,  85, 171,  85, 171,  85, 171 },
		{  86, 170,  86, 170,  86, 170,  86, 170 }, {  87, 169,  87, 169,  87, 169,  87, 169 },
		{  88, 168,  88, 168,  88, 168,  88, 168 }, {  89, 167,  89, 167,  89, 167,  89, 167 },
		{  90, 166,  90, 166,  90, 166,  90, 166 }, {  91, 165,  91, 165,  91, 165,  91, 165 },
		{  92, 164,  92, 164,  92, 164,  92, 164 }, {  93, 163,  93, 163,  93, 163,  93, 163 },
		{  94, 162,  94, 162,  94, 162,  94, 162 }, {  95, 161,  95, 161,  95, 161,  95, 161 },
		{  96, 160,  96, 160,  96, 160,  96, 160 }, {  97, 159,  97, 159,  97, 159,  97, 159 },
		{  98, 158,  98, 158,  98, 158,  98, 158 }, {  99, 157,  99, 157,  99, 157,  99, 157 },
		{ 100, 156, 100, 156, 100, 156, 100, 156 }, { 101, 155, 101, 155, 101, 155, 101, 155 },
		{ 102, 154, 102, 154, 102, 154, 102, 154 }, { 103, 153, 103, 153, 103, 153, 103, 153 },
		{ 104, 152, 104, 152, 104, 152, 104, 152 }, { 105, 151, 105, 151, 105, 151, 105, 151 },
		{ 106, 150, 106, 150, 106, 150, 106, 150 }, { 107, 149, 107, 149, 107, 149, 107, 149 },
		{ 108, 148, 108, 148, 108, 148, 108, 148 }, { 109, 147, 109, 147, 109, 147, 109, 147 },
		{ 110, 146, 110, 146, 110, 146, 110, 146 }, { 111, 145, 111, 145, 111, 145, 111, 145 },
		{ 112, 144, 112, 144, 112, 144, 112, 144 }, { 113, 143, 113, 143, 113, 143, 113, 143 },
		{ 114, 142, 114, 142, 114, 142, 114, 142 }, { 115, 141, 115, 141, 115, 141, 115, 141 },
		{ 116, 140, 116, 140, 116, 140, 116, 140 }, { 117, 139, 117, 139, 117, 139, 117, 139 },
		{ 118, 138, 118, 138, 118, 138, 118, 138 }, { 119, 137, 119, 137, 119, 137, 119, 137 },
		{ 120, 136, 120, 136, 120, 136, 120, 136 }, { 121, 135, 121, 135, 121, 135, 121, 135 },
		{ 122, 134, 122, 134, 122, 134, 122, 134 }, { 123, 133, 123, 133, 123, 133, 123, 133 },
		{ 124, 132, 124, 132, 124, 132, 124, 132 }, { 125, 131, 125, 131, 125, 131, 125, 131 },
		{ 126, 130, 126, 130, 126, 130, 126, 130 }, { 127, 129, 127, 129, 127, 129, 127, 129 },
		{ 128, 128, 128, 128, 128, 128, 128, 128 }, { 129, 127, 129, 127, 129, 127, 129, 127 },
		{ 130, 126, 130, 126, 130, 126, 130, 126 }, { 131, 125, 131, 125, 131, 125, 131, 125 },
		{ 132, 124, 132, 124, 132, 124, 132, 124 }, { 133, 123, 133, 123, 133, 123, 133, 123 },
		{ 134, 122, 134, 122, 134, 122, 134, 122 }, { 135, 121, 135, 121, 135, 121, 135, 121 },
		{ 136, 120, 136, 120, 136, 120, 136, 120 }, { 137, 119, 137, 119, 137, 119, 137, 119 },
		{ 138, 118, 138, 118, 138, 118, 138, 118 }, { 139, 117, 139, 117, 139, 117, 139, 117 },
		{ 140, 116, 140, 116, 140, 116, 140, 116 }, { 141, 115, 141, 115, 141, 115, 141, 115 },
		{ 142, 114, 142, 114, 142, 114, 142, 114 }, { 143, 113, 143, 113, 143, 113, 143, 113 },
		{ 144, 112, 144, 112, 144, 112, 144, 112 }, { 145, 111, 145, 111, 145, 111, 145, 111 },
		{ 146, 110, 146, 110, 146, 110, 146, 110 }, { 147, 109, 147, 109, 147, 109, 147, 109 },
		{ 148, 108, 148, 108, 148, 108, 148, 108 }, { 149, 107, 149, 107, 149, 107, 149, 107 },
		{ 150, 106, 150, 106, 150, 106, 150, 106 }, { 151, 105, 151, 105, 151, 105, 151, 105 },
		{ 152, 104, 152, 104, 152, 104, 152, 104 }, { 153, 103, 153, 103, 153, 103, 153, 103 },
		{ 154, 102, 154, 102, 154, 102, 154, 102 }, { 155, 101, 155, 101, 155, 101, 155, 101 },
		{ 156, 100, 156, 100, 156, 100, 156, 100 }, { 157,  99, 157,  99, 157,  99, 157,  99 },
		{ 158,  98, 158,  98, 158,  98, 158,  98 }, { 159,  97, 159,  97, 159,  97, 159,  97 },
		{ 160,  96, 160,  96, 160,  96, 160,  96 }, { 161,  95, 161,  95, 161,  95, 161,  95 },
		{ 162,  94, 162,  94, 162,  94, 162,  94 }, { 163,  93, 163,  93, 163,  93, 163,  93 },
		{ 164,  92, 164,  92, 164,  92, 164,  92 }, { 165,  91, 165,  91, 165,  91, 165,  91 },
		{ 166,  90, 166,  90, 166,  90, 166,  90 }, { 167,  89, 167,  89, 167,  89, 167,  89 },
		{ 168,  88, 168,  88, 168,  88, 168,  88 }, { 169,  87, 169,  87, 169,  87, 169,  87 },
		{ 170,  86, 170,  86, 170,  86, 170,  86 }, { 171,  85, 171,  85, 171,  85, 171,  85 },
		{ 172,  84, 172,  84, 172,  84, 172,  84 }, { 173,  83, 173,  83, 173,  83, 173,  83 },
		{ 174,  82, 174,  82, 174,  82, 174,  82 }, { 175,  81, 175,  81, 175,  81, 175,  81 },
		{ 176,  80, 176,  80, 176,  80, 176,  80 }, { 177,  79, 177,  79, 177,  79, 177,  79 },
		{ 178,  78, 178,  78, 178,  78, 178,  78 }, { 179,  77, 179,  77, 179,  77, 179,  77 },
		{ 180,  76, 180,  76, 180,  76, 180,  76 }, { 181,  75, 181,  75, 181,  75, 181,  75 },
		{ 182,  74, 182,  74, 182,  74, 182,  74 }, { 183,  73, 183,  73, 183,  73, 183,  73 },
		{ 184,  72, 184,  72, 184,  72, 184,  72 }, { 185,  71, 185,  71, 185,  71, 185,  71 },
		{ 186,  70, 186,  70, 186,  70, 186,  70 }, { 187,  69, 187,  69, 187,  69, 187,  69 },
		{ 188,  68, 188,  68, 188,  68, 188,  68 }, { 189,  67, 189,  67, 189,  67, 189,  67 },
		{ 190,  66, 190,  66, 190,  66, 190,  66 }, { 191,  65, 191,  65, 191,  65, 191,  65 },
		{ 192,  64, 192,  64, 192,  64, 192,  64 }, { 193,  63, 193,  63, 193,  63, 193,  63 },
		{ 194,  62, 194,  62, 194,  62, 194,  62 }, { 195,  61, 195,  61, 195,  61, 195,  61 },
		{ 196,  60, 196,  60, 196,  60, 196,  60 }, { 197,  59, 197,  59, 197,  59, 197,  59 },
		{ 198,  58, 198,  58, 198,  58, 198,  58 }, { 199,  57, 199,  57, 199,  57, 199,  57 },
		{ 200,  56, 200,  56, 200,  56, 200,  56 }, { 201,  55, 201,  55, 201,  55, 201,  55 },
		{ 202,  54, 202,  54, 202,  54, 202,  54 }, { 203,  53, 203,  53, 203,  53, 203,  53 },
		{ 204,  52, 204,  52, 204,  52, 204,  52 }, { 205,  51, 205,  51, 205,  51, 205,  51 },
		{ 206,  50, 206,  50, 206,  50, 206,  50 }, { 207,  49, 207,  49, 207,  49, 207,  49 },
		{ 208,  48, 208,  48, 208,  48, 208,  48 }, { 209,  47, 209,  47, 209,  47, 209,  47 },
		{ 210,  46, 210,  46, 210,  46, 210,  46 }, { 211,  45, 211,  45, 211,  45, 211,  45 },
		{ 212,  44, 212,  44, 212,  44, 212,  44 }, { 213,  43, 213,  43, 213,  43, 213,  43 },
		{ 214,  42, 214,  42, 214,  42, 214,  42 }, { 215,  41, 215,  41, 215,  41, 215,  41 },
		{ 216,  40, 216,  40, 216,  40, 216,  40 }, { 217,  39, 217,  39, 217,  39, 217,  39 },
		{ 218,  38, 218,  38, 218,  38, 218,  38 }, { 219,  37, 219,  37, 219,  37, 219,  37 },
		{ 220,  36, 220,  36, 220,  36, 220,  36 }, { 221,  35, 221,  35, 221,  35, 221,  35 },
		{ 222,  34, 222,  34, 222,  34, 222,  34 }, { 223,  33, 223,  33, 223,  33, 223,  33 },
		{ 224,  32, 224,  32, 224,  32, 224,  32 }, { 225,  31, 225,  31, 225,  31, 225,  31 },
		{ 226,  30, 226,  30, 226,  30, 226,  30 }, { 227,  29, 227,  29, 227,  29, 227,  29 },
		{ 228,  28, 228,  28, 228,  28, 228,  28 }, { 229,  27, 229,  27, 229,  27, 229,  27 },
		{ 230,  26, 230,  26, 230,  26, 230,  26 }, { 231,  25, 231,  25, 231,  25, 231,  25 },
		{ 232,  24, 232,  24, 232,  24, 232,  24 }, { 233,  23, 233,  23, 233,  23, 233,  23 },
		{ 234,  22, 234,  22, 234,  22, 234,  22 }, { 235,  21, 235,  21, 235,  21, 235,  21 },
		{ 236,  20, 236,  20, 236,  20, 236,  20 }, { 237,  19, 237,  19, 237,  19, 237,  19 },
		{ 238,  18, 238,  18, 238,  18, 238,  18 }, { 239,  17, 239,  17, 239,  17, 239,  17 },
		{ 240,  16, 240,  16, 240,  16, 240,  16 }, { 241,  15, 241,  15, 241,  15, 241,  15 },
		{ 242,  14, 242,  14, 242,  14, 242,  14 }, { 243,  13, 243,  13, 243,  13, 243,  13 },
		{ 244,  12, 244,  12, 244,  12, 244,  12 }, { 245,  11, 245,  11, 245,  11, 245,  11 },
		{ 246,  10, 246,  10, 246,  10, 246,  10 }, { 247,   9, 247,   9, 247,   9, 247,   9 },
		{ 248,   8, 248,   8, 248,   8, 248,   8 }, { 249,   7, 249,   7, 249,   7, 249,   7 },
		{ 250,   6, 250,   6, 250,   6, 250,   6 }, { 251,   5, 251,   5, 251,   5, 251,   5 },
		{ 252,   4, 252,   4, 252,   4, 252,   4 }, { 253,   3, 253,   3, 253,   3, 253,   3 },
		{ 254,   2, 254,   2, 254,   2, 254,   2 }, { 255,   1, 255,   1, 255,   1, 255,   1 }
	}
};
#endif // defined(__SSE2__)



/***************************************************************************
    VMX/ALTIVEC TABLES
***************************************************************************/

#if defined(__ALTIVEC__)
#include <altivec.h>
const struct _rgbvmx_statics rgbvmx_statics =
{
	{ 255, 255, 255, 255, 255, 255, 255, 255 },
	{
		{   0, 256,   0, 256,   0, 256,   0, 256 }, {   1, 255,   1, 255,   1, 255,   1, 255 },
		{   2, 254,   2, 254,   2, 254,   2, 254 }, {   3, 253,   3, 253,   3, 253,   3, 253 },
		{   4, 252,   4, 252,   4, 252,   4, 252 }, {   5, 251,   5, 251,   5, 251,   5, 251 },
		{   6, 250,   6, 250,   6, 250,   6, 250 }, {   7, 249,   7, 249,   7, 249,   7, 249 },
		{   8, 248,   8, 248,   8, 248,   8, 248 }, {   9, 247,   9, 247,   9, 247,   9, 247 },
		{  10, 246,  10, 246,  10, 246,  10, 246 }, {  11, 245,  11, 245,  11, 245,  11, 245 },
		{  12, 244,  12, 244,  12, 244,  12, 244 }, {  13, 243,  13, 243,  13, 243,  13, 243 },
		{  14, 242,  14, 242,  14, 242,  14, 242 }, {  15, 241,  15, 241,  15, 241,  15, 241 },
		{  16, 240,  16, 240,  16, 240,  16, 240 }, {  17, 239,  17, 239,  17, 239,  17, 239 },
		{  18, 238,  18, 238,  18, 238,  18, 238 }, {  19, 237,  19, 237,  19, 237,  19, 237 },
		{  20, 236,  20, 236,  20, 236,  20, 236 }, {  21, 235,  21, 235,  21, 235,  21, 235 },
		{  22, 234,  22, 234,  22, 234,  22, 234 }, {  23, 233,  23, 233,  23, 233,  23, 233 },
		{  24, 232,  24, 232,  24, 232,  24, 232 }, {  25, 231,  25, 231,  25, 231,  25, 231 },
		{  26, 230,  26, 230,  26, 230,  26, 230 }, {  27, 229,  27, 229,  27, 229,  27, 229 },
		{  28, 228,  28, 228,  28, 228,  28, 228 }, {  29, 227,  29, 227,  29, 227,  29, 227 },
		{  30, 226,  30, 226,  30, 226,  30, 226 }, {  31, 225,  31, 225,  31, 225,  31, 225 },
		{  32, 224,  32, 224,  32, 224,  32, 224 }, {  33, 223,  33, 223,  33, 223,  33, 223 },
		{  34, 222,  34, 222,  34, 222,  34, 222 }, {  35, 221,  35, 221,  35, 221,  35, 221 },
		{  36, 220,  36, 220,  36, 220,  36, 220 }, {  37, 219,  37, 219,  37, 219,  37, 219 },
		{  38, 218,  38, 218,  38, 218,  38, 218 }, {  39, 217,  39, 217,  39, 217,  39, 217 },
		{  40, 216,  40, 216,  40, 216,  40, 216 }, {  41, 215,  41, 215,  41, 215,  41, 215 },
		{  42, 214,  42, 214,  42, 214,  42, 214 }, {  43, 213,  43, 213,  43, 213,  43, 213 },
		{  44, 212,  44, 212,  44, 212,  44, 212 }, {  45, 211,  45, 211,  45, 211,  45, 211 },
		{  46, 210,  46, 210,  46, 210,  46, 210 }, {  47, 209,  47, 209,  47, 209,  47, 209 },
		{  48, 208,  48, 208,  48, 208,  48, 208 }, {  49, 207,  49, 207,  49, 207,  49, 207 },
		{  50, 206,  50, 206,  50, 206,  50, 206 }, {  51, 205,  51, 205,  51, 205,  51, 205 },
		{  52, 204,  52, 204,  52, 204,  52, 204 }, {  53, 203,  53, 203,  53, 203,  53, 203 },
		{  54, 202,  54, 202,  54, 202,  54, 202 }, {  55, 201,  55, 201,  55, 201,  55, 201 },
		{  56, 200,  56, 200,  56, 200,  56, 200 }, {  57, 199,  57, 199,  57, 199,  57, 199 },
		{  58, 198,  58, 198,  58, 198,  58, 198 }, {  59, 197,  59, 197,  59, 197,  59, 197 },
		{  60, 196,  60, 196,  60, 196,  60, 196 }, {  61, 195,  61, 195,  61, 195,  61, 195 },
		{  62, 194,  62, 194,  62, 194,  62, 194 }, {  63, 193,  63, 193,  63, 193,  63, 193 },
		{  64, 192,  64, 192,  64, 192,  64, 192 }, {  65, 191,  65, 191,  65, 191,  65, 191 },
		{  66, 190,  66, 190,  66, 190,  66, 190 }, {  67, 189,  67, 189,  67, 189,  67, 189 },
		{  68, 188,  68, 188,  68, 188,  68, 188 }, {  69, 187,  69, 187,  69, 187,  69, 187 },
		{  70, 186,  70, 186,  70, 186,  70, 186 }, {  71, 185,  71, 185,  71, 185,  71, 185 },
		{  72, 184,  72, 184,  72, 184,  72, 184 }, {  73, 183,  73, 183,  73, 183,  73, 183 },
		{  74, 182,  74, 182,  74, 182,  74, 182 }, {  75, 181,  75, 181,  75, 181,  75, 181 },
		{  76, 180,  76, 180,  76, 180,  76, 180 }, {  77, 179,  77, 179,  77, 179,  77, 179 },
		{  78, 178,  78, 178,  78, 178,  78, 178 }, {  79, 177,  79, 177,  79, 177,  79, 177 },
		{  80, 176,  80, 176,  80, 176,  80, 176 }, {  81, 175,  81, 175,  81, 175,  81, 175 },
		{  82, 174,  82, 174,  82, 174,  82, 174 }, {  83, 173,  83, 173,  83, 173,  83, 173 },
		{  84, 172,  84, 172,  84, 172,  84, 172 }, {  85, 171,  85, 171,  85, 171,  85, 171 },
		{  86, 170,  86, 170,  86, 170,  86, 170 }, {  87, 169,  87, 169,  87, 169,  87, 169 },
		{  88, 168,  88, 168,  88, 168,  88, 168 }, {  89, 167,  89, 167,  89, 167,  89, 167 },
		{  90, 166,  90, 166,  90, 166,  90, 166 }, {  91, 165,  91, 165,  91, 165,  91, 165 },
		{  92, 164,  92, 164,  92, 164,  92, 164 }, {  93, 163,  93, 163,  93, 163,  93, 163 },
		{  94, 162,  94, 162,  94, 162,  94, 162 }, {  95, 161,  95, 161,  95, 161,  95, 161 },
		{  96, 160,  96, 160,  96, 160,  96, 160 }, {  97, 159,  97, 159,  97, 159,  97, 159 },
		{  98, 158,  98, 158,  98, 158,  98, 158 }, {  99, 157,  99, 157,  99, 157,  99, 157 },
		{ 100, 156, 100, 156, 100, 156, 100, 156 }, { 101, 155, 101, 155, 101, 155, 101, 155 },
		{ 102, 154, 102, 154, 102, 154, 102, 154 }, { 103, 153, 103, 153, 103, 153, 103, 153 },
		{ 104, 152, 104, 152, 104, 152, 104, 152 }, { 105, 151, 105, 151, 105, 151, 105, 151 },
		{ 106, 150, 106, 150, 106, 150, 106, 150 }, { 107, 149, 107, 149, 107, 149, 107, 149 },
		{ 108, 148, 108, 148, 108, 148, 108, 148 }, { 109, 147, 109, 147, 109, 147, 109, 147 },
		{ 110, 146, 110, 146, 110, 146, 110, 146 }, { 111, 145, 111, 145, 111, 145, 111, 145 },
		{ 112, 144, 112, 144, 112, 144, 112, 144 }, { 113, 143, 113, 143, 113, 143, 113, 143 },
		{ 114, 142, 114, 142, 114, 142, 114, 142 }, { 115, 141, 115, 141, 115, 141, 115, 141 },
		{ 116, 140, 116, 140, 116, 140, 116, 140 }, { 117, 139, 117, 139, 117, 139, 117, 139 },
		{ 118, 138, 118, 138, 118, 138, 118, 138 }, { 119, 137, 119, 137, 119, 137, 119, 137 },
		{ 120, 136, 120, 136, 120, 136, 120, 136 }, { 121, 135, 121, 135, 121, 135, 121, 135 },
		{ 122, 134, 122, 134, 122, 134, 122, 134 }, { 123, 133, 123, 133, 123, 133, 123, 133 },
		{ 124, 132, 124, 132, 124, 132, 124, 132 }, { 125, 131, 125, 131, 125, 131, 125, 131 },
		{ 126, 130, 126, 130, 126, 130, 126, 130 }, { 127, 129, 127, 129, 127, 129, 127, 129 },
		{ 128, 128, 128, 128, 128, 128, 128, 128 }, { 129, 127, 129, 127, 129, 127, 129, 127 },
		{ 130, 126, 130, 126, 130, 126, 130, 126 }, { 131, 125, 131, 125, 131, 125, 131, 125 },
		{ 132, 124, 132, 124, 132, 124, 132, 124 }, { 133, 123, 133, 123, 133, 123, 133, 123 },
		{ 134, 122, 134, 122, 134, 122, 134, 122 }, { 135, 121, 135, 121, 135, 121, 135, 121 },
		{ 136, 120, 136, 120, 136, 120, 136, 120 }, { 137, 119, 137, 119, 137, 119, 137, 119 },
		{ 138, 118, 138, 118, 138, 118, 138, 118 }, { 139, 117, 139, 117, 139, 117, 139, 117 },
		{ 140, 116, 140, 116, 140, 116, 140, 116 }, { 141, 115, 141, 115, 141, 115, 141, 115 },
		{ 142, 114, 142, 114, 142, 114, 142, 114 }, { 143, 113, 143, 113, 143, 113, 143, 113 },
		{ 144, 112, 144, 112, 144, 112, 144, 112 }, { 145, 111, 145, 111, 145, 111, 145, 111 },
		{ 146, 110, 146, 110, 146, 110, 146, 110 }, { 147, 109, 147, 109, 147, 109, 147, 109 },
		{ 148, 108, 148, 108, 148, 108, 148, 108 }, { 149, 107, 149, 107, 149, 107, 149, 107 },
		{ 150, 106, 150, 106, 150, 106, 150, 106 }, { 151, 105, 151, 105, 151, 105, 151, 105 },
		{ 152, 104, 152, 104, 152, 104, 152, 104 }, { 153, 103, 153, 103, 153, 103, 153, 103 },
		{ 154, 102, 154, 102, 154, 102, 154, 102 }, { 155, 101, 155, 101, 155, 101, 155, 101 },
		{ 156, 100, 156, 100, 156, 100, 156, 100 }, { 157,  99, 157,  99, 157,  99, 157,  99 },
		{ 158,  98, 158,  98, 158,  98, 158,  98 }, { 159,  97, 159,  97, 159,  97, 159,  97 },
		{ 160,  96, 160,  96, 160,  96, 160,  96 }, { 161,  95, 161,  95, 161,  95, 161,  95 },
		{ 162,  94, 162,  94, 162,  94, 162,  94 }, { 163,  93, 163,  93, 163,  93, 163,  93 },
		{ 164,  92, 164,  92, 164,  92, 164,  92 }, { 165,  91, 165,  91, 165,  91, 165,  91 },
		{ 166,  90, 166,  90, 166,  90, 166,  90 }, { 167,  89, 167,  89, 167,  89, 167,  89 },
		{ 168,  88, 168,  88, 168,  88, 168,  88 }, { 169,  87, 169,  87, 169,  87, 169,  87 },
		{ 170,  86, 170,  86, 170,  86, 170,  86 }, { 171,  85, 171,  85, 171,  85, 171,  85 },
		{ 172,  84, 172,  84, 172,  84, 172,  84 }, { 173,  83, 173,  83, 173,  83, 173,  83 },
		{ 174,  82, 174,  82, 174,  82, 174,  82 }, { 175,  81, 175,  81, 175,  81, 175,  81 },
		{ 176,  80, 176,  80, 176,  80, 176,  80 }, { 177,  79, 177,  79, 177,  79, 177,  79 },
		{ 178,  78, 178,  78, 178,  78, 178,  78 }, { 179,  77, 179,  77, 179,  77, 179,  77 },
		{ 180,  76, 180,  76, 180,  76, 180,  76 }, { 181,  75, 181,  75, 181,  75, 181,  75 },
		{ 182,  74, 182,  74, 182,  74, 182,  74 }, { 183,  73, 183,  73, 183,  73, 183,  73 },
		{ 184,  72, 184,  72, 184,  72, 184,  72 }, { 185,  71, 185,  71, 185,  71, 185,  71 },
		{ 186,  70, 186,  70, 186,  70, 186,  70 }, { 187,  69, 187,  69, 187,  69, 187,  69 },
		{ 188,  68, 188,  68, 188,  68, 188,  68 }, { 189,  67, 189,  67, 189,  67, 189,  67 },
		{ 190,  66, 190,  66, 190,  66, 190,  66 }, { 191,  65, 191,  65, 191,  65, 191,  65 },
		{ 192,  64, 192,  64, 192,  64, 192,  64 }, { 193,  63, 193,  63, 193,  63, 193,  63 },
		{ 194,  62, 194,  62, 194,  62, 194,  62 }, { 195,  61, 195,  61, 195,  61, 195,  61 },
		{ 196,  60, 196,  60, 196,  60, 196,  60 }, { 197,  59, 197,  59, 197,  59, 197,  59 },
		{ 198,  58, 198,  58, 198,  58, 198,  58 }, { 199,  57, 199,  57, 199,  57, 199,  57 },
		{ 200,  56, 200,  56, 200,  56, 200,  56 }, { 201,  55, 201,  55, 201,  55, 201,  55 },
		{ 202,  54, 202,  54, 202,  54, 202,  54 }, { 203,  53, 203,  53, 203,  53, 203,  53 },
		{ 204,  52, 204,  52, 204,  52, 204,  52 }, { 205,  51, 205,  51, 205,  51, 205,  51 },
		{ 206,  50, 206,  50, 206,  50, 206,  50 }, { 207,  49, 207,  49, 207,  49, 207,  49 },
		{ 208,  48, 208,  48, 208,  48, 208,  48 }, { 209,  47, 209,  47, 209,  47, 209,  47 },
		{ 210,  46, 210,  46, 210,  46, 210,  46 }, { 211,  45, 211,  45, 211,  45, 211,  45 },
		{ 212,  44, 212,  44, 212,  44, 212,  44 }, { 213,  43, 213,  43, 213,  43, 213,  43 },
		{ 214,  42, 214,  42, 214,  42, 214,  42 }, { 215,  41, 215,  41, 215,  41, 215,  41 },
		{ 216,  40, 216,  40, 216,  40, 216,  40 }, { 217,  39, 217,  39, 217,  39, 217,  39 },
		{ 218,  38, 218,  38, 218,  38, 218,  38 }, { 219,  37, 219,  37, 219,  37, 219,  37 },
		{ 220,  36, 220,  36, 220,  36, 220,  36 }, { 221,  35, 221,  35, 221,  35, 221,  35 },
		{ 222,  34, 222,  34, 222,  34, 222,  34 }, { 223,  33, 223,  33, 223,  33, 223,  33 },
		{ 224,  32, 224,  32, 224,  32, 224,  32 }, { 225,  31, 225,  31, 225,  31, 225,  31 },
		{ 226,  30, 226,  30, 226,  30, 226,  30 }, { 227,  29, 227,  29, 227,  29, 227,  29 },
		{ 228,  28, 228,  28, 228,  28, 228,  28 }, { 229,  27, 229,  27, 229,  27, 229,  27 },
		{ 230,  26, 230,  26, 230,  26, 230,  26 }, { 231,  25, 231,  25, 231,  25, 231,  25 },
		{ 232,  24, 232,  24, 232,  24, 232,  24 }, { 233,  23, 233,  23, 233,  23, 233,  23 },
		{ 234,  22, 234,  22, 234,  22, 234,  22 }, { 235,  21, 235,  21, 235,  21, 235,  21 },
		{ 236,  20, 236,  20, 236,  20, 236,  20 }, { 237,  19, 237,  19, 237,  19, 237,  19 },
		{ 238,  18, 238,  18, 238,  18, 238,  18 }, { 239,  17, 239,  17, 239,  17, 239,  17 },
		{ 240,  16, 240,  16, 240,  16, 240,  16 }, { 241,  15, 241,  15, 241,  15, 241,  15 },
		{ 242,  14, 242,  14, 242,  14, 242,  14 }, { 243,  13, 243,  13, 243,  13, 243,  13 },
		{ 244,  12, 244,  12, 244,  12, 244,  12 }, { 245,  11, 245,  11, 245,  11, 245,  11 },
		{ 246,  10, 246,  10, 246,  10, 246,  10 }, { 247,   9, 247,   9, 247,   9, 247,   9 },
		{ 248,   8, 248,   8, 248,   8, 248,   8 }, { 249,   7, 249,   7, 249,   7, 249,   7 },
		{ 250,   6, 250,   6, 250,   6, 250,   6 }, { 251,   5, 251,   5, 251,   5, 251,   5 },
		{ 252,   4, 252,   4, 252,   4, 252,   4 }, { 253,   3, 253,   3, 253,   3, 253,   3 },
		{ 254,   2, 254,   2, 254,   2, 254,   2 }, { 255,   1, 255,   1, 255,   1, 255,   1 }
	}
};
#endif // defined(__ALTIVEC__)
