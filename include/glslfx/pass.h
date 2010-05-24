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

#ifndef __GLSL_FX_PASS_H
#define __GLSL_FX_PASS_H

#include <glslfx/log.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <map>

namespace glslfx {

	class pass {
	private:
		typedef struct {
			std::string path;
			GLuint shader;
		} entry;
		typedef std::pair<GLenum, entry> pair;
		typedef std::map<GLenum, entry> map;
		typedef map::const_iterator const_iterator;
		typedef map::iterator iterator;

	public:
		~pass();

		/**
		 * Get the name of the pass.
		 */
		const std::string& name() const;

		/**
		 * Get the (resolved) path to the root source for a given shader.
		 * @param target Which shader to get path from.
		 * @param dst Output
		 */
		int path(GLenum target, std::string& dst) const;

		/**
		 * Get the processed source for a given shader.
		 * @param target Which shader to get source from.
		 * @param dst Output
		 */
		int source(GLenum target, std::string& dst) const;

		/**
		 * Get the OpenGL handle to the shader program, or 0 if the program
		 * hasn't been properly compiled yet.
		 */
		GLint program() const;

		/**
		 * Bind the shader program and its layout.
		 * @param vertices A pointer to a stream of vertices.
		 */
		void bind(const GLvoid* vertices) const;

		/**
		 * Unbind the effect, eg glUseProgram(0)
		 */
		void unbind() const;

		/**
		 * Set the path to a shader pass.
		 * @param target Which shader to set.
		 * @param path
		 */
		void set_path(GLenum target, const std::string& path);

		/**
		 * Compile shader program.
		 */
   		int compile(log* log);

		/**
		 * Set the vertex layout.
		 * @param layout Pointer to an array of layout descriptions.
		 * @param Size of the vertex structure, eg a single vertex.
		 * @param n Size of layout array,
		 */
		int set_layout(struct layout_desc_t* layout, size_t stride, size_t n);

		/**
		 * Tell if a shader program is valid or not, it is considered valid if no errors were produced
		 * during compilation.
		 */
		bool is_valid() const;

	private:
		friend class technique;

		typedef struct {
			GLint attrib;     /* shader attribute index */
			GLint components; /* number of components. 1-4 */
			GLenum type;      /* datatype of attribute */
			off_t offset;     /* offset in struct */
		} layout_entry;

		pass(const effect* ep, const std::string& name);

		/**
		 * Find an entry in the map. Returns 0 on success or error code.
		 */
		int find_entry(GLenum target, entry& dst) const;

		const effect* ep; /* owner */

		const std::string _name; /* name of the pass */
		map _shader;             /* map of shader resouces */

		GLuint _sp;              /* shader program */

		struct {
			layout_entry* entry;
			size_t stride;
			size_t n;
		} _layout;   /* vertex layout */
	};
};

#endif /* __GLSL_FX_PASS_H */
