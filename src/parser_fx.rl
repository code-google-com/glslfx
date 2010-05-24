/**
 * Copyright (c) 2010, David Sveningsson <ext-glslfx@sidvind.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif /* HAVE_CONFIG_H */

#include "glslfx/effect.h"
#include "glslfx/glslfx.h"
#include <GL/gl.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>

#define BUFLEN 1024

struct state_t {
	char buffer[BUFLEN+1];
	int buflen;
	int cs;
	int top;
	int act;
	char* ts;
	char* te;

	int* stack;
	int stack_size;

	GLenum program_type;

	technique* cur_tech;
	pass* cur_pass;
};


%%{
	machine fx_parser;
	access fsm->;

	prepush {
		if ( fsm->top + 1 > fsm->stack_size ){
			fsm->stack_size += 5;
			fsm->stack = (int*)realloc(fsm->stack, fsm->stack_size * sizeof(int));
		}
	}

    # Append to the buffer.
	action append {
		if ( fsm->buflen < BUFLEN ){
			fsm->buffer[fsm->buflen++] = fc;
		}
	}

	# Terminate a buffer.
	action term {
		if ( fsm->buflen < BUFLEN )
			fsm->buffer[fsm->buflen++] = 0;
	}

	action term_program_type {
		if ( fsm->buflen < BUFLEN )
			fsm->buffer[fsm->buflen++] = 0;
		if ( strcmp(fsm->buffer, "vertex") == 0 ){
			fsm->program_type = GL_VERTEX_SHADER;
		}
		if ( strcmp(fsm->buffer, "geometry") == 0 ){
			fsm->program_type = GL_GEOMETRY_SHADER;
		}
		if ( strcmp(fsm->buffer, "fragment") == 0 ){
			fsm->program_type = GL_FRAGMENT_SHADER;
		}
	}

	action clear { fsm->buflen = 0; }

	name = alnum+ >clear $append %term;
	file_unquoted = [^\n \t;]+ >clear $append %term;
	file_quoted =  [^'"]* >clear $append %term;
	file = ( ['"] file_quoted ['"] | file_unquoted );
    program_type = ('vertex'|'fragment'|'geometry') >clear $append %term_program_type;
	string = [^0]+ >clear $append %term;
	# name = [a-zA-Z]+;

 pass := |*
	'}' => { fret; };
space;
program_type ':' space* file => {
	fsm->cur_pass->set_path(fsm->program_type, fsm->buffer);
};
	 *|;

 technique := |*
	 space;
'pass' space+ name space+ '{' => {
	pass* pass = fsm->cur_tech->pass_new(std::string(fsm->buffer));
	fsm->cur_pass = pass;

	fcall pass;
};
	'}' => { fret; };
	*|;

main := (
		 space* 'technique' space+ name space+ '{' @{
			 technique* tech = ep->technique_new(std::string(fsm->buffer));
			 fsm->cur_tech = tech;
			 fcall technique;
			 printf("tech fin\n");
		 }
		 )+;
}%%

%% write data;

static int parse_fx_int(FILE* fp, effect* ep, struct state_t* fsm){
	char buf[BUFLEN] = {0};
	int have = 0;
	int done = 0;

	assert(fp);
	assert(fsm);

	fsm->buflen = 0;

	%% write init;

	while ( !done ){
		char *p = buf + have, *pe, *eof = 0;
		int len, space = BUFLEN - have;

		if ( space == 0 ){
			printf("token error\n");
			return E_TOKEN_ERROR;
		}

		len = fread(p, 1, space, fp);
		pe = p + len;

		/* Check if this is the end of file. */
		if ( len < space ) {
			eof = pe;
			done = 1;
		}
                        
		%% write exec;

		if ( fsm->cs == fx_parser_error ) {
			printf("parse error\n");
			return E_PARSE_ERROR;
		}

		if ( fsm->ts == 0 )
			have = 0;
		else {
			/* There is a prefix to preserve, shift it over. */
			have = pe - fsm->ts;
			memmove( buf, fsm->ts, have );
			fsm->te = buf + (fsm->te - fsm->ts);
			fsm->ts = buf;
		}

	}

	return 0;
}

int effect::parse_fx(FILE* fp){
     struct state_t fsm;
     assert(fp);

     fsm.stack = NULL;
     fsm.stack_size = 0;
     return parse_fx_int(fp, this, &fsm);
}
