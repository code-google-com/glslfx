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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "glslfx/technique.h"
#include "glslfx/effect.h"
#include "glslfx/pass.h"

technique::technique(const effect* ep, const std::string& name)
	: ep(ep)
	, _name(name) {

}

technique::~technique(){
	for ( iterator it = _pass.begin(); it != _pass.end(); ++it ){
		delete (*it);
	}
}

const std::string& technique::name() const {
	return _name;
}

int technique::compile(log* log){
	int ret;

	for ( iterator it = pass_begin(); it != pass_end(); ++it ){
		pass* p = *it;
		if ( ( ret = p->compile(log) ) != 0 ){
			return ret;
		}
	}

	return 0;
}

technique::const_iterator technique::pass_begin() const {
	return _pass.begin();
}

technique::const_iterator technique::pass_end() const {
	return _pass.end();
}

technique::iterator technique::pass_begin() {
	return _pass.begin();
}

technique::iterator technique::pass_end() {
	return _pass.end();
}

pass* technique::pass_new(const std::string& name){
	pass* tmp = new pass(ep, name);
	_pass.push_back(tmp);
	return tmp;
}

bool technique::is_valid() const {
	for ( const_iterator it = pass_begin(); it != pass_end(); ++it ){
		pass* p = *it;
		if ( !p->is_valid() ){
			return false;
		}
	}

	return true;
}

int technique::set_layout(layout_desc* layout, size_t stride, size_t n){
	int ret;

	for ( iterator it = pass_begin(); it != pass_end(); ++it ){
		pass* p = *it;
		if ( ( ret = p->set_layout(layout, stride, n) ) != 0 ){
			return ret;
		}
	}

	return 0;
}
