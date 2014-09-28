#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static struct retro_hw_render_callback hw_render;

#include "glsym/glsym.h"

int vloc,cloc,tloc,utloc;
int last_blendmode;

static GLuint tex;
static GLuint prog;
static GLuint vbo;

GLfloat glverts[2*4],glcols[4*4],glvtexs[2*4];
GLfloat UseTexture=0.0f;

static const GLfloat vertex_data[] = {
//VERTEX
   -1.0, -1.0,
    1.0, -1.0,
   -1.0,  1.0,
    1.0,  1.0,
//COLOR
   1.0, 0.0, 0.0, 1.0,
   1.0, 0.0, 1.0, 1.0,
   0.0, 1.0, 1.0, 1.0,
   1.0, 1.0, 0.0, 1.0,
//TEXCOORD
   0, 0,
   1, 0, 
   0, 1, 
   1, 1, 
};

static const char *vertex_shader[] = {
   "attribute vec2 aVertex;",
   "attribute vec2 aTexCoord;\n", 
   "attribute vec4 aColor;",
   "varying vec4 color;",
   "varying vec2 vTex;\n",
   "void main() {",
   "  gl_Position = vec4(aVertex, 0.0, 1.0);",
   "  vTex =  aTexCoord ; color = aColor;"
   "}",
};

static const char *fragment_shader[] = {
   "#ifdef GL_ES\n",
   "precision mediump float;\n",
   "#endif\n",
   "varying vec2 vTex;\n",
   "varying vec4 color;\n",
   "uniform float uUseTexture;\n",
   "uniform sampler2D sTex0;\n",
   "void main() {",
   "   vec4 texColor = texture2D(sTex0, vTex) * color * uUseTexture ;\n",
   "   vec4 vertColor = color * (1.0 - uUseTexture);\n", 
   "   gl_FragColor =  texColor + vertColor;\n",
   "}",
};

static void compile_program(void)
{
	prog = glCreateProgram();
   	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
   	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

   	glShaderSource(vert, ARRAY_SIZE(vertex_shader), vertex_shader, 0);
   	glShaderSource(frag, ARRAY_SIZE(fragment_shader), fragment_shader, 0);
   	glCompileShader(vert);
   	glCompileShader(frag);
	
	glAttachShader(prog, vert);
   	glAttachShader(prog, frag);

   	glLinkProgram(prog);

   	glDeleteShader(vert);
   	glDeleteShader(frag);
}

static void setup_vao(void)
{
   	glUseProgram(prog);
	
	//setup_loc
   	glUniform1i(glGetUniformLocation(prog, "sTex0"), 0);
   	vloc = glGetAttribLocation(prog, "aVertex");
   	tloc = glGetAttribLocation(prog, "aTexCoord");
   	cloc = glGetAttribLocation(prog, "aColor");
	utloc= glGetUniformLocation(prog, "uUseTexture");

	//setup_texture
      	glGenTextures(1, &tex);

      	glBindTexture(GL_TEXTURE_2D, tex);
      	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1600, 1200, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,NULL );

	//setup_vbo
   	glGenBuffers(1, &vbo);

   	glBindBuffer(GL_ARRAY_BUFFER, vbo);
   	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STREAM_DRAW);
   	glBindBuffer(GL_ARRAY_BUFFER, 0);

   	glBindTexture(GL_TEXTURE_2D, 0);

   	glUseProgram(0);
}

static void context_reset(void)
{
   	fprintf(stderr, "Context reset!\n");
   	rglgen_resolve_symbols(hw_render.get_proc_address);
   	compile_program();
   	setup_vao(); 

   	glDisable(GL_DEPTH_TEST);
}

static void context_destroy(void)
{
   	fprintf(stderr, "Context destroy!\n");

   	glDeleteBuffers(1,&vbo);
   	vbo = 0;
   	glDeleteProgram(prog);
   	prog = 0;   
}


INLINE void set_blendmode(int blendmode)
{
	// try to minimize texture state changes
	if (blendmode != last_blendmode)
	{
		switch (blendmode)
		{
			case BLENDMODE_NONE:
				glDisable(GL_BLEND);
				break;
			case BLENDMODE_ALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case BLENDMODE_RGB_MULTIPLY:
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				break;
			case BLENDMODE_ADD:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
		}

		last_blendmode = blendmode;
	}
}

INLINE float round_nearest(float f)
{
	return floor(f + 0.5f);
}

static void do_glflush(){  

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	video_cb(RETRO_HW_FRAME_BUFFER_VALID, rtwi, rthe, 0);
}


//#define DBG_QUAD 1
#ifdef DBG_QUAD
#define PRINTQUAD() \
if (prim->texture.base){\
		printf("w:%d h:%d rwpx:%d ",prim->texture.width,prim->texture.height,prim->texture.rowpixels);\
		printf("uv1(%f,%f) ",prim->texcoords.tl.u,prim->texcoords.tl.v);\
		printf("uv2(%f,%f) ",prim->texcoords.tr.u,prim->texcoords.tr.v);\
		printf("uv3(%f,%f) ",prim->texcoords.bl.u,prim->texcoords.bl.v);\
		printf("uv4(%f,%f) ",prim->texcoords.br.u,prim->texcoords.br.v);\
}\
		printf("c(%f,%f,%f,%f) ",prim->color.r,prim->color.g,prim->color.b,prim->color.a);\
		printf("p1(%f,%f) ",prim->bounds.x0,prim->bounds.y1);\
		printf("p2(%f,%f) ",prim->bounds.x1,prim->bounds.y1);\
		printf("p3(%f,%f) ",prim->bounds.x0,prim->bounds.y0);\
		printf("p4(%f,%f) ",prim->bounds.x1,prim->bounds.y0);\
		printf("flag:%d ",prim->flags & (PRIMFLAG_TEXFORMAT_MASK | PRIMFLAG_BLENDMODE_MASK));\
		printf("\n");
#else
#define PRINTQUAD()
#endif

#define prep_vertex_attrib()\
			glBindBuffer(GL_ARRAY_BUFFER, vbo);\
   			glVertexAttribPointer(vloc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);\
   			glEnableVertexAttribArray(vloc);\
   			glVertexAttribPointer(cloc, 4, GL_FLOAT, GL_FALSE, 0, (void*)(8 * sizeof(GLfloat)));\
   			glEnableVertexAttribArray(cloc);\
   			glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, 0,  (void*)(24 * sizeof(GLfloat)) );\
   			glEnableVertexAttribArray(tloc);\
   			glBindBuffer(GL_ARRAY_BUFFER, 0);\
   			glUniform1f(utloc,UseTexture );


//============================================================
//  copyline_palette16
//============================================================

INLINE void copyline_palette16(UINT32 *dst, const UINT16 *src, int width, const rgb_t *palette, int xborderpix, int xprescale)
{
	int x;

	assert(xborderpix == 0 || xborderpix == 1);
	if (xborderpix)
		*dst++ = 0xff000000 | palette[*src];
	for (x = 0; x < width; x++)
	{
		int srcpix = *src++;
		for (int x2 = 0; x2 < xprescale; x2++)
			*dst++ = 0xff000000 | palette[srcpix];
	}
	if (xborderpix)
		*dst++ = 0xff000000 | palette[*--src];
}



//============================================================
//  copyline_palettea16
//============================================================

INLINE void copyline_palettea16(UINT32 *dst, const UINT16 *src, int width, const rgb_t *palette, int xborderpix, int xprescale)
{
	int x;

	assert(xborderpix == 0 || xborderpix == 1);
	if (xborderpix)
		*dst++ = palette[*src];
	for (x = 0; x < width; x++)
	{
		int srcpix = *src++;
		for (int x2 = 0; x2 < xprescale; x2++)
			*dst++ = palette[srcpix];
	}
	if (xborderpix)
		*dst++ = palette[*--src];
}



//============================================================
//  copyline_rgb32
//============================================================

INLINE void copyline_rgb32(UINT32 *dst, const UINT32 *src, int width, const rgb_t *palette, int xborderpix, int xprescale)
{
	int x;

	assert(xborderpix == 0 || xborderpix == 1);

	// palette (really RGB map) case
	if (palette != NULL)
	{
		if (xborderpix)
		{
			rgb_t srcpix = *src;
			*dst++ = 0xff000000 | palette[0x200 + srcpix.r()] | palette[0x100 + srcpix.g()] | palette[srcpix.b()];
		}
		for (x = 0; x < width; x++)
		{
			rgb_t srcpix = *src++;
			for (int x2 = 0; x2 < xprescale; x2++)
			{
				*dst++ = 0xff000000 | palette[0x200 + srcpix.r()] | palette[0x100 + srcpix.g()] | palette[srcpix.b()];
			}
		}
		if (xborderpix)
		{
			rgb_t srcpix = *--src;
			*dst++ = 0xff000000 | palette[0x200 + srcpix.r()] | palette[0x100 + srcpix.g()] | palette[srcpix.b()];
		}
	}

	// direct case
	else
	{
		if (xborderpix)
			*dst++ = 0xff000000 | *src;
		for (x = 0; x < width; x++)
		{
			rgb_t srcpix = *src++;

			for (int x2 = 0; x2 < xprescale; x2++)
			{
				*dst++ = 0xff000000 | srcpix;
			}
		}
		if (xborderpix)
			*dst++ = 0xff000000 | *--src;
	}
}

//============================================================
//  copyline_argb32
//============================================================

INLINE void copyline_argb32(UINT32 *dst, const UINT32 *src, int width, const rgb_t *palette, int xborderpix, int xprescale)
{
	int x;

	assert(xborderpix == 0 || xborderpix == 1);

	// palette (really RGB map) case
	if (palette != NULL)
	{
		if (xborderpix)
		{
			rgb_t srcpix = *src;
			*dst++ = (srcpix & 0xff000000) | palette[0x200 + srcpix.r()] | palette[0x100 + srcpix.g()] | palette[srcpix.b()];
		}
		for (x = 0; x < width; x++)
		{
			rgb_t srcpix = *src++;
			for (int x2 = 0; x2 < xprescale; x2++)
				*dst++ = (srcpix & 0xff000000) | palette[0x200 + srcpix.r()] | palette[0x100 + srcpix.g()] | palette[srcpix.b()];
		}
		if (xborderpix)
		{
			rgb_t srcpix = *--src;
			*dst++ = (srcpix & 0xff000000) | palette[0x200 + srcpix.r()] | palette[0x100 + srcpix.g()] | palette[srcpix.b()];
		}
	}

	// direct case
	else
	{
		if (xborderpix)
			*dst++ = *src;
		for (x = 0; x < width; x++)
		{
			rgb_t srcpix = *src++;
			for (int x2 = 0; x2 < xprescale; x2++)
				*dst++ = srcpix;
		}
		if (xborderpix)
			*dst++ = *--src;
	}
}

INLINE UINT32 ycc_to_rgb(UINT8 y, UINT8 cb, UINT8 cr)
{
	/* original equations:

	    C = Y - 16
	    D = Cb - 128
	    E = Cr - 128

	    R = clip(( 298 * C           + 409 * E + 128) >> 8)
	    G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
	    B = clip(( 298 * C + 516 * D           + 128) >> 8)

	    R = clip(( 298 * (Y - 16)                    + 409 * (Cr - 128) + 128) >> 8)
	    G = clip(( 298 * (Y - 16) - 100 * (Cb - 128) - 208 * (Cr - 128) + 128) >> 8)
	    B = clip(( 298 * (Y - 16) + 516 * (Cb - 128)                    + 128) >> 8)

	    R = clip(( 298 * Y - 298 * 16                        + 409 * Cr - 409 * 128 + 128) >> 8)
	    G = clip(( 298 * Y - 298 * 16 - 100 * Cb + 100 * 128 - 208 * Cr + 208 * 128 + 128) >> 8)
	    B = clip(( 298 * Y - 298 * 16 + 516 * Cb - 516 * 128                        + 128) >> 8)

	    R = clip(( 298 * Y - 298 * 16                        + 409 * Cr - 409 * 128 + 128) >> 8)
	    G = clip(( 298 * Y - 298 * 16 - 100 * Cb + 100 * 128 - 208 * Cr + 208 * 128 + 128) >> 8)
	    B = clip(( 298 * Y - 298 * 16 + 516 * Cb - 516 * 128                        + 128) >> 8)
	*/
	int r, g, b, common;

	common = 298 * y - 298 * 16;
	r = (common +                        409 * cr - 409 * 128 + 128) >> 8;
	g = (common - 100 * cb + 100 * 128 - 208 * cr + 208 * 128 + 128) >> 8;
	b = (common + 516 * cb - 516 * 128                        + 128) >> 8;

	if (r < 0) r = 0;
	else if (r > 255) r = 255;
	if (g < 0) g = 0;
	else if (g > 255) g = 255;
	if (b < 0) b = 0;
	else if (b > 255) b = 255;

	return rgb_t(0xff, r, g, b);
}

//============================================================
//  copyline_yuy16_to_argb
//============================================================

INLINE void copyline_yuy16_to_argb(UINT32 *dst, const UINT16 *src, int width, const rgb_t *palette, int xborderpix, int xprescale)
{
	int x;

	assert(xborderpix == 0 || xborderpix == 2);
	assert(width % 2 == 0);

	// palette (really RGB map) case
	if (palette != NULL)
	{
		if (xborderpix)
		{
			UINT16 srcpix0 = src[0];
			UINT16 srcpix1 = src[1];
			UINT8 cb = srcpix0 & 0xff;
			UINT8 cr = srcpix1 & 0xff;
			*dst++ = ycc_to_rgb(palette[0x000 + (srcpix0 >> 8)], cb, cr);
			*dst++ = ycc_to_rgb(palette[0x000 + (srcpix0 >> 8)], cb, cr);
		}
		for (x = 0; x < width / 2; x++)
		{
			UINT16 srcpix0 = *src++;
			UINT16 srcpix1 = *src++;
			UINT8 cb = srcpix0 & 0xff;
			UINT8 cr = srcpix1 & 0xff;
			for (int x2 = 0; x2 < xprescale/2; x2++)
			{
				*dst++ = ycc_to_rgb(palette[0x000 + (srcpix0 >> 8)], cb, cr);
				*dst++ = ycc_to_rgb(palette[0x000 + (srcpix1 >> 8)], cb, cr);
			}
		}
		if (xborderpix)
		{
			UINT16 srcpix1 = *--src;
			UINT16 srcpix0 = *--src;
			UINT8 cb = srcpix0 & 0xff;
			UINT8 cr = srcpix1 & 0xff;
			*dst++ = ycc_to_rgb(palette[0x000 + (srcpix1 >> 8)], cb, cr);
			*dst++ = ycc_to_rgb(palette[0x000 + (srcpix1 >> 8)], cb, cr);
		}
	}

	// direct case
	else
	{
		if (xborderpix)
		{
			UINT16 srcpix0 = src[0];
			UINT16 srcpix1 = src[1];
			UINT8 cb = srcpix0 & 0xff;
			UINT8 cr = srcpix1 & 0xff;
			*dst++ = ycc_to_rgb(srcpix0 >> 8, cb, cr);
			*dst++ = ycc_to_rgb(srcpix0 >> 8, cb, cr);
		}
		for (x = 0; x < width; x += 2)
		{
			UINT16 srcpix0 = *src++;
			UINT16 srcpix1 = *src++;
			UINT8 cb = srcpix0 & 0xff;
			UINT8 cr = srcpix1 & 0xff;
			for (int x2 = 0; x2 < xprescale/2; x2++)
			{
				*dst++ = ycc_to_rgb(srcpix0 >> 8, cb, cr);
				*dst++ = ycc_to_rgb(srcpix1 >> 8, cb, cr);
			}
		}
		if (xborderpix)
		{
			UINT16 srcpix1 = *--src;
			UINT16 srcpix0 = *--src;
			UINT8 cb = srcpix0 & 0xff;
			UINT8 cr = srcpix1 & 0xff;
			*dst++ = ycc_to_rgb(srcpix1 >> 8, cb, cr);
			*dst++ = ycc_to_rgb(srcpix1 >> 8, cb, cr);
		}
	}
}

static void gl_draw_primitives(const render_primitive_list &primlst,int minwidth,int minheight){  

	
	if(init3d==1){
		printf("initGL\n");
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		last_blendmode = BLENDMODE_ALPHA;

		init3d=0;

		//glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClearDepth(1.0f);
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LEQUAL);
		glDisable(GL_DEPTH_TEST);
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//TODO: only if machine->options().antialias()
		{
			// enable antialiasing for lines
			glEnable(GL_LINE_SMOOTH);
			// enable antialiasing for points
			glEnable(GL_POINT_SMOOTH);
			// prefer quality to speed
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		}
		//TODO: only to match machine->options().beam()
		glLineWidth(2.0);
		glPointSize(2.0);

	}

  	glBindFramebuffer(GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
   	glViewport(0, 0, minwidth, minheight); 

		for (const render_primitive *prim = primlst.first(); prim != NULL; prim = prim->next())
      {
	 	 UseTexture=0.0;

         switch (prim->type)
         {

			case render_primitive::LINE:

		glUseProgram(prog);

		set_blendmode(PRIMFLAG_GET_BLENDMODE(prim->flags));

		if (((prim->bounds.x1 - prim->bounds.x0) == 0) && ((prim->bounds.y1 - prim->bounds.y0) == 0))
		{
			//GLPOINT

			glverts[0]=2*(prim->bounds.x0/minwidth-0.5);
			glverts[1]=2*(prim->bounds.y0/minheight-0.5);
			glcols[0]=prim->color.r;
			glcols[1]=prim->color.g;
			glcols[2]=prim->color.b;
			glcols[3]=prim->color.a;

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
    			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glverts)/4, &glverts[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

    			glBindBuffer(GL_ARRAY_BUFFER, vbo);
    			glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), sizeof(glcols)/4, &glcols[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

			prep_vertex_attrib();

			glDrawArrays(GL_POINTS, 0, 1);

			glDisableVertexAttribArray(vloc);
			glDisableVertexAttribArray(cloc);

		}
		else
		{
			glverts[0]=2*(prim->bounds.x0/minwidth-0.5);
			glverts[1]=2*(prim->bounds.y0/minheight-0.5);
			glverts[2]=2*(prim->bounds.x1/minwidth-0.5);
			glverts[3]=2*(prim->bounds.y1/minheight-0.5);

			glcols[0]=prim->color.r;
			glcols[1]=prim->color.g;
			glcols[2]=prim->color.b;
			glcols[3]=prim->color.a;		
			glcols[4]=prim->color.r;
			glcols[5]=prim->color.g;
			glcols[6]=prim->color.b;
			glcols[7]=prim->color.a;

    			glBindBuffer(GL_ARRAY_BUFFER, vbo);
    			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glverts)/2, &glverts[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

    			glBindBuffer(GL_ARRAY_BUFFER, vbo);
    			glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), sizeof(glcols)/2, &glcols[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

			prep_vertex_attrib();

			glDrawArrays(GL_LINES, 0, 2);

			glDisableVertexAttribArray(vloc);
			glDisableVertexAttribArray(cloc);
		}

               break;

          case render_primitive::QUAD:
		
		//PRINTQUAD();

		glColor4f(prim->color.r, prim->color.g, prim->color.b, prim->color.a);
		set_blendmode(PRIMFLAG_GET_BLENDMODE(prim->flags));

                if (!prim->texture.base){

			glUseProgram(prog);

			glverts[0]=2*(prim->bounds.x0/minwidth-0.5);
			glverts[1]=2*(prim->bounds.y0/minheight-0.5);
			glverts[2]=2*(prim->bounds.x1/minwidth-0.5);
			glverts[3]=2*(prim->bounds.y0/minheight-0.5);
			glverts[6]=2*(prim->bounds.x1/minwidth-0.5);
			glverts[7]=2*(prim->bounds.y1/minheight-0.5);
			glverts[4]=2*(prim->bounds.x0/minwidth-0.5);
			glverts[5]=2*(prim->bounds.y1/minheight-0.5);

			glcols[0]=prim->color.r;
			glcols[1]=prim->color.g;
			glcols[2]=prim->color.b;
			glcols[3]=prim->color.a;
			glcols[4]=prim->color.r;
			glcols[5]=prim->color.g;
			glcols[6]=prim->color.b;
			glcols[7]=prim->color.a;
			glcols[8]=prim->color.r;
			glcols[9]=prim->color.g;
			glcols[10]=prim->color.b;
			glcols[11]=prim->color.a;
			glcols[12]=prim->color.r;
			glcols[13]=prim->color.g;
			glcols[14]=prim->color.b;
			glcols[15]=prim->color.a;

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
    			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glverts), &glverts[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

    			glBindBuffer(GL_ARRAY_BUFFER, vbo);
   			glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), sizeof(glcols), &glcols[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

			prep_vertex_attrib();

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glDisableVertexAttribArray(vloc);
			glDisableVertexAttribArray(cloc);
	

		}
		else{

   			glUseProgram(prog);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,tex);

			//TODO: only if video_config.filter
			if ((PRIMFLAG_GET_SCREENTEX(prim->flags)) /*&& video_config.filter*/)
			{
				// screen textures get the user's choice of filtering
				glTexParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				// non-screen textures will never be filtered
				glTexParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			// set wrapping mode appropriately
			if (prim->flags & PRIMFLAG_TEXWRAP_MASK)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			}

			PRINTQUAD();

			int y;
			UINT8 *dst;

#define COPYLOOP(format,tex) {\
 	for (y = 0; y < prim->texture.height; y++){\
		dst = (UINT8 *)(videoBuffer + y * prim->texture.width);\
		copyline_##format((UINT32 *)dst,tex + y * prim->texture.rowpixels, prim->texture.width, prim->texture.palette, 0, 1);\
	}\
}
			switch (PRIMFLAG_GET_TEXFORMAT(prim->flags))
			{
					case TEXFORMAT_PALETTE16:
						COPYLOOP(palette16,(UINT16 *)prim->texture.base);
						break;

					case TEXFORMAT_PALETTEA16:
						COPYLOOP(palettea16,(UINT16 *)prim->texture.base);
						break;

					case TEXFORMAT_RGB32:
						COPYLOOP(rgb32,(UINT32 *)prim->texture.base);
						break;

					case TEXFORMAT_ARGB32:
						COPYLOOP(argb32,(UINT32 *)prim->texture.base);
						break;

					case TEXFORMAT_YUY16:
						COPYLOOP(yuy16_to_argb,(UINT16 *)prim->texture.base);
						break;

					default:
						osd_printf_error("Unknown texture blendmode=%d format=%d\n", PRIMFLAG_GET_BLENDMODE(prim->flags), PRIMFLAG_GET_TEXFORMAT(prim->flags));
						break;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,prim->texture.width,prim->texture.height,0,GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,videoBuffer);

			glverts[0]=2*(prim->bounds.x0/minwidth-0.5);
			glverts[1]=2*(prim->bounds.y0/minheight-0.5);
			glverts[2]=2*(prim->bounds.x1/minwidth-0.5);
			glverts[3]=2*(prim->bounds.y0/minheight-0.5);
			glverts[4]=2*(prim->bounds.x0/minwidth-0.5);
			glverts[5]=2*(prim->bounds.y1/minheight-0.5);	
			glverts[6]=2*(prim->bounds.x1/minwidth-0.5);
			glverts[7]=2*(prim->bounds.y1/minheight-0.5);

			glcols[0]=prim->color.r;
			glcols[1]=prim->color.g;
			glcols[2]=prim->color.b;
			glcols[3]=prim->color.a;
			glcols[4]=prim->color.r;
			glcols[5]=prim->color.g;
			glcols[6]=prim->color.b;
			glcols[7]=prim->color.a;
			glcols[8]=prim->color.r;
			glcols[9]=prim->color.g;
			glcols[10]=prim->color.b;
			glcols[11]=prim->color.a;
			glcols[12]=prim->color.r;
			glcols[13]=prim->color.g;
			glcols[14]=prim->color.b;
			glcols[15]=prim->color.a;

			glvtexs[0] = prim->texcoords.tl.u;
			glvtexs[1] = prim->texcoords.tl.v;
			glvtexs[2] = prim->texcoords.tr.u;
			glvtexs[3] = prim->texcoords.tr.v;
			glvtexs[4] = prim->texcoords.bl.u;
			glvtexs[5] = prim->texcoords.bl.v;
			glvtexs[6] = prim->texcoords.br.u;
			glvtexs[7] = prim->texcoords.br.v;

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glverts), &glverts[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

    			glBindBuffer(GL_ARRAY_BUFFER, vbo);
   			glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), sizeof(glcols), &glcols[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

    			glBindBuffer(GL_ARRAY_BUFFER, vbo);
   			glBufferSubData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), sizeof(glvtexs), &glvtexs[0]);
    			glBindBuffer(GL_ARRAY_BUFFER, 0);

			UseTexture=1.0;
			prep_vertex_attrib();

   			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glDisableVertexAttribArray(vloc);
			glDisableVertexAttribArray(cloc);
			glDisableVertexAttribArray(tloc);

		}

               break;

            default:
               throw emu_fatalerror("Unexpected render_primitive type");
         }
      }

}

