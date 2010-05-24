#include <GL/glslfx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* src(const char* name){
	char* srcdir = getenv("srcdir");
	char* buf;
	if ( asprintf(&buf, "%s/tests/%s", srcdir, name) < 0 ){
		return 0;
	}

	return buf;
}

int main(int argc, const char* argv[]){
	glslfx_init();

	struct glslfx_t* ep = 0;

	if ( glslfx_open(src("simple.glslfx"), &ep) != 0 ){
		return -1;
	}

	glslfx_technique tech = NULL;
	while ( ( tech = glslfx_enumerate_technique(ep, tech) ) ){
		glslfx_pass pass = NULL;
		printf("Technique (%s)\n", glslfx_technique_get_name(tech));

		while ( ( pass = glslfx_enumerate_pass(tech, pass) ) ){
			printf("\tPass (%s)\n", glslfx_pass_get_name(pass));
			printf("\t\tVertex: %s\n", glslfx_pass_get_path(pass, GL_VERTEX_SHADER));
			printf("\t\tGeometry: %s\n", glslfx_pass_get_path(pass, GL_GEOMETRY_SHADER));
			printf("\t\tFragment: %s\n", glslfx_pass_get_path(pass, GL_FRAGMENT_SHADER));
		}
	}

	glslfx_destroy(&ep);

	glslfx_cleanup();
	return 0;
}
