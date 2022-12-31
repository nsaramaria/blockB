
#include "Graphics.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "GLES2/gl2ext.h"
using namespace glm;

// moving initialisaiton and processing to here

Display*				x_display;
Window                  win;
	
Graphics::Graphics()
{
	// don't really want to do anything here but could move init here
}
Graphics::~Graphics()
{
	
}


void Graphics::esInitContext(Target_State *p_state)
{
	if (p_state != NULL)
	{
		memset(p_state, 0, sizeof(Target_State));
	}
}

bool Graphics::GetDisplay()
{
	/*
	 * X11 native display initialization
	 */

	x_display = XOpenDisplay(nullptr);
	if (x_display == NULL)
	{
		printf("Sorry to say we can't open an XDisplay and this will fail\n");
		return false;
	}
	else
		printf("we got an XDisplay\n");

	root = DefaultRootWindow(x_display);
	return true;
}

/*
Set up the EGL and in the case of Raspberry, DispmanX
Set up OpenGL states needed
Also set up the X11 window for rendering on non Taspberry (also used for Gui cursor use on Raspberry)
*/
#ifdef RASPBERRY // do the raspberry set up which needs a few extra things, the current res and the requred size of the frame buffer


void Graphics::init_ogl(Target_State* state, int width, int height, int fbx, int fby)
{
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	EGLint majorVersion;
	EGLint minorVersion;

	//RPI setup is a little different to normal EGL
	DISPMANX_ELEMENT_HANDLE_T DispmanElementH;
	DISPMANX_DISPLAY_HANDLE_T DispmanDisplayH;
	DISPMANX_UPDATE_HANDLE_T DispmanUpdateH;
	VC_RECT_T dest_rect;
	VC_RECT_T src_rect;
	EGLConfig config;

	// we also need an xwindow
	Window root;
	XSetWindowAttributes swa;
	XSetWindowAttributes  xattr;
	Atom wm_state;
	XWMHints hints;
	XEvent xev;




	/*
	 * X11 native display initialization
	 * Still need this for Img mouse and keys
	 */

	x_display = XOpenDisplay(NULL);
	if (x_display == NULL)
	{
		printf("Sorry to say we can't create an Xwindow and this will fail");
		exit(0);      // we need to trap this;
	}
	else
		printf("we got an xdisplay\n");


	root = DefaultRootWindow(x_display);
	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
	swa.background_pixmap = None;
	swa.background_pixel = 0;
	swa.border_pixel = 0;
	swa.override_redirect = true;
	win = XCreateWindow(
		x_display,
		root,
		0,		// puts it at the top left of the screen
		0,
		width,		//set size  
		height,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&swa);

	if (win == 0)
	{
		printf("Sorry to say we can't create a window and this will fail\n");
		exit(0);       // we need to trap this;
	}
	else
		printf("we got an (Native) XWindow\n");


	XSelectInput(x_display, win, KeyPressMask | KeyReleaseMask);
	xattr.override_redirect = TRUE;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	hints.input = TRUE;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);
	bcm_host_init();   //RPI needs this

	// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(state->display, &majorVersion, &minorVersion);

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, GiveMeGLES2);
	assert(state->context != EGL_NO_CONTEXT);

	// create an EGL window surface

	state->width = fbx;
	state->height = fby;

	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.width = width;  // it needs to know our window size
	dest_rect.height = height;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = (fbx) << 16;
	src_rect.height = (fby) << 16;

	DispmanDisplayH = vc_dispmanx_display_open(0);
	DispmanUpdateH = vc_dispmanx_update_start(0);

	DispmanElementH = vc_dispmanx_element_add(
		DispmanUpdateH,
		DispmanDisplayH,
		0/*layer*/,
		&dest_rect,
		0/*source*/,
		&src_rect,
		DISPMANX_PROTECTION_NONE,
		0 /*alpha value*/,
		0/*clamp*/,
		(DISPMANX_TRANSFORM_T)0/*transform*/);

	state->nativewindow.element = DispmanElementH;
	state->nativewindow.width = fbx;
	state->nativewindow.height = fby;
	vc_dispmanx_update_submit_sync(DispmanUpdateH);

	state->surface = eglCreateWindowSurface(state->display, config, &(state->nativewindow), NULL);
	assert(state->surface != EGL_NO_SURFACE);

	// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);

	EGLBoolean test = eglSwapInterval(state->display, 0);         // 1 to lock speed to 60fps (assuming we are able to maintain it), 0 for immediate swap (may cause tearing) which will indicate actual frame rate

		// Some OpenGLES2.0 states that we might need
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(TRUE);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);

	//these are the options you can have for the depth, play with them?
	//#define GL_NEVER                          0x0200
	//#define GL_LESS                           0x0201
	//#define GL_EQUAL                          0x0202
	//#define GL_LEQUAL                         0x0203
	//#define GL_GREATER                        0x0204
	//#define GL_NOTEQUAL                       0x0205
	//#define GL_GEQUAL                         0x0206

	glViewport(0, 0, state->width, state->height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glCullFace(GL_BACK);


	printf("This SBC supports version %i.%i of EGL\n", majorVersion, minorVersion);
	printf("This GPU supplied by  :%s\n", glGetString(GL_VENDOR));
	printf("This GPU supports     :%s\n", glGetString(GL_VERSION));
	printf("This GPU Renders with :%s\n", glGetString(GL_RENDERER));
	printf("This GPU supports     :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("This GPU supports these extensions	:%s\n", glGetString(GL_EXTENSIONS));
	int t;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t);
	printf("This GPU supports %i Texture units\n", t); //pi 0- 3b+  only 8 textures
	this->ResolutionX = fbx;
	this->ResolutionY = fby;

	if (glGetError() == GL_NO_ERROR)	return;
	else
		printf("Oh bugger, Some part of the EGL/OGL graphic init failed\n");
}
#else // the standard X11 set up for non raspberry systems (or Raspberry 4B with X11)

void Graphics::init_ogl(Target_State* state, int width, int height, int FBResX, int FBResY)

{

	XSetWindowAttributes swa;
	XSetWindowAttributes  xattr;
	Atom wm_state;
	XWMHints hints;
	XEvent xev;

#define ES_WINDOW_RGB           0
	state->width = width;
	state->height = height;

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;


	root = DefaultRootWindow(x_display);

	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
	swa.background_pixmap = None;
	swa.background_pixel = 0;
	swa.border_pixel = 0;
	swa.override_redirect = true;

	win = XCreateWindow(
		x_display,
		root,
		0,		// puts it at the top left of the screen
		0,
		state->width,	//set size  
		state->height,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&swa);

	if (win == 0)
	{
		printf("Sorry to say we can't create a window and this will fail\n");
		exit(0);       // we need to trap this;
	}
	else
		printf("we got an (Native) XWindow\n");

	state->nativewindow = (EGLNativeWindowType)win;

	XSelectInput(x_display, win, KeyPressMask | KeyReleaseMask);
	xattr.override_redirect = TRUE;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	hints.input = TRUE;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);


	char* title = (char*)"x11 Demo";
	// make the window visible on the screen
	XMapWindow(x_display, win);
	XStoreName(x_display, win, title);

	// get identifiers for the provided atom name strings
	wm_state = XInternAtom(x_display, "_NET_WM_STATE", FALSE);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = FALSE;
	XSendEvent(
		x_display,
		DefaultRootWindow(x_display),
		FALSE,
		SubstructureNotifyMask,
		&xev);
	// end of xdisplay

	// Get EGLDisplay	
	egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);       //eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (egldisplay == EGL_NO_DISPLAY)
	{
		printf("Sorry to say we have an GetDisplay error and this will fail");
		exit(0);          // we need to trap this;
	}
	else  	printf("we got an EGLDisplay\n");

	// Initialize EGL
	if (!eglInitialize(egldisplay, &majorVersion, &minorVersion))
	{
		printf("Sorry to say we have an EGLinit error and this will fail");
		EGLint err = eglGetError();    // should be getting error values that make sense now
		exit(err);            // we need to trap this;
	}
	else 	printf("we initialised EGL\n");


	// Get configs
	if (!eglGetConfigs(egldisplay, NULL, 0, &numConfigs))
	{
		printf("Sorry to say we have EGL config errors and this will fail");
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}
	else 	printf("we got %i Configs\n", numConfigs);


	// Choose config
	if (!eglChooseConfig(egldisplay, attribute_list, &config, 1, &numConfigs))
	{
		printf("Sorry to say we have config choice issues, and this will fail");
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}
	else	printf("we chose our config\n");




	// Create a GL context

#ifdef GLES3
	context = eglCreateContext(egldisplay, config, NULL, GiveMeGLES3);
#else
	context = eglCreateContext(egldisplay, config, EGL_NO_CONTEXT, GiveMeGLES2);
#endif
	if (context == EGL_NO_CONTEXT)
	{
		EGLint err = eglGetError();
		exit(err);             // we need to trap this;
	}
	else	printf("Created a context ok\n");

	PFNGLDEBUGMESSAGECALLBACKKHRPROC peglDebugMessageControlKHR = (PFNGLDEBUGMESSAGECALLBACKKHRPROC)eglGetProcAddress("glDebugMessageCallback");
	if (!(peglDebugMessageControlKHR != 0)) {
		printf("failed to eglGetProcAddress eglDebugMessageControlKHR\n");
	}
	else {
		GLDEBUGPROCKHR DebugFn = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH_KHR:
			case GL_DEBUG_SEVERITY_MEDIUM_KHR:
				std::cout << message << std::endl;
			case GL_DEBUG_SEVERITY_LOW_KHR:
			case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
			default:
				break; //Ignore.
			}
		};
		peglDebugMessageControlKHR(DebugFn, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
	}
	// Create a surface
	surface = eglCreateWindowSurface(egldisplay, config, state->nativewindow, NULL);     // this fails with a segmentation error?

	if (surface == EGL_NO_SURFACE)
	{
		EGLint err = eglGetError();
		exit(err);               // we need to trap this;
	}
	else 	printf("we got a Surface\n");


	// Make the context current
	if (!eglMakeCurrent(egldisplay, surface, surface, context))
	{
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}

	state->display = egldisplay;
	state->surface = surface;
	state->context = context;

	EGLBoolean test = eglSwapInterval(egldisplay, 01);        // 1 to lock speed to 60fps (assuming we are able to maintain it), 0 for immediate swap (may cause tearing) which will indicate actual frame rate
	// on xu4 this seems to have no effect

	// Some OpenGLES2.0 states that we might need
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(TRUE);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);

	//these are the options you can have for the depth, play with them?
	//#define GL_NEVER                          0x0200
	//#define GL_LESS                           0x0201
	//#define GL_EQUAL                          0x0202
	//#define GL_LEQUAL                         0x0203
	//#define GL_GREATER                        0x0204
	//#define GL_NOTEQUAL                       0x0205
	//#define GL_GEQUAL                         0x0206

	int t; // a dum,ing variable to extra some useful data

	printf("This SBC supports version %i.%i of EGL\n", majorVersion, minorVersion);
	printf("This GPU supplied by  :%s\n", glGetString(GL_VENDOR));
	printf("This GPU supports     :%s\n", glGetString(GL_VERSION));
	printf("This GPU Renders with :%s\n", glGetString(GL_RENDERER));
	printf("This GPU supports     :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &t);
	printf("This GPU MaxTexSize is    :%i\n", t);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t);
	printf("This GPU supports %i Texture units\n", t); // pi4 16


	printf("This GPU supports these extensions	:%s\n", glGetString(GL_EXTENSIONS));

	glViewport(0, 0, state->width, state->height);
	glClearColor(0.1f, 0.4f, 0.0f, 1.0f);
	//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glCullFace(GL_BACK);

	if (glGetError() == GL_NO_ERROR)	return;
	else
		printf("Oh bugger, Some part of the EGL/OGL graphic init failed\n");
}

#endif // RASPBERRY

/*
 Now we have be able to create a shader object, pass the shader source
 and them compile the shader.
*/
GLuint Graphics::LoadShader(GLenum type, const char *shaderSrc)
{	
	// 1st create the shader object
		GLuint TheShader = glCreateShader(type);
	
	if (TheShader == 0) return FALSE; // can't allocate so stop.
	
// pass the shader source then compile it
	glShaderSource(TheShader, 1, &shaderSrc, NULL);
	glCompileShader(TheShader);
	
	GLint  IsItCompiled;
	
	// After the compile we need to check the status and report any errors
		glGetShaderiv(TheShader, GL_COMPILE_STATUS, &IsItCompiled);
	if (!IsItCompiled)
	{
		GLint RetinfoLen = 0;
		glGetShaderiv(TheShader, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{
			 // standard output for errors
			char* infoLog = (char*) malloc(sizeof(char) * RetinfoLen);
			glGetShaderInfoLog(TheShader, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling this shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(TheShader);
		return FALSE;
	}
	return TheShader;
}

/*
Initialise an object so that it uses the correct shader to display
return int TRUE if all good.
*/

int Graphics::Init(ObjectModel* TheModel)
{
	//BRIAN INITIAL CODE
	// hard code for now student can optimise :D

	//	GLbyte vShaderStr[] =
	//	"precision mediump float;     \n"
	//	"attribute vec3 a_position; \n"
	//	"attribute vec3 a_colour; \n"
	//	//"attribute vec2 a_texCoord;   \n"
	//	"uniform mat4 MVP;            \n"
	//	//"varying vec2 v_texCoord;     \n"
	//	"varying vec3 v_colour;     \n"
	//	"void main()                  \n"
	//	"{ 							  \n"
	//	" gl_Position =  MVP * vec4(a_position,1);\n"
	//	//" v_texCoord = a_texCoord;  \n"
	//	"v_colour=a_colour;\n"

	//	"}                            \n";

	//GLbyte fShaderStr[] =
	//	"precision mediump float;                            \n"
	//	//"varying vec2 v_texCoord;                            \n"
	//	"varying vec3 v_colour;     \n"
	//	//"uniform sampler2D s_texture;                        \n"
	//	//"uniform vec4  Ambient;\n"
	//	"void main()                                         \n"
	//	"{                                                   \n"
	//	//"  gl_FragColor = texture2D( s_texture, v_texCoord )*Ambient;\n"
	//	"  gl_FragColor = vec4(v_colour,1);\n"
	//	//"  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
	//	"}                                                   \n";
	//	


	GLbyte vShaderStr[] =
		"precision mediump float;     \n"
		"attribute vec3 a_position; \n"
		"attribute vec2 a_texCoord;   \n"
		"varying vec2 v_texCoord;     \n"
		"uniform mat4 MVP;            \n"
		"void main()                  \n"
		"{ 							  \n"
		" gl_Position =  MVP * vec4(a_position,1);\n"
		" v_texCoord = a_texCoord;  \n"
		"}                            \n";

	GLbyte fShaderStr[] =
		"precision mediump float;                            \n"
		"varying vec2 v_texCoord;                            \n"
		"uniform sampler2D s_texture1;                        \n"
		"uniform sampler2D s_texture2;                        \n"
		"int blending_val;                        \n"
		"void main()                                         \n"
		"{                                                   \n"
		"  gl_FragColor = mix(texture2D( s_texture1, v_texCoord ),texture2D( s_texture2, v_texCoord ), blending_val );\n"
		"}                                                   \n";

	//WORKING CODE
	
	//GLbyte vShaderStr[] =
	//	"precision mediump float;     \n"
	//	"attribute vec3 a_position; \n"
	//	//"attribute vec3 a_colour; \n"
	//	"attribute vec2 a_texCoord;   \n"
	//	"uniform mat4 MVP;            \n"
	//	"varying vec2 v_texCoord;     \n"
	//	//"varying vec3 v_colour;     \n"
	//	"void main()                  \n"
	//	"{ 							  \n"
	//	" gl_Position =  MVP * vec4(a_position,1);\n"
	//	" v_texCoord = a_texCoord;  \n"
	//	//"v_colour=a_colour;\n"

	//	"}                            \n";

	//GLbyte fShaderStr[] =
	//	//"#version 300 es"
	//	"precision mediump float;                            \n"
	//	"varying vec2 v_texCoord;                            \n"
	//	//"varying vec3 v_colour;     \n"
	//	"uniform sampler2D s_texture;                        \n"
	//	//"uniform vec4  Ambient;\n"
	//	"void main()                                         \n"
	//	"{                                                   \n"
	//	//"  gl_FragColor = texture2D( s_texture, v_texCoord )*Ambient;\n"
	//	//"  gl_FragColor = vec4(v_colour,1.0);\n"
	//	"  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
	//	"}                                                   \n";

	// Load and compile the vertex/fragment shaders
   TheModel->vertexShader = LoadShader(GL_VERTEX_SHADER, (const char*)vShaderStr);
	TheModel->fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*)fShaderStr);
	
	// Create the program object	
		TheModel->programObject = glCreateProgram();
	if (TheModel->programObject == 0) 	return 0;
	
	// now we have the V and F shaders  attach them to the progam object
	glAttachShader(TheModel->programObject, TheModel->vertexShader);
	glAttachShader(TheModel->programObject, TheModel->fragmentShader);
	
	// Link the program
		glLinkProgram(TheModel->programObject);
	// Check the link status
		GLint AreTheylinked;
	glGetProgramiv(TheModel->programObject, GL_LINK_STATUS, &AreTheylinked);
	if (!AreTheylinked)
	{
		GLint RetinfoLen = 0;
		// check and report any errors
				glGetProgramiv(TheModel->programObject, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * RetinfoLen);
			glGetProgramInfoLog(TheModel->programObject, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(TheModel->programObject);
		return FALSE;
	}

	
	// Get the attribute locations
    TheModel->positionLoc = glGetAttribLocation(TheModel->programObject, "a_position");
	TheModel->colourLoc = glGetAttribLocation(TheModel->programObject, "a_colour");
	TheModel->texCoordLoc = glGetAttribLocation(TheModel->programObject, "a_texCoord");

	// Get the sampler location
	TheModel->samplerLoc = glGetUniformLocation(TheModel->programObject, "s_texture");


	
	if (glGetError() == GL_NO_ERROR)	return TRUE;
	else
		printf("Oh bugger, Model graphic init failed\n");
	return FALSE;
	
}



///
// Create a simple width x height texture image with RGBA format
//
GLuint Graphics::CreateSimpleTexture2D(int width, int height, char* TheData)
{
	// Texture object handle
	GLuint textureId;

	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generate a texture object
	glGenTextures(1, &textureId);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Load the texture

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		TheData);
	
	if (glGetError() != GL_NO_ERROR) printf("Oh bugger");

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	if (glGetError() == GL_NO_ERROR)	return textureId;
	printf("Oh bugger");

	return textureId;
}


void Graphics::GetRes(uint32_t &ReturnX, uint32_t &ReturnY)
{
	
//#ifdef RASPBERRY // raspberry dosn't really work well with xrandr.. so we simply return our current user set res
//	uint32_t width;
//	uint32_t height;
//	graphics_get_display_size(0, &width, &height);
//		
//	ReturnX =  width;
//	ReturnY  = height;
//	
//#endif

	// you will need to make sure you have  installed libXrandr to use this..
	//xhisudo apt-get install libxrandr-dev
		
	//
	//     CONNECT TO X-SERVER, GET ROOT WINDOW ID
	//
	//
	//     GET POSSIBLE SCREEN RESOLUTIONS
	//
	xrrs   = XRRSizes(x_display, 0, &num_sizes);
	//
	//     LOOP THROUGH ALL POSSIBLE RESOLUTIONS,
	//     GETTING THE SELECTABLE DISPLAY FREQUENCIES
	//
	
	
	for(int i = 0 ; i < num_sizes ; i++) {
		short   *rates;
		

		printf("\n\t%2i : %4i x %4i   (%4imm x%4imm ) ", i, xrrs[i].width, xrrs[i].height, xrrs[i].mwidth, xrrs[i].mheight);

		rates = XRRRates(x_display, 0, i, &num_rates);

		for (int j = 0; j < num_rates; j++) {
			possible_frequencies[i][j] = rates[j];
			printf("%4i ", rates[j]);
		}
	}

	printf("\n");
	
	
}
void Graphics::SetRes(int index, uint32_t &screenX, uint32_t& screenY)
{
	//
	//     Set RESOLUTION AND FREQUENCY
	//
	GetRes(screenX, screenY); // check what we have
// store the original info for and end
	conf                   = XRRGetScreenInfo(x_display, DefaultRootWindow(x_display));
	original_rate          = XRRConfigCurrentRate(conf);
	original_size_id       = XRRConfigCurrentConfiguration(conf, &original_rotation);

	printf("\n\tCURRENT SIZE ID  : %i\n", original_size_id);
	printf("\tCURRENT ROTATION : %i \n", original_rotation);
	printf("\tCURRENT RATE     : %i Hz\n\n", original_rate);
	// find 1280x720		
		
		//
		int SelectedRes = index;  // pretty much any system can handle 1024x768 
		printf("\tCHANGED TO %i x %i PIXELS, %i Hz\n\n", xrrs[SelectedRes].width, xrrs[SelectedRes].height, possible_frequencies[SelectedRes][0]);
	XRRSetScreenConfigAndRate(x_display, conf, DefaultRootWindow(x_display) , SelectedRes, RR_Rotate_0, possible_frequencies[SelectedRes][0], CurrentTime);
	screenX = xrrs[SelectedRes].width;
	screenY = xrrs[SelectedRes].height;
}

void Graphics::RestoreRes()
{
	printf("\tRESTORING %i x %i PIXELS, %i Hz\n\n", xrrs[original_size_id].width, xrrs[original_size_id].height, original_rate);
	XRRSetScreenConfigAndRate(x_display, conf, DefaultRootWindow(x_display), original_size_id, original_rotation, original_rate, CurrentTime);
	
}	

