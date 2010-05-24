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
#include "glslfx/log.h"
#include "glslfx/technique.h"
#include <cstdio>
#include <errno.h>

effect::effect(const std::string& filename)
	: _filename(filename) {

	/* setup dirref */
	{
		size_t s = filename.find_last_of('/');
		if ( s == std::string::npos ){
			_dirref = ".";
		} else {
			_dirref = filename.substr(0, s);
		}
	}
}

effect::~effect(){

}

int effect::parse(){
	/* open file */
	FILE* fp = fopen(_filename.c_str(), "r");

	/* see if file actaully exists */
	if ( !fp ){
		return ENOENT;
	}

	/* parse fx-file */
	int ret;
	if ( ( ret = parse_fx(fp) ) != 0 ){
		return ret;
	}

	return 0;
}

int effect::compile(log* log){
	int ret;

	for ( iterator it = technique_begin(); it != technique_end(); ++it ){
		technique* tech = it->second;
		if ( ( ret = tech->compile(log) ) != 0 ){
			return ret;
		}
	}

	return 0;
}

const std::string& effect::filename() const {
	return _filename;
}

const std::string& effect::dirref() const {
	return _dirref;
}

std::string effect::resolve_path(const std::string& in) const {
	return _dirref + "/" + in;
}

technique* effect::technique_get(const std::string& name) {
	iterator it = _techniques.find(name);
	if ( it == _techniques.end() ){
		return NULL;
	}

	return it->second;
}

effect::const_iterator effect::technique_begin() const {
	return _techniques.begin();
}

effect::const_iterator effect::technique_end() const {
	return _techniques.end();
}

effect::iterator effect::technique_begin(){
	return _techniques.begin();
}

effect::iterator effect::technique_end(){
	return _techniques.end();
}

technique* effect::technique_new(const std::string& name){
	technique* tmp = new technique(this, name);
	_techniques.insert(pair(name, tmp));
	return tmp;
}

bool effect::is_valid() const {
	for ( const_iterator it = technique_begin(); it != technique_end(); ++it ){
		technique* tech = it->second;
		if ( !tech->is_valid() ){
			return false;
		}
	}

	return true;
}

int effect::set_layout(layout_desc* layout, size_t stride, size_t n){
	int ret;

	for ( iterator it = technique_begin(); it != technique_end(); ++it ){
		technique* tech = it->second;
		if ( ( ret = tech->set_layout(layout, stride, n) ) != 0 ){
			return ret;
		}
	}

	return 0;
}
