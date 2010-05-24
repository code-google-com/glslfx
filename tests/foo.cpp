#include <GL/glew.h>
#include <glslfx/glslfx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>

/**
 * Resolve paths by finding the project root and concat with path.
 * @param name path based from project root
 * @return new path
 */
char* src(const char* name){
	char* srcdir = getenv("srcdir");
	char* buf;
	if ( asprintf(&buf, "%s/tests/%s", srcdir, name) < 0 ){
		return 0;
	}

	return buf;
}

/* simple vector structure */
typedef struct vector3f_t {
	float x;
	float y;
	float z;
} vector3f;

/* sample vertex */
struct vertex_t {
	vector3f pos;
	vector3f normal;
};

/* sample layout */
static glslfx::layout_desc layout[] = {
	{"pos",    3, GL_FLOAT, offsetof(vertex_t, pos)},
	{"normal", 3, GL_FLOAT, offsetof(vertex_t, normal)},
};

/* sample object */
static const struct vertex_t vertices[4] = {
	{{-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
	{{-1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}}
};
static const unsigned int indices[4] = {
	0, 1, 2, 3
};

/* selected technique */
glslfx::technique* tech = NULL;

/**
 * Render callback
 */
void render(){
	/* clean surface */
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/* reset modelview matrix */
	glLoadIdentity();

	/* setup view matrix */
	gluLookAt(
			  10.0f, 10.0f, 10.0f,
			   0.0f,  0.0f,  0.0f,
			   0.0f,  1.0f,  0.0f
			  );

	/* iterate all passes and render the scene once for each pass */
	for ( glslfx::technique::iterator it = tech->pass_begin(); it != tech->pass_end(); ++it){
		glslfx::pass* p = *it;

		/* bind the shader program and setup the neccesary attribute pointers. */
		p->bind(vertices);

		/* render vertices */
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, indices);
	}

	/* flip buffer */
	SDL_GL_SwapBuffers();
}

void input(bool& run){
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			if ( event.key.keysym.sym == SDLK_ESCAPE ){
				run = false;
			}
			break;
		case SDL_QUIT:
			run = false;
			break;
		}
	}
}

void setup(){
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(1,0,1,1);
}

void resize(int width, int height){
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, ((GLfloat)width) / height, 0.001, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, const char* argv[]){
	int ret = 0;
	int width = 800, height = 600;
	bool run = false;

	glslfx::effect ep(src("simple.glslfx"));
	glslfx::log log;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(width, height,0,SDL_OPENGL|SDL_DOUBLEBUF);

	glewInit();

	setup();
	resize(width, height);

	/* initialize library */
	glslfx_init();

	/* parse effect */
	if ( ep.parse() != 0 ){
		ret = -1;
		goto error;
	}

	/* compile all techniques and their passes */
	if ( ep.compile(&log) != 0 ){
		ret = -1;
		goto error;
	}

	/* get one of the techniques */
	if ( ( tech = ep.technique_get("simple") ) == NULL ){
		ret = -1;
		goto error;
	}

	/* make sure the effect is valid */
	if ( !ep.is_valid() ){
		ret = -1;
		goto error;
	}

	/* set sample layout */
	ep.set_layout(layout, sizeof(vertex_t), 2);

	run = true;
	while ( run ){
		render();
		input(run);
	}

 error:
	/* show log messages */
	for ( glslfx::log::iterator it = log.begin(); it != log.end(); ++it ){
		glslfx::log::entry entry = *it;

		if ( entry.generic ){
			fprintf(stderr, "%s\n", entry.message.c_str());
		} else {
			fprintf(stderr, "%s:%d %s %s: %s\n",
					entry.file.c_str(), entry.line,
					entry.severity.c_str(), entry.ref.c_str(),
					entry.message.c_str());
		}
	}

	/* library cleanup */
	glslfx_cleanup();

	SDL_Quit();
	return ret;
}
