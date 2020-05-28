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



void execute_command(CommandParser command_parser);

void command_listener()
{
	CommandParser command_parser = CommandParser();

	string raw_command;
	cout << endl << endl;
	logger("Enter your command with '/'");
	while (true)
	{
		cout << "> ";
		getline(cin, raw_command);
		if (raw_command.at(0) == '/')
		{
			command_parser.accept(raw_command);
			execute_command(command_parser);
		}
	}
}

void execute_command(CommandParser command_parser)
{
	extern Player player;

	string keyword = command_parser.get_command_keyword();
	size_t words = command_parser.get_segment_size_except_keyword();

	if (!keyword.compare("help") || !keyword.compare("h"))
	{
		logger("You can use command starting with /.");
		logger("example) /tp 3.3 5 -22.1");
		logger("example) /look 0 0 5");
		return;
	}
	else if (!keyword.compare("tp"))
	{
		if (words != 3)
		{
			logger("Invalid parameter. usage: /tp <x> <y> <z>");
			return;
		}
		
		vec3 orig = player.pos;

		float xf = stof(command_parser.get_segment(0));
		float yf = stof(command_parser.get_segment(1));
		float zf = stof(command_parser.get_segment(2));

		player.pos = vec3(xf, yf, zf);
		logger("Teleported to %d, %d, %d to %d, %d, %d", (int)orig.x, (int)orig.y, (int)orig.z, (int)xf, (int)yf, (int)zf);
		return;
	}
	else if (!keyword.compare("look"))
	{
		if (words != 3)
		{
			logger("Invalid parameter. usage: /look <x> <y> <z>");
			return;
		}

		float xf = stof(command_parser.get_segment(0));
		float yf = stof(command_parser.get_segment(1));
		float zf = stof(command_parser.get_segment(2));

		player.camera.set_looking_at(vec3(xf, yf, zf));
		logger("Now looking at %.2f, %.2f, %.2f ", xf, yf, zf);
		return;
	}
	else if (!keyword.compare("fix") || !keyword.compare("unfix"))
	{
		if (words == 0)
		{
			if (!keyword.compare("fix"))
			{
				player.set_fix(true);
				logger("Player position fixed.");
				return;
			}
			player.set_fix(false);
			logger("Player position unfixed.");
			return;
		}
		else if (words == 1)
		{
			if (!command_parser.get_segment(0).compare("true"))
			{
				player.set_fix(true);
				logger("Player position fixed.");
				return;
			}
			else if (!command_parser.get_segment(0).compare("false"))
			{
				player.set_fix(false);
				logger("Player position unfixed.");
				return;
			}
		}
		else
		{
			logger("Invalid parameter. usage: /fix <Boolean> or /fix");
			return;
		}
	}

	cout << "No command keyword for '" << keyword << "'" << endl;
	cout << "Try /help or /h to see how to use." << endl;
}