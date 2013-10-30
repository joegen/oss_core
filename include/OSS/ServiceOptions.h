// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef OSS_SERVICEOPTIONS_H_INCLUDED
#define	OSS_SERVICEOPTIONS_H_INCLUDED


#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/detail/ptree_utils.hpp>


namespace OSS {


class ServiceOptions
{
public:
  enum OptionType
  {
    CommandLineOption,
    DaemonOption,
    ConfigOption
  };

  enum ParseOptionsFlags
  {
    NoOptionsFlag = 0,
    AddComandLineOptionsFlag = 1 << 0,
    AddDaemonOptionsFlag = 1 << 1,
    AddConfigOptionsFlag = 1 << 2,
    InitLoggerFlag = 1 << 3,
    DisplayVersionOnInitFlag = 1 << 4,
    DisplayExceptionFlag = 1 << 5,
    ValidateRequiredParametersFlag = 1 << 6,
    DefaultOptionsFlag = AddComandLineOptionsFlag |
                       AddDaemonOptionsFlag |
                       AddConfigOptionsFlag |
                       InitLoggerFlag |
                       DisplayVersionOnInitFlag |
                       ValidateRequiredParametersFlag
  };

  ServiceOptions(int argc, char** argv,
    const std::string& daemonName,
    const std::string& version,
    const std::string& copyright);

  ServiceOptions(const std::string& configFile);

  ~ServiceOptions();

  void addOptionFlag(
    char shortForm,
    const std::string& optionName,
    const std::string description,
    OptionType type);

  void addOptionFlag(
    const std::string& optionName,
    const std::string description,
    OptionType type);

  void addOptionString(
    char shortForm,
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionString(
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionStringVector(
    char shortForm,
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionStringVector(
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionInt(
    char shortForm,
    const std::string& optionName,
    const std::string description, 
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionInt(
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionIntVector(
    char shortForm,
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  void addOptionIntVector(
    const std::string& optionName,
    const std::string description,
    OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/);

  template<typename T>
  void addOption(char shortForm, const std::string& optionName, const std::string description, OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/)
  {
    boost::program_options::options_description* options;
    if (type == CommandLineOption)
      options = &_commandLineOptions;
    else if (type == DaemonOption)
      options = &_daemonOptions;
    else if (type == ConfigOption)
      options = &_configOptions;
    else
      assert(false);

    std::ostringstream strm;
    strm << optionName << "," << shortForm;
    options->add_options()(strm.str().c_str(), boost::program_options::value<T>(), description.c_str());

    if (required)
      registerRequiredParameters(optionName, altOptionName);
  }

  template<typename T>
  void addOption(const std::string& optionName, const std::string description, OptionType type,
    bool required = false,
    const std::string& altOptionName = std::string() /* alternative option if this option is required and is missing*/)
  {
    boost::program_options::options_description* options;
    if (type == CommandLineOption)
      options = &_commandLineOptions;
    else if (type == DaemonOption)
      options = &_daemonOptions;
    else if (type == ConfigOption)
      options = &_configOptions;
    else
      assert(false);

    options->add_options()(optionName.c_str(), boost::program_options::value<T>(), description.c_str());

    if (required)
      registerRequiredParameters(optionName, altOptionName);
  }

  void displayUsage(std::ostream& strm) const;

  void displayVersion(std::ostream& strm) const;

  std::size_t hasOption(const std::string& optionName, bool consolidate) const;

  size_t hasConfigOption(const std::string& optionName) const;

  bool getOption(
    const std::string& optionName,
    std::string& value,
    const std::string& defValue = std::string()) const;

  bool getOption(const std::string& optionName, std::vector<std::string>& value) const;

  bool getOption(const std::string& optionName, int& value) const;

  bool getOption(const std::string& optionName, int& value, int defValue) const;

  bool getOption(const std::string& optionName, std::vector<int>& value) const;

  bool getOption(const std::string& optionName, bool& value, bool defValue) const;

  template<typename T>
  bool getOption(const std::string& optionName, T& value) const
  {
    if (!hasOption(optionName, false))
    {
      //
      // Check if ptree has it
      //
      if (_hasConfig)
      {
        try
        {
          value = _ptree.get<T>(optionName.c_str());
          return true;
        }catch(...)
        {
          return false;
        }
      }
    }
    value = _options[optionName.c_str()].as<T>();
    return true;
  }

  template<typename T>
  bool getOption(const std::string& optionName, T& value, T defValue) const
  {
    if (!hasOption(optionName, false))
    {
      //
      // Check if ptree has it
      //
      if (_hasConfig)
      {
        try
        {
          value = _ptree.get<T>(optionName.c_str());
          return true;
        }catch(...)
        {
          value = defValue;
        }
      }
      else
      {
        value = defValue;
      }
    }else
    {
      value = _options[optionName.c_str()].as<T>();
    }
    return true;
  }

  void addDaemonOptions();

  void addCommandLineOptions();

  bool checkCommandLineOptions();

  bool checkDaemonOptions();

  bool checkConfigOptions();

  bool checkOptions(ParseOptionsFlags parseOptionsFlags, int& exitCode);

  bool parseOptions(ParseOptionsFlags parseOptionsFlags = DefaultOptionsFlag);

  void initlogger();

  static void  daemonize(int argc, char** argv, bool& isdaemonized);

  static void waitForTerminationRequest();

  static void catch_global();
protected:
  void registerRequiredParameters(const std::string& optionName, const std::string& altOptionName);
  bool validateRequiredParameters();

  int _argc;
  char** _argv;
  std::string _daemonName;
  std::string _version;
  std::string _copyright;

  boost::program_options::options_description _daemonOptions;
  boost::program_options::options_description _commandLineOptions;
  boost::program_options::options_description _configOptions;
  boost::program_options::options_description _optionItems;
  boost::program_options::variables_map _options;
  //
  // Pre-parsed values
  //
  bool _isDaemon;
  std::string _pidFile;
  std::string _configFile;
  boost::property_tree::ptree _ptree;
  bool _hasConfig;
  bool _isConfigOnly;
  //
  // A vector the contains required parameters
  //
  std::vector<std::string> _required;
  //
  // A map that contains alternative parameter for a required parameter.
  //
  std::map<std::string, std::string> _alternative;

  friend class ServiceOptionsTest;
  bool _unitTestMode;
};



//
// Inlines
//

inline bool ServiceOptions::getOption(const std::string& optionName, int& value) const
{
  return getOption<int>(optionName, value);
}

inline bool ServiceOptions::getOption(const std::string& optionName, int& value, int defValue) const
{
  return getOption<int>(optionName, value, defValue);
}

inline bool ServiceOptions::getOption(const std::string& optionName, std::string& value, const std::string& defValue) const
{
  return getOption<std::string>(optionName, value, defValue);
}

inline void ServiceOptions::addOptionInt(
  char shortForm,
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption<int>(shortForm, optionName, description, type, required, altOptionName);
}

inline void ServiceOptions::addOptionInt(
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption<int>(optionName, description, type, required, altOptionName);
}

inline void ServiceOptions::addOptionString(
  char shortForm,
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption<std::string>(shortForm, optionName, description, type, required, altOptionName);
}

inline void ServiceOptions::addOptionString(
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption<std::string>(optionName, description, type, required, altOptionName);
}


inline void ServiceOptions::addOptionStringVector(
  char shortForm,
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption< std::vector<std::string> >(shortForm, optionName, description, type, required, altOptionName);
}

inline void ServiceOptions::addOptionStringVector(
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption< std::vector<std::string> >(optionName, description, type, required, altOptionName);
}

inline void ServiceOptions::addOptionIntVector(
  char shortForm,
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption< std::vector<int> >(shortForm, optionName, description, type, required, altOptionName);
}

inline void ServiceOptions::addOptionIntVector(
  const std::string& optionName,
  const std::string description,
  OptionType type,
  bool required,
  const std::string& altOptionName)
{
  addOption< std::vector<int> >(optionName, description, type, required, altOptionName);
}


} // OSS

#endif	/// OSS_SERVICEOPTIONS_H_INCLUDED

