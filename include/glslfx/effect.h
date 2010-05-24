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

#ifndef __GLSL_FX_EFFECT_H
#define __GLSL_FX_EFFECT_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <map>
#include <vector>
#include <string>

namespace glslfx {

	/**
	 * Vertex layout description.
	 */
	typedef struct layout_desc_t {
		std::string name; /* name of component (must match in shader pass) */
		GLint components; /* number of components. 1-4 */
		GLenum type;      /* datatype of attribute */
		off_t offset;     /* offset in struct */
	} layout_desc;

	/**
	 * Effect class, a representation of an fx-file.
	 */
	class effect {
		private:
			/* technique storage */
			typedef std::pair<std::string, technique*> pair;
			typedef std::map<std::string, technique*> map;

			/* filename-table storage */
			typedef std::pair<std::string, unsigned int> file_entry;
			typedef std::vector<file_entry> file_table;

		public:
			typedef map::const_iterator const_iterator;
			typedef map::iterator iterator;

			effect(const std::string& filename);
			~effect();

			/**
			 * Parse effect file.
			 */
			int parse();

			/**
			 * Compiles the effect shaders, if log is present (non-null) validation report is written to it.
			 */
			int compile(log* log);

			const std::string& filename() const;
			const std::string& dirref() const;

			/**
			 * Resolve a path referenced in an effect.
			 */
			std::string resolve_path(const std::string& in) const;

			/**
			 * Create a new technique.
			 */
			technique* technique_new(const std::string& name);

			/**
			 * Get an existing technique by name.
			 */
			technique* technique_get(const std::string& name);

			const_iterator technique_begin() const;
			const_iterator technique_end() const;
			iterator technique_begin();
			iterator technique_end();

			/**
			 * Set the vertex layout used by all techniques and passes. If they
			 * don't share the same layout, call set_layout on them manually.
			 * @param layout
			 * @param stride size of a single vertex (in bytes)
			 * @param n
			 * @return
			 */
			int set_layout(layout_desc* layout, size_t stride, size_t n);

			/**
			 * Tells whenever the effect is valid (it is considered valid if all the techniques and passes
			 * are considered valid.
			 */
			bool is_valid() const;

		private:
			int parse_fx(FILE* fp);

			std::string _filename; /* filename of the effect */
			std::string _dirref;   /* the directory the filename resides in and all paths referenced in the
									* effect are relative to. */

			map _techniques;
			file_table _file_table;
	};

}

#endif /* __GLSL_FX_EFFECT_H */
