/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{	
	/* StorageClass:
		'A' (Normal Storage)
		'B' (Volatile Storage)
		'C' (Const Storage)
		'Z' (Executable Storage)
	*/
	struct storageClass{
		std::string sClass;
		std::string postfix;
		
		int display;
		storageClass(){}

		void checkPrefix(std::string &str)
		{
			std::string postfix;

			if(consume(str,"E")) //__ptr64
			{
				this->postfix = " __ptr64";
			}
		else	if(consume(str,"F")) //__unaligned
			{
				this->postfix = " __unaligned";
			}
		else	if(consume(str,"I")) //__restrict
			{
				this->postfix = " __restrict";
			}
			this->getNextStorageClass(str);
		}
		void getNextStorageClass(std::string &str)
		{
			std::string out = "";
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
				if(str[0]=='A'||str[0]=='Z') this->display = 0;
				else this->display = 1;	
				if(consume(str,options[i][0]))
				{
					this->sClass = options[i][1];
					return;
				}
				i++;
			}
			checkPrefix(str);
		}
		void parse(std::string &str)
		{
			this->display = 1;
if(DO_DEBUG)		debug("parsing next storage class...\t",str);
			this->getNextStorageClass(str);
		}
		std::string toString()
		{
			if(display==1) return this->sClass;
			else return "";
		}
		std::string getPostfix()
		{
			return this->postfix;
		}
	};

	/* Data Type Codes

		signed char    C
		char           D
		unsigned char  E
		short          F
		unsigned short G
		int            H
		unsigned int   I
		long           J
		unsigned long  K
		float          M
		double         N
		long double    O
		pointer        P (see below)
		array          Q (see below)
		struct/class   V (see below)
		void           X (terminates argument list)
		elipsis        Z (terminates argument list)
	*/
	/* Consider this:
		- getnextdatatype.  do not checkexceptions in that method.
		- check exceptions in this method.  take note of the special exceptions.
		- in the case of something like a "Q" datatype in an arglist, you may then add the "const" after processing and checking exceptions.
	 */
	struct dataTypeCode{
		std::string contents;
		std::string pointerStorageClass;
		
		std::string exceptionCode;
		
		int isPointer;
		functionTypeCode fTC;
		dataTypeCode(){}		
		void parse(std::string &str)
		{
			this->isPointer = 0;
			std::string dt = getNextDataType(str);
			this->exceptionCode = dt;
			std::string ce = checkExceptions(dt,str);
			dt = ce;
			this->contents = dt;
			if(str_match(dt,"P6"))
			{
				this->isPointer = 1;
				this->fTC.parse(str);
				consume(str,"@");
				storageClass sC;
				sC.parse(str);
				this->pointerStorageClass = sC.toString();
			}
		}
		std::string toString(std::string sC,std::string name)
		{
			if(this->isPointer)
			{
				std::string out = this->fTC.returnValueTypeCode
				.append(" (")
				.append(this->fTC.cConvention)
				.append("*")
				.append(this->pointerStorageClass);
				
				if(name.length()>0)
				out = out
				.append(" ")
				.append(name);
				
				out = out
				.append(")(")
				.append(this->fTC.argList)
				.append(")")
				;
				return rmws(out);
			}
			else
			{
				std::string out = this->contents;
				if(sC.length()>0) out = out.append(" ").append(sC);
				if(name.length()>0) out = out.append(" ").append(name);
				return rmws(out);
			}
		}
		std::string toGCC()
		{
			std::string out;
			if(this->isPointer) out = out.append("P");
			out = out.append(contents);
			
			return out;
		}
	};

	/* Function Argument Type Lists
	Function arguments types are listed left to right using the following codes.
	If the parameter list is not void and does not end in elipsis,
	its encoding is terminated by an @. 
	*/
		argumentList::argumentList(){}
		void argumentList::parse(std::string &str)
		{
			local_backref.clear();
			while(1)
			{	
				if(consume(str,"@")) break;
				
				dataTypeCode dTC;
				dTC.parse(str);
								
				std::string add = dTC.toString("","");//hack
				
		if(DO_DEBUG)	debug("arglist parsed:\t",add);
				//std::cout << GCC_MANGLE << "\n";
				
				if(str_match(dTC.exceptionCode,"Q"))
				{
					add = add.append(" ").append("const");
				}
				
				if(str_match(add,"Z")||str_match(add,"z"))
				{
					std::string one = "1";
					if(GCC_MANGLE && DO_DEBUG) debug("Z\t",one);
					args.push_back(std::string("...")); //Is this acceptable?
					break;
				}
				else if(str_match(add,"void")||str_match(add,"v"))
				{
					args.push_back(add);
					break;
				}
				else
				{
					if(hasNumericFirstChar(add))
					{
						int index = (int) (add[0]-'0');
						int index2 = ((int)local_backref.size())-1;
						if(index > index2) add = global_backref[index];
						else add = local_backref[index];
					}
					args.insert(args.end(),add);
					local_backref.insert(local_backref.end(),add);
				}
			}
		}
		std::string argumentList::toString()
		{
			std::string out;
			out = implode(",",this->args);
			return out;
		}
		std::string argumentList::toGCC()
		{
			std::string out;
			out = implode("",this->args);
			return out;
		}

	/* Function Calling Convention Codes

		__cdecl -> "A"
		__fastcall -> "I"
		__stdcall -> "G" 
	*/
	struct callingConvention{
		std::string cConvention;
		callingConvention(){}
		void parse(std::string &str)
		{
			this->cConvention = getNextCallingConvention(str);
		}
		std::string toString()
		{
			return this->cConvention;
		}
	};
	
	/* FunctionTypeCode:
		CallingConvention ReturnValueTypeCode ArgumentList
	*/
		functionTypeCode::functionTypeCode(){}
		void functionTypeCode::parse(std::string &str)
		{
if(DO_DEBUG)		debug("parsing calling convention.\t",str);
			callingConvention cC;
			cC.parse(str);
			this->cConvention = cC.toString();
			
if(DO_DEBUG)		debug("parsing return value type code.\t",str);
			dataTypeCode rVTC;
			rVTC.parse(str);
			this->returnValueTypeCode = rVTC.toString("","");//hack

if(DO_DEBUG)		debug("parsing argument list.\t",str);
			argumentList aL;
			aL.parse(str);
			this->argList = aL.toString();
			this->aL_gcc = aL.toGCC();
		}
		std::string functionTypeCode::toString(std::string name)
		{
			std::string out = "";
			return out
			.append(this->returnValueTypeCode)
			.append(" ")
			.append(this->cConvention)
			.append(" ")
			.append(name)
			.append("(")
			.append(this->argList)
			.append(")")
//			.append(this->postfix)
			;
		}
		std::string functionTypeCode::toGCC()
		{
			return this->aL_gcc;
		}

	struct functionTypeCode2{
		std::string
			container,
			cConvention,
			returnValueTypeCode,
			argList,
			postfix,
			aL_gcc
			;
		storageClass sC;
		
		functionTypeCode2(){}
		void parse(std::string &str)
		{
			sC.parse(str);
			this->container = sC.toString();
			this->postfix = sC.getPostfix();
			
			callingConvention cC;
			cC.parse(str);
			this->cConvention = cC.toString();

			if(consume(str,"@")) this->returnValueTypeCode = ""; /* Maybe this is the convention of a different type of function code with its own rules.
										Please modify the code accordingly. */
			else
			{
				dataTypeCode rVTC;
				rVTC.parse(str);
				this->returnValueTypeCode = rVTC.toString("","");//hack
			}
			argumentList aL;
			aL.parse(str);
			this->argList = aL.toString();
			this->aL_gcc = aL.toGCC();
		}
		std::string toString(std::string name)
		{
			std::string out = "";
			return out
			.append(" ")
			.append(this->returnValueTypeCode)
			.append(" ")
			.append(this->cConvention)
			.append(" ")
			.append(name)
			.append("(")
			.append(this->argList)
			.append(")")
			.append(sC.toString())
			.append(" ")
			.append(this->postfix)
			;
		}
		std::string toGCC()
		{
			return this->aL_gcc;
		}

	};

	//Please unify _nqualifiedTypeCode and _ualifiedTypeCode into a single struct when convenient.
	struct typeCode{
	};
	
	/* UnqualifiedTypeCode:
		'Y' FunctionTypeCode
		'3' DataTypeCode
	*/
	struct unqualifiedTypeCode{
		std::string
			funcType,
			dataType;
		int
			isFunction,
			isData,
			isStatic;
		
		functionTypeCode fTypeCode;
		functionTypeCode sTypeCode;
		dataTypeCode dTypeCode;
		
		unqualifiedTypeCode(){}
		void parse(std::string &str)
		{
if (DO_DEBUG)		debug("unqualifiedTypeCode...",str);

			this->isFunction = this->isData = this->isStatic = 0;
			if(consume(str,"Y")) //FunctionTypeCode
			{
				this->fTypeCode.parse(str);
				this->isFunction = 1;
			}
			else if(consume(str,"3")) //DataTypeCode
			{
				this->dTypeCode.parse(str);
				this->isData = 1;
			}
			else if(consume(str,"S")||consume(str,"C")) //static -- please review/check for rules, if any
			{
				this->sTypeCode.parse(str);
				this->isStatic = 1;
			}
			else if(consume(str,"6")) //static -- please review/check for rules, if any
			{
				return;
			}
		}
		std::string toString(std::string name,std::string sC)
		{
			if(this->isFunction) return this->fTypeCode.toString(name);
		else	if(this->isData) return this->dTypeCode.toString(sC,name);//are these valid?  the storageclass may be different...
		else	if(this->isStatic) return this->sTypeCode.toString(name);
		}
		std::string toGCC()
		{
			if(this->isFunction) return this->fTypeCode.toGCC();
		else	if(this->isData) return "";//this->dTypeCode.toGCC();//are these valid?  the storageclass may be different...
		else	if(this->isStatic) return this->sTypeCode.toGCC();
		}
	};

	/* QualifiedTypeCode:
		'Q' FunctionTypeCode
		'2' DataTypeCode
	*/
	struct qualifiedTypeCode{
		std::string
			fTypeCode,
			dataType,
			modifier;
		int
			isFunction,
			isData,
			isStatic,
			isVirtual;
		functionTypeCode2 fTypeCode2;
		functionTypeCode sTypeCode;
		dataTypeCode dTypeCode;
		
		qualifiedTypeCode(){}
		void parse(std::string &str)
		{
			this->isFunction = this->isData = this->isStatic = this->isVirtual = 0;
			if(consume(str,"Q"))
			{
				this->isFunction = 1;
				this->fTypeCode2.parse(str);
				this->modifier = "public: ";
			}
			else if(consume(str,"2")) /* Please try to fully understand the calling conventions at work here */
			{
				this->isData = 1;
				this->dTypeCode.parse(str);
				this->modifier = "public: static ";
			}
			else if(consume(str,"4") || consume(str,"3")) /* Please try to fully understand the calling conventions at work here */
			{
				this->isData = 1;
				this->dTypeCode.parse(str);
			}
			else if(consume(str,"S")||consume(str,"C")) //static -- please review/check for rules, if any
			{
				this->modifier = "public: static";
				this->isStatic = 1;
				this->sTypeCode.parse(str);
			}
			else if(consume(str,"Y")) //complex
			{
				this->isStatic = 1;
				sTypeCode.parse(str);
			}
			else if(consume(str,"I")) //complex
			{
				this->isFunction = 1;
				this->modifier = "protected:";
				fTypeCode2.parse(str);
			}
			else if(consume(str,"U")) //virtual
			{
				this->isVirtual = 1;
				this->fTypeCode2.parse(str);
				this->modifier = "public: virtual ";
			}
			else if(consume(str,"6")) //as in "6B"
			{
				return;
			}
		}
		std::string toString(std::string name,std::string sC)
		{			
			if(this->isFunction) return this->fTypeCode2.toString(name);
		else	if(this->isVirtual) return this->fTypeCode2.toString(name);
		else	if(this->isData) return this->dTypeCode.toString(sC,name);
		else	if(this->isStatic)
			{
				return this->sTypeCode.toString(name);
			}
		else	return sC.append(" ").append(name);
		}
		std::string toGCC()
		{
			if(this->isFunction) return this->fTypeCode2.toGCC();
		else	if(this->isVirtual) return this->fTypeCode2.toGCC();
		else	if(this->isData) return "";//this->dTypeCode.toGCC();
		else	if(this->isStatic) return this->sTypeCode.toGCC();
		else	return "";
		}
		std::string getModifier()
		{
			return this->modifier;
		}
	};

	/* See Wikipedia article for more details: https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B_Name_Mangling
	Operators
	Operator names are represented by a question mark followed by a single character rather than being spelled out. Examples:

		myclass::myclass          ?0
		myclass::~myclass         ?1
		myclass::operator new     ?2
		myclass::operator delete  ?3
		myclass::operator=        ?4
		myclass::operator+        ?H
		myclass::operator++       ?E
	*/
	/* BasicName:
		'?' operatorCode
		string '@
	*/
			basicName::basicName(){}
			void basicName::parse(std::string &str)
			{
if (DO_DEBUG)			std::cout << "Parsing basic name.\n";
				
				this->hasOperator = 0;
				this->hasTemplate = 0;
				this->nameFragment = "";
				this->templateStr = "";
				
				if(consume(str,"?$")) //Template
				{
					templateArg tA;
					tA.parse(str);
					this->hasTemplate = 1;
					this->templateStr = tA.toString();
					this->gcc_template = tA.toGCC();
					
					return;
				}
				else if(consume(str,"??")) //Nested Name
				{
					str = std::string("?").append(str);
					std::string dm = parseMangledName(str);
					//consume(str,"@");
					this->nameFragment = std::string("`").append(dm).append("'");
					return;
				}
				else if(consume(str,"?")) //Operator
				{
					char i = str[0];
					this->operatorCode = getNextOpCode(str);
					
					if
					(
						(!GCC_MANGLE) &&
						(i=='0'||i=='1')
					) this->operatorCode = this->operatorCode.append("\\q");
					this->hasOperator = 1;
					return;
				}
				else //Name fragment
				{
//					debug("parsing name fragment:",str);
					char c = consume1(str);

					int index = (int) (c-'0');
					if(index >= 0 && index <= 9 && this->nameFragment.length()==0)
					{
						std::vector<std::string> gbr = getGlobalBackRef();
						
					if	(index>(gbr.size()-1)) this->nameFragment = std::string("[backref]");
					else	
						{							
							this->nameFragment = gbr[index];

							std::stringstream len;
							len << this->nameFragment.length();
							std::string length = len.str();
							
							if(GCC_MANGLE) this->nameFragment = length.append(this->nameFragment);
						}
						return;
					}

					while(c!='@')
					{
						this->nameFragment += c;
						c = consume1(str);
					}
if (DO_DEBUG)				debug("name-fragment end:",str);
					global_backref.insert(global_backref.end(),this->nameFragment);
					
					std::stringstream len;
					len << this->nameFragment.length();
					std::string length = len.str();
					
					if(GCC_MANGLE) this->nameFragment = length.append(this->nameFragment);
					return;
				}
			}
			std::string basicName::toString()
			{
				if(this->hasOperator) return this->operatorCode;
				else if(this->hasTemplate) return this->templateStr;
				else return this->nameFragment;
			}
			std::string basicName::toGCC()
			{
				if(this->hasOperator) return this->operatorCode;
				else if(this->hasTemplate) return this->gcc_template;
				else return this->nameFragment;
			}
	
	/* Qualification:
		( string '@' )* 
	*/
	struct qualification{
		std::vector<std::string> contents;
		std::vector<basicName> bN_contents;
		
		qualification(){}
		void parse(std::string &str)
		{
if (DO_DEBUG)		std::cout << "Parsing qualification.\n";
			while(str[0]!='@')
			{
				if(isdigit(str[0]))
				{
					char c = consume1(str);
					int index = c-'0';
					std::vector<std::string> gbr = getGlobalBackRef();

				if	(index>(gbr.size()-1)) this->contents.push_back("[backref]");
				else	this->contents.push_back(gbr[index]);
				
				continue;
				}
				
				basicName bN;
				bN.parse(str);
				std::string add = bN.toString();
				this->contents.insert(this->contents.end(),add);
				this->bN_contents.push_back(bN);
			}
if (DO_DEBUG)		debug("qualification end.\t",str);
			//if(was_numeric) consume(str,"@");
		}
		std::string toString()
		{
			std::reverse(this->contents.begin(),this->contents.end());
			return implode("::",this->contents);
		}
		std::string toGCC()
		{
			std::reverse(this->bN_contents.begin(),this->bN_contents.end());
			std::string out = "";
			for(int i = 0; i < bN_contents.size(); i++)
			{
				out = out.append(bN_contents[i].toGCC());
			}
			return out;
		}
	};
	
	/* Name with Template Arguments
		Name fragments starting with ?$ have template arguments. This kind of name looks like this:

		    Prefix ?$
		    Name terminated by @
		    Template argument list
	*/
		templateArg::templateArg(){}
		void templateArg::parse(std::string &str)
		{
if (DO_DEBUG)		debug("template-begin:\t",str);
			this->bN.parse(str);
			this->name = this->bN.toString();
			
			this->aL.parse(str);
			this->arguments = this->aL.toString();
			
			//qualification q;
			//q.parse(str);
			//consume(str,"@");
			
			//this->qualificn = q.toString();
			
			//consume(str,"@");
if (DO_DEBUG)		debug("template-end:\t",str);																																																														
		}
		std::string templateArg::toString()
		{
			return this->name.append("<").append(this->arguments).append(">");
		}
		std::string templateArg::toGCC()
		{
			return this->name.append("I").append(this->aL.toGCC()).append("E");
		}
}
