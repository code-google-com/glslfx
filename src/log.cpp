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

#include "glslfx/log.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

log::log(){

}

log::~log(){

}

log::const_iterator log::begin() const{
	return _entries.begin();
}

log::const_iterator log::end() const{
	return _entries.end();
}

log::iterator log::begin(){
	return _entries.begin();
}

log::iterator log::end(){
	return _entries.end();
}

void log::message(unsigned int line,
				  const std::string& file,
				  const std::string& severity,
				  const std::string& ref,
				  const std::string& message){
	entry tmp;
	tmp.generic = false;
	tmp.line = line;
	tmp.file = file;
	tmp.severity = severity;
	tmp.ref = ref;
	tmp.message = message;

	_entries.push_back(tmp);
}

void log::format(unsigned int line,
				 const std::string& file,
				 const std::string& severity,
				 const std::string& ref,
				 const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vformat(line, file, severity, ref, fmt, ap);
	va_end(ap);
}

void log::vformat(unsigned int line,
				  const std::string& file,
				  const std::string& severity,
				  const std::string& ref,
				  const char* fmt, va_list ap){
	char* tmp = NULL;

	/* create string */
	if ( vasprintf(&tmp, fmt, ap) == -1 ){
		return; /* @todo handle ENOMEM*/
	}

	message(line, file, severity, ref, tmp);
	free(tmp);
}

void log::generic(const std::string& message){
	entry tmp;
	tmp.generic = true;
	tmp.line = 0;
	tmp.file = "";
	tmp.severity = "";
	tmp.ref = "";
	tmp.message = message;

	_entries.push_back(tmp);
}
