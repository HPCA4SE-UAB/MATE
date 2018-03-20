
/**************************************************************************
*                    Universitat Autonoma de Barcelona,					  *
*              HPC4SE: http://grupsderecerca.uab.cat/hpca4se/             *
*                        Analysis and Tuning Group, 					  *
*					            2002 - 2018                  			  */
/**************************************************************************
*	  See the LICENSE.md file in the base directory for more details      *
*									-- 									  *
*	This file is part of MATE.											  *	
*																		  *
*	MATE is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by  *
*	the Free Software Foundation, either version 3 of the License, or     *
*	(at your option) any later version.									  *
*																		  *
*	MATE is distributed in the hope that it will be useful,				  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		  *
*	GNU General Public License for more details.						  *
*																		  *
*	You should have received a copy of the GNU General Public License     *
*	along with MATE.  If not, see <http://www.gnu.org/licenses/>.         *
*																		  *
***************************************************************************/

// MATE
// Copyright (C) 2002-2008 Ania Sikora, UAB.

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __SYSLOG_H__
#define __SYSLOG_H__

// Local includes
#include "DateTime.h" 
#include "Config.h"
#include "auto_vector.h"

// C++ includes
#include <string>
#include <memory>
#include <fstream>

#define MaxLineLen 1024

namespace Common{
	enum LogSeverity
	{
		DEBUG		= 1,	// A log generated during debugging of the software.
		INFO		= 2,	// An informational message.
		WARNING		= 4,	// A warning message that the system administrator might want to know about
		ERROR		= 8,	// One of the software components caused an error or exception.
		FATAL		= 16	// One of the software components is no longer functional.
	};

	/**
	 * @class LogEntry
	 * @brief Entry on a log.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class LogEntry
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param s Log severity, can be DEBUG, INFO, WARNING, ERROR or FATAL.
		 * @param message Message.
		 */
		LogEntry (LogSeverity s, std::string const & message)
			: _severity (s), _message (message)
		{}

		/**
		 * @brief Returns the date when the entry was performed.
		 */
		DateTime const & GetTimestamp () const  { return _t; }

		/**
		 * @brief Returns log severity.
		 *
		 * DEBUG	A log generated during debugging of the software.
		 * INFO		An informational message.
		 * WARNING	A warning message that the system administrator might
		 * 			want to know about
		 * ERROR	One of the software components caused an error or exception.
		 * FATAL	One of the software components is no longer functional.
		 *
		 */
		LogSeverity GetSeverity () const { return _severity; }

		/**
		 * @brief Returns a string containing the log message.
		 */
		std::string const & GetMessage () const { return _message; }
	private:
		DateTime	_t;
		LogSeverity	_severity;
		std::string	_message;
	};

	/**
	 * @class LogFilter
	 * @brief Abstract class, validates logs.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class LogFilter
	{
	public:

		/**
		 * @brief Constructor.
		 */
		virtual ~LogFilter () {}

		/**
		 * @brief Filters log entry.
		 * @return True if entry is accepted, false otherwise.
		 */
		virtual bool Accept (LogEntry const & entry) const = 0;
	};

	typedef std::auto_ptr<LogFilter> LogFilterPtr;

	/**
	 * @class LogFormatter
	 * @brief Abstract class, Gives logs the correct format.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class LogFormatter
	{
	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~LogFormatter () {}

		/**
		 * @brief Returns a string containing the log header.
		 */
		virtual std::string GetLogHeader () const = 0;

		/**
		 * @brief Returns a string containing the log footer.
		 */
		virtual std::string GetLogFooter () const = 0;

		/**
		 * @brief Returns a string containing the LogEntry object formatted.
		 */
		virtual std::string Format (LogEntry const & entry) const = 0;
	};

	typedef std::auto_ptr<LogFormatter> LogFormatterPtr;

	/**
	 * @class Logger
	 * @brief Abstract class, tracks and stores information
	 * about events of interest happening in a system.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class Logger
	{
	public:

		/**
		 * @brief Destructor.
		 */
		virtual ~Logger () {}

        /**
         * @brief Constructor
         * @param entry
         */
		virtual void Log (LogEntry const & entry) = 0;

		/**
		 * @brief Sets logger name.
		 */
		void SetName (std::string const & name) { _name = name; }

		/**
		 * @brief Returns a string containing the logger name.
		 */
		std::string const & GetName () const { return _name; }

		/**
		 * @brief Sets the LogFilter to be used by the logger.
		 */
		virtual void SetFilter (LogFilterPtr & filter) = 0;

		/**
		 * @brief Returns the LogFilter the logger uses.
		 */
		virtual LogFilter const * GetFilter () const = 0;

		/**
		 * @brief Sets the LogFormatter to be used by the logger.
		 */
		virtual void SetFormatter (LogFormatterPtr & formatter) = 0;

		/**
		 * @brief Returns the LogFormatter the logger uses.
		 */
		virtual LogFormatter const * GetFormatter () const = 0;

	private:
		std::string _name;
	};

	typedef std::auto_ptr<Logger> LoggerPtr;

	/**
	 * @class BasicLogFilter
	 * @brief Filters LogEntry objects to be inserted in a log.
	 *
	 * @extends LogFilter
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class BasicLogFilter: public LogFilter
	{
	public:

		/**
		 * @brief Constructor.
		 */
		BasicLogFilter ();

		/**
		 * @brief Constructor.
		 * @param mask Severity mask.
		 */
		BasicLogFilter (int mask) : _mask (mask) {}

		/**
		 * @brief Returns true if the log is accepted, false otherwise.
		 */
		bool Accept (LogEntry const & entry) const
		{
			return (entry.GetSeverity () & _mask) != 0;
		}

	private:

		int _mask;

	};

	/**
	 * @class BasicLogFormatter
	 * @brief Formats LogEntry objects to be inserted in a log.
	 *
	 * @extends LogFormatter
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class BasicLogFormatter: public LogFormatter
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 */
		BasicLogFormatter ()
			: _showTimestamp (true), _showSeverity (true), _showChannel (true)
		{}

		/**
		 * @brief Constructor.
		 *
		 * @param cfg A Config object containing initial settings of
		 * the log formatter.
		 *
		 */
		BasicLogFormatter (Config & cfg);

		/**
		 * @brief Returns a string containing the log header.
		 */
		std::string GetLogHeader () const;

		/**
		 * @brief Returns a string containing the log footer.
		 */
		std::string GetLogFooter () const;

		/**
		 * @brief Returns a string containing the LogEntry object formatted.
		 */
		std::string Format (LogEntry const & entry) const;

		/**
		 * @brief Enables the timestamp view.
		 */
		void ShowTimestamp (bool value) { _showTimestamp = value; }

		/**
		 * @brief Enables the severity view.
		 */
		void ShowSeverity (bool value) { _showSeverity = value; }

		/**
		 * @brief Enables the channel view.
		 */
		void ShowChannel (bool value) { _showChannel = value; }

	private:

		static std::string GetSeverityString (LogSeverity s);
		std::string FormatTimestamp (DateTime const & dt) const;

	private:
		// date format
		bool	_showTimestamp;
		bool	_showSeverity;
		bool	_showChannel;
		string	_prefix;
	};

	/**
	 * @class BasicLogger
	 * @brief Stores information of events in a system.
	 *
	 * @extends Logger
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class BasicLogger: public Logger
	{
	public:

		/**
		 * @brief Constructor.
		 */
		BasicLogger ();

		/**
		 * @brief Sets the LogFilter to be used by the logger.
		 */
		void SetFilter (LogFilterPtr & filter) { _filter = filter; }

		/**
		 * @brief Returns the LogFilter the logger uses.
		 */
		LogFilter const * GetFilter () const { return _filter.get (); }

		/**
		 * @brief Sets the LogFormatter to be used by the logger.
		 */
		void SetFormatter (LogFormatterPtr & formatter) { _formatter = formatter; }

		/**
		 * @brief Returns the LogFormatter the logger uses.
		 */
		LogFormatter const * GetFormatter () const { return _formatter.get (); }

		/**
		 * @brief Inserts an entry to the log.
		 * @return True if the insert was successful, false otherwise.
		 */
		bool Accept (LogEntry const & entry) const;

	protected:

		LogFilterPtr	_filter;
		LogFormatterPtr _formatter;

	};

	/**
	 * @class StreamLogger
	 * @brief Stores the logged information into a stream.
	 *
	 * @extends BasicLogger
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class StreamLogger: public BasicLogger
	{
	public:

		/**
		 * @brief Constructor.
		 */
		StreamLogger (std::ostream & stream);

		/**
		 * @brief Destructor.
		 */
		~StreamLogger ();

		/**
		 * @brief Inserts an entry to the log.
		 */
		void Log (LogEntry const & entry);

	private:

		std::ostream & _stream;

	};

	/**
	 * @class FileLogger
	 * @brief Stores information of interest into a file.
	 *
	 * @extends BasicLogger
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */

	// ------------------------------------------------------------
	class FileLogger: public BasicLogger
	{
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param filepath Path of the file where the log will be stored.
		 * @para append Flag that determines if the file will be overwritten or
		 * the logs will be appended, default false.
		 *
		 * @throw SysException
		 */
		FileLogger (std::string const & filepath, bool append = false);

		/**
		 * @brief Destructor.
		 */
		~FileLogger ();

		/**
		 * @brief Inserts a new entry to the log.
		 */
		void Log (LogEntry const & entry);

	private:
		std::ofstream _stream;
	};

	/**
	 * @class Syslog
	 * @brief Holds and manages a loggers on the system.
	 *
	 * This is a static class that can't be instantiated.
	 *
	 * @version 1.0b
	 * @since 1.0b
	 * @author Ania Sikora, 2002
	 */
	
	// ------------------------------------------------------------
	class Syslog
	{
	public:
		typedef auto_vector<Logger> LoggerVector;
		typedef auto_iterator<Logger> LoggerIterator;

	public:
		/**
		 * @brief Configures the system with a default configuration.
		 *
		 * This configuration uses an only Logger that outputs it's logs
		 * on the standard error output.
		 *
		 *@throw SysException
		 */
		static void Configure ();

		/**
		 * @brief Configures the logger with a given Config.
		 */
		static void Configure (Config & cfg, string loggerName = "");

		/**
		 * @brief Adds an entry with the given event to all the loggers.
		 */
		static void LogEvent (LogSeverity s, std::string const & message);

		/**
		 * @brief Logs an event with DEBUG level of severity.
		 *
		 * Uses a string as a parameter.
		 *
		 */
		static void Debug (std::string const & message);

		/**
		 * @brief Logs an event with INFO level of severity.
		 *
		 * Uses a string as a parameter.
		 *
		 */
		static void Info (std::string const & message);

		/**
		 * @brief Logs an event with WARNING level of severity.
		 *
		 * Uses a string as a parameter.
		 *
		 */
		static void Warn (std::string const & message);

		/**
		 * @brief Logs an event with ERROR level of severity.
		 *
		 * Uses a string as a parameter.
		 *
		 */
		static void Error (std::string const & message);

		/**
		 * @brief Logs an event with FATAL level of severity.
		 *
		 * Uses a string as a parameter.
		 *
		 */
		static void Fatal (std::string const & message);

		/**
		 * @brief Logs an event with DEBUG level of severity.
		 *
		 * Uses a char * as a parameter.
		 *
		 */
		static void Debug (char * formatStr, ...);

		/**
		 * @brief Logs an event with INFO level of severity.
		 *
		 * Uses a char * as a parameter.
		 *
		 */
		static void Info (char * formatStr, ...);

		/**
		 * @brief Logs an event with WARNING level of severity.
		 *
		 * Uses a char * as a parameter.
		 *
		 */
		static void Warn (char * formatStr, ...);

		/**
		 * @brief Logs an event with ERROR level of severity.
		 *
		 * Uses a char * as a parameter.
		 *
		 */
		static void Error (char * formatStr, ...);

		/**
		 * @brief Logs an event with FATAL level of severity.
		 *
		 * Uses a char * as a parameter.
		 *
		 */
		static void Fatal (char * formatStr, ...);

		/**
		 * @brief Returns true if the system log is enabled, false otherwise.
		 */
		static bool CanWrite () { return _masterSwitch; }

		/**
		 * @brief Returns logger with given name.
		 */
		static Logger const * GetLogger (std::string const & name);

		/**
		 * @brief Adds a new logger to the system log.
		 */
		static void AddLogger (LoggerPtr & logger);

		/**
		 * @brief Not implemented.
		 */
		static void RemoveLogger (std::string const & name);

		/**
		 * @brief Returns an iterator pointing to the first logger.
		 */
		static LoggerIterator GetLoggers () { return _loggers.begin(); }
	
	private:
		Syslog () {}
		static int GetSeverityMask (int level);

		static bool			_masterSwitch;  // master log switch
		static LoggerVector _loggers;		// vector of loggers

	};
}

using namespace Common;

inline void Syslog::Info (std::string const & message)
{
	Syslog::LogEvent (INFO, message);
}

inline void Syslog::Debug (std::string const & message)
{
	Syslog::LogEvent (DEBUG, message);
}

inline void Syslog::Warn (std::string const & message)
{
	Syslog::LogEvent (WARNING, message);
}

inline void Syslog::Fatal (std::string const & message)
{
	Syslog::LogEvent (FATAL, message);
}

inline void Syslog::Error (std::string const & message)
{
	Syslog::LogEvent (ERROR, message);
}

#endif


