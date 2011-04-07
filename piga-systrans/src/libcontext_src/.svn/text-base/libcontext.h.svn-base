#ifndef LIBCONTEXT_H
#define LIBCONTEXT_H

/**
 * \file libcontext.h
 * \author Martin Peres (martin<dot>peres<At>ensi-bourges<dot>fr)
 * \date 27-08-2009
 */

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
	///Define a boolean
	typedef enum
	{
		///FALSE
		CONTEXT_FALSE=0,

		///TRUE
		CONTEXT_TRUE=1
	}context_bool;

	///Define context_result
	typedef enum
	{
		///Accepted
		CONTEXT_ACCEPTED=0,

		///Refused
		CONTEXT_REFUSED=1,

		///Error
		CONTEXT_ERROR=2
	}context_result;

	//Callbacks
		///id=
		typedef void (*contextChangedCallbackSuccess)(unsigned int id, context_result result);
		typedef void (*contextChangedCallbackError)(unsigned int id, const char* error, const char* message);
		typedef void (*contextChangedCallback)(const char* previousContext, const char* nextContext, void* user_data);

	//Public interface

	/*! \brief Is your application already registered ?
	* \return  Returns CONTEXT_TRUE if the application is registered*/
	context_bool context_is_registered();

	/*! \brief Register your application onto the context Daemon
	* \param app_name The name of your application.
	* \return  Returns CONTEXT_TRUE if the application has been successfuly registered*/
	context_bool context_register_application(const char* app_name);

	/*! \brief Deprecated. See context_changed*/
	context_result domain_changed(const char* param_name, ...);

	/*! \brief Send your internal context to the daemon. Should be called like this: domain_changed(var1_name, var1_data, var2_name, var2_data, ..., NULL, NULL);
	* \return  Returns CONTEXT_ACCEPTED if the state is accepted by the daemon, CONTEXT_REFUSED if the daemon refused it and CONTEXT_ERROR if an error occured (see context_getLastError()). If the daemon refused your internal state, you should go to another one that will be accepted (go back to the previous one or display an error message) !!*/
	context_result context_changed(const char* param_name, ...);

	/*! \brief Deprecated. See context_changed_async.*/
	unsigned int domain_changed_async(contextChangedCallbackSuccess success_cb,
								contextChangedCallbackError error_cb,
								const char* param_name, ...);

	/*! \brief It the same than domain_changed but async, see domain_changed() for more details.
	* \param success_cb The callback to be called when the call succeded (check result)
	* \param error_cb The callback to call in case of an error.
	* \return  Returns the ID of the call. This will also be put as a parameter in the callbacks so as you can match them.*/
	unsigned int context_changed_async(contextChangedCallbackSuccess success_cb,
								contextChangedCallbackError error_cb,
								const char* param_name, ...);

	/*! \brief Checks what would be the system context if we had sent this state to the daemon. This is the same logic than domain_changed() with only some extra parameters.
	* \param[out] required_context The buffer where the name of the context will be put.
	* \param maxlen The size of the buffer required_context minus one.
	* \return  Returns CONTEXT_TRUE if everything went fine and if the context is stored in required_context. CONTEXT_FALSE otherwise.*/
	context_bool context_required_domain(char* required_context, size_t maxlen, const char* param_name, ...);

	/*! \brief Get the global context of the currently registered process
	* \param state_buf out: The buffer that will receive the name of the context.
	* \param max_size in: The size of the buffer
	* \return  Returns CONTEXT_TRUE if the context has been retrieved, CONTEXT_FALSE otherwise*/
	context_bool context_current_domain(char* state_buf, unsigned int max_size);

	/*! \brief Returns the last error that happened.
	* \return  Returns the last error that happened.*/
	const char* context_getLastError();

	///Should not be used yet, it is not ready !!
	void context_set_domain_changed_callback(contextChangedCallback ccc, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // LIBCONTEXT_H
