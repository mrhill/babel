#ifndef bbBABEL_LOG_H_
#define bbBABEL_LOG_H_

/** @file
	Logging.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupLog Logging
@{*/

/** Constants specifying log levels */
typedef enum
{
    bbFatal = 0,/**< Log level: Fatal error message.
                     To be instantly displayed to the user. */
    bbErr,      /**< Log level: Error message.
                     To be instantly displayed to the user. */
    bbWarning,  /**< Log level: Warning message, 
                     To be instantly displayed to the user. */
    bbMsg,      /**< Log level: Informational message.
                     To be instantly displayed to the user. */
    bbStatus,   /**< Log level: Status message.
                     To be indirectly displayed in a status fascility, 
                     usually a very short message */
    bbVerbose,  /**< Log level: Verbose message.
                     To be displayed to the user with possible delay,
                     usually this log level will be disabled. */
    bbDebug,    /**< Log level: Debug messages.
                     To be displayed to the user with possible delay,
                     intended for the development process. */
    bbUser,     /**< Log level: User defined.
                     A user defined log level. */

    bbNumLogLevels /**< Number of defined log levels. */

} bbLOGLEVEL;

/** Log a message with given log level.
    @param loglevel Log level
    @param pFmt Format string (see bbPrintf)
*/
void bbLog(const bbLOGLEVEL loglevel, const bbCHAR* pFmt, ...);

/** Log a message with given log level.
    @param loglevel Log level
    @param pFmt Format string
*/
void bbLogV(const bbLOGLEVEL loglevel, const bbCHAR* pFmt, bbVALIST);

/** Prototype for user log handler.
    A function of this type can be set as log handler using #bbSetLogHandler.
    A call of #bbLog will be forwarded to the currently set log handler,
    if the log level specified by the first parameter of #bbLog is enabled.
    @param loglevel Log level to log for
    @param pMsg Log message
*/
typedef void (*bbLOGHANDLER)(const bbLOGLEVEL loglevel, const bbCHAR* pMsg);

/** Set a new log handler.
    @param loghandler Callback address of log handler
    @return Old log handler
*/
bbLOGHANDLER bbLogSetHandler(bbLOGHANDLER loghandler);

/** Default log handler.
    @param loglevel Log level to log for
    @param pMsg Log message
    This handler is installed to the log system by default. It prints
    the log message via bbPrintf(). It can also be chain-called from a
    custom log handler.
*/
void bbLogDefaultHandler(const bbLOGLEVEL loglevel, const bbCHAR* pMsg);

/** Enable or disable log messages of specified log level.
    By default all log levels are enabled.
    @param bbLOGLEVEL Log level
    @param enable 0 to disable, !=0 to enable
    @return Old state
*/
bbUINT bbLogLevelEnable(const bbLOGLEVEL loglevel, const bbUINT enable);

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_LOG_H_ */
