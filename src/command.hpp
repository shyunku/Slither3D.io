#pragma once
#include <sstream>
#include <thread>

using namespace std;
using std::thread;

typedef class command_parser_
{
public:
	vector<string> segments;
	string raw_string;
	string command_string;

	command_parser_(){}
	void accept(string raw)
	{
		raw_string = raw;
		command_string = raw.substr(1, raw.size() - 1);
		
		stringstream stream(command_string);
		string tmp;

		segments.clear();
		while (stream >> tmp)
		{
			segments.push_back(tmp);
		}
	}
	// get segment except keyword
	string get_segment(size_t pos)
	{
		if (pos > segments.size() - 2)
		{
			cout << "Command Parse Error: no segment as index (" << pos << ")" << endl;
			return "";
		}
		return segments.at(pos + 1);
	}
	string get_command_keyword()
	{
		if (segments.empty())
		{
			cout << "Not Valid command." << endl;
			return "";
		}

		return segments.at(0);
	}
	size_t get_segment_size_except_keyword()
	{
		return segments.size() - 1;
	}
}CommandParser;