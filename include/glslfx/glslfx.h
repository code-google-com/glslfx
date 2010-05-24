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

#ifndef __GLSL_FX_H
#define __GLSL_FX_H

/**
 * All functions return zero if successful or an error code if not.
 *
 * GLEW is assumed to be initialized before *any* usage.
 */

#include <GL/glew.h>
#include <GL/gl.h>
#include <glslfx/forward.h>
#include <glslfx/log.h>
#include <glslfx/pass.h>
#include <glslfx/technique.h>
#include <glslfx/effect.h>

/**
 */
int glslfx_init();

/**
 */
int glslfx_cleanup();

namespace glslfx {
	enum {
		/* errors relating to parsing of fx-files */
		E_PARSE_ERROR = -1,
		E_TOKEN_ERROR = -2,

		/* general errors */
		E_NOT_FOUND = -1001,
		E_NOT_SET   = -1002
	};

	enum vendor_t {
		VENDOR_UNKNOWN,

		VENDOR_ATI,
		VENDOR_NVIDIA,
		VENDOR_OTHER
	};

	/**
	 * Try to identify the current GPU vendor.
	 * @see vendor_t
	 */
	vendor_t get_vendor();

	typedef unsigned int path_handle_t;

	/**
	 * Store a path in the path database.
	 * @param path Path to store.
	 * @param handle Returns the handle.
	 */
	int path_store(const std::string& path, path_handle_t& handle);

	/**
	 * Retrieve a path from a handle.
	 * @param handle Handle to lookup.
	 * @param path Returns the path.
	 * @return
	 */
	int path_retrieve(const path_handle_t handle, std::string& path);
}

#endif /* __GLSL_FX_H */
