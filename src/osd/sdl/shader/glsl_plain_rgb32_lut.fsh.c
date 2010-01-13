const char glsl_plain_rgb32_lut_fsh_src[] =
"\n"
"#pragma optimize (on)\n"
"#pragma debug (off)\n"
"\n"
"uniform sampler2D color_texture;\n"
"uniform sampler2D colortable_texture;\n"
"uniform vec2      colortable_sz;         // orig size for full bgr\n"
"uniform vec2      colortable_pow2_sz;    // orig size for full bgr\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color_tex;\n"
"	vec2 color_map_coord;\n"
"	float colortable_scale = (colortable_sz.x/3.0) / colortable_pow2_sz.x;\n"
"\n"
"	// normalized texture coordinates ..\n"
"	color_tex = texture2D(color_texture, gl_TexCoord[0].st) * ((colortable_sz.x/3.0)-1.0)/colortable_pow2_sz.x;// lookup space \n"
"\n"
"	color_map_coord.x = color_tex.b;\n"
"	gl_FragColor.b    = texture2D(colortable_texture, color_map_coord).b;\n"
"\n"
"	color_map_coord.x = color_tex.g + colortable_scale;\n"
"	gl_FragColor.g    = texture2D(colortable_texture, color_map_coord).g;\n"
"\n"
"	color_map_coord.x = color_tex.r + 2.0 * colortable_scale;\n"
"	gl_FragColor.r    = texture2D(colortable_texture, color_map_coord).r;\n"
"}\n"
"\n"
;
