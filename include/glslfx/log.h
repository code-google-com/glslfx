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

#ifndef __GLSL_FX_LOG_H
#define __GLSL_FX_LOG_H

#include <string>
#include <vector>
#include <cstdarg>

namespace glslfx {

	class log {
	public:
		/**
		 * Entry structure. If generic is set to true the entry just contains
		 * a message and all the other fields is undefined.
		 */
		typedef struct {
			bool generic;         /* whenever this is a full log message or just
			                       * a generic message, in which case only the
			                       * message field is usable. */
			unsigned int line;    /* line the message refers to */
			std::string file;     /* file the message refers to */
			std::string severity; /* severity (error, warning etc) */
			std::string ref;      /* reference number (eg #C1234) */
			std::string message;  /* actual message */
		} entry;

	private:
		typedef std::vector<entry> vector;

	public:
		typedef vector::const_iterator const_iterator;
		typedef vector::iterator iterator;

		log();
		~log();

		const_iterator begin() const;
		const_iterator end() const;
		iterator begin();
		iterator end();

		/**
		 * Write a message to the log.
		 * @param line
		 * @param file
		 * @param severity
		 * @param ref
		 * @param message
		 */
		void message(unsigned int line,
					 const std::string& file,
					 const std::string& severity,
					 const std::string& ref,
					 const std::string& message);

		/**
		 * Write a printf-style formated message to the log.
		 * @param line
		 * @param file
		 * @param severity
		 * @param ref
		 * @param fmt
		 */
		void format(unsigned int line,
					const std::string& file,
					const std::string& severity,
					const std::string& ref,
					const char* fmt, ...);

		/**
		 * Write a printf-style formated message to the log.
		 * @param line
		 * @param file
		 * @param severity
		 * @param ref
		 * @param fmt
		 * @param ap
		 */
		void vformat(unsigned int line,
					 const std::string& file,
					 const std::string& severity,
					 const std::string& ref,
					 const char* fmt, va_list ap);

		/**
		 * Write a generic message to the log.
		 * @param message
		 */
		void generic(const std::string& message);


	private:
		vector _entries;
	};

}

#endif /* __GLSL_FX_LOG_H */
