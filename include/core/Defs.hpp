#ifndef DEFS_H
#define DEFS_H

#define GODOT_TYPE_CHECKS

namespace godot {

enum Error {
	OK,
	FAILED, ///< Generic fail error
	ERR_UNAVAILABLE, ///< What is requested is unsupported/unavailable
	ERR_UNCONFIGURED, ///< The object being used hasnt been properly set up yet
	ERR_UNAUTHORIZED, ///< Missing credentials for requested resource
	ERR_PARAMETER_RANGE_ERROR, ///< Parameter given out of range (5)
	ERR_OUT_OF_MEMORY, ///< Out of memory
	ERR_FILE_NOT_FOUND,
	ERR_FILE_BAD_DRIVE,
	ERR_FILE_BAD_PATH,
	ERR_FILE_NO_PERMISSION,	// (10)
	ERR_FILE_ALREADY_IN_USE,
	ERR_FILE_CANT_OPEN,
	ERR_FILE_CANT_WRITE,
	ERR_FILE_CANT_READ,
	ERR_FILE_UNRECOGNIZED, // (15)
	ERR_FILE_CORRUPT,
	ERR_FILE_MISSING_DEPENDENCIES,
	ERR_FILE_EOF,
	ERR_CANT_OPEN, ///< Can't open a resource/socket/file
	ERR_CANT_CREATE,		// (20)
	ERR_QUERY_FAILED,
	ERR_ALREADY_IN_USE,
	ERR_LOCKED, ///< resource is locked
	ERR_TIMEOUT,
	ERR_CANT_CONNECT,		// (25)
	ERR_CANT_RESOLVE,
	ERR_CONNECTION_ERROR,
	ERR_CANT_AQUIRE_RESOURCE,
	ERR_CANT_FORK,
	ERR_INVALID_DATA, ///< Data passed is invalid	(30)
	ERR_INVALID_PARAMETER, ///< Parameter passed is invalid
	ERR_ALREADY_EXISTS, ///< When adding, item already exists
	ERR_DOES_NOT_EXIST, ///< When retrieving/erasing, it item does not exist
	ERR_DATABASE_CANT_READ, ///< database is full
	ERR_DATABASE_CANT_WRITE, ///< database is full	(35)
	ERR_COMPILATION_FAILED,
	ERR_METHOD_NOT_FOUND,
	ERR_LINK_FAILED,
	ERR_SCRIPT_FAILED,
	ERR_CYCLIC_LINK,		// (40)
	ERR_INVALID_DECLARATION,
	ERR_DUPLICATE_SYMBOL,
	ERR_PARSE_ERROR,
	ERR_BUSY,
	ERR_SKIP,			// (45)
	ERR_HELP, ///< user requested help!!
	ERR_BUG, ///< a bug in the software certainly happened, due to a double check failing or unexpected behavior.
	ERR_PRINTER_ON_FIRE, /// the parallel port printer is engulfed in flames
	ERR_OMFG_THIS_IS_VERY_VERY_BAD, ///< shit happens, has never been used, though
	ERR_WTF = ERR_OMFG_THIS_IS_VERY_VERY_BAD ///< short version of the above
};

	namespace helpers {
		template <typename T, typename ValueT>
		T append_all (T appendable, ValueT value) {
			appendable.append(value);
			return appendable;
		}

		template <typename T, typename ValueT, typename... Args>
		T append_all (T appendable, ValueT value, Args... args) {
			appendable.append(value);
			return append_all(appendable, args...);
		}

		template <typename T>
		T append_all (T appendable) {
			return appendable;
		}

		template <typename KV, typename KeyT, typename ValueT>
		KV add_all (KV kv, KeyT key, ValueT value) {
			kv[key] = value;
			return kv;
		}

		template <typename KV, typename KeyT, typename ValueT, typename... Args>
		KV add_all (KV kv, KeyT key, ValueT value, Args... args) {
			kv[key] = value;
			return add_all(kv, args...);
		}

		template <typename KV>
		KV add_all (KV kv) {
			return kv;
		}
	}

}

#include <stdio.h>
#include <malloc.h>

typedef float real_t;

#define CMP_EPSILON 0.00001
#define CMP_EPSILON2 (CMP_EPSILON*CMP_EPSILON)
#define Math_PI 3.14159265358979323846

#define _PLANE_EQ_DOT_EPSILON 0.999
#define _PLANE_EQ_D_EPSILON 0.0001


#ifndef ERR_FAIL_COND_V
#define ERR_FAIL_COND_V(cond, ret) do { if (cond) { return ret; } } while(0)
#endif


#ifndef ERR_FAIL_V
#define ERR_FAIL_V(a) return a
#endif

#ifndef ERR_FAIL_INDEX
#define ERR_FAIL_INDEX(a, b)
#endif


#ifndef ERR_PRINT
#define ERR_PRINT(msg) fprintf(stderr, "ERROR: %S\n", (msg).unicode_str())
#endif

#ifndef ERR_FAIL_INDEX_V
#define ERR_FAIL_INDEX_V(a, b, c)
#endif


#ifndef ERR_FAIL_COND
#define ERR_FAIL_COND(a) do { if (a) { fprintf(stderr, #a); return; } } while(0)
#endif


#endif // DEFS_H
