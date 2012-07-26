// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
//
// Copyright (c) OSS Software Solutions
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


#include <typeinfo>

#include "OSS/Exception.h"


namespace OSS {

Exception::Exception(int code): _pNested(0), _code(code)
{
}


Exception::Exception(const std::string& msg, int code): _msg(msg), _pNested(0), _code(code)
{
}


Exception::Exception(const std::string& msg, const std::string& arg, int code): _msg(msg), _pNested(0), _code(code)
{
	if (!arg.empty())
	{
		_msg.append(": ");
		_msg.append(arg);
	}
}


Exception::Exception(const std::string& msg, const Exception& nested, int code): _msg(msg), _pNested(nested.clone()), _code(code)
{
}


Exception::Exception(const Exception& exc):
	std::exception(exc),
	_msg(exc._msg),
	_code(exc._code)
{
	_pNested = exc._pNested ? exc._pNested->clone() : 0;
}

Exception::Exception(const std::exception& exc):
	_code(0),
  _pNested(0)
{
	_msg = exc.what();
}

Exception::Exception(const boost::system::system_error& exc) :
    _pNested(0)
{
  _msg = exc.what();
  _code = exc.code().value();
}
	
Exception::~Exception() throw()
{
	delete _pNested;
}


Exception& Exception::operator = (const Exception& exc)
{
	if (&exc != this)
	{
		delete _pNested;
		_msg     = exc._msg;
		_pNested = exc._pNested ? exc._pNested->clone() : 0;
		_code    = exc._code;
	}
	return *this;
}


Exception& Exception::operator = (const std::exception& exc)
{
  if (&exc != this)
	{
		delete _pNested;
    _pNested = 0;
    _code = 0;
    _msg = exc.what();
	}
	return *this;
}

Exception& Exception::operator = (const boost::system::system_error& exc)
{
  delete _pNested;
  _pNested = 0;
  _code = exc.code().value();
  _msg = exc.what();
	return *this;
}

const char* Exception::name() const throw()
{
	return "Exception";
}


const char* Exception::className() const throw()
{
	return typeid(*this).name();
}

	
const char* Exception::what() const throw()
{
  if(_msg.empty())
    return name();
  return _msg.c_str();
}

	
std::string Exception::displayText() const
{
	std::string txt = name();
	if (!_msg.empty())
	{
		txt.append(": ");
		txt.append(_msg);
	}
	return txt;
}


Exception* Exception::clone() const
{
	return new Exception(*this);
}


void Exception::rethrow() const
{
	throw *this;
}


OSS_IMPLEMENT_EXCEPTION(LogicException, Exception, "Logic exception")
OSS_IMPLEMENT_EXCEPTION(AssertionViolationException, LogicException, "Assertion violation")
OSS_IMPLEMENT_EXCEPTION(NullPointerException, LogicException, "Null pointer")
OSS_IMPLEMENT_EXCEPTION(BugcheckException, LogicException, "Bugcheck")
OSS_IMPLEMENT_EXCEPTION(InvalidArgumentException, LogicException, "Invalid argument")
OSS_IMPLEMENT_EXCEPTION(NotImplementedException, LogicException, "Not implemented")
OSS_IMPLEMENT_EXCEPTION(RangeException, LogicException, "Out of range")
OSS_IMPLEMENT_EXCEPTION(IllegalStateException, LogicException, "Illegal state")
OSS_IMPLEMENT_EXCEPTION(InvalidAccessException, LogicException, "Invalid access")
OSS_IMPLEMENT_EXCEPTION(SignalException, LogicException, "Signal received")
OSS_IMPLEMENT_EXCEPTION(UnhandledException, LogicException, "Unhandled exception")

OSS_IMPLEMENT_EXCEPTION(RuntimeException, Exception, "Runtime exception")
OSS_IMPLEMENT_EXCEPTION(NotFoundException, RuntimeException, "Not found")
OSS_IMPLEMENT_EXCEPTION(ExistsException, RuntimeException, "Exists")
OSS_IMPLEMENT_EXCEPTION(TimeoutException, RuntimeException, "Timeout")
OSS_IMPLEMENT_EXCEPTION(SystemException, RuntimeException, "System exception")
OSS_IMPLEMENT_EXCEPTION(RegularExpressionException, RuntimeException, "Error in regular expression")
OSS_IMPLEMENT_EXCEPTION(LibraryLoadException, RuntimeException, "Cannot load library")
OSS_IMPLEMENT_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException, "Library already loaded")
OSS_IMPLEMENT_EXCEPTION(NoThreadAvailableException, RuntimeException, "No thread available")
OSS_IMPLEMENT_EXCEPTION(PropertyNotSupportedException, RuntimeException, "Property not supported")
OSS_IMPLEMENT_EXCEPTION(PoolOverflowException, RuntimeException, "Pool overflow")
OSS_IMPLEMENT_EXCEPTION(NoPermissionException, RuntimeException, "No permission")
OSS_IMPLEMENT_EXCEPTION(OutOfMemoryException, RuntimeException, "Out of memory")
OSS_IMPLEMENT_EXCEPTION(DataException, RuntimeException, "Data error")

OSS_IMPLEMENT_EXCEPTION(DataFormatException, DataException, "Bad data format")
OSS_IMPLEMENT_EXCEPTION(SyntaxException, DataException, "Syntax error")
OSS_IMPLEMENT_EXCEPTION(CircularReferenceException, DataException, "Circular reference")
OSS_IMPLEMENT_EXCEPTION(PathSyntaxException, SyntaxException, "Bad path syntax")
OSS_IMPLEMENT_EXCEPTION(IOException, RuntimeException, "I/O error")
OSS_IMPLEMENT_EXCEPTION(FileException, IOException, "File access error")
OSS_IMPLEMENT_EXCEPTION(FileExistsException, FileException, "File exists")
OSS_IMPLEMENT_EXCEPTION(FileNotFoundException, FileException, "File not found")
OSS_IMPLEMENT_EXCEPTION(PathNotFoundException, FileException, "Path not found")
OSS_IMPLEMENT_EXCEPTION(FileReadOnlyException, FileException, "File is read-only")
OSS_IMPLEMENT_EXCEPTION(FileAccessDeniedException, FileException, "Access to file denied")
OSS_IMPLEMENT_EXCEPTION(CreateFileException, FileException, "Cannot create file")
OSS_IMPLEMENT_EXCEPTION(OpenFileException, FileException, "Cannot open file")
OSS_IMPLEMENT_EXCEPTION(WriteFileException, FileException, "Cannot write file")
OSS_IMPLEMENT_EXCEPTION(ReadFileException, FileException, "Cannot read file")
OSS_IMPLEMENT_EXCEPTION(UnknownURISchemeException, RuntimeException, "Unknown URI scheme")

OSS_IMPLEMENT_EXCEPTION(NetException, IOException, "NET Exception");

OSS_IMPLEMENT_EXCEPTION(ApplicationException, Exception, "Application exception")
OSS_IMPLEMENT_EXCEPTION(BadCastException, RuntimeException, "Bad cast exception")

} // OSS

