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

#include "glslfx/pass.h"
#include "glslfx/glslfx.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <cassert>
#include <errno.h>
#include <sstream>

#ifdef WIN32
#	define _CRT_SECURE_NO_WARNINGS
#	include <windows.h>
#	include <yvals.h>
#endif

#if defined(_MSC_VER) && defined(_HAS_TR1)
#	define HAVE_TR1_REGEX 1
#endif

/* currently the status of TR1 regex in libstdc++ is suboptimal so it is
 * disabled for now */
#ifdef __GNUC__
#	undef HAVE_TR1_REGEX
#endif

#ifdef HAVE_TR1_REGEX
#	include <tr1/regex>
	using std::tr1::cmatch;
	using std::tr1::regex;
	using std::tr1::regex_search;
#else /* HAVE_TR1_REGEX */
#	define BOOST_REGEX_DYN_LINK /* boost::regex does not seem to link properly
	                             * when using the static library. Must force it
	                             * to use dynamic library. */
#	include <boost/regex.hpp>
	using boost::cmatch;
	using boost::regex;
	using boost::regex_search;
#endif /* HAVE_TR1_REGEX */

/**
 * Similar to strchr but looks for the first character matching fun.
 */
static const char* strchr_fun(const char* s, int(*fun)(int c)){
	const char* p = s;
	while ( *p && !fun(*p) ){
		p++;
	}

	/* to match strchr which return NULL if it wasn't found */
	if ( !*p ){
		return NULL;
	}

	return p;
}

/**
 * Get the termination character for an include path reference.
 * '"' -> '"'
 * '<' -> '>'
 * All other 0
 */
static char term(char ch){
	if ( ch == '"' ) return '"';
	if ( ch == '<' ) return '>';
	return 0;
}

/**
 * Extract the filename from a preprocessor include line.
 * The #-sign is expected to be stripped already, eg:
 * include <foo>
 */
static int get_include(const char* line, char** filename){
	/*
	 * delim ------+
	 *             v
	 *     include <foo>
	 *              ^ ^
	 *              | |
	 *  sp ---------+ |
	 *  ep -----------+
	 */

	char delim = *(line + 8);
	const char* sp = NULL;
	const char* ep = NULL;
	size_t size = 0;

	assert(line);
	assert(filename);

	/* reset pointer */
	*filename = NULL;

	/* ensure valid delimiter */
	if ( !(delim == '<' || delim == '"' ) ){
		return E_PARSE_ERROR;
	}

	/* calculate offsets */
	sp = line + 9;
	ep = strchr(sp, term(delim));
	size = ep - sp;

	/* copy filename */
	*filename = strndup(sp, size);

	return 0;
}

static int source_file(const effect* ep,
					   const pass* pass,
					   std::stringstream& stream,
					   const std::string& filename,
					   FILE* fp,
					   glslfx::log* log){

	int ret;
	char* line = NULL;   /* current line */
	size_t line_len = 0; /* length of current line */

	size_t line_nr = 0;
	ssize_t n;

	int emit_source; /* whenever to emit the sourceline or not (eg #include should not be emitted) */

	assert(ep);
	assert(pass);
	assert(fp);

	/* get handle to current file */
	path_handle_t path_handle;
	if ( ( ret = glslfx::path_store(filename, path_handle) ) != 0 ){
		return ret;
	}

	/* process each line */
	while ( ( n = getline(&line, &line_len, fp) ) != -1 ){
		char ch = *line;
		emit_source = 1;
		line_nr++;

		/* look for preprocessor commands */
		if ( ch == '#' ){
			const char* cmd = strchr_fun(line, isalpha);
			int emit_line = 0;

			/* include new file */
			if ( strncmp(cmd, "include", 7) == 0 ){
				char* refpath = NULL;
				std::string path;
				FILE* inc = NULL;

				/* skip source emit */
				emit_source = 0;

				/* get the path */
				if ( ( ret = get_include(cmd, &refpath) ) != 0 ){
					free(line);
					return ret;
				}

				/* resole path and try to open it */
				path = ep->resolve_path(refpath);
				inc = fopen(path.c_str(), "r");
				free(refpath);

				/* check if file exists */
				if ( !inc ){
					log->format(line_nr, 0, filename.c_str(), "error", "%s: No such file or directory", path.c_str());
					free(line);

					return ENOENT;
				}

				/* write new line marker (cannot do it for the root source since it expects #version to
				 * be the first line of the file */
				{
					path_handle_t inc_handle;
					if ( ( ret = glslfx::path_store(path, inc_handle) ) != 0 ){
						return ret;
					}
					stream << "#line 1 " << inc_handle << std::endl;
				}

				/* process sourcefile */
				if ( ( ret = source_file(ep, pass, stream, path, inc, log) ) != 0 ){
					free(line);
					return ret;
				}

				emit_line = 1;
			}

			/* emit a new #line */
			if ( emit_line ){
				stream << "#line " << line_nr << " " << path_handle << std::endl;
			}
		}

		/* output line */
		if ( emit_source ){
			stream << line;
		}
	}

	free(line);

	return 0;
}

static int source(const effect* ep,
		   const pass* pass,
		   const std::string& filename,
		   FILE* fp,
		   std::string& dst,
		   glslfx::log* log){

	std::stringstream stream;
	int ret;

	/* assert parameters */
	assert(ep);
	assert(pass);
	assert(fp);

	if ( ( ret = source_file(ep, pass, stream, filename, fp, log) ) != 0 ){
		return ret;
	}

	dst = stream.str();
	return 0;
}

static int parse_log(GLuint target, glslfx::log* log){
	GLint size;

	void (*query_func)(GLuint target, GLenum pname, GLint* param) = NULL;
	void (*get_func)(GLuint target, GLsizei maxLength, GLsizei* length, GLchar* infoLog) = NULL;

	/* setup function pointers depending on whenever we want the log for a
	 * shader object or shader program. */
	if ( glIsShader(target) ){
		query_func = glGetShaderiv;
		get_func = glGetShaderInfoLog;
	} else{
		query_func = glGetProgramiv;
		get_func = glGetProgramInfoLog;
	}

	/* get size of log */
	query_func(target, GL_INFO_LOG_LENGTH, &size);
	if ( size == 0 ){ /* no log */
		return 0;
	}

	/* allocate log buffer */
	std::auto_ptr<char> buffer((char*)malloc(size));
	if ( !buffer.get() ){
		return ENOMEM;
	}

	/* get log from driver */
	get_func(target, size, &size, buffer.get());

	/* push messages to log */
	char* ctx = NULL;
	char* line = strtok_r(buffer.get(), "\n", &ctx);
	while ( line ){
		/* hack for some strings which has initial whitespace */
		while ( *line && isspace(*line) ){
			line++;
		}

		/* if the line was only whitespace we may skip processing. */
		if ( !*line ){
			break;
		}

		/* result variables */
		cmatch res;             /* regexp result */
		int path_handle;        /* path handle */
		int line_nr;            /* line number */
		std::string ref;        /* error number */
		std::string tag;        /* error or warning */
		std::string message;    /* actual messsage */
		bool succeded = false;  /* whenever the parsing succeded or not */

		/* need to do different parsing depending on which vendor is used since
		 * they have different styles of the messages. */
		switch ( get_vendor() ){
			case VENDOR_ATI:
				/* ERROR: 0:19: error(#160) Cannot convert from '4-component vector of float' to 'default out mediump 2-component vector of float' */
				{
					/*                     TAG          line_nr         tag           ref              message       */
					regex rx("(?:[A-Z]+)\\: ([0-9]+)\\:([0-9]+)\\: ([a-zA-Z]+)[(]([a-zA-Z0-9#]+)[)] (.+)");
					if ( !regex_search(line, res, rx) ){
						break;
					}

					path_handle = atoi(res[1].str().c_str());
					line_nr = atoi(res[2].str().c_str());
					tag = res[3];
					ref = res[4];
					message = res[5];
					succeded = true;
				}
				break;

			case VENDOR_NVIDIA:
				/* 0(5) : warning C7533: global variable gl_ModelViewProjectionMatrix is deprecated after version 120 */
				{
					/*                       line_nr          tag           ref             message      */
					regex rx("0[(]([0-9]+)[)] \\: ([a-zA-Z]+) ([a-zA-Z0-9]+)\\: (.+)");
					if ( !regex_search(line, res, rx) ){
						break;
					}

					path_handle = atoi(res[1].str().c_str());
					line_nr = atoi(res[2].str().c_str());
					tag = res[3];
					ref = res[4];
					message = res[5];
					succeded = true;
				}
				break;

			case VENDOR_UNKNOWN:
			case VENDOR_OTHER:
				break;
		}

		if ( succeded ){
			std::string path;
			if ( glslfx::path_retrieve(path_handle, path) != 0 ){
				path = "<unknown>";
			}
			log->message(line_nr, path, tag, ref, message);
		} else {
			log->generic(line);
		}
		line = strtok_r(NULL, "\n", &ctx);
	}

	return 0;
}

static int compile(GLenum target, const std::string& src, GLuint& shader, glslfx::log* log){
	const char* src_ptr = src.c_str();

	printf("shader source:\n%s\n", src_ptr);

	/* compile */
	shader = glCreateShader(target);
	glShaderSource(shader, 1, &src_ptr, 0);
	glCompileShader(shader);

	/* if no log is provided nothing more needs to be done */
	if ( !log ){
		return 0;
	}

	return parse_log(shader, log);
}

pass::pass(const effect* ep, const std::string& name)
	: ep(ep)
	, _name(name)
	, _sp(0) {

	_layout.entry = NULL;
	_layout.stride = 0;
	_layout.n = 0;
}

pass::~pass(){

}

const std::string& pass::name() const {
	return _name;
}

int pass::find_entry(GLenum target, entry& dst) const {
	/* search for entry */
	const_iterator it = _shader.find(target);

	/* if no entry could be found dst is left alone and an errors is raised. */
	if ( it == _shader.end() ){
		return E_NOT_SET;
	}

	dst = it->second;
	return 0;
}

int pass::path(GLenum target, std::string& dst) const {
	entry tmp;
	int ret;

	/* search for entry */
	if ( ( ret = find_entry(target, tmp) ) != 0 ){
		return ret;
	}

	/* return path associated with this entry */
	dst.assign(tmp.path);
	return 0;
}

int pass::source(GLenum target, std::string& dst) const {
	entry tmp;
	int ret;

	/* search for entry */
	if ( ( ret = find_entry(target, tmp) ) != 0 ){
		return ret;
	}

	/* resolve path and try to open it */
	std::string path = ep->resolve_path(tmp.path);
	FILE* fp = fopen(path.c_str(), "r");

	/* check if the file exists */
	if ( !fp ){
		return ENOENT;
	}

	/* read source */
	return ::source(ep, this, path, fp, dst, 0);
}

GLint pass::program() const {
	/* not implemented */
	return 0;
}

void pass::bind(const GLvoid* vertices) const {
	static const pass* current = NULL;

	if ( current ){
		current->unbind();
	}

	glUseProgram(_sp);


	GLint mv = glGetUniformLocation(_sp, "mv");
	GLint p = glGetUniformLocation(_sp, "p");

	float buf[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, buf);
	glUniformMatrix4fv(mv, 1, GL_FALSE, buf);

	glGetFloatv(GL_PROJECTION_MATRIX, buf);
	glUniformMatrix4fv(p, 1, GL_FALSE, buf);

	for ( unsigned int i = 0; i < _layout.n; i++ ){
		const layout_entry& e = _layout.entry[i];

		glEnableVertexAttribArray(e.attrib);
		glVertexAttribPointer(e.attrib, e.components, e.type, GL_FALSE, _layout.stride, ((const char*)vertices) + e.offset);
	}

	current = this;
}

void pass::unbind() const {
	for ( unsigned int i = 0; i < _layout.n; i++ ){
		const layout_entry& e = _layout.entry[i];

		glDisableVertexAttribArray(e.attrib);
	}

	glUseProgram(0);
}

void pass::set_path(GLenum target, const std::string& path){
	entry tmp;
	tmp.path = path;
	tmp.shader = 0;

	_shader.insert(pair(target, tmp));
}

int pass::compile(log* log){
	int ret;

	/* compile all shaders */
	for ( iterator it = _shader.begin(); it != _shader.end(); ++it ){
		std::string src;

		if ( ( ret = source(it->first, src) ) != 0 ){
			return ret;
		}

		if ( ( ret = ::compile(it->first, src, it->second.shader, log) ) != 0 ){
			return ret;
		}
	}

	_sp = glCreateProgram();

	/* attach shaders to program */
	for ( iterator it = _shader.begin(); it != _shader.end(); ++it ){
		glAttachShader(_sp, it->second.shader);
	}

	/* and link */
	glLinkProgram(_sp);

	return parse_log(_sp, log);
}

bool pass::is_valid() const {
	/* early return */
	if ( _sp == 0 ){
		return false;
	}

	GLint status;

	/* check if driver consider the program valid */
	glValidateProgram(_sp);
	glGetProgramiv(_sp, GL_VALIDATE_STATUS, &status);

	return status == GL_TRUE;
}

int pass::set_layout(struct layout_desc_t* layout, size_t stride, size_t n){
	_layout.entry = new layout_entry[n];
	_layout.stride = stride;
	_layout.n = n;

	for ( unsigned int i = 0; i < n; i++ ){
		_layout.entry[i].attrib     = glGetAttribLocation(_sp, layout[i].name.c_str());
		_layout.entry[i].components = layout[i].components;
		_layout.entry[i].type       = layout[i].type;
		_layout.entry[i].offset     = layout[i].offset;
	}

	return 0;
}


