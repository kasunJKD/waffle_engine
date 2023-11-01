#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>

#include <GL/glx.h>
#include "glcorearb.h"
#include <dlfcn.h>

void* platform_load_gl_function(const char* funName) {
    void* proc = dlsym(RTLD_DEFAULT, funName);
    if (!proc) {
        void* openglDLL = dlopen("libGL.so.1", RTLD_LAZY); // libGL.so.1 is the name of the OpenGL library on most Linux systems
        if (!openglDLL) {
            // Handle the error, e.g., using perror() to print an error message
            return NULL;
        }

        proc = dlsym(openglDLL, funName);
        if (!proc) {
            // Handle the error, e.g., using perror() to print an error message
            return NULL;
        }
    }

    return proc;
}

static PFNGLCREATEPROGRAMPROC glCreateProgram_ptr;
static PFNGLDELETETEXTURESEXTPROC glDeleteTextures_ptr;
static PFNGLGENTEXTURESEXTPROC glGenTextures_ptr;
static PFNGLBINDTEXTUREEXTPROC glBindTexture_ptr;
static PFNGLDRAWBUFFERPROC glDrawBuffer_ptr;
static PFNGLDRAWARRAYSEXTPROC glDrawArrays_ptr;
static PFNGLCREATESHADERPROC glCreateShader_ptr;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_ptr;
static PFNGLUNIFORM1FPROC glUniform1f_ptr;
static PFNGLUNIFORM2FVPROC glUniform2fv_ptr;
static PFNGLUNIFORM3FVPROC glUniform3fv_ptr;
static PFNGLUNIFORM1IPROC glUniform1i_ptr;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_ptr;
static PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor_ptr;
static PFNGLACTIVETEXTUREPROC glActiveTexture_ptr;
static PFNGLBUFFERSUBDATAPROC glBufferSubData_ptr;
static PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced_ptr;
static PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer_ptr;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus_ptr;
static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers_ptr;
static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D_ptr;
static PFNGLDRAWBUFFERSPROC glDrawBuffers_ptr;
static PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers_ptr;
static PFNGLBLENDFUNCIPROC glBlendFunci_ptr;
static PFNGLBLENDEQUATIONPROC glBlendEquation_ptr;
static PFNGLCLEARBUFFERFVPROC glClearBufferfv_ptr;
static PFNGLSHADERSOURCEPROC glShaderSource_ptr;
static PFNGLCOMPILESHADERPROC glCompileShader_ptr;
static PFNGLGETSHADERIVPROC glGetShaderiv_ptr;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_ptr;
static PFNGLATTACHSHADERPROC glAttachShader_ptr;
static PFNGLLINKPROGRAMPROC glLinkProgram_ptr;
static PFNGLVALIDATEPROGRAMPROC glValidateProgram_ptr;
static PFNGLGETPROGRAMIVPROC glGetProgramiv_ptr;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_ptr;
static PFNGLGENBUFFERSPROC glGenBuffers_ptr;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_ptr;
static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation_ptr;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray_ptr;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_ptr;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_ptr;
static PFNGLBINDBUFFERPROC glBindBuffer_ptr;
static PFNGLBINDBUFFERBASEPROC glBindBufferBase_ptr;
static PFNGLBUFFERDATAPROC glBufferData_ptr;
static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv_ptr;
static PFNGLUSEPROGRAMPROC glUseProgram_ptr;
static PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_ptr;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers_ptr;
static PFNGLDELETEPROGRAMPROC glDeleteProgram_ptr;
static PFNGLDETACHSHADERPROC glDetachShader_ptr;
static PFNGLDELETESHADERPROC glDeleteShader_ptr;
static PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced_ptr;
static PFNGLGENERATEMIPMAPPROC glGenerateMipmap_ptr;
static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback_ptr;


void load_gl_functions()
{
  // Load OpenGL Functions from the Operating System / Graphics Card
  glCreateProgram_ptr = (PFNGLCREATEPROGRAMPROC)platform_load_gl_function("glCreateProgram");
  glDeleteTextures_ptr = (PFNGLDELETETEXTURESEXTPROC)platform_load_gl_function("glDeleteTexturesEXT");
  glGenTextures_ptr = (PFNGLGENTEXTURESEXTPROC)platform_load_gl_function("glGenTexturesEXT");
  glBindTexture_ptr = (PFNGLBINDTEXTUREEXTPROC)platform_load_gl_function("glBindTextureEXT");
  glDrawArrays_ptr = (PFNGLDRAWARRAYSEXTPROC)platform_load_gl_function("glDrawArraysEXT");
  glCreateShader_ptr = (PFNGLCREATESHADERPROC) platform_load_gl_function("glCreateShader");
  glGetUniformLocation_ptr = (PFNGLGETUNIFORMLOCATIONPROC) platform_load_gl_function("glGetUniformLocation");
  glUniform1f_ptr = (PFNGLUNIFORM1FPROC) platform_load_gl_function("glUniform1f");
  glUniform2fv_ptr = (PFNGLUNIFORM2FVPROC) platform_load_gl_function("glUniform2fv");
  glUniform3fv_ptr = (PFNGLUNIFORM3FVPROC) platform_load_gl_function("glUniform3fv");
  glUniform1i_ptr = (PFNGLUNIFORM1IPROC) platform_load_gl_function("glUniform1i");
  glUniformMatrix4fv_ptr = (PFNGLUNIFORMMATRIX4FVPROC) platform_load_gl_function("glUniformMatrix4fv");
  glVertexAttribDivisor_ptr = (PFNGLVERTEXATTRIBDIVISORPROC) platform_load_gl_function("glVertexAttribDivisor");
  glActiveTexture_ptr = (PFNGLACTIVETEXTUREPROC) platform_load_gl_function("glActiveTexture");
  glBufferSubData_ptr = (PFNGLBUFFERSUBDATAPROC) platform_load_gl_function("glBufferSubData");
  glDrawArraysInstanced_ptr = (PFNGLDRAWARRAYSINSTANCEDPROC) platform_load_gl_function("glDrawArraysInstanced");
  glBindFramebuffer_ptr = (PFNGLBINDFRAMEBUFFERPROC) platform_load_gl_function("glBindFramebuffer");
  glCheckFramebufferStatus_ptr = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) platform_load_gl_function("glCheckFramebufferStatus");
  glGenFramebuffers_ptr = (PFNGLGENFRAMEBUFFERSPROC) platform_load_gl_function("glGenFramebuffers");
  glFramebufferTexture2D_ptr = (PFNGLFRAMEBUFFERTEXTURE2DPROC) platform_load_gl_function("glFramebufferTexture2D");
  glDrawBuffers_ptr = (PFNGLDRAWBUFFERSPROC) platform_load_gl_function("glDrawBuffers");
  glDeleteFramebuffers_ptr = (PFNGLDELETEFRAMEBUFFERSPROC) platform_load_gl_function("glDeleteFramebuffers");
  glBlendFunci_ptr = (PFNGLBLENDFUNCIPROC) platform_load_gl_function("glBlendFunci");
  glBlendEquation_ptr = (PFNGLBLENDEQUATIONPROC) platform_load_gl_function("glBlendEquation");
  glClearBufferfv_ptr = (PFNGLCLEARBUFFERFVPROC) platform_load_gl_function("glClearBufferfv");
  glShaderSource_ptr = (PFNGLSHADERSOURCEPROC) platform_load_gl_function("glShaderSource");
  glCompileShader_ptr = (PFNGLCOMPILESHADERPROC) platform_load_gl_function("glCompileShader");
  glGetShaderiv_ptr = (PFNGLGETSHADERIVPROC) platform_load_gl_function("glGetShaderiv");
  glGetShaderInfoLog_ptr = (PFNGLGETSHADERINFOLOGPROC) platform_load_gl_function("glGetShaderInfoLog");
  glAttachShader_ptr = (PFNGLATTACHSHADERPROC) platform_load_gl_function("glAttachShader");
  glLinkProgram_ptr = (PFNGLLINKPROGRAMPROC) platform_load_gl_function("glLinkProgram");
  glValidateProgram_ptr = (PFNGLVALIDATEPROGRAMPROC) platform_load_gl_function("glValidateProgram");
  glGetProgramiv_ptr = (PFNGLGETPROGRAMIVPROC) platform_load_gl_function("glGetProgramiv");
  glGetProgramInfoLog_ptr = (PFNGLGETPROGRAMINFOLOGPROC) platform_load_gl_function("glGetProgramInfoLog");
  glGenBuffers_ptr = (PFNGLGENBUFFERSPROC) platform_load_gl_function("glGenBuffers");
  glGenVertexArrays_ptr = (PFNGLGENVERTEXARRAYSPROC) platform_load_gl_function("glGenVertexArrays");
  glGetAttribLocation_ptr = (PFNGLGETATTRIBLOCATIONPROC) platform_load_gl_function("glGetAttribLocation");
  glBindVertexArray_ptr = (PFNGLBINDVERTEXARRAYPROC) platform_load_gl_function("glBindVertexArray");
  glEnableVertexAttribArray_ptr = (PFNGLENABLEVERTEXATTRIBARRAYPROC) platform_load_gl_function("glEnableVertexAttribArray");
  glVertexAttribPointer_ptr = (PFNGLVERTEXATTRIBPOINTERPROC) platform_load_gl_function("glVertexAttribPointer");
  glBindBuffer_ptr = (PFNGLBINDBUFFERPROC) platform_load_gl_function("glBindBuffer");
  glBindBufferBase_ptr = (PFNGLBINDBUFFERBASEPROC) platform_load_gl_function("glBindBufferBase");
  glBufferData_ptr = (PFNGLBUFFERDATAPROC) platform_load_gl_function("glBufferData");
  glGetVertexAttribPointerv_ptr = (PFNGLGETVERTEXATTRIBPOINTERVPROC) platform_load_gl_function("glGetVertexAttribPointerv");
  glUseProgram_ptr = (PFNGLUSEPROGRAMPROC) platform_load_gl_function("glUseProgram");
  glDeleteVertexArrays_ptr = (PFNGLDELETEVERTEXARRAYSPROC) platform_load_gl_function("glDeleteVertexArrays");
  glDeleteBuffers_ptr = (PFNGLDELETEBUFFERSPROC) platform_load_gl_function("glDeleteBuffers");
  glDeleteProgram_ptr = (PFNGLDELETEPROGRAMPROC) platform_load_gl_function("glDeleteProgram");
  glDetachShader_ptr = (PFNGLDETACHSHADERPROC) platform_load_gl_function("glDetachShader");
  glDeleteShader_ptr = (PFNGLDELETESHADERPROC) platform_load_gl_function("glDeleteShader");
  glDrawElementsInstanced_ptr = (PFNGLDRAWELEMENTSINSTANCEDPROC) platform_load_gl_function("glDrawElementsInstanced");
  glGenerateMipmap_ptr = (PFNGLGENERATEMIPMAPPROC) platform_load_gl_function("glGenerateMipmap");
  glDebugMessageCallback_ptr = (PFNGLDEBUGMESSAGECALLBACKPROC)platform_load_gl_function("glDebugMessageCallback");
}

// #############################################################################
//                           OpenGL Function Wrappers
// #############################################################################
GLAPI GLuint APIENTRY glCreateProgram (void)
{
  return glCreateProgram_ptr();
}

GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
{
  glDeleteTextures_ptr(n, textures);
}

GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures)
{
  glGenTextures_ptr(n, textures);
}

GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture)
{
  glBindTexture_ptr(target, texture);
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays_ptr(mode, first, count);
}

GLuint glCreateShader(GLenum shaderType)
{
    return glCreateShader_ptr(shaderType);
}

GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
    return glGetUniformLocation_ptr(program, name);
}

void glUniform1f(GLint location, GLfloat v0)
{
    glUniform1f_ptr(location, v0);
}

void glUniform2fv(GLint location, GLsizei count, const GLfloat* value)
{
    glUniform2fv_ptr(location, count, value);
}

void glUniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
    glUniform3fv_ptr(location, count, value);
}

void glUniform1i(GLint location, GLint v0)
{
    glUniform1i_ptr(location, v0);
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
    glUniformMatrix4fv_ptr(location, count, transpose, value);
}

void glVertexAttribDivisor(GLuint index, GLuint divisor)
{
    glVertexAttribDivisor_ptr(index, divisor);
}

void glActiveTexture(GLenum texture)
{
    glActiveTexture_ptr(texture);
}

void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
    glBufferSubData_ptr(target, offset, size, data);
}

void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    glDrawArraysInstanced_ptr(mode, first, count, instanceCount);
}

void glBindFramebuffer(GLenum target, GLuint framebuffer)
{
    glBindFramebuffer_ptr(target, framebuffer);
}

GLenum glCheckFramebufferStatus(GLenum target)
{
    return glCheckFramebufferStatus_ptr(target);
}

void glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
    glGenFramebuffers_ptr(n, framebuffers);
}

void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    glFramebufferTexture2D_ptr(target, attachment, textarget, texture, level);
}

void glDrawBuffers(GLsizei n, const GLenum* bufs)
{
    glDrawBuffers_ptr(n, bufs);
}

void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
    glDeleteFramebuffers_ptr(n, framebuffers);
}

void glBlendFunci(GLuint buf, GLenum src, GLenum dst)
{
    glBlendFunci_ptr(buf, src, dst);
}

void glBlendEquation(GLenum mode)
{
    glBlendEquation_ptr(mode);
}

void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
    glClearBufferfv_ptr(buffer, drawbuffer, value);
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* strings, const GLint* lengths)
{
    glShaderSource_ptr(shader, count, strings, lengths);
}

void glCompileShader(GLuint shader)
{
    glCompileShader_ptr(shader);
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
    glGetShaderiv_ptr(shader, pname, params);
}

void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
    glGetShaderInfoLog_ptr(shader, bufSize, length, infoLog);
}

void glAttachShader(GLuint program, GLuint shader)
{
    glAttachShader_ptr(program, shader);
}

void glLinkProgram(GLuint program)
{
    glLinkProgram_ptr(program);
}

void glValidateProgram(GLuint program)
{
    glValidateProgram_ptr(program);
}

void glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
    glGetProgramiv_ptr(program, pname, params);
}

void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
    glGetProgramInfoLog_ptr(program, bufSize, length, infoLog);
}

void glGenBuffers(GLsizei n, GLuint* buffers)
{
    glGenBuffers_ptr(n, buffers);
}

void glGenVertexArrays(GLsizei n, GLuint* arrays)
{
    glGenVertexArrays_ptr(n, arrays);
}

GLint glGetAttribLocation(GLuint program, const GLchar* name)
{
    return glGetAttribLocation_ptr(program, name);
}

void glBindVertexArray(GLuint array)
{
    glBindVertexArray_ptr(array);
}

void glEnableVertexAttribArray(GLuint index)
{
    glEnableVertexAttribArray_ptr(index);
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
{
    glVertexAttribPointer_ptr(index, size, type, normalized, stride, pointer);
}

void glBindBuffer(GLenum target, GLuint buffer)
{
    glBindBuffer_ptr(target, buffer);
}

void glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
    glBindBufferBase_ptr(target, index, buffer);
}

void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
    glBufferData_ptr(target, size, data, usage);
}

void glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
    glGetVertexAttribPointerv_ptr(index, pname, pointer);
}

void glUseProgram(GLuint program)
{
    glUseProgram_ptr(program);
}

void glDeleteVertexArrays(GLsizei n, const GLuint* arrays)
{
    glDeleteVertexArrays_ptr(n, arrays);
}

void glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
    glDeleteBuffers_ptr(n, buffers);
}

void glDeleteProgram(GLuint program)
{
    glDeleteProgram_ptr(program);
}

void glDetachShader (GLuint program, GLuint shader)
{
    glDetachShader_ptr(program, shader);
}

void glDeleteShader(GLuint shader)
{
    glDeleteShader_ptr(shader);
}

void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount)
{
    glDrawElementsInstanced_ptr(mode, count, type, indices, instancecount);
}

void glGenerateMipmap(GLenum target)
{
    glGenerateMipmap_ptr(target);
}

void glDebugMessageCallback (GLDEBUGPROC callback, const void *userParam)
{
  glDebugMessageCallback_ptr(callback, userParam);
}

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static bool isExtensionSupported(const char *extList, const char *extension) {
	const char *start;
	const char *where, *terminator;

	where = strchr(extension, ' ');
	if (where || *extension == '\0') {
		return false;
	}

	for (start=extList;;) {
		where = strstr(start, extension);

		if (!where) {
		 	break;
		}

		terminator = where + strlen(extension);

		if ( where == start || *(where - 1) == ' ' ) {
			if ( *terminator == ' ' || *terminator == '\0' ) {
				return true;
			}
		}	

		start = terminator;
	}

	return false;
}

int main()
{
    Display *display = XOpenDisplay(NULL);
    XEvent ev;

    int screen;
    screen = XDefaultScreen(display);

	// Check GLX version
	GLint majorGLX, minorGLX = 0;
	glXQueryVersion(display, &majorGLX, &minorGLX);
	if (majorGLX <= 1 && minorGLX < 2) {
		std::cout << "GLX 1.2 or greater is required.\n";
		XCloseDisplay(display);
		return 1;
	}
	else {
		std::cout << "GLX client version: " << glXGetClientString(display, GLX_VERSION) << '\n';
		std::cout << "GLX client vendor: " << glXGetClientString(display, GLX_VENDOR) << "\n";
		std::cout << "GLX client extensions:\n\t" << glXGetClientString(display, GLX_EXTENSIONS) << "\n";

		std::cout << "GLX server version: " << glXQueryServerString(display, screen, GLX_VERSION) << "\n";
		std::cout << "GLX server vendoe: " << glXQueryServerString(display, screen, GLX_VENDOR) << "\n";
		std::cout << "GLX server extensions:\n\t " << glXQueryServerString(display, screen, GLX_EXTENSIONS) << "\n";
	}
	// GLX, create XVisualInfo, this is the minimum visuals we want
	GLint glxAttribs[] = {
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		None
	};
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), glxAttribs, &fbcount);
	if (fbc == 0) {
		std::cout << "Failed to retrieve framebuffer.\n";
		XCloseDisplay(display);
		return 1;
	}
	std::cout << "Found " << fbcount << " matching framebuffers.\n";

	// Pick the FB config/visual with the most samples per pixel
	std::cout << "Getting best XVisualInfo\n";
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
	for (int i = 0; i < fbcount; ++i) {
		XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
		if ( vi != 0) {
			int samp_buf, samples;
			glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
			//std::cout << "  Matching fbconfig " << i << ", SAMPLE_BUFFERS = " << samp_buf << ", SAMPLES = " << samples << ".\n";

			if ( best_fbc < 0 || (samp_buf && samples > best_num_samp) ) {
				best_fbc = i;
				best_num_samp = samples;
			}
			if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
				worst_fbc = i;
			worst_num_samp = samples;
		}
		XFree( vi );
	}
	std::cout << "Best visual info index: " << best_fbc << "\n";
	GLXFBConfig bestFbc = fbc[ best_fbc ];
	XFree( fbc ); // Make sure to free this!

	XVisualInfo* visual = glXGetVisualFromFBConfig( display, bestFbc );

	if (visual == 0) {
		std::cout << "Could not create correct visual window.\n";
		XCloseDisplay(display);
		return 1;
	}

	if (screen != visual->screen) {
		std::cout << "screenId(" << screen << ") does not match visual->screen(" << visual->screen << ").\n";
		XCloseDisplay(display);
		return 1;

	}
	
	// Open the window
	XSetWindowAttributes windowAttribs;
	windowAttribs.border_pixel = BlackPixel(display, screen);
	windowAttribs.background_pixel = WhitePixel(display, screen);
	windowAttribs.override_redirect = True;
	windowAttribs.colormap = XCreateColormap(display, RootWindow(display, screen), visual->visual, AllocNone);
	windowAttribs.event_mask = ExposureMask;
	Window window = XCreateWindow(display, RootWindow(display, screen), 0, 0, 320, 200, 0, visual->depth, InputOutput, visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &windowAttribs);

	// Create GLX OpenGL context
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	const char *glxExts = glXQueryExtensionsString( display,  screen );
	std::cout << "Late extensions:\n\t" << glxExts << "\n\n";
	if (glXCreateContextAttribsARB == 0) {
		std::cout << "glXCreateContextAttribsARB() not found.\n";
	}
	
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 2,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};

	GLXContext context = 0;
	if (!isExtensionSupported( glxExts, "GLX_ARB_create_context")) {
		context = glXCreateNewContext( display, bestFbc, GLX_RGBA_TYPE, 0, True );
	}
	else {
		context = glXCreateContextAttribsARB( display, bestFbc, 0, true, context_attribs );
	}
	XSync( display, False );

	// Verifying that context is a direct context
	if (!glXIsDirect (display, context)) {
		std::cout << "Indirect GLX rendering context obtained\n";
	}
	else {
		std::cout << "Direct GLX rendering context obtained\n";
	}
	glXMakeCurrent(display, window, context);

	std::cout << "GL Vendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "GL Renderer: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "GL Version: " << glGetString(GL_VERSION) << "\n";
	std::cout << "GL Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    /* process window close event through event handler so XNextEvent does not fail */
    Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    /* select kind of events we are interested in */
    XSelectInput(display, window, ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | StructureNotifyMask);

	XStoreName(display, window, "waffle engine");

	XClearWindow(display, window);
	XMapRaised(display, window);

	// Set GL Sample stuff
	glClearColor(0.5f, 0.6f, 0.7f, 1.0f);

	load_gl_functions();

    //read shaders
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open("shaders/quad.vert");
        fShaderFile.open("shaders/quad.frag");
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //doing everything in the shader itself
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

	XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);
	
    // Resize window
    unsigned int change_values = CWWidth | CWHeight;
    XWindowChanges values;
    values.width = 800;
    values.height = 600;
    XConfigureWindow(display, window, change_values, &values);

    int x, y;
    char str[25] = {0}; 
    KeySym keysym = 0;
    int len = 0;

    int quit = 0;
    while(!quit) {
		if (XPending(display) > 0) {
       XNextEvent(display, &ev);
		switch(ev.type) {
			case Expose: {
				XGetWindowAttributes(display, window, &attribs);
				glViewport(0, 0, attribs.width, attribs.height);
			}break;
			case KeymapNotify:
                XRefreshKeyboardMapping(&ev.xmapping);
            break;
            case KeyPress:
                len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
                if (len > 0) {
                    std::cout << "Key pressed: " << str << " - " << len << " - " << keysym <<'\n';
                }
                if (keysym == XK_Escape) {
                    quit = 1;
                }
            break;
            case KeyRelease:
                len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
                if (len > 0) {
                    std::cout << "Key released: " << str << " - " << len << " - " << keysym <<'\n';
                }
            break;
			case ButtonPress:
				if (ev.xbutton.button == 1) {
					std::cout << "Left mouse down\n";
				}
				else if (ev.xbutton.button == 2) {
					std::cout << "Middle mouse down\n";
				}
				else if (ev.xbutton.button == 3) {
					std::cout << "Right mouse down\n";
				}
				else if (ev.xbutton.button == 4) {
					std::cout << "Mouse scroll up\n";
				} 
				else if (ev.xbutton.button == 5) {
					std::cout << "Mouse scroll down\n";
				}
			break;
			case ButtonRelease:
				if (ev.xbutton.button == 1) {
					std::cout << "Left mouse up\n";
				}
				else if (ev.xbutton.button == 2) {
					std::cout << "Middle mouse up\n";
				}
				else if (ev.xbutton.button == 3) {
					std::cout << "Right mouse up\n";
				}
			break;
			case MotionNotify:
				x = ev.xmotion.x;
				y = ev.xmotion.y;
				std::cout << "Mouse X:" << x << ", Y: " << y << "\n";
			break;
			case EnterNotify:
				std::cout << "Mouse enter\n";
			break;
			case LeaveNotify:
				std::cout << "Mouse leave\n";
			break;
			case ClientMessage: {
				if (ev.xclient.data.l[0] == del_window) {
					quit = 1;
				}
			}break;
			case DestroyNotify:{
				quit = 1;
			}break;
		} 
		}
		// OpenGL Rendering
		glClear(GL_COLOR_BUFFER_BIT);

		// draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

		// Present frame
		glXSwapBuffers(display, window);
    }

	// Cleanup GLX
	glXDestroyContext(display, context);

    // Cleanup X11
	XFree(visual);
	XFreeColormap(display, windowAttribs.colormap);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
    return 0;
}
