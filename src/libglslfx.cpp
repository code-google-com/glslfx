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

#include "glslfx/glslfx.h"
#include <cstring>
#include <map>

int glslfx_init(){
	return 0;
}

int glslfx_cleanup(){
	return 0;
}

namespace glslfx {
	static vendor_t g_vendor = VENDOR_UNKNOWN;

	typedef std::map<path_handle_t, std::string> path_map;
	typedef std::pair<path_handle_t, std::string> path_pair;
	typedef path_map::iterator path_iterator;

	static path_map g_paths;
	unsigned int g_path_counter = 0;

	vendor_t get_vendor(){
		/* if vendor is unknown try to guess */
		if ( g_vendor == VENDOR_UNKNOWN ){
			const char* vendor_id = (const char*)glGetString(GL_VENDOR);
			g_vendor = VENDOR_OTHER;

			if ( strcmp(vendor_id, "ATI Technologies Inc.") == 0 ){
				g_vendor = VENDOR_ATI;
			}

			if ( strcmp(vendor_id, "NVIDIA Corporation") == 0 ){
				g_vendor = VENDOR_NVIDIA;
			}
		}

		return g_vendor;
	}

	int path_store(const std::string& path, path_handle_t& handle){
		/* search if path is already stored */
		for ( path_iterator it = g_paths.begin(); it != g_paths.end(); ++it ){
			if ( it->second == path ){
				handle = it->first;
				return 0;
			}
		}

		/* store */
		handle = g_path_counter++;
		g_paths.insert(path_pair(handle, path));
		return 0;
	}

	int path_retrieve(const path_handle_t handle, std::string& path){
		path_iterator it = g_paths.find(handle);

		if ( it == g_paths.end() ){
			return E_NOT_FOUND;
		}

		path = it->second;
		return 0;
	}
}
