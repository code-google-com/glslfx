#include "glslfx/glslfx.h"
#include <GL/glew.h>
#include <GL/glx.h>
#include <cstdio>
#include <cstring>

static int level = 1; /* verbosity level: 0-2 where 0 is quiet */
static enum {
	VALIDATE,
	ENUMERATE
} mode = VALIDATE;

#ifndef TESS_EVALUATION_SHADER
#	define TESS_EVALUATION_SHADER 0x8E87
#	define TESS_CONTROL_SHADER 0x8E88
#endif

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef Bool (*glXMakeContextCurrentARBProc)(Display*, GLXDrawable, GLXDrawable, GLXContext);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
glXMakeContextCurrentARBProc glXMakeContextCurrentARB = 0;

int parse(const char* path){
	int ret;

	glslfx::log log;
	glslfx::effect ep(path);

	if ( ( ret = ep.parse() ) != 0 ){
		fprintf(stderr, "Failed to parse '%s'.\n", path);
		return ret;
	}

	if ( ( ret = ep.compile(&log) ) != 0 ){
		fprintf(stderr, "Failed to parse '%s'.\n", path);
		return ret;
	}

	return 0;
}

int enumerate(const char* path){
	int ret;

	glslfx::log log;
	glslfx::effect ep(path);

	if ( ( ret = ep.parse() ) != 0 ){
		fprintf(stderr, "Failed to parse '%s'.\n", path);
		return ret;
	}

	for ( glslfx::effect::const_iterator it = ep.technique_begin(); it != ep.technique_end(); ++it ){
		const glslfx::technique* tech = it->second;

		printf("technique '%s'\n", tech->name().c_str());

		for ( glslfx::technique::const_iterator jt = tech->pass_begin(); jt != tech->pass_end(); ++jt ){
			const glslfx::pass* p = *jt;

			printf("  pass '%s'\n", p->name().c_str());

			GLenum stage[5] = { GL_VERTEX_SHADER, TESS_EVALUATION_SHADER, TESS_CONTROL_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };
			const char* name[5] = { "vertex", "evaluation", "control", "geometry", "fragment" };
			for ( unsigned int i = 0; i < 5; i++ ){
				std::string path;
				if ( p->path(stage[i], path) == 0 ){
					printf("    %s:\t\"%s\"\n", name[i], path.c_str());
				} else {
					printf("    %s:\tnot defined\n", name[i]);
				}
			}

		}
	}

	return 0;
}

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension){
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if ( where || *extension == '\0' )
		return false;

	/* It takes a bit of care to be fool-proof about parsing the
	 OpenGL extensions string. Don't be fooled by sub-strings,
	 etc. */
	for ( start = extList; ; ) {
		where = strstr( start, extension );

		if ( !where )
			break;

		terminator = where + strlen( extension );

		if ( where == start || *(where - 1) == ' ' )
			if ( *terminator == ' ' || *terminator == '\0' )
				return true;

		start = terminator;
	}

	return false;
}



int opengl_init(){
	Display* dpy = XOpenDisplay(0);
	if ( !dpy ){
		fprintf(stderr, "Failed to open display\n");
		return 1;
	}

	const char *glxExts = glXQueryExtensionsString(dpy, DefaultScreen(dpy));
	if ( !isExtensionSupported(glxExts, "GLX_ARB_create_context") ){
		fprintf(stderr, "GLX_ARB_create_context is not supported!\n");
		return 1;
	}

	static int visual_attribs[] = {
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
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
		None
	};

	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visual_attribs, &fbcount );
	if ( !fbc ){
		fprintf(stderr, "Failed to get FBConfig\n");
		return 1;
	}

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");
	glXMakeContextCurrentARB = (glXMakeContextCurrentARBProc)glXGetProcAddressARB( (const GLubyte *) "glXMakeContextCurrent");

	if ( !(glXCreateContextAttribsARB && glXMakeContextCurrentARB) ){
		fprintf(stderr, "missing support for GLX_ARB_create_context\n");
		XFree(fbc);
		return 1;
	}

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		//GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};

	GLXContext ctx = glXCreateContextAttribsARB(dpy, fbc[0], 0, True, context_attribs );
	if ( !ctx ){
		fprintf(stderr, "Failed to create opengl context\n");
		XFree(fbc);
		return 1;
	}

	XSync(dpy, False);

	if ( !glXMakeContextCurrent(dpy, 0, 0, ctx) ){
		/* some drivers does not support context without default framebuffer, so fallback on
		 * using the default window.
		 */
		if ( !glXMakeContextCurrent(dpy, DefaultRootWindow(dpy), DefaultRootWindow(dpy), ctx) ){
			fprintf(stderr, "failed to make current\n");
			XFree(fbc);
			return 1;
		}
	}

	printf("vendor: %s\n", (const char*)glGetString(GL_VENDOR));

	GLenum err = glewInit();

	if (GLEW_OK != err){
		fprintf(stderr, "failed to initialize glew\n");
		XFree(fbc);
		return 1;
	}

	XFree(fbc);
	return 0;
}

void show_usage(){
	fprintf(stdout, "Usage: glslfx-validator [OPTION] FILE...\n");
	fprintf(stdout, "Parses and .glslfx-file and validates each technique/passes\n");
	fprintf(stdout, "\n");

	fprintf(stdout, "  -q, --quiet\tdo not show any output, only set return code.\n");
	fprintf(stdout, "  -v, --verbose\tin addition to warnings and errors, show eventual other\n"
	                "\t\tmessages from the driver.\n");
	fprintf(stdout, "      --enumerate\tenumerate the techniques and passes in an effect.\n");
	fprintf(stdout, "  -h, --help\tdisplay this help and exit.\n");
}

int run(int argc, const char* argv[]){
	int ret = 0;

	for ( int i = 1; i < argc; i++ ){
		const char* arg = argv[i];

		if ( arg[0] == '-' ){ /* parse flags */
			const char* long_flag;

			switch ( arg[1] ){
				case 'q': level = 0; break;
				case 'v': level = 0; break;
				case '-': /* long flags */
					long_flag = &arg[2];
					if ( strcmp(long_flag, "quiet") == 0){
						level = 0;
						continue;
					}

					if ( strcmp(long_flag, "verbose") == 0){
						level = 2;
						continue;
					}

					if ( strcmp(long_flag, "enumerate") == 0 ){
						mode = ENUMERATE;
						continue;
					}

					/* fallthrough */

				default:
					fprintf(stderr, "glslfx-validator: unrecognized option '%s'\n", arg);
					fprintf(stderr, "Try `glsl-validator --help' for more information\n");
					return 1;
			}
		} else { /* path */
			switch ( mode ){
				case VALIDATE: ret |= parse(arg); break;
				case ENUMERATE: ret |= enumerate(arg); break;
			}

			mode = VALIDATE; /* reset mode */
		}
	}

	return ret;
}

int main(int argc, const char* argv[]){
	int ret = 0;

	if ( argc == 1 ){
		show_usage();
		return 1;
	}

	if ( ( ret = opengl_init() ) != 0 ){
		return ret;
	}

	if ( ( ret = glslfx_init() ) != 0 ){
		return ret;
	}

	ret = run(argc, argv);

	glslfx_cleanup();

	return ret;
}
