/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cctype>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "./header.hpp"
#include "./functions.cpp"
#include "./structures.cpp"
#include "./datatypes.cpp"

#include "./cxa_demangle.cpp"

namespace gcc_demangler
{
	std::string demangle(std::string str)
	{
		int status;
		const char* in = str.c_str();
		char* out = abi::__cxa_demangle(in,0,0,&status);
		std::string result(out);
		return result;
	}
}

namespace cxx_demangler
{	
	/* MangledName:
		'?' BasicName Qualification '@' QualifiedTypeCode StorageClass
		'?' BasicName '@' UnqualifiedTypeCode StorageClass
	*/
	std::string parseMangledName(std::string &str){
		
if (DO_DEBUG)	debug("parsing mangled name...\t",str);
		std::string out = "";
		if(!consume(str,"?")) return NULL;

		basicName bN;
		unqualifiedTypeCode uTC;
		storageClass sC;
		qualification q;
		qualifiedTypeCode qTC;

		bN.parse(str,global_backref);
		std::string bn = bN.toString();
if (DO_DEBUG)	debug("basicname:",bn);
		
		if(consume(str,"@"))
		{
			//basicName, scope, unqualifiedtypecode, storageclass
			uTC.parse(str);
			
			sC.parse(str);
			
			out = out
			.append(
				uTC.toString
				(
					bn,
					sC.toString()
				)
			)
			.append(sC.getPostfix())
			;
			
			if(GCC_MANGLE) out = bN.toGCC().append(uTC.toGCC());
		}
		else
		{
			//basicName, scope, qualification, qualifiedtypecode, storageclass
			//scope = "public:";

			q.parse(str,global_backref);
			consume(str,"@");
			
if (DO_DEBUG)		debug("parsing qtc:\t",str);
			qTC.parse(str);

if (DO_DEBUG)		debug("parsing storage class:\t",str);
			sC.parse(str);
			
			if(str_match(bn,"\\q")) bn = q.contents[0];
		else	if(str_match(bn,"~\\q")) bn = std::string("~").append(q.contents[0]);
			
			if(!GCC_MANGLE)
			{
				out = out
				.append(qTC.getModifier())
				.append(" ")
				.append(
					qTC.toString
					(
						q.toString(global_backref).append("::").append(bn),
						sC.toString()
					)
				)
				.append(sC.getPostfix())
				;
			}
			if(GCC_MANGLE && bN.hasOperator != 2) out = std::string("N").append(q.toGCC(global_backref)).append(bN.toGCC()).append("E").append(qTC.toGCC());
			else if(GCC_MANGLE) out = bN.toGCC().append(std::string("N")).append(q.toGCC(global_backref)).append("E").append(qTC.toGCC());
		}

		global_backref.clear();
if (DO_DEBUG)	std::cout << "mangled-name end\n\n";
		out = replace(rmws(trim(out)),">>","> >");
		out = replace(out,"^int","");
		if(GCC_MANGLE && !NESTED) out = std::string("_Z").append(out);
		
		return out;
	}

	std::string demangle(std::string str,int code){
		//0 - Demangle MSVC as prototype.
		//1 - Demangle MSVC, remangle as GCC.
		std::string solution;
		
		if(code==1) GCC_MANGLE = 1;
		
		if(!code)
		{
			solution = cxx_demangler::read_line();
			solution = rmws(trim(solution));
			std::cout
			<< "Soln:\t"
			<< solution
			<< std::endl;
		}
		
		global_backref.clear();
		
		std::string s = parseMangledName(str);
		
		GCC_MANGLE = 0;

		if(!code)
		{	int cc = eq(s,solution);
			if(!cc)
			{
				std::cout << "Out:\t" << s << std::endl;
				if(str.length() > 0) std::cout << "str remains: " << str << std::endl;
				std::cout << "Results do not match. (" << s.length() << ":" << solution.length() << ")" << std::endl;
				exit(1);
			}
		}
		
		if(str.length() > 0) std::cout << "str remains: " << str << std::endl;
		return s;
	}
}

int main(int argc, char** argv){
	std::cout << "Enter Query." << std::endl;
	int i = 1;
	
	int start = 0;
	int stop = 256;
	
	while(1)
	{
		std::string input, input2;
		std::cin >> input;
		input2 = input;
		
		if(i>stop) break; //debugging stop-code; nothing ever begins with "Z"
		
		if(i<start)
		{
			i++;
			std::string x = cxx_demangler::read_line();
			continue;
		} //debugging skip-code; nothing ever begins with "Y"
		
		for(int j = 0; j < 100; j++) std::cout << "#";
		std::cout << std::endl;
		
		std::cout << i << ":" << std::endl;
		
		std::cout << "In:\t" << input << std::endl;
		std::string result = cxx_demangler::demangle(input,0);
		std::string result2 = cxx_demangler::demangle(input2,1);
		std::cout << "Out:\t" << result << std::endl;
		std::cout << "GCC:\t" << result2 << std::endl;
		std::cout << "GCC_DM:\t" << gcc_demangler::demangle(result2) << std::endl << std::endl;
		i++;
	}
	return 0;
}
