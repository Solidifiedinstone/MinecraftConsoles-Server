// ServerProperties.cpp
// Implementation of server.properties file loading/saving

#ifdef _DEDICATED_SERVER
#include "stdafx_server.h"
#else
#include "stdafx.h"
#endif

#include "ServerProperties.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <locale>
#include <codecvt>

ServerProperties::ServerProperties(const wstring& filename)
	: m_filename(filename)
	, m_modified(false)
{
	if (!load(filename))
	{
		// File doesn't exist - generate defaults
		generateDefaults();
		save(filename);
	}
}

ServerProperties::~ServerProperties()
{
	// Auto-save if modified
	if (m_modified)
	{
		save();
	}
}

bool ServerProperties::load(const wstring& filename)
{
	m_properties.clear();

	// Try to open the file
	string narrowFilename = wstringToString(filename);
	ifstream file(narrowFilename);

	if (!file.is_open())
	{
		// File doesn't exist - that's OK, we'll create it on save
		return false;
	}

	string line;
	while (getline(file, line))
	{
		// Skip empty lines and comments
		if (line.empty() || line[0] == '#' || line[0] == '!')
		{
			continue;
		}

		// Convert to wide string and parse
		wstring wline = stringToWstring(line);
		wstring key, value;

		if (parseLine(wline, key, value))
		{
			m_properties[key] = value;
		}
	}

	file.close();
	return true;
}

bool ServerProperties::parseLine(const wstring& line, wstring& key, wstring& value)
{
	// Find the '=' separator
	size_t pos = line.find(L'=');
	if (pos == wstring::npos)
	{
		// Also try ':'
		pos = line.find(L':');
		if (pos == wstring::npos)
		{
			return false;
		}
	}

	key = trim(line.substr(0, pos));
	value = trim(line.substr(pos + 1));

	// Handle escape sequences in value
	wstring unescaped;
	for (size_t i = 0; i < value.length(); i++)
	{
		if (value[i] == L'\\' && i + 1 < value.length())
		{
			switch (value[i + 1])
			{
			case L'n': unescaped += L'\n'; i++; break;
			case L'r': unescaped += L'\r'; i++; break;
			case L't': unescaped += L'\t'; i++; break;
			case L'\\': unescaped += L'\\'; i++; break;
			default: unescaped += value[i]; break;
			}
		}
		else
		{
			unescaped += value[i];
		}
	}
	value = unescaped;

	return true;
}

wstring ServerProperties::trim(const wstring& str)
{
	size_t start = str.find_first_not_of(L" \t\r\n");
	if (start == wstring::npos)
		return L"";

	size_t end = str.find_last_not_of(L" \t\r\n");
	return str.substr(start, end - start + 1);
}

wstring ServerProperties::stringToWstring(const string& str)
{
	if (str.empty())
		return L"";

	// Use UTF-8 conversion
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (size <= 0)
		return L"";

	wstring result(size - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
	return result;
}

string ServerProperties::wstringToString(const wstring& wstr)
{
	if (wstr.empty())
		return "";

	// Use UTF-8 conversion
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (size <= 0)
		return "";

	string result(size - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
	return result;
}

wstring ServerProperties::getString(const wstring& key, const wstring& defaultValue)
{
	auto it = m_properties.find(key);
	if (it != m_properties.end())
	{
		return it->second;
	}

	// Store default value
	m_properties[key] = defaultValue;
	m_modified = true;
	return defaultValue;
}

int ServerProperties::getInt(const wstring& key, int defaultValue)
{
	wstring strValue = getString(key, to_wstring(defaultValue));

	try
	{
		return stoi(strValue);
	}
	catch (...)
	{
		return defaultValue;
	}
}

bool ServerProperties::getBoolean(const wstring& key, bool defaultValue)
{
	wstring strValue = getString(key, defaultValue ? L"true" : L"false");

	// Convert to lowercase for comparison
	wstring lower = strValue;
	transform(lower.begin(), lower.end(), lower.begin(), towlower);

	if (lower == L"true" || lower == L"yes" || lower == L"1" || lower == L"on")
		return true;
	if (lower == L"false" || lower == L"no" || lower == L"0" || lower == L"off")
		return false;

	return defaultValue;
}

__int64 ServerProperties::getLong(const wstring& key, __int64 defaultValue)
{
	wstring strValue = getString(key, to_wstring(defaultValue));

	try
	{
		return _wtoi64(strValue.c_str());
	}
	catch (...)
	{
		return defaultValue;
	}
}

void ServerProperties::setString(const wstring& key, const wstring& value)
{
	m_properties[key] = value;
	m_modified = true;
}

void ServerProperties::setInt(const wstring& key, int value)
{
	setString(key, to_wstring(value));
}

void ServerProperties::setBoolean(const wstring& key, bool value)
{
	setString(key, value ? L"true" : L"false");
}

void ServerProperties::setLong(const wstring& key, __int64 value)
{
	setString(key, to_wstring(value));
}

bool ServerProperties::hasProperty(const wstring& key) const
{
	return m_properties.find(key) != m_properties.end();
}

void ServerProperties::generateDefaults()
{
	m_properties[L"server-port"] = L"25565";
	m_properties[L"max-players"] = L"8";
	m_properties[L"level-name"] = L"world";
	m_properties[L"level-seed"] = L"";
	m_properties[L"level-type"] = L"default";
	m_properties[L"gamemode"] = L"0";
	m_properties[L"difficulty"] = L"2";
	m_properties[L"pvp"] = L"true";
	m_properties[L"spawn-animals"] = L"true";
	m_properties[L"spawn-monsters"] = L"true";
	m_properties[L"spawn-npcs"] = L"true";
	m_properties[L"generate-structures"] = L"true";
	m_properties[L"motd"] = L"A MinecraftConsoles Server";
	m_modified = false; // Already saving right after
}

bool ServerProperties::save()
{
	return save(m_filename);
}

bool ServerProperties::save(const wstring& filename)
{
	string narrowFilename = wstringToString(filename);
	ofstream file(narrowFilename);

	if (!file.is_open())
	{
		return false;
	}

	// Write header comment
	file << "# Minecraft Server Properties" << endl;
	file << "# Generated by MinecraftConsoles Dedicated Server" << endl;
	file << endl;

	// Write properties
	for (const auto& pair : m_properties)
	{
		string key = wstringToString(pair.first);
		string value = wstringToString(pair.second);

		// Escape special characters in value
		string escaped;
		for (char c : value)
		{
			switch (c)
			{
			case '\n': escaped += "\\n"; break;
			case '\r': escaped += "\\r"; break;
			case '\t': escaped += "\\t"; break;
			case '\\': escaped += "\\\\"; break;
			default: escaped += c; break;
			}
		}

		file << key << "=" << escaped << endl;
	}

	file.close();
	m_modified = false;
	return true;
}
