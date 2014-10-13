
#ifndef MOD_FOO_H
#define	MOD_FOO_H

#ifdef __cplusplus
#define EXTERN_C_BLOCK_BEGIN    extern "C" {
#define EXTERN_C_BLOCK_END      }
#define EXTERN_C_FUNC           extern "C"
#else
#define EXTERN_C_BLOCK_BEGIN
#define EXTERN_C_BLOCK_END
#define EXTERN_C_FUNC
#endif


#include <cstddef>

#include <httpd.h>
#include <http_protocol.h>
#include <http_config.h>
#include <http_log.h>

#define LOG_ERROR(req, fmt, ...)     ap_log_error(APLOG_MARK, APLOG_ERR, 0, req->server, fmt, __VA_ARGS__)
#define LOG_DEBUG(req, fmt, ...)     ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, req->server, fmt, __VA_ARGS__)

#endif	/* MOD_FOO_H */

