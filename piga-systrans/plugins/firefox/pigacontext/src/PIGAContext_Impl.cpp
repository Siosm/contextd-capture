#include "PIGAContext_Impl.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <dlfcn.h>

#include "nsMemory.h"

NS_IMPL_ISUPPORTS1(PIGAContextImpl, PIGAContext)

bool PIGAContextImpl::register_firefox()
{
	if(is_registered!=NULL)
	{
		if(!is_registered())
		{
			if(register_application!=NULL)
			{
				context_bool res=register_application("firefox");
				if(res==CONTEXT_FALSE)
					fprintf(stderr, "PIGAContext : Could not register Firefox !\n");
				else
					fprintf(stderr, "PIGAContext : Firefox has been Registered !\n");
				
				return res==CONTEXT_TRUE;
			}
			else
				fprintf(stderr, "PIGAContext : Firefox cannot be registered because the symbol 'context_register_application' doesn't exist !\n");
		}
		else
		{
			//fprintf(stderr, "PIGAContext : Firefox is already registered !\n");
			return true;
		}
	}
	else
		fprintf(stderr, "PIGAContext : Firefox cannot be registered because the symbol 'context_is_registered' doesn't exist !\n");
		
	return false;
}

PIGAContextImpl::PIGAContextImpl() : accepted_text("accepted"), refused_text("refused"), error_text("error")
{
	//Open libcontext
    dlhandle=dlopen("libcontext.so", RTLD_LAZY);
    if(dlhandle!=NULL)
    {
    	//Get function pointers from the lib (it is much simpler to 
		register_application=(_context_register_application)dlsym(dlhandle, "context_register_application");
		is_registered=(_context_is_registered)dlsym(dlhandle, "context_is_registered");
		context_changed=(_context_changed)dlsym(dlhandle, "context_changed");
		context_required_context=(_context_required_context)dlsym(dlhandle, "context_required_context");
		current_local_context=(_current_local_context)dlsym(dlhandle, "context_current_local_context");
		current_global_context=(_current_global_context)dlsym(dlhandle, "context_current_global_context");
		context_getLastError=(_context_getLastError)dlsym(dlhandle, "context_getLastError");
		context_set_context_changed_callback=(_context_set_context_changed_callback)dlsym(dlhandle, "context_set_context_changed_callback");
		
		std::string missing_symbols="";
		if(!register_application)
			missing_symbols+="context_register_application; ";
		if(!is_registered)
			missing_symbols+="context_is_registered; ";
		if(!context_changed)
			missing_symbols+="context_changed; ";
		if(!context_required_context)
			missing_symbols+="context_required_context; ";
		if(!current_local_context)
			missing_symbols+="context_current_local_context; ";
		if(!current_global_context)
			missing_symbols+="context_current_global_context; ";
		if(!context_getLastError)
			missing_symbols+="context_getLastError; ";
		if(!context_set_context_changed_callback)
			missing_symbols+="context_set_context_changed_callback; ";
		
		//If anything went wrong while getting the function pointers
		if(missing_symbols!="")
			fprintf(stderr, "PIGAContext : Not all symbols are present in libcontext.so.\n	Missing symbols are : %s\n	Check your libcontext version !\n", missing_symbols.c_str());
	
		//Register firefox
		register_firefox();
		
		context_set_context_changed_callback(onContextChanged, this);
    }
    else
    	fprintf(stderr, "PIGAContext : Libcontext.so cannot be found. Check you already installed it and that the lib is directly accessible !\n");
}

PIGAContextImpl::~PIGAContextImpl()
{
	//Free the lib and pointers
	if(dlhandle!=NULL)
    	dlclose(dlhandle);
}

/* void urlChanged (in string URL); */
NS_IMETHODIMP PIGAContextImpl::UrlChanged(const char *URL, char **_retval)
{
	//Try to register Firefox (if it didn't work before)
	register_firefox();
	
	//If the symbol context_changed exists
	if(context_changed)
	{
		context_result result;
		
		std::string url=URL, protocol, host, s_port, path;
		if(parseURL(url, protocol, host, s_port, path))
		{
			//Send the new context
			result=context_changed("url", url.c_str(), "host", host.c_str(), \
				"path", path.c_str(), "protocol", protocol.c_str(), \
				"port", s_port.c_str(), NULL, NULL);
		}
		else
			result=context_changed("url", url.c_str(), NULL, NULL);
			
		//Set the return value
		if(result==CONTEXT_ACCEPTED)
			*_retval=reinterpret_cast<char*>(nsMemory::Clone(accepted_text, strlen(accepted_text)+1));
		else if(result==CONTEXT_REFUSED)
			*_retval=reinterpret_cast<char*>(nsMemory::Clone(refused_text, strlen(refused_text)+1));
		else if(result==CONTEXT_ERROR)
			*_retval=reinterpret_cast<char*>(nsMemory::Clone(error_text, strlen(error_text)+1));
	}
	else
	{
		//Set the return value to false
		*_retval=reinterpret_cast<char*>(nsMemory::Clone(error_text, strlen(error_text)+1));
	
		fprintf(stderr, "PIGAContext UrlChanged : The symbol context_changed has not been found in libcontext.so and so, cannot be used !\n");
	}

	return NS_OK;
}

/* string requiredContext (in string URL); */
NS_IMETHODIMP PIGAContextImpl::RequiredContext(const char *URL, char **_retval)
{
	//Try to register Firefox (if it didn't work before)
	register_firefox();
	
	//If the symbol context_changed exists
	if(context_changed && URL)
	{
		context_bool result;
		char required_context[101];
		
		std::string url=URL, protocol, host, s_port, path;
		if(parseURL(url, protocol, host, s_port, path))
		{
			//Send the new context
			result=context_required_context(required_context, sizeof(required_context), "url", url.c_str(), "host", host.c_str(), \
				"path", path.c_str(), "protocol", protocol.c_str(), \
				"port", s_port.c_str(), NULL, NULL);
		}
		else
			result=context_required_context(required_context, sizeof(required_context), "url", url.c_str(), NULL, NULL);
			
		//Set the return value
		if(result==CONTEXT_TRUE)
			*_retval=reinterpret_cast<char*>(nsMemory::Clone(required_context, strlen(required_context)+1));
		else if(result==CONTEXT_FALSE)
			*_retval=reinterpret_cast<char*>(nsMemory::Clone("", 1));
	}
	else
	{
		//Set the return value to false
		*_retval=reinterpret_cast<char*>(nsMemory::Clone("", 1));
	
		fprintf(stderr, "PIGAContext UrlChanged : The symbol context_changed has not been found in libcontext.so or the given URL is NULL\n");
	}

	return NS_OK;
}

/* string currentGlobalContext (); */
NS_IMETHODIMP PIGAContextImpl::CurrentGlobalContext(char **_retval)
{
	//Try to register Firefox (if it didn't work before)
	register_firefox();
	
	static char global_context[1024];
	if(current_global_context)
	{
		context_bool res = current_global_context(global_context, sizeof(global_context)-1);
		*_retval=reinterpret_cast<char*>(nsMemory::Clone(global_context, strlen(global_context)+1));
		return res==CONTEXT_TRUE?NS_OK:NS_ERROR_FAILURE;
	}
	else
		return NS_ERROR_FAILURE;
}

/* string currentLocalContext (); */
NS_IMETHODIMP PIGAContextImpl::CurrentLocalContext(char **_retval)
{
	//Try to register Firefox (if it didn't work before)
	register_firefox();
	
	static char local_context[1024];
	if(current_local_context)
	{
		context_bool res = current_local_context(local_context, sizeof(local_context)-1);
		*_retval=reinterpret_cast<char*>(nsMemory::Clone(local_context, strlen(local_context)+1));
		return res==CONTEXT_TRUE?NS_OK:NS_ERROR_FAILURE;
	}
	else
		return NS_ERROR_FAILURE;
}

/* string getLastError (); */
NS_IMETHODIMP PIGAContextImpl::GetLastError(char **_retval)
{
	//Try to register Firefox (if it didn't work before)
	register_firefox();
	
	if(context_getLastError)
	{
		const char* lerror=context_getLastError();
		*_retval=reinterpret_cast<char*>(nsMemory::Clone(lerror, strlen(lerror)+1));
		return NS_OK;
	}
	else
		return NS_ERROR_FAILURE;
    
}

bool PIGAContextImpl::parseURL(const std::string url, std::string& protocol, std::string& host, std::string& port, std::string& path)
{
	size_t pos_protocol=url.find_first_of("://");
	if(pos_protocol!=std::string::npos)
	{
		protocol=url.substr(0, pos_protocol);

		size_t pos_host=url.find_first_of("/", pos_protocol+3);
		if(pos_host!=std::string::npos)
		{
			host=url.substr(pos_protocol+3, pos_host-pos_protocol-3);

			//Search for the port
			size_t pos_port=host.find_first_of(":");
			if(pos_port!=std::string::npos)
			{
				port=host.substr(pos_port+1);
				host=host.substr(0, pos_port);
			}
			else if(protocol=="https")
				port="443";
			else if(protocol=="ftp")
				port="21";
			else if(protocol=="http")
				port="80";
			else
				port="unknown";

			path=url.substr(pos_host);
			
			return true;
		}
		else
			fprintf(stderr, "PIGAContext UrlChanged : There is no host in this URL !\n");
	}
	else
		fprintf(stderr, "PIGAContext UrlChanged : There is no protocol in this URL !\n");
	
	return false;
}

#include "nsIObserverService.h"
#include "nsCOMPtr.h"
#include "nsStringAPI.h"
#include "nsEmbedString.h"
#include "nsServiceManagerUtils.h"
void PIGAContextImpl::onContextChanged(const char* previousContext, const char* nextContext, void* user_data)
{
	//Fire event !!
	fprintf(stderr, "	PIGAContext context changed from '%s' to '%s'\n", previousContext, nextContext);
	
	if(strcmp(previousContext, nextContext)!=0)
	{
		nsCOMPtr<nsIObserverService> os = nsnull;
		nsresult rv;
		os = do_GetService(NS_OBSERVERSERVICE_CONTRACTID, &rv);
		if(NS_SUCCEEDED(rv))
		{
			nsEmbedCString str(nextContext);
			nsEmbedString ustr;
			NS_CStringToUTF16(str, NS_CSTRING_ENCODING_ASCII, ustr);

			rv = os->NotifyObservers((nsISupports*)user_data, "globalContextChanged", ustr.get());// 
			
			if(!NS_SUCCEEDED(rv))
				fprintf(stderr, "	NotifyObservers failed !!\n");
		}
		else
			fprintf(stderr, "	get service failed !!\n");
	}
}
