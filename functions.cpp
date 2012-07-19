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
		char out = s[0];
		s = s.substr(1,s.length()-1);
		return out;
	}
	int consume(std::string &s, std::string ss){
		for(int i = 0; i < ss.length(); i++)
		{
			if(i >= s.length()) return 0;
			if(s[i] != ss[i]) return 0;
		}
		s = s.substr(ss.length(),s.length()-ss.length());
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
		if(a.compare(b)==0) return 1;
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
}
