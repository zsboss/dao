
/*
  +------------------------------------------------------------------------+
  | Dao Framework                                                          |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#include "kernel/fcall.h"

#include <Zend/zend_API.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_execute.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/hash.h"
#include "kernel/exception.h"
#include "kernel/backtrace.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/object.h"

#include "interned-strings.h"

int dao_has_constructor_ce(const zend_class_entry *ce)
{
	while (ce) {
		if (ce->constructor) {
			return 1;
		}

		ce = ce->parent;
	}

	return 0;
}

zval* _dao_call(zval *retval_ptr, zval *object, zend_class_entry *obj_ce, zend_function **fn_proxy, const char *function_name, size_t function_name_len, int param_count, zval* args[])
{
	int i = 0, result;
	zend_fcall_info fci;
	zval retval;
	zval params[50];

	while(i < param_count && i < 50) {
		ZVAL_COPY_VALUE(&params[i], args[i]);
		i++;
	}

	fci.size = sizeof(fci);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	fci.retval = retval_ptr ? retval_ptr : &retval;
	fci.param_count = param_count;
	fci.params = params;

	if (!fn_proxy && !obj_ce) {
		/* no interest in caching and no information already present that is
		 * needed later inside zend_call_function. */
		ZVAL_STRINGL(&fci.function_name, function_name, function_name_len);
		result = zend_call_function(&fci, NULL);
		zval_ptr_dtor(&fci.function_name);
	} else {
		zend_fcall_info_cache fcic;
		ZVAL_UNDEF(&fci.function_name); /* Unused */

		if (!obj_ce) {
			obj_ce = object ? Z_OBJCE_P(object) : NULL;
		}
		if (!fn_proxy || !*fn_proxy) {
			HashTable *function_table = obj_ce ? &obj_ce->function_table : EG(function_table);
			fcic.function_handler = zend_hash_str_find_ptr(
				function_table, function_name, function_name_len);
			if (fcic.function_handler == NULL) {
				/* error at c-level */
				zend_error_noreturn(E_CORE_ERROR, "Couldn't find implementation for method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", function_name);
			}
			if (fn_proxy) {
				*fn_proxy = fcic.function_handler;
			}
		} else {
			fcic.function_handler = *fn_proxy;
		}

		fcic.calling_scope = obj_ce;
		if (object) {
			fcic.called_scope = Z_OBJCE_P(object);
		} else {
			zend_class_entry *called_scope = zend_get_called_scope(EG(current_execute_data));

			if (obj_ce &&
			    (!called_scope ||
			     !instanceof_function(called_scope, obj_ce))) {
				fcic.called_scope = obj_ce;
			} else {
				fcic.called_scope = called_scope;
			}
		}
		fcic.object = object ? Z_OBJ_P(object) : NULL;
		result = zend_call_function(&fci, &fcic);
	}
	if (result == FAILURE) {
		/* error at c-level */
		if (!obj_ce) {
			obj_ce = object ? Z_OBJCE_P(object) : NULL;
		}
		if (!EG(exception)) {
			zend_error_noreturn(E_CORE_ERROR, "Couldn't execute method %s%s%s", obj_ce ? ZSTR_VAL(obj_ce->name) : "", obj_ce ? "::" : "", function_name);
		}
	}
	if (!retval_ptr) {
		zval_ptr_dtor(&retval);
		return NULL;
	}
	return retval_ptr;
}

int dao_call_user_func_args(zval *retval, zval *handler, zval *params, int param_count)
{
	zval ret = {}, *retval_ptr = (retval != NULL) ? retval : &ret;
	int status;

#if PHP_VERSION_ID >= 70100
	if ((status = call_user_function(NULL, NULL, handler, retval_ptr, param_count, params)) == FAILURE || EG(exception)) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
	}
#else
	if ((status = call_user_function(EG(function_table), NULL, handler, retval_ptr, param_count, params)) == FAILURE || EG(exception)) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
	}
#endif

	return status;
}

int dao_call_user_func_params(zval *retval, zval *handler, int param_count, zval *params[])
{
	zval ret = {}, *retval_ptr = (retval != NULL) ? retval : &ret;
	zval *arguments;
	int i, status;

	arguments = param_count ? safe_emalloc(sizeof(zval), param_count, 0) : NULL;

	i = 0;
	while(i < param_count) {
		if (params[i]) {
			ZVAL_COPY_VALUE(&arguments[i], params[i]);
		} else {
			ZVAL_NULL(&arguments[i]);
		}
		i++;
	}

#if PHP_VERSION_ID >= 70100
	if ((status = call_user_function(NULL, NULL, handler, retval_ptr, param_count, arguments)) == FAILURE || EG(exception)) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
	}
#else
	if ((status = call_user_function(EG(function_table), NULL, handler, retval_ptr, param_count, arguments)) == FAILURE || EG(exception)) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
	}
#endif

	efree(arguments);

	return status;
}

int dao_call_user_func_array(zval *retval, zval *handler, zval *params)
{
	zval ret = {}, *retval_ptr = (retval != NULL) ? retval : &ret, *arguments = NULL, *param;
	int params_count = 0, i, status;

	if (params && Z_TYPE_P(params) != IS_ARRAY && Z_TYPE_P(params) > IS_NULL) {
		status = FAILURE;
		php_error_docref(NULL, E_WARNING, "Invalid arguments supplied for dao_call_user_func_array()");
		return status;
	}

	if (params && Z_TYPE_P(params) == IS_ARRAY) {
		params_count = zend_hash_num_elements(Z_ARRVAL_P(params));
		arguments = (zval*)emalloc(sizeof(zval) * params_count);
		i = 0;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(params), param) {
			ZVAL_COPY_VALUE(&arguments[i], param);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		params_count = 0;
		arguments = NULL;
	}

#if PHP_VERSION_ID >= 70100
	if ((status = call_user_function(NULL, NULL, handler, retval_ptr, params_count, arguments)) == FAILURE || EG(exception)) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
	}
#else
	if ((status = call_user_function(EG(function_table), NULL, handler, retval_ptr, params_count, arguments)) == FAILURE || EG(exception)) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
	}
#endif

	efree(arguments);

	return status;
}

static zend_always_inline zend_bool dao_is_derived_class(zend_class_entry *child_class, zend_class_entry *parent_class) /* {{{ */
{
	child_class = child_class->parent;
	while (child_class) {
		if (child_class == parent_class) {
			return 1;
		}
		child_class = child_class->parent;
	}

	return 0;
}

static zend_never_inline zend_function *dao_get_function(zend_class_entry *scope, zend_string *function_name)
{
	zval *func;
	zend_function *fbc;

	func = zend_hash_find(&scope->function_table, function_name);
	if (func != NULL) {
		fbc = Z_FUNC_P(func);
		return fbc;
	}

	return NULL;
}

static zend_result dao_call_user_function(zend_function *fn, zend_class_entry *called_scope, zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[])
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;

	fci.size = sizeof(fci);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	ZVAL_COPY_VALUE(&fci.function_name, function_name);
	fci.retval = retval_ptr;
	fci.param_count = param_count;
	fci.params = params;

	fci.named_params = NULL;

	if (fn != NULL) {
		fcic.function_handler = fn;
		fcic.object = object ? Z_OBJ_P(object) : NULL;
		fcic.called_scope = called_scope;

		return zend_call_function(&fci, &fcic);
	}
	return zend_call_function(&fci, NULL);
}

int dao_call_method_with_params(zval *retval, zval *object, zend_class_entry *ce, dao_call_type type, const char *method_name, uint method_len, uint param_count, zval *params[])
{
	zval func_name = {}, ret = {}, *retval_ptr = (retval != NULL) ? retval : &ret, obj = {};
	zval *arguments;
	zend_function *fbc = NULL;

	int i, status;

	if (type != dao_fcall_function) {
		if (type != dao_fcall_ce && type != dao_fcall_self && type != dao_fcall_static) {
			if (object == NULL || Z_TYPE_P(object) != IS_OBJECT) {
				dao_throw_exception_format(spl_ce_RuntimeException, "Trying to call method %s on a non-object", method_name);
				return FAILURE;
			}
		}

		if (object == NULL || Z_TYPE_P(object) != IS_OBJECT) {
			if (zend_get_this_object(EG(current_execute_data))){
				ZVAL_OBJ(&obj, zend_get_this_object(EG(current_execute_data)));
				object = &obj;
			}
		}

		if (!ce && object && Z_TYPE_P(object) == IS_OBJECT) {
			ce = Z_OBJCE_P(object);
		}
		assert(ce != NULL);

		zend_string *str_methodname = zend_string_init(method_name, method_len, 0);
		if (type != dao_fcall_parent) {
			fbc = dao_get_function(ce, str_methodname);
		} else {
			fbc = dao_get_function(ce->parent, str_methodname);
		}
		if (fbc) {
			ZVAL_STR(&func_name, str_methodname);
		} else {
			zend_string_release(str_methodname);

			switch (type) {
				case dao_fcall_ce:
					array_init_size(&func_name, 2);
					add_next_index_string(&func_name, ce->name->val);
					add_next_index_stringl(&func_name, method_name, method_len);
					break;
				case dao_fcall_parent:
					if (dao_memnstr_str_str(method_name, method_len, SL("::"))) {
						dao_fast_explode_str_str(&func_name, SL("::"), method_name, method_len);
					} else {
						array_init_size(&func_name, 2);
						add_next_index_string(&func_name, ISV(parent));
						add_next_index_stringl(&func_name, method_name, method_len);
					}
					break;
				case dao_fcall_self:
					assert(ce != NULL);
					array_init_size(&func_name, 2);
					add_next_index_string(&func_name, ISV(self));
					add_next_index_stringl(&func_name, method_name, method_len);
					break;
				case dao_fcall_method:
					array_init_size(&func_name, 2);
					Z_TRY_ADDREF_P(object);
					add_next_index_zval(&func_name, object);
					add_next_index_stringl(&func_name, method_name, method_len);
					break;
				default:
					dao_throw_exception_format(spl_ce_RuntimeException, "Error call type %d for cmethod %s", type, method_name);
					return FAILURE;
			}
		}
	} else {
		ZVAL_STRINGL(&func_name, method_name, method_len);
	}

	arguments = param_count ? safe_emalloc(sizeof(zval), param_count, 0) : NULL;

	i = 0;
	while(i < param_count) {
		if (params[i] && Z_TYPE_P(params[i]) > IS_NULL) {
			ZVAL_COPY_VALUE(&arguments[i], params[i]);
		} else {
			ZVAL_NULL(&arguments[i]);
		}
		i++;
	}

	if (
		(status = dao_call_user_function(fbc, ce, object, &func_name, retval_ptr, param_count, arguments)) == FAILURE || EG(exception)
	) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
		if (!EG(exception)) {
			switch (type) {
				case dao_fcall_function:
					zend_error(E_ERROR, "Call to undefined function %s()", method_name);
					break;
				case dao_fcall_parent:
					zend_error(E_ERROR, "Call to undefined method parent::%s()", method_name);
					break;
				case dao_fcall_self:
					zend_error(E_ERROR, "Call to undefined method self::%s()", method_name);
					break;
				case dao_fcall_static:
					zend_error(E_ERROR, "Call to undefined function static::%s()", method_name);
					break;
				case dao_fcall_ce:
					zend_error(E_ERROR, "Call to undefined method %s::%s()", ce->name->val, method_name);
					break;
				case dao_fcall_method:
					zend_error(E_ERROR, "Call to undefined method %s::%s()", Z_OBJCE_P(object)->name->val, method_name);
					break;
				default:
					zend_error(E_ERROR, "Call to undefined method ?::%s()", method_name);
			}
		}
	}
	zval_ptr_dtor(&func_name);
	efree(arguments);
	if (retval == NULL) {
		if (!Z_ISUNDEF(ret)) {
			zval_ptr_dtor(&ret);
		}
	}

	return status;
}
