/*
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{
	// trim from start
	static inline std::string &ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
			return ltrim(rtrim(s));
	}

	char consume1(std::string &s){
		if(s.length()==0) return '\0';
		
		char out = s[0];
		s.erase(0,1);

if (DO_DEBUG)	std::cout<<"consume1:"<<out<<std::endl;

		return out;
	}
	int consume(std::string &s, std::string ss){
		if(s.length()==0) return 0;
		
		for(int i = 0; i < ss.length(); i++)
		{
			if(i >= s.length()) return 0;
			if(s[i] != ss[i]) return 0;
		}
		
if (DO_DEBUG)	debug("consume:",ss);
		
		s.erase(0,ss.length());
		return 1;
	}

	std::string implode(std::string delimiter, std::vector<std::string> array){
		std::string out = "";
		for(int i = 0; i < array.size(); i++)
		{
			out.append(array[i]);
			if(i<array.size()-1) out.append(delimiter);
		}
		return out;
	}

	int str_match(std::string a, std::string b){
		if(a.compare(b)==0)
		{
			return 1;
		}
		else return 0;
	}

	int hasNumericFirstChar(std::string s){
		for(int i = 0; i <= 9; i++)
		{
			if(s[0]-'0' == i)
			{
				return 1;
			}
		}
		return 0;
	}

	void cat(std::string &str1, std::string str2)
	{
		str1 = str1.append(str2);
	}

	std::string newcat(std::string str1, std::string str2)
	{
		return str1.append(str2);
	}

	std::string rmws(std::string str)
	{
		std::string out;
		for(int i = 0; i < str.length(); i++)
		{
			if(i>0 && str[i]==' ' && str[i-1]==' ')
			{
				
			}
			else out+=str[i];
		}
		return out;
	}
	void debug(std::string a, std::string &b)
	{
		std::cout << "\n" << a << b << "\n\n";
	}
	std::string read_line()
	{
		std::string s;
		char c;
		scanf("%c",&c);
		if(c=='\n') scanf("%c",&c);
		
		while(c!='\n')
		{
			s = s.append(1,c);
			scanf("%c",&c);
		}
		return s;
	}
	std::string replace(std::string str, std::string from, std::string to) {
		if(from.empty()) return str;
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
		return str;
	}
	int eq(std::string a, std::string b)
	{
		//if(a.length()!=b.length()) return 0;
		for(int i = 0; i < b.length(); i++)
		{
			if(a[i]!=b[i]) return 0;
		}
		return 1;
	}
	std::string i2s(int x)
	{
		std::stringstream ss;
		ss << x;
		std::string result = ss.str();
		return result;
	}
	void list(std::vector<std::string> s)
	{
		std::cout << "[";
		for(int i = 0; i < s.size(); i++)
		{
			std::cout << s[i];
			if(i<s.size()-1) std::cout << ",";
		}
		std::cout << "]" << "\n";
	}
}
