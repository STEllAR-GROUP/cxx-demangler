/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{
	std::string checkExceptions(std::string s,std::string &str)
	{
		if(str_match(s,"P6")) //Function Pointer
		{
/*			functionTypeCode ftc;
			ftc.parse(str);
			return std::string("(").append(ftc.toString("myname")).append(")");
*/			
			return "P6";
			
			std::string callingConvention = getNextCallingConvention(str);
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString("","");//hack
			argumentList aL;
			aL.parse(str);
			std::string argList = aL.toString();
			consume(str,"Z");
			std::string n = "";
			
			return n
			.append(returnValueTypeCode)
			.append(" (")
			.append(callingConvention)
			.append("*)")
			.append("(")
			.append(argList)
			.append(")")
			;
		}
	else	if(str_match(s,"P8")) //Method Pointer?
		{
			basicName bN;
			bN.parse(str);
			qualification q;
			q.parse(str);
			consume(str,"@");
			
			std::string callingConvention = getNextCallingConvention(str);
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString("","");//hack
			argumentList aL;
			aL.parse(str);
			std::string argList = aL.toString();

			storageClass c;
			c.parse(str);

			std::string n = "";
			
			return "[named-pointer?]";
		}
	else	if(str_match(s,"P")) //Needs revision -- another pointer
		{
			storageClass sC;
			sC.parse(str);
			std::string dataType = getNextDataType(str);
			std::string checked = checkExceptions(dataType,str);
			dataType = checked;
			
			if(GCC_MANGLE) return std::string("P").append(dataType);

			std::string out = dataType.append(" * ").append(sC.postfix);
			if(sC.display==1) out = out.append(" ").append(sC.toString());
			return out;
/*			return std::string("(")
			.append(prefix)
			.append(" ")
			.append(dataType)
			.append(" *")
			.append(")")
			;
*/		}
	else	if(str_match(s,"U")) //struct
		{
			basicName bN;
			bN.parse(str);
			
			qualification q;
			q.parse(str);
			consume(str,"@");
									
			return std::string("struct ")
			.append(q.toString())
			.append("::")
			.append(bN.toString());
		}
	else	if(str_match(s,"*")) //Pointer
		{
			std::string type = getNextDataType(str);
			std::string check = checkExceptions(type,str);
			type = check;
			
			return type
			.append(" *");
		}
	else	if(str_match(s,"V")) //Class
		{			
			basicName bN;
			bN.parse(str);
			std::string basicName = checkExceptions(bN.toString(),str);
			
			qualification q;
			q.parse(str);
			consume(str,"@");
						
			std::string qualn = q.toString();

			if(hasNumericFirstChar(basicName))
			{
				int index = (int) (basicName[0]-'0');
				std::vector<std::string> gbr = getGlobalBackRef();
				//basicName = gbr[index];
				basicName = "[backref]";
			}
			if(qualn.length()>0) return std::string("class ").append(qualn).append("::").append(basicName);
		else	if(!GCC_MANGLE) return std::string("class ").append(basicName);
			
			if(qualn.length()>0) return std::string("N").append(q.toGCC()).append(bN.toGCC()).append("E");
			else return std::string("N").append(bN.toGCC()).append("E");
		}
	else	if(str_match(s,"Q")) //Array
		{
			/*std::string callingConvention = getNextCallingConvention(str);
			
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString("","");//hack
			return returnValueTypeCode
			//.append(" * const")
			.append(" const *")
			;*/
			
			storageClass sC;
			sC.parse(str);
			dataTypeCode rVTC;
			rVTC.parse(str);
			
			//I think this has to have another "const" added (at the end? verify) if passed as an argument.
			
			return rVTC.toString("","")//hack
			.append(" ")
			.append(sC.toString())
			.append(" * ")
			.append(sC.postfix)
			;
		}
	else	if(str_match(s,"?")) //Modifier
		{
			storageClass sC;
			sC.parse(str);
			std::string attribute =  sC.toString();
			
			std::string type = getNextDataType(str);
			std::string check = checkExceptions(type,str);
			type = check;
			
			std::string dataType = checkExceptions(type,str);

			return std::string("").append(dataType).append(" ").append(attribute);
		}
	else	if(str_match(s,"W")) //Enum
		{
			std::string out = "enum";
			std::string type = getRealType(str);
			
			qualification q;
			q.parse(str);
			std::string qualification = q.toString();
			consume(str,"@");
			
			return out
			.append(" ")
			.append(qualification)
			;
		}
	else	if(str_match(s,"&")) //Reference
		{
			storageClass sC;
			sC.parse(str);
			std::string sClass = sC.toString(); //Going by vc++filt's output conventions.  Or maybe it actually is callingConvention.
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString("","");//hack
			std::string out =
				returnValueTypeCode.append(" ")
				.append(sClass)
				.append(" ")
				.append("&");
				
				if(sC.postfix.length()>0) out = out
				.append(" ")
				.append(sC.postfix);
				
			if(GCC_MANGLE) out = std::string("R").append(rVTC.toGCC());
				
				return out;
		}
	else	return s;
	}

	std::string getNextDataType(std::string &s)
	{
if (DO_DEBUG)	std::cout << "Finding next data type... " << s << std::endl << std::endl;
		
		consume(s,"$$");	//This is a hack, fix it when you understand it.
		if(consume(s,"$")) //These are hacks, please clean up and make valid
		{
			if(consume(s,"0")) //Hack -- Please incorporate real definitions.
			{
				if(isdigit(s[0]))
				{
					consume1(s);
					return "[int-val]";
				}
				while(s[0]!='@') consume1(s);
				consume(s,"@");
				return "[int-val]";
			}
			else if(consume(s,"H") || consume(s,"1")) //$1?...: no documentation.
			{	//Unknown properties according to mearie.org...
				//Please create sub-scopes for these nested mangled names, or this may result in incorrect backreference output elsewhere.
				std::string mN = parseMangledName(s);
				consume(s,"@");
				return "[mangled-name]";
			}
		}
		
		std::string options[100][3] =
		{
			{"@X",	 "v",	"void"},
			{"@",	 "@",	"@"},

			{"0",	 "0",	"0"},
			{"1",	 "1",	"1"},
			{"2",	 "2",	"2"},
			{"3",	 "3",	"3"},
			{"4",	 "4",	"4"},
			{"5",	 "5",	"5"},
			{"6",	 "6",	"6"},
			{"7",	 "7",	"7"},
			{"8",	 "8",	"8"},
			{"9",	 "9",	"9"},

			{"A6",	"P6",	"P6"},	//This is a hack, fix it when you understand it.
			
			{"A",	"&",	"&"},	//Type modifier (reference)
			{"B",	 "???",	"[Type Modifier (Volatile Reference)]"},	//Type modifier (reference)
			{"C",	 "a",	"signed char"},
			{"D",	 "c",	"char"},
			{"E",	 "h",	"unsigned char"},
			{"F",	 "s",	"short"},
			{"G",	 "t",	"unsigned short"},
			{"H",	 "i",	"int"},
			{"I",	 "j",	"unsigned int"},
			{"J",	 "l",	"long"},
			{"K",	 "m",	"unsigned long"},
			{"M",	 "f",	"float"},
			{"N",	 "d",	"double"},
			{"O",	 "e",	"long double"},

			{"PA",	"*",	"*"},

			{"P6",	"P6",	"P6"},
			{"P8",	"P8",	"P8"},	//These P* types need to be combined under a single struct or something
			
			{"P",	 "P",	"P"},
			{"Q",	 "Q",	"Q"},
			{"S",	 "???",	"[Type modifier (const volatile pointer)]"},
			{"U",	"???",	"U"},
			{"V",	 "V",	"V"},
			{"W",	 "W",	"W"},
			{"X",	 "v",	"void"},
			{"Z",	 "z",	"Z"},
			{"?",	"?",	"?"},
			
			//Implement all of these once you have internet access again.
			{"_0",	"???",	"{_0}"},
			{"_1",	"???",	"{_1}"},
			{"_2",	"???",	"{_2}"},
			{"_3",	"???",	"{_3}"},
			{"_4",	"???",	"{_4}"},
			{"_5",	"???",	"{_5}"},
			{"_6",	"???",	"{_6}"},
			{"_7",	"???",	"{_7}"},
			{"_8",	"???",	"{_8}"},
			{"_9",	"???",	"{_9}"},

			{"_A",	"???",	"{_A}"},
			{"_B",	"???",	"{_B}"},
			{"_C",	"???",	"{_C}"},
			{"_D",	"???",	"{_D}"},
			{"_E",	"???",	"{_E}"},
			{"_F",	"???",	"{_F}"},
			{"_G",	"???",	"{_G}"},
			{"_H",	"???",	"{_H}"},
			{"_I",	"???",	"{_I}"},
			{"_J",	"???",	"{_J}"},
			{"_K",	"???",	"unsigned __int64"},
			{"_L",	"???",	"{_L}"},
			{"_M",	"???",	"{_M}"},
			{"_N",	"???",	"bool"},
			{"_O",	"???",	"{_O}"},
			{"_P",	"???",	"{_P}"},
			{"_Q",	"???",	"{_Q}"},
			{"_R",	"???",	"{_R}"},
			{"_S",	"???",	"{_S}"},
			{"_T",	"???",	"{_T}"},
			{"_U",	"???",	"{_U}"},
			{"_V",	"???",	"{_V}"},
			{"_W",	"???",	"{_W}"},
			{"_X",	"???",	"{_X}"},
			{"_Y",	"???",	"{_Y}"},
			{"_Z",	"???",	"{_Z}"},
			
			{"",	"",	""}
		};
		int i = 0;
		while(options[i][0].length()>0)
		{
			if(consume(s,options[i][0]))
			{
				std::string result;
				if(GCC_MANGLE) result = options[i][1];
				else result = options[i][2];//checkExceptions(options[i][1],s);
				return result;
			}
			i++;
		}
		return "";
	}

	/* For more information regarding class modifiers, please refer to:
	 * https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B_Name_Mangling#Type_Modifier
	 */
	std::string getNextStorageClass(std::string &str)
	{
		std::string out = "";
/*		
		std::string options[100][2] =
		{
			{"A",	"normal"},
			{"B",	"const"},
			{"C",	"volatile"},
			{"D",	"const volatile"},
			{"E",	"far"},
			{"F",	"const far"},
			{"G",	"volatile far"},
			{"H",	"const volatile far"},
			{"I",	"huge"},
			{"Z",	"executable"}
		};
*/
		std::string options[100][2] =
		{
			{"A",	"normal"},
			{"B",	"const"},
			{"C",	"volatile"},
			{"D",	"const volatile"},
			{"Z",	"executable"},
			{"",""}
		};
		int i = 0;
		
		while(options[i][0].length()>0)
		{
			if(consume(str,options[i][0]))
			{
				if(GCC_MANGLE) return options[i][1];
				return options[i][2];
			}
			i++;
		}
		return checkPrefix(str);
	}
	std::string checkPrefix(std::string &str)
	{
		std::string prefix;
		std::string postfix;

		if(consume(str,"E")) //__ptr64
		{
			prefix = getNextStorageClass(str);
			postfix = "__ptr64";
			return prefix.append(" ").append(postfix);
		}
	else	if(consume(str,"F")) //__unaligned
		{
			postfix = getNextStorageClass(str);
			prefix = "__unaligned";
			return prefix.append(" ").append(postfix);
		}
	else	if(consume(str,"I")) //__restrict
		{
			prefix = getNextStorageClass(str);
			postfix = "__restrict";
			return prefix.append(" ").append(postfix);
		}
		return "";
	}
	std::string getNextCallingConvention(std::string &s)
	{
		if(consume(s,"A")||consume(s,"B")) return "__cdecl";
	else	if(consume(s,"C")||consume(s,"D")) return "__pascal";
	else	if(consume(s,"E")||consume(s,"F")) return "__thiscall";
	else	if(consume(s,"G")||consume(s,"H")) return "__stdcall";
	else	if(consume(s,"I")||consume(s,"J")) return "__fastcall";
	}

	std::string getNextOpCode(std::string &s)
	{
		std::vector<std::string> gbr = getGlobalBackRef();
		if(consume(s,"0"))
		{
			if(GCC_MANGLE) return "C1";
			return "";
		}
	else	if(consume(s,"1"))
	{
		if(GCC_MANGLE) return "D1";
		return "~";
	}
	else	if(consume(s,"2"))
	{
		if(GCC_MANGLE) return "nw";
		return "operator new";
	}
	else	if(consume(s,"3"))
	{
		if(GCC_MANGLE) return "dl";
		return "operator delete";
	}
	else	if(consume(s,"4"))
	{
		if(GCC_MANGLE) return "aS";
		return "operator=";
	}
	else	if(consume(s,"5"))
	{
		if(GCC_MANGLE) return "rs";
		return "operator>>";
	}
	else	if(consume(s,"6"))
	{
		if(GCC_MANGLE) return "ls";
		return "operator<<";
	}
	else	if(consume(s,"7"))
	{
		if(GCC_MANGLE) return "nt";
		return "operator!";
	}
	else	if(consume(s,"8"))
	{
		if(GCC_MANGLE) return "eq";
		return "operator==";
	}
	else	if(consume(s,"9"))
	{
		if(GCC_MANGLE) return "ne";
		return "operator!=";
	}
	else	if(consume(s,"A"))
	{
		if(GCC_MANGLE) return "ix";
		return "operator[]";
	}
	else	if(consume(s,"B")) return "operator returntype";
	else	if(consume(s,"C")) 
	{
		if(GCC_MANGLE) return "pt";
		return "operator->";
	}
	else	if(consume(s,"D"))
	{
		if(GCC_MANGLE) return "ml";
		return "operator*";
	}
	else	if(consume(s,"E"))
	{
		if(GCC_MANGLE) return "pp";
		return "operator++";
	}
	else	if(consume(s,"F"))
	{
		if(GCC_MANGLE) return "mm";
		return "operator--";
	}
	else	if(consume(s,"G"))
	{
		if(GCC_MANGLE) return "mi";
		return "operator-";
	}
	else	if(consume(s,"H"))
	{
		if(GCC_MANGLE) return "pl";
		return "operator+";
	}
	else	if(consume(s,"I"))
	{
		if(GCC_MANGLE) return "an";
		return "operator&";
	}
	else	if(consume(s,"J"))
	{
		if(GCC_MANGLE) return "pm";
		return "operator->*";
	}
	else	if(consume(s,"K"))
	{
		if(GCC_MANGLE) return "dv";
		return "operator/";
	}
	else	if(consume(s,"L"))
	{
		if(GCC_MANGLE) return "rm";
		return "operator%";
	}
	else	if(consume(s,"M"))
	{
		if(GCC_MANGLE) return "lt";
		return "operator<";
	}
	else	if(consume(s,"N"))
	{
		if(GCC_MANGLE) return "le";
		return "operator<=";
	}
	else	if(consume(s,"O"))
	{
		if(GCC_MANGLE) return "gt";
		return "operator>";
	}
	else	if(consume(s,"P"))
	{
		if(GCC_MANGLE) return "ge";
		return "operator>=";
	}
	else	if(consume(s,"Q"))
	{
		if(GCC_MANGLE) return "cm";
		return "operator,";
	}
	else	if(consume(s,"R"))
	{
		if(GCC_MANGLE) return "cl";
		return "operator()";
	}
	else	if(consume(s,"S"))
	{
		if(GCC_MANGLE) return "co";
		return "operator~";
	}
	else	if(consume(s,"T")) 
	{
		if(GCC_MANGLE) return "eo";
		return "operator^";
	}
	else	if(consume(s,"U"))
	{
		if(GCC_MANGLE) return "or";
		return "operator|";
	}
	else	if(consume(s,"V"))
	{
		if(GCC_MANGLE) return "aa";
		return "operator&&";
	}
	else	if(consume(s,"W"))
	{
		if(GCC_MANGLE) return "oo";
		return "operator||";
	}
	else	if(consume(s,"X"))
	{
		if(GCC_MANGLE) return "mL";
		return "operator*=";
	}
	else	if(consume(s,"Y"))
	{
		if(GCC_MANGLE) return "pL";
		return "operator+=";
	}
	else	if(consume(s,"Z"))
	{
		if(GCC_MANGLE) return "mI";
		return "operator-=";
	}
	else	if(consume(s,"_0"))
	{
		if(GCC_MANGLE) return "dV";
		return "operator/=";
	}
	else	if(consume(s,"_1"))
	{
		if(GCC_MANGLE) return "rM";
		return "operator%=";
	}
	else	if(consume(s,"_2"))
	{
		if(GCC_MANGLE) return "rS";
		return "operator>>=";
	}
	else	if(consume(s,"_3"))
	{
		if(GCC_MANGLE) return "lS";
		return "operator<<=";
	}
	else	if(consume(s,"_4"))
	{
		if(GCC_MANGLE) return "aN";
		return "operator&=";
	}
	else	if(consume(s,"_5"))
	{
		if(GCC_MANGLE) return "ne";
		return "operator!=";
	}
	else	if(consume(s,"_6"))
	{
		if(GCC_MANGLE) return "eO";
		return "operator^=";
	}
	else	if(consume(s,"_7")) return "`vftable'";
	else	if(consume(s,"_B"))
		{
			return std::string("'local static guard'");
		}
	else	if(consume(s,"_F")) return "`default constructor closure'";
	
	
	else	if(consume(s,"_") && consume1(s)) return "{_X}";	//Hack, please add real definitions
	}

	std::string getRealType(std::string &s)
	{
		if(consume(s,"0")) return "char";
	else	if(consume(s,"1")) return "unsigned char";
	else	if(consume(s,"2")) return "short";
	else	if(consume(s,"3")) return "unsigned short";
	else	if(consume(s,"4")) return "int";
	else	if(consume(s,"5")) return "unsigned int";
	else	if(consume(s,"6")) return "long";
	else	if(consume(s,"7")) return "unsigned long";
	}
}
