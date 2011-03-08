#include "PIGAContext.h"
#include <libcontext.h>
#include <string>

#define PIGACONTEXT_CONTRACTID \
  "@sds-project/PIGA;1"
  
typedef context_bool (*_context_register_application)(const char* app_name);
typedef context_bool (*_context_is_registered)();
typedef context_result (*_context_changed)(const char* param_name, ...);
typedef context_bool (*_context_required_context)(char* required_context, size_t maxlen, const char* param_name, ...);
typedef context_bool (*_current_local_context)(char* state_buf, unsigned int max_size);
typedef context_bool (*_current_global_context)(char* state_buf, unsigned int max_size);
typedef const char* (*_context_getLastError)();
typedef const char* (*_context_set_context_changed_callback)(contextChangedCallback ccc, void* user_data);


class PIGAContextImpl : public PIGAContext
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_PIGACONTEXT

  PIGAContextImpl();

private:
  ~PIGAContextImpl();
  
  //returned values
  const char* accepted_text;
  const char* refused_text;
  const char* error_text;
  
  void* dlhandle;
  bool register_firefox();
  bool parseURL(const std::string URL, std::string& protocol, std::string& host, std::string& port, std::string& path);
  
  static void onContextChanged(const char* previousContext, const char* nextContext, void* user_data);
  
  _context_register_application register_application;
  _context_is_registered is_registered;
  _context_changed context_changed;
  _context_required_context context_required_context;
  _current_local_context current_local_context;
  _current_global_context current_global_context;
  _context_getLastError context_getLastError;
  _context_set_context_changed_callback context_set_context_changed_callback;

protected:
  /* additional members */
};
