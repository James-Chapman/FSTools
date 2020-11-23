#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>


class Args {
private:
	std::unordered_map<std::string, std::string> m_requiredArgs;
	std::unordered_map<std::string, std::string> m_optionalArgs;
	std::unordered_map<std::string, bool> m_switches;
	friend class ArgParser;
public:
	std::string operator [] (std::string arg) {
		if (m_requiredArgs.size() > 0) {
			auto it = m_requiredArgs.find(arg);
			if (it != m_requiredArgs.end()) {
				return it->second;
			}
		}
		if (m_optionalArgs.size() > 0) {
			auto it = m_optionalArgs.find(arg);
			if (it != m_optionalArgs.end()) {
				return it->second;
			}
		}
		if (m_switches.size() > 0) {
			auto it = m_switches.find(arg);
			if (it != m_switches.end()) {
				if (it->second == true) {
					return it->first;
				}
				else {
					return "";
				}
			}
		}
		throw std::invalid_argument("Invalid argument passed to operator[]... " + arg);
	}
};


class ArgParser {
private:
	Args m_args;
	std::unordered_map<std::string, std::string> m_requiredArgDescriptions;
	std::unordered_map<std::string, std::string> m_optionalArgDescriptions;
	std::unordered_map<std::string, std::string> m_switchDescriptions;

public:
	ArgParser()
	{}

	void AddArg(const std::string arg, const std::string description, bool required=false) 
	{
		if (required == true) {
			m_requiredArgDescriptions.insert(std::pair<std::string, std::string>(arg, description));
			m_args.m_requiredArgs.insert(std::pair<std::string, std::string>(arg, ""));
		}
		else {
			m_optionalArgDescriptions.insert(std::pair<std::string, std::string>(arg, description));
			m_args.m_requiredArgs.insert(std::pair<std::string, std::string>(arg, ""));
		}
	}

	void AddSwitch(const std::string arg, const std::string description)
	{
		m_switchDescriptions.insert(std::pair<std::string, std::string>(arg, description));
		m_args.m_switches.insert(std::pair<std::string, bool>(arg, false));
	}

	std::string GetHelpString()
	{
		std::stringstream ss;
		ss << "PARAMETERS:\n\n";
		if (m_requiredArgDescriptions.size() > 0) {
			ss << "* Required arguments:\n";
			for (auto it : m_requiredArgDescriptions) {
				ss << "    " << std::setw(6) << std::left << it.first << " : " << it.second << "\n";
			}
			ss << "\n";
		}
		if (m_optionalArgDescriptions.size() > 0) {
			ss << "* Optional arguments:\n";
			for (auto it : m_optionalArgDescriptions) {
				ss << "    " << std::setw(6) << std::left << it.first << " : " << it.second << "\n";
			}
			ss << "\n";
		}
		if (m_switchDescriptions.size() > 0) {
			ss << "* Switches:\n";
			for (auto it : m_switchDescriptions) {
				ss << "    " << std::setw(6) << std::left << it.first << " : " << it.second << "\n";
			}
			ss << "\n";
		}
		return ss.str();
	}

	std::string GetUsageString(std::string arg0)
	{
		std::stringstream ss;
		ss << "USAGE:\n"
			<< "  " << arg0;
		if (m_requiredArgDescriptions.size() > 0) {
			for (auto it : m_requiredArgDescriptions) {
				ss << " " << it.first << " <param_value>";
			}
		}
		if (m_optionalArgDescriptions.size() > 0) {
			for (auto it : m_optionalArgDescriptions) {
				ss << " [" << it.first << " <param_value>]";
			}
		}
		if (m_switchDescriptions.size() > 0) {
			for (auto it : m_switchDescriptions) {
				ss << " [" << it.first << "]";
			}
		}
		ss << "\n\n";
		return ss.str();
	}

	Args ParseArgs(int argc, char* argv[])
	{
		int argcount = 0;
		for (int i = 1; i < argc; i++) {
			bool found = false;
			std::string arg = argv[i];
			if (arg[0] == '-') argcount++;

			{
				auto it = m_args.m_requiredArgs.find(arg);
				if (it != m_args.m_requiredArgs.end()) {
					found = true;
					it->second = argv[++i];
				}
			}
			if (found) continue;

			{
				auto it = m_args.m_optionalArgs.find(arg);
				if (it != m_args.m_optionalArgs.end()) {
					found = true;
					it->second = argv[++i];
				}
			}
			if (found) continue;

			{
				auto it = m_args.m_switches.find(arg);
				if (it != m_args.m_switches.end()) {
					found = true;
					it->second = true;
				}
			}
			if (!found) {
				throw std::invalid_argument("Unrecognised parameter " + arg);
			}
		}
		if (argcount < m_args.m_requiredArgs.size()) {
			throw std::runtime_error("Missing required parameters");
		}
		return m_args;
	}

};

