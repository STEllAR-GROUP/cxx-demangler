/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{
	//Identifies special codes and acts accordingly.
	std::string checkExceptions(std::string s,std::string &str,std::vector<std::string> &backref)
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
			rVTC.parse(str,backref);
			std::string returnValueTypeCode = rVTC.toString("","","");//hack
			argumentList aL;
			aL.parse(str,backref);
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
			bN.parse(str,backref);
			qualification q;
			q.parse(str,backref);
			consume(str,"@");
			
			std::string callingConvention = getNextCallingConvention(str);
			dataTypeCode rVTC;
			rVTC.parse(str,backref);
			std::string returnValueTypeCode = rVTC.toString("","","");//hack
			argumentList aL;
			aL.parse(str,backref);
			std::string argList = aL.toString();

			storageClass c;
			c.parse(str);

			std::string n = "";

			return	std::string("(")
				.append(callingConvention)
				.append(" ")
				.append(q.toString(backref))
				.append("::")
				.append(bN.toString())
				.append("::")
				.append(rVTC.toString("","",""))
				.append(")");
		}
	else	if(str_match(s,"P")) //Needs revision -- another pointer
		{
			storageClass sC;
			sC.parse(str);
			std::string dataType = getNextDataType(str,backref);
			std::string checked = checkExceptions(dataType,str,backref);
			dataType = checked;
			
			if(GCC_MANGLE) return std::string("P").append(dataType);

			/*
			std::string out = dataType.append(" * ").append(sC.suffix);
			if(sC.display==1) out = out.append(" ").append(sC.toString());
			*/

			//Fixes "char const * __ptr64" formatting.  Please verify that this works properly in all scenarios.
			
			std::string out = dataType;
			if(sC.display==1) out = out.append(" ").append(sC.toString());
			out = out.append(" * ").append(sC.suffix);
			
			//std::cout << out << "\n";
			
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
			bN.parse(str,backref);
			
			qualification q;
			q.parse(str,backref);
			consume(str,"@");
									
			//return std::string("struct ").append(q.toString()).append("::").append(bN.toString());
			
			std::string out;
			
			if(q.toString(backref).length()>0 && !GCC_MANGLE)
				out = std::string("struct ")
				.append(q.toString(backref))
				.append("::")
				.append(bN.toString());
				
			else if(!GCC_MANGLE) out = std::string("struct ").append(bN.toGCC()); //TODO: Qualification here?
			else if(GCC_MANGLE && q.toString(backref).length()>0) out = std::string("NS0_").append(q.toGCC(backref)).append(bN.toGCC()).append("E");
			else if(GCC_MANGLE) out = std::string("NS0_").append(bN.toGCC()).append("E");
			
			return out;
		}
	else	if(str_match(s,"*")) //Pointer
		{
			std::string type = getNextDataType(str,backref);
			std::string check = checkExceptions(type,str,backref);
			type = check;
			
			if(!GCC_MANGLE) return type.append(" *");
			else return std::string("P").append(type);
		}
	else	if(str_match(s,"V")) //Class
		{			
			basicName bN;
			bN.parse(str,backref);
			
			std::string basicName = checkExceptions(bN.toString(),str,backref);
			
			qualification q;
			q.parse(str,backref);
			consume(str,"@");
						
			std::string qualn = q.toString(backref);

			if(hasNumericFirstChar(basicName))
			{
				int index = (int) (basicName[0]-'0');
				std::vector<std::string> gbr = getGlobalBackRef();
				//basicName = gbr[index];
				basicName = "[backref]";
			}
			
			std::string out;
			
			if(qualn.length()>0 && !GCC_MANGLE) out = std::string("class ").append(qualn).append("::").append(basicName);
			else if(!GCC_MANGLE) out = std::string("class ").append(basicName);		
			else if(GCC_MANGLE && qualn.length()>0) out = std::string("NS0_").append(q.toGCC(backref)).append(bN.toGCC()).append("E");
			else if(GCC_MANGLE) out = std::string("NS0_").append(bN.toGCC()).append("E");
			
			return out;
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
			rVTC.parse(str,backref);
			
			//I think this has to have another "const" added (at the end? verify) if passed as an argument.
			
			if(!GCC_MANGLE)
			{
				return rVTC.toString("","","")//hack
				.append(" ")
				.append(sC.toString())
				.append(" * ")
				.append(sC.suffix)
				;
			}
			else return std::string("P").append(rVTC.toString("","",""));
		}
	else	if(str_match(s,"?")) //Modifier
		{
			storageClass sC;
			sC.parse(str);
			std::string attribute =  sC.toString();
			
			std::string type = getNextDataType(str,backref);
			std::string check = checkExceptions(type,str,backref);
			type = check;
			
			std::string dataType = checkExceptions(type,str,backref);

			return std::string("").append(dataType).append(" ").append(attribute);
		}
	else	if(str_match(s,"W")) //Enum
		{
			std::string out = "enum";
			std::string type = getRealType(str);
			
			qualification q;
			q.parse(str,backref);
			std::string qualification = q.toString(backref);
			consume(str,"@");
			
			if(!GCC_MANGLE)
			{
				return out
				.append(" ")
				.append(qualification)
				;
			}
			else return "NS0_1EE";
		}
	else	if(str_match(s,"&")) //Reference
		{
			storageClass sC;
			sC.parse(str);
			std::string sClass = sC.toString(); //Going by vc++filt's output conventions.  Or maybe it actually is callingConvention.
			dataTypeCode rVTC;
			rVTC.parse(str,backref);
			std::string returnValueTypeCode = rVTC.toString("","","");//hack
			std::string out =
				returnValueTypeCode.append(" ")
				.append(sClass)
				.append(" ")
				.append("&");
				
				if(sC.suffix.length()>0) out = out
				.append(" ")
				.append(sC.suffix);
				
			if(GCC_MANGLE) out = std::string("R").append(sC.toGCC()).append(rVTC.toGCC());
				return out;
		}
	else	return s;
	}

	std::string getNextDataType(std::string &s, std::vector<std::string> &backref)
	{
if (DO_DEBUG)	std::cout << "Finding next data type... " << s << std::endl << std::endl;
		
		consume(s,"$$");	//This is a hack, fix it when you understand it.
		if(consume(s,"$")) //These are hacks, please clean up and make valid
		{
			if(consume(s,"0"))
			{
				std::string lit;
				if(isdigit(s[0]))
				{
					char c = consume1(s);
					int i = c-'0';
					lit = i2s(i+1);
				}
				else
				{
					std::string hex;
					while(s[0]!='@') hex.push_back(consume1(s));
					consume(s,"@");
					lit = i2s(hex2dec(hex));
				}
				if(!GCC_MANGLE) return std::string("^int").append(lit);
				else return std::string("Li").append(lit).append("E");
			}
			else if(consume(s,"H") || consume(s,"1")) //$1?...: no documentation.
			{	//Unknown properties according to mearie.org...
				//Please create sub-scopes for these nested mangled names, or this may result in incorrect backreference output elsewhere.
				mangledName mN;
				mN.parse(s,backref);
				consume(s,"@");
				return std::string("&");
			}
		}
		
		std::string options[100][3] =
		{
			{"@X",	 "v",	"void"},
//			{"@",	 "@",	"@"},

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
//			{"B",	 "???",	"[Type Modifier (Volatile Reference)]"},	//Type modifier (reference)
			
			//Primitive datatypes: C to O; all others should be considered "extended"
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
//			{"S",	 "???",	"[Type modifier (const volatile pointer)]"},
			{"U",	 "U",	"U"}, //struct
			{"V",	 "V",	"V"}, //class
			{"W",	 "W",	"W"}, //enum
			{"X",	 "v",	"void"},
			{"Z",	 "z",	"Z"}, //... ("unlimited arguments")
			{"?",	"?",	"?"},
			
			//Implement all of these once you have internet access again.
//			{"_0",	"???",	"{_0}"},
//			{"_1",	"???",	"{_1}"},
//			{"_2",	"???",	"{_2}"},
//			{"_3",	"???",	"{_3}"},
//			{"_4",	"???",	"{_4}"},
//			{"_5",	"???",	"{_5}"},
//			{"_6",	"???",	"{_6}"},
//			{"_7",	"???",	"{_7}"},
//			{"_8",	"???",	"{_8}"},
//			{"_9",	"???",	"{_9}"},

//			{"_A",	"???",	"{_A}"},
//			{"_B",	"???",	"{_B}"},
//			{"_C",	"???",	"{_C}"},
			{"_D",	"???",	"__int8"},
			{"_E",	"???",	"unsigned __int8"},
			{"_F",	"???",	"__int16"},
			{"_G",	"???",	"unsigned __int16"},
			{"_H",	"???",	"__int32"},
			{"_I",	"???",	"unsigned __int32"},
			{"_J",	"x",	"__int64"}, //or long long
			{"_K",	"y",	"unsigned __int64"}, //or unsigned long long
			{"_L",	"???",	"__int128"},
			{"_M",	"???",	"unsigned __int128"},
			{"_N",	"b",	"bool"},
//			{"_O",	"???",	"{_O}"}, //Array
//			{"_P",	"???",	"{_P}"},
//			{"_Q",	"???",	"{_Q}"},
//			{"_R",	"???",	"{_R}"},
//			{"_S",	"???",	"{_S}"},
			{"_T",	"e",	"long double"}, //see calling_conventions
//			{"_U",	"???",	"{_U}"},
//			{"_V",	"???",	"{_V}"},
			{"_W",	"w",	"wchar_t"},
//			{"_X",	"???",	"{_X}"}, //Complex Type (coclass)
//			{"_Y",	"???",	"{_Y}"}, //Complex Type (cointerface)
			{"_Z",	"e",	"long double"}, //see calling_conventions
		};
		int i = 0;
		while(options[i][0].length()>0)
		{
			if(consume(s,options[i][0]))
			{
				std::string result;
				if(GCC_MANGLE)
				{
					if(str_match(options[i][1],"???")||str_match(options[i][1],""))
					{
						std::string error("unidentified GCC data type code (cxx_demangler::getNextDataType())");
						throw error;
					}
					result = options[i][1];
				}
				else result = options[i][2];//checkExceptions(options[i][1],s);
				return result;
			}
			i++;
		}
		std::string err("unidentified data type code (cxx_demangler::getNextDataType())");
		throw err;
		
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
	else	if(consume(s,"_7") && !GCC_MANGLE) return "`vftable'";
	else	if(consume(s,"_8") && !GCC_MANGLE) return "`vbtable'";
	else	if(consume(s,"_9") && !GCC_MANGLE) return "`vcall'";
	else	if(consume(s,"_A") && !GCC_MANGLE) return "`typeof'";
	else	if(consume(s,"_B") && !GCC_MANGLE) return std::string("`local static guard'");
	else	if(consume(s,"_C") && !GCC_MANGLE) return std::string("`string'");
	else    if(consume(s,"_D") && !GCC_MANGLE) return "`vbase destructor'";
	else    if(consume(s,"_E") && !GCC_MANGLE) return "`vector deleting destructor'";
	else	if(consume(s,"_F") && !GCC_MANGLE) return "`default constructor closure'";
	else	if(consume(s,"_G") && !GCC_MANGLE) return "`scalar deleting destructor'";
	else	if(consume(s,"_H") && !GCC_MANGLE) return "`vector constructor iterator'";
	else	if(consume(s,"_I") && !GCC_MANGLE) return "`vector destructor iterator'";
	else	if(consume(s,"_J") && !GCC_MANGLE) return "`vector vbase constructor iterator'";
	else	if(consume(s,"_K") && !GCC_MANGLE) return "`virtual displacement map'";
	else	if(consume(s,"_L") && !GCC_MANGLE) return "`eh vector constructor iterator'";
	else	if(consume(s,"_M") && !GCC_MANGLE) return "`eh vector destructor iterator'";
	else	if(consume(s,"_N") && !GCC_MANGLE) return "`eh vector vbase constructor iterator'";
	else	if(consume(s,"_O") && !GCC_MANGLE) return "`copy constructor closure'";
	else	if(consume(s,"_P") && !GCC_MANGLE) return "`udt returning'";
	//else	if(consume(s,"_Q")) return "`'";
	//else	if(consume(s,"_R")) return "`'"; //RTTI-related code
	else	if(consume(s,"_S") && !GCC_MANGLE) return "`local vftable'";
	else	if(consume(s,"_T") && !GCC_MANGLE) return "`local vftable constructor closure'";
	else	if(consume(s,"_U"))
	{
		if(GCC_MANGLE) return "na";
		return "operator new[]";
	}
	else	if(consume(s,"_V"))
	{
		if(GCC_MANGLE) return "da";
		return "operator delete[]";
	}	
	else	if(consume(s,"_X") && !GCC_MANGLE) return "`placement delete closure'";
	else	if(consume(s,"_Y") && !GCC_MANGLE) return "`placement delete[] closure'";
	else	if(consume(s,"__A") && !GCC_MANGLE) return "`managed vector constructor iterator'";
	else	if(consume(s,"__B") && !GCC_MANGLE) return "`managed vector destructor iterator'";
	else	if(consume(s,"__C") && !GCC_MANGLE) return "`eh vector copy constructor iterator'";
	else	if(consume(s,"__D") && !GCC_MANGLE) return "`eh vector vbase copy constructor iterator'";
	//else	if(consume(s,"_") && consume1(s)) return "{_X}";	//Hack, please add real definitions
	
	std::string err("unidentified operator code (cxx_demangler::storageClass::getNextOpCode())");
	throw err;
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
	
	std::string err("unidentified real type");
	throw err;
	}
}
