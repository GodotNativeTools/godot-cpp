#ifndef GODOT_GLOBAL_HPP
#define GODOT_GLOBAL_HPP

#include <gdnative_api_struct.gen.h>

namespace godot {

extern "C" const godot_gdnative_core_api_struct *api;

extern "C" const godot_gdnative_ext_nativescript_api_struct *nativescript_api;
extern "C" const godot_gdnative_ext_pluginscript_api_struct *pluginscript_api;
extern "C" const godot_gdnative_ext_android_api_struct *android_api;
extern "C" const godot_gdnative_ext_xr_api_struct *xr_api;
extern "C" const godot_gdnative_ext_videodecoder_api_struct *videodecoder_api;
extern "C" const godot_gdnative_ext_net_api_struct *net_api;

extern "C" const void *gdnlib;

class Godot {

public:
	static void print_warning(const char *description, const char *function, const char *file, int line);
	static void print_error(const char *description, const char *function, const char *file, int line);
	static void print_script_error(const char *description, const char *function, const char *file, int line);

	static void gdnative_init(godot_gdnative_init_options *o);
	static void gdnative_terminate(godot_gdnative_terminate_options *o);
	static void nativescript_init(void *handle);
	static void nativescript_terminate(void *handle);

	static void gdnative_profiling_add_data(const char *p_signature, uint64_t p_time);
};

struct _RegisterState {
	static void *nativescript_handle;
	static int language_index;
};

} // namespace godot

#endif
