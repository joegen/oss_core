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


#ifndef OSS_EXCEPTION_H_INCLUDED
#define OSS_EXCEPTION_H_INCLUDED


#include <stdexcept>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>
#include "OSS/OSS.h"


namespace OSS {


class OSS_API Exception: public std::exception
	/// This is the base class for all exceptions defined
	/// in the OSS class library.
{
public:
	Exception(const std::string& msg, int code = 0);
		/// Creates an exception.

	Exception(const std::string& msg, const std::string& arg, int code = 0);
		/// Creates an exception.

	Exception(const std::string& msg, const Exception& nested, int code = 0);
		/// Creates an exception and stores a clone
		/// of the nested exception.

	Exception(const Exception& exc);
		/// Copy constructor.

  Exception(const std::exception& exc);
		/// Copy constructor.

  Exception(const boost::system::system_error& exc);
		/// Copy constructor.
		
	~Exception() throw();
		/// Destroys the exception and deletes the nested exception.

	Exception& operator = (const Exception& exc);
		/// Assignment operator.

  Exception& operator = (const std::exception& exc);
    /// Assignment operator.

  Exception& operator = (const boost::system::system_error& exc);

	virtual const char* name() const throw();
		/// Returns a static string describing the exception.
		
	virtual const char* className() const throw();
		/// Returns the name of the exception class.
		
	virtual const char* what() const throw();
		/// Returns a static string describing the exception.
		///
		/// Same as name(), but for compatibility with std::exception.
		
	const Exception* nested() const;
		/// Returns a pointer to the nested exception, or
		/// null if no nested exception exists.
			
	const std::string& message() const;
		/// Returns the message text.
			
	int code() const;
		/// Returns the exception code if defined.
		
	std::string displayText() const;
		/// Returns a string consisting of the
		/// message name and the message text.

	virtual Exception* clone() const;
		/// Creates an exact copy of the exception.
		///
		/// The copy can later be thrown again by
		/// invoking rethrow() on it.
		
	virtual void rethrow() const;
		/// (Re)Throws the exception.
		///
		/// This is useful for temporarily storing a
		/// copy of an exception (see clone()), then
		/// throwing it again.

protected:
	Exception(int code = 0);
		/// Standard constructor.
		
private:
	std::string _msg;
	Exception*  _pNested;
	int			_code;
};


//
// inlines
//
inline const Exception* Exception::nested() const
{
	return _pNested;
}


inline const std::string& Exception::message() const
{
	return _msg;
}


inline int Exception::code() const
{
	return _code;
}


//
// Macros for quickly declaring and implementing exception classes.
// Unfortunately, we cannot use a template here because character
// pointers (which we need for specifying the exception name)
// are not allowed as template arguments.
//
#define OSS_DECLARE_EXCEPTION(API, CLS, BASE) \
	class API CLS: public BASE														\
	{																				\
	public:																			\
		CLS(int code = 0);															\
		CLS(const std::string& msg, int code = 0);									\
		CLS(const std::string& msg, const std::string& arg, int code = 0);			\
		CLS(const std::string& msg, const OSS::Exception& exc, int code = 0);		\
		CLS(const CLS& exc);														\
		~CLS() throw();																\
		CLS& operator = (const CLS& exc);											\
		const char* name() const throw();											\
		const char* className() const throw();										\
		OSS::Exception* clone() const;												\
		void rethrow() const;														\
	};


#define OSS_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)	\
	CLS::CLS(int code): BASE(code)		\
	{					\
	}					\
	CLS::CLS(const std::string& msg, int code): BASE(msg, code)	\
	{					\
	}					\
	CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code)		\
	{					\
	}					\
	CLS::CLS(const std::string& msg, const OSS::Exception& exc, int code): BASE(msg, exc, code)	\
	{					\
	}					\
	CLS::CLS(const CLS& exc): BASE(exc)	\
	{					\
	}					\
	CLS::~CLS() throw()			\
	{					\
	}					\
	CLS& CLS::operator = (const CLS& exc)			\
	{					\
		BASE::operator = (exc);	\
		return *this;	\
	}	\
	const char* CLS::name() const throw()	\
	{				\
		return NAME;	\
	}		\
	const char* CLS::className() const throw() \
	{																								\
		return typeid(*this).name();	\
	}				\
	OSS::Exception* CLS::clone() const \
	{																								\
		return new CLS(*this); 	\
	}			\
	void CLS::rethrow() const \
	{			\
		throw *this;	\
	}


#define OSS_CREATE_INLINE_EXCEPTION(CLS, BASE, NAME) \
	class OSS_API CLS: public BASE	{ \
	public: \
		CLS(int code = 0); \
		CLS(const std::string& msg, int code = 0); \
		CLS(const std::string& msg, const std::string& arg, int code = 0); \
		CLS(const std::string& msg, const OSS::Exception& exc, int code = 0); \
		CLS(const CLS& exc); \
		~CLS() throw();																\
		CLS& operator = (const CLS& exc); \
		const char* name() const throw(); \
		const char* className() const throw();	\
		OSS::Exception* clone() const;	\
		void rethrow() const; \
	}; \
        inline CLS::CLS(int code): BASE(code)		\
	{		                    \
	}			            \
	inline CLS::CLS(const std::string& msg, int code): BASE(msg, code) \
	{			\
	}			\
	inline CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code)		\
	{ \
	}  \
	inline CLS::CLS(const std::string& msg, const OSS::Exception& exc, int code): BASE(msg, exc, code)	\
	{	\
	}       \
	inline CLS::CLS(const CLS& exc): BASE(exc)	\
	{						\
	}						\
	inline CLS::~CLS() throw()			\
	{						\
	}						\
	inline CLS& CLS::operator = (const CLS& exc)	\
	{						\
		BASE::operator = (exc);			\
		return *this;				\
	}						\
	inline const char* CLS::name() const throw()	\
	{						\
		return NAME;				\
	}						\
	inline const char* CLS::className() const throw()		\
	{								\
		return typeid(*this).name();                            \
	}								\
	inline OSS::Exception* CLS::clone() const			\
	{								\
		return new CLS(*this);					\
	}								\
	inline void CLS::rethrow() const				\
	{								\
		throw *this;						\
	}


//
// Standard exception classes
//
OSS_DECLARE_EXCEPTION(OSS_API, LogicException, Exception)
OSS_DECLARE_EXCEPTION(OSS_API, AssertionViolationException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, NullPointerException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, BugcheckException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, InvalidArgumentException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, NotImplementedException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, RangeException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, IllegalStateException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, InvalidAccessException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, SignalException, LogicException)
OSS_DECLARE_EXCEPTION(OSS_API, UnhandledException, LogicException)

OSS_DECLARE_EXCEPTION(OSS_API, RuntimeException, Exception)
OSS_DECLARE_EXCEPTION(OSS_API, NotFoundException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, ExistsException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, TimeoutException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, SystemException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, RegularExpressionException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, LibraryLoadException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, LibraryAlreadyLoadedException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, NoThreadAvailableException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, PropertyNotSupportedException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, PoolOverflowException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, NoPermissionException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, OutOfMemoryException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, DataException, RuntimeException)

OSS_DECLARE_EXCEPTION(OSS_API, DataFormatException, DataException)
OSS_DECLARE_EXCEPTION(OSS_API, SyntaxException, DataException)
OSS_DECLARE_EXCEPTION(OSS_API, CircularReferenceException, DataException)
OSS_DECLARE_EXCEPTION(OSS_API, PathSyntaxException, SyntaxException)
OSS_DECLARE_EXCEPTION(OSS_API, IOException, RuntimeException)
OSS_DECLARE_EXCEPTION(OSS_API, FileException, IOException)
OSS_DECLARE_EXCEPTION(OSS_API, FileExistsException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, FileNotFoundException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, PathNotFoundException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, FileReadOnlyException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, FileAccessDeniedException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, CreateFileException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, OpenFileException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, WriteFileException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, ReadFileException, FileException)
OSS_DECLARE_EXCEPTION(OSS_API, UnknownURISchemeException, RuntimeException)

OSS_DECLARE_EXCEPTION(OSS_API, ApplicationException, Exception)
OSS_DECLARE_EXCEPTION(OSS_API, BadCastException, RuntimeException)

OSS_DECLARE_EXCEPTION(OSS_API, NetException, IOException);

} // namespace OSS


#endif //OSS_EXCEPTION_H_INCLUDED

