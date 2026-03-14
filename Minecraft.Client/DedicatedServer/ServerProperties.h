// ServerProperties.h
// Load/save server.properties configuration file
// Implements Java-style properties file parsing

#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

using namespace std;

class ServerProperties
{
public:
	// Constructor - loads properties from file
	ServerProperties(const wstring& filename);
	~ServerProperties();

	// Get property values with defaults
	wstring getString(const wstring& key, const wstring& defaultValue);
	int getInt(const wstring& key, int defaultValue);
	bool getBoolean(const wstring& key, bool defaultValue);
	__int64 getLong(const wstring& key, __int64 defaultValue);

	// Set property values
	void setString(const wstring& key, const wstring& value);
	void setInt(const wstring& key, int value);
	void setBoolean(const wstring& key, bool value);
	void setLong(const wstring& key, __int64 value);

	// Save properties to file
	bool save();
	bool save(const wstring& filename);

	// Check if a property exists
	bool hasProperty(const wstring& key) const;

	// Get the filename
	const wstring& getFilename() const { return m_filename; }

private:
	// Load properties from file
	bool load(const wstring& filename);

	// Generate default properties
	void generateDefaults();

	// Parse a line from the properties file
	bool parseLine(const wstring& line, wstring& key, wstring& value);

	// Trim whitespace from string
	static wstring trim(const wstring& str);

	// Convert string to wide string
	static wstring stringToWstring(const string& str);

	// Convert wide string to string
	static string wstringToString(const wstring& wstr);

private:
	wstring m_filename;
	unordered_map<wstring, wstring> m_properties;
	bool m_modified;
};
