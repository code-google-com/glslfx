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

#ifndef __GLSL_FX_TECHNIQUE_H
#define __GLSL_FX_TECHNIQUE_H

#include <string>
#include <vector>

namespace glslfx {

	class technique {
	private:
		typedef std::vector<pass*> vector;

	public:
		typedef vector::const_iterator const_iterator;
		typedef vector::iterator iterator;

		~technique();

		const std::string& name() const;

		/**
		 * Creates a new pass for this technique.
		 * @param name Name of the pass.
		 */
		pass* pass_new(const std::string& name);

		/**
		 * Compile technique.
		 */
   		int compile(log* log);

		const_iterator pass_begin() const;
		const_iterator pass_end() const;
		iterator pass_begin();
		iterator pass_end();

		/**
		 * Set the vertex layout used by all passes. If they don't share the
		 * same layout, call set_layout on them manually.
		 * @param layout
		 * @param size
		 * @param n
		 */
		int set_layout(struct layout_desc_t* layout, size_t stride, size_t n);

		bool is_valid() const;

	private:
		friend class effect;

		technique(const effect* ep, const std::string& name);
		const effect* ep; /* owner */

		const std::string _name;
		vector _pass;
	};

}

#endif /* __GLSL_FX_TECHNIQUE_H */
