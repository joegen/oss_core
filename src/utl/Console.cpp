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

#include <signal.h>
#include "OSS/UTL/Console.h"
#include "OSS/UTL/linenoise.h"

namespace OSS {
namespace UTL {


typedef std::map<std::string, std::vector<std::string> > CompletionMap;
static CompletionMap completionMap;
static CompletionMap hintsMap;
static bool hintBold = true;
static int hintColor = 35;
static bool hasRegisteredCompletionCb = false;
static bool hasRegisteredHintsCb = false;
static std::string sigIntMsg;

static void completion_cb(const char *buf, linenoiseCompletions *lc)
{
  if (!buf || !strlen(buf) || !lc)
  {
    return;
  }
  
  for (CompletionMap::iterator completionIter = completionMap.lower_bound(buf);
    completionIter != completionMap.end(); completionIter++)
  {
    if (OSS::string_starts_with(completionIter->first, buf))
    {
      for (std::vector<std::string>::iterator iter = completionIter->second.begin();
        iter != completionIter->second.end(); iter++)
      {
        linenoiseAddCompletion(lc, iter->c_str());
      }
    }
    else
    {
      break;
    }
  }
}

static char *hints_cb(const char *buf, int *color, int *bold)
{
  CompletionMap::iterator hintIter = hintsMap.find(buf);
  if (hintIter != hintsMap.end())
  {
    *color = hintColor;
    *bold = hintBold;
    return (char*)hintIter->second.front().c_str();
  }
  return 0;
}

std::string Console::prompt(const std::string& message)
{
  std::string ret;
  char* line = linenoise(message.c_str());
  if (line)
  {
    ret = line;
    linenoiseHistoryAdd(line);
  }
  free(line);
  return ret;
}

void Console::registerCompletion(const std::string& prefix, const std::string& completion)
{
  if (!hasRegisteredCompletionCb)
  {
    linenoiseSetCompletionCallback(completion_cb);
    hasRegisteredCompletionCb = true;
  }
  
  CompletionMap::iterator iter = completionMap.find(prefix);
  if (iter == completionMap.end())
  {
    std::vector<std::string> items;
    items.push_back(completion);
    completionMap[prefix] = items;
  }
  else
  {
    iter->second.push_back(completion);
  }
}

void Console::registerHint(const std::string& prefix, const std::string& hint)
{
  if (!hasRegisteredHintsCb)
  {
    linenoiseSetHintsCallback(hints_cb);
    hasRegisteredHintsCb = true;
  }
  
  CompletionMap::iterator iter = hintsMap.find(prefix);
  if (iter == hintsMap.end())
  {
    std::vector<std::string> items;
    items.push_back(hint);
    hintsMap[prefix] = items;
  }
  else
  {
    iter->second[0] = hint;
  }
}

void Console::setHintProperties(int color, bool bold)
{
  hintColor = color;
  hintBold = bold;
}

void Console::setMultiLine(bool multiLine)
{
  linenoiseSetMultiLine(multiLine);
}
  



} } // OSS::UTL
  



