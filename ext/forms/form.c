
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

#include "forms/form.h"
#include "forms/elementinterface.h"
#include "forms/exception.h"
#include "tag.h"
#include "di/injectable.h"
#include "diinterface.h"
#include "filterinterface.h"
#include "validation.h"
#include "validation/exception.h"
#include "validation/message/group.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/hash.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/file.h"
#include "kernel/string.h"

#include "interned-strings.h"

/**
 * Dao\Forms\Form
 *
 * This component allows to build forms using an object-oriented interface
 */
zend_class_entry *dao_forms_form_ce;

PHP_METHOD(Dao_Forms_Form, __construct);
PHP_METHOD(Dao_Forms_Form, setAction);
PHP_METHOD(Dao_Forms_Form, getAction);
PHP_METHOD(Dao_Forms_Form, setMethod);
PHP_METHOD(Dao_Forms_Form, getMethod);
PHP_METHOD(Dao_Forms_Form, setEnctype);
PHP_METHOD(Dao_Forms_Form, getEnctype);
PHP_METHOD(Dao_Forms_Form, setOption);
PHP_METHOD(Dao_Forms_Form, getOption);
PHP_METHOD(Dao_Forms_Form, setOptions);
PHP_METHOD(Dao_Forms_Form, getOptions);
PHP_METHOD(Dao_Forms_Form, setEntity);
PHP_METHOD(Dao_Forms_Form, getEntity);
PHP_METHOD(Dao_Forms_Form, getElements);
PHP_METHOD(Dao_Forms_Form, bind);
PHP_METHOD(Dao_Forms_Form, isValid);
PHP_METHOD(Dao_Forms_Form, getMessages);
PHP_METHOD(Dao_Forms_Form, getMessagesFor);
PHP_METHOD(Dao_Forms_Form, hasMessagesFor);
PHP_METHOD(Dao_Forms_Form, add);
PHP_METHOD(Dao_Forms_Form, render);
PHP_METHOD(Dao_Forms_Form, get);
PHP_METHOD(Dao_Forms_Form, label);
PHP_METHOD(Dao_Forms_Form, getLabel);
PHP_METHOD(Dao_Forms_Form, getValue);
PHP_METHOD(Dao_Forms_Form, setValue);
PHP_METHOD(Dao_Forms_Form, getValues);
PHP_METHOD(Dao_Forms_Form, has);
PHP_METHOD(Dao_Forms_Form, remove);
PHP_METHOD(Dao_Forms_Form, clear);
PHP_METHOD(Dao_Forms_Form, count);
PHP_METHOD(Dao_Forms_Form, rewind);
PHP_METHOD(Dao_Forms_Form, current);
PHP_METHOD(Dao_Forms_Form, key);
PHP_METHOD(Dao_Forms_Form, next);
PHP_METHOD(Dao_Forms_Form, valid);
PHP_METHOD(Dao_Forms_Form, appendMessage);
PHP_METHOD(Dao_Forms_Form, appendMessages);
PHP_METHOD(Dao_Forms_Form, toArray);
PHP_METHOD(Dao_Forms_Form, __toString);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, entity)
	ZEND_ARG_INFO(0, userOptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setaction, 0, 0, 1)
	ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setmethod, 0, 0, 1)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setenctype, 0, 0, 1)
	ZEND_ARG_INFO(0, enctype)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setoption, 0, 0, 2)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_getoption, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, defaultValue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setoptions, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setentity, 0, 0, 1)
	ZEND_ARG_INFO(0, entity)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_bind, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, entity, IS_OBJECT, 1)
	ZEND_ARG_TYPE_INFO(0, whitelist, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_isvalid, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, entity, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_getmessages, 0, 0, 0)
	ZEND_ARG_INFO(0, byItemName)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_getmessagesfor, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_hasmessagesfor, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_add, 0, 0, 1)
	ZEND_ARG_INFO(0, element)
	ZEND_ARG_INFO(0, postion)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_render, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, attributes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_get, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_label, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, attributes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_getlabel, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_getvalue, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_setvalue, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_getvalues, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_has, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_remove, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_clear, 0, 0, 0)
	ZEND_ARG_INFO(0, fields)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_appendmessage, 0, 0, 2)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_forms_form_appendmessages, 0, 0, 2)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, messages)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_forms_form_method_entry[] = {
	PHP_ME(Dao_Forms_Form, __construct, arginfo_dao_forms_form___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Forms_Form, setAction, arginfo_dao_forms_form_setaction, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getAction, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, setMethod, arginfo_dao_forms_form_setmethod, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getMethod, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, setEnctype, arginfo_dao_forms_form_setenctype, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getEnctype, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, setOption, arginfo_dao_forms_form_setoption, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getOption, arginfo_dao_forms_form_getoption, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, setOptions, arginfo_dao_forms_form_setoptions, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getOptions, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, setEntity, arginfo_dao_forms_form_setentity, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getEntity, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getElements, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, bind, arginfo_dao_forms_form_bind, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, isValid, arginfo_dao_forms_form_isvalid, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getMessages, arginfo_dao_forms_form_getmessages, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getMessagesFor, arginfo_dao_forms_form_getmessagesfor, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, hasMessagesFor, arginfo_dao_forms_form_hasmessagesfor, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, add, arginfo_dao_forms_form_add, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, render, arginfo_dao_forms_form_render, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, get, arginfo_dao_forms_form_get, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, label, arginfo_dao_forms_form_label, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getLabel, arginfo_dao_forms_form_getlabel, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getValue, arginfo_dao_forms_form_getvalue, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, setValue, arginfo_dao_forms_form_setvalue, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, getValues, arginfo_dao_forms_form_getvalues, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, has, arginfo_dao_forms_form_has, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, remove, arginfo_dao_forms_form_remove, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, clear, arginfo_dao_forms_form_clear, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, count, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, rewind, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, current, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, key, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, next, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, valid, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, appendMessage, arginfo_dao_forms_form_appendmessage, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, appendMessages, arginfo_dao_forms_form_appendmessages, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, toArray, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Forms_Form, __toString, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Forms\Form initializer
 */
DAO_INIT_CLASS(Dao_Forms_Form){

	DAO_REGISTER_CLASS_EX(Dao\\Forms, Form, forms_form, dao_di_injectable_ce, dao_forms_form_method_entry, 0);

	zend_declare_property_null(dao_forms_form_ce, SL("_position"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_forms_form_ce, SL("_entity"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_forms_form_ce, SL("_options"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_forms_form_ce, SL("_data"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_forms_form_ce, SL("_filterData"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_forms_form_ce, SL("_elements"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_forms_form_ce, SL("_messages"), ZEND_ACC_PROTECTED);

	zend_declare_class_constant_long(dao_forms_form_ce, SL("VALUES_RAW"), DAO_FROM_VALUES_RAW);
	zend_declare_class_constant_long(dao_forms_form_ce, SL("VALUES_AS_ARRAY"), DAO_FROM_VALUES_AS_ARRAY);

	zend_class_implements(dao_forms_form_ce, 2, spl_ce_Countable, zend_ce_iterator);

	return SUCCESS;
}

/**
 * Dao\Forms\Form constructor
 *
 * @param object $entity
 * @param array $userOptions
 */
PHP_METHOD(Dao_Forms_Form, __construct){

	zval *entity = NULL, *options = NULL;

	dao_fetch_params(0, 0, 2, &entity, &options);

	if (!entity) {
		entity = &DAO_GLOBAL(z_null);
	}

	if (!options) {
		options = &DAO_GLOBAL(z_null);
	}

	if (Z_TYPE_P(entity) != IS_NULL) {
		if (Z_TYPE_P(entity) != IS_OBJECT) {
			DAO_THROW_EXCEPTION_STR(dao_forms_exception_ce, "The base entity is not valid");
			return;
		}

		dao_update_property(getThis(), SL("_entity"), entity);
	}

	/**
	 * Update the user options
	 */
	if (Z_TYPE_P(options) == IS_ARRAY) {
		dao_update_property(getThis(), SL("_options"), options);
	}

	/**
	 * Check for an 'initialize' method and call it
	 */
	if (dao_method_exists_ex(getThis(), SL("initialize")) == SUCCESS) {
		DAO_CALL_METHOD(NULL, getThis(), "initialize", entity, options);
	}
}

/**
 * Sets the form's action
 *
 * @param string $action
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, setAction){

	zval *action, option = {};

	dao_fetch_params(0, 1, 0, &action);

	ZVAL_STRING(&option, "action");
	dao_update_property_array(getThis(), SL("_options"), &option, action);
	zval_ptr_dtor(&option);

	RETURN_THIS();
}

/**
 * Returns the form's action
 *
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, getAction){

	zval options = {}, option = {};

	ZVAL_STRING(&option, "action");
	dao_read_property(&options, getThis(), SL("_options"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(return_value, &options, &option, PH_COPY)) {
		RETVAL_NULL();
	}
	zval_ptr_dtor(&option);
}

/**
 * Sets the form's method
 *
 * @param string $action
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, setMethod){

	zval *method, option = {};

	dao_fetch_params(0, 1, 0, &method);

	ZVAL_STRING(&option, "method");
	dao_update_property_array(getThis(), SL("_options"), &option, method);
	zval_ptr_dtor(&option);

	RETURN_THIS();
}

/**
 * Returns the form's method
 *
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, getMethod){

	zval options = {}, option = {};

	ZVAL_STRING(&option, "method");
	dao_read_property(&options, getThis(), SL("_options"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(return_value, &options, &option, PH_COPY)) {
		RETVAL_NULL();
	}
	zval_ptr_dtor(&option);
}

/**
 * Sets the form's enctype
 *
 * @param string $action
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, setEnctype){

	zval *enctype, option = {};

	dao_fetch_params(0, 1, 0, &enctype);

	ZVAL_STRING(&option, "enctype");
	dao_update_property_array(getThis(), SL("_options"), &option, enctype);
	zval_ptr_dtor(&option);

	RETURN_THIS();
}

/**
 * Returns the form's enctype
 *
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, getEnctype){

	zval options = {}, option = {};

	ZVAL_STRING(&option, "enctype");
	dao_read_property(&options, getThis(), SL("_options"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(return_value, &options, &option, PH_COPY)) {
		RETVAL_NULL();
	}
	zval_ptr_dtor(&option);
}

/**
 * Sets an option for the form
 *
 * @param string $option
 * @param mixed $value
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, setOption){

	zval *option, *value;

	dao_fetch_params(0, 2, 0, &option, &value);

	dao_update_property_array(getThis(), SL("_options"), option, value);
	RETURN_THIS();
}

/**
 * Returns the value of an option if present
 *
 * @param string $option
 * @param mixed $defaultValue
 * @return mixed
 */
PHP_METHOD(Dao_Forms_Form, getOption){

	zval *option, *default_value = NULL, options = {}, value = {};

	dao_fetch_params(0, 1, 1, &option, &default_value);

	if (!default_value) {
		default_value = &DAO_GLOBAL(z_null);
	}

	dao_read_property(&options, getThis(), SL("_options"), PH_NOISY|PH_READONLY);
	if (dao_array_isset_fetch(&value, &options, option, PH_READONLY)) {
		RETURN_CTOR(&value);
	}

	RETURN_CTOR(default_value);
}

/**
 * Sets options for the element
 *
 * @param array $options
 * @return Dao\Forms\ElementInterface
 */
PHP_METHOD(Dao_Forms_Form, setOptions){

	zval *options;

	dao_fetch_params(0, 1, 0, &options);

	dao_update_property(getThis(), SL("_options"), options);

	RETURN_THIS();
}

/**
 * Returns the options for the element
 *
 * @return array
 */
PHP_METHOD(Dao_Forms_Form, getOptions){


	RETURN_MEMBER(getThis(), "_options");
}

/**
 * Sets the entity related to the model
 *
 * @param object $entity
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, setEntity){

	zval *entity;

	dao_fetch_params(0, 1, 0, &entity);

	if (Z_TYPE_P(entity) != IS_NULL && Z_TYPE_P(entity) != IS_OBJECT) {
		zend_throw_exception_ex(dao_forms_exception_ce, 0, "'%s' must be an object or NULL", "entity");
		return;
	}

	dao_update_property(getThis(), SL("_entity"), entity);
	RETURN_THIS();
}

/**
 * Returns the entity related to the model
 *
 * @return object
 */
PHP_METHOD(Dao_Forms_Form, getEntity){


	RETURN_MEMBER(getThis(), "_entity");
}

/**
 * Returns the form elements added to the form
 *
 * @return Dao\Forms\ElementInterface[]
 */
PHP_METHOD(Dao_Forms_Form, getElements){


	RETURN_MEMBER(getThis(), "_elements");
}

/**
 * Binds data to the entity
 *
 * @param array $data
 * @param object $entity
 * @param array $whitelist
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, bind){

	zval *data, *entity, *whitelist = NULL, elements = {}, service_name = {}, dependency_injector = {}, filter = {}, filter_data = {}, *value;
	zend_string *str_key;
	ulong idx;

	dao_fetch_params(1, 1, 2, &data, &entity, &whitelist);

	if (!entity) {
		entity = &DAO_GLOBAL(z_null);
	}

	if (!whitelist) {
		whitelist = &DAO_GLOBAL(z_null);
	}

	DAO_MM_CALL_METHOD(NULL, getThis(), "setentity", entity);

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(elements) != IS_ARRAY) {
		DAO_MM_THROW_EXCEPTION_STR(dao_forms_exception_ce, "There are no elements in the form");
		return;
	}

	ZVAL_STR(&service_name, IS(filter));

	DAO_MM_CALL_METHOD(&dependency_injector, getThis(), "getdi");
	DAO_MM_ADD_ENTRY(&dependency_injector);
	DAO_MM_VERIFY_INTERFACE(&dependency_injector, dao_diinterface_ce);

	DAO_MM_CALL_METHOD(&filter, &dependency_injector, "getshared", &service_name);
	DAO_MM_ADD_ENTRY(&filter);
	DAO_MM_VERIFY_INTERFACE(&filter, dao_filterinterface_ce);

	array_init(&filter_data);
	DAO_MM_ADD_ENTRY(&filter_data);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), idx, str_key, value) {
		zval key = {}, element = {}, filters = {}, filtered_value = {}, method = {};
		if (str_key) {
			ZVAL_STR(&key, str_key);
		} else {
			ZVAL_LONG(&key, idx);
		}

		if (!dao_array_isset_fetch(&element, &elements, &key, PH_READONLY)) {
			continue;
		}

		/**
		 * Check if the item is in the whitelist
		 */
		if (Z_TYPE_P(whitelist) == IS_ARRAY) {
			if (!dao_fast_in_array(&key, whitelist)) {
				continue;
			}
		}

		/**
		 * Check if the method has filters
		 */
		DAO_MM_CALL_METHOD(&filters, &element, "getfilters");
		DAO_MM_ADD_ENTRY(&filters);
		if (zend_is_true(&filters)) {
			/**
			 * Sanitize the filters
			 */
			DAO_MM_CALL_METHOD(&filtered_value, &filter, "sanitize", value, &filters);
			DAO_MM_ADD_ENTRY(&filtered_value);
		} else {
			ZVAL_COPY_VALUE(&filtered_value, value);
		}

		if (Z_TYPE_P(entity) == IS_OBJECT) {
			DAO_CONCAT_SV(&method, "set", &key);
			DAO_MM_ADD_ENTRY(&method);
			zend_str_tolower(Z_STRVAL(method), Z_STRLEN(method));

			/**
			 * Use the setter if any available
			 */
			if (dao_method_exists(entity, &method) == SUCCESS) {
				DAO_MM_CALL_METHOD(NULL, entity, Z_STRVAL(method), &filtered_value);
				continue;
			}

			/**
			 * Use the public property if it doesn't have a setter
			 */
			dao_update_property_zval_zval(entity, &key, &filtered_value);
		} else {
			dao_array_update(&filter_data, &key, &filtered_value, PH_COPY);
		}
	} ZEND_HASH_FOREACH_END();

	dao_update_property(getThis(), SL("_filterData"), &filter_data);
	dao_update_property(getThis(), SL("_data"), data);
	RETURN_MM();
}

/**
 * Validates the form
 *
 * @param array $data
 * @param object $entity
 * @return boolean
 */
PHP_METHOD(Dao_Forms_Form, isValid){

	zval *_data = NULL, *entity = NULL, data = {}, elements = {}, status = {}, not_failed = {}, messages = {}, *element;

	dao_fetch_params(0, 0, 2, &_data, &entity);

	if (!entity) {
		entity = &DAO_GLOBAL(z_null);
	}

	if (_data) {
		DAO_CALL_METHOD(NULL, getThis(), "bind", _data, entity);
	}

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);

	if (Z_TYPE(elements) != IS_ARRAY) {
		RETURN_TRUE;
	}

	dao_read_property(&data, getThis(), SL("_data"), PH_NOISY|PH_READONLY);

	/**
	 * Check if there is a method 'beforeValidation'
	 */
	if (dao_method_exists_ex(getThis(), SL("beforevalidation")) == SUCCESS) {
		DAO_CALL_METHOD(&status, getThis(), "beforevalidation", &data, entity);
		if (DAO_IS_FALSE(&status)) {
			RETURN_CTOR(&status);
		}
	}

	ZVAL_TRUE(&not_failed);

	array_init(&messages);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL(elements), element) {
		zval validators = {}, name = {}, prepared_validators = {}, *validator, validation = {}, filters = {}, element_messages = {};
		DAO_CALL_METHOD(&validators, element, "getvalidators");
		if (Z_TYPE(validators) == IS_ARRAY) {
			if (dao_fast_count_ev(&validators)) {

				/**
				 * Element's name
				 */
				DAO_CALL_METHOD(&name, element, "getname");

				/**
				 * Prepare the validators
				 */
				array_init(&prepared_validators);

				ZEND_HASH_FOREACH_VAL(Z_ARRVAL(validators), validator) {
					zval scope = {};
					array_init_size(&scope, 2);
					dao_array_append(&scope, &name, PH_COPY);
					dao_array_append(&scope, validator, PH_COPY);
					dao_array_append(&prepared_validators, &scope, 0);
				} ZEND_HASH_FOREACH_END();

				/**
				 * Create an implicit validation
				 */
				object_init_ex(&validation, dao_validation_ce);
				DAO_CALL_METHOD(NULL, &validation, "__construct", &prepared_validators);

				/**
				 * Get filters in the element
				 */
				DAO_CALL_METHOD(&filters, element, "getfilters");

				/**
				 * Assign the filters to the validation
				 */
				if (Z_TYPE(filters) == IS_ARRAY) {
					DAO_CALL_METHOD(NULL, &validation, "setfilters", &name, &filters);
				}
				zval_ptr_dtor(&filters);

				/**
				 * Perform the validation
				 */
				DAO_CALL_METHOD(&element_messages, &validation, "validate", &data, entity);
				if (dao_fast_count_ev(&element_messages)) {
					dao_array_update(&messages, &name, &element_messages, PH_COPY);

					ZVAL_FALSE(&not_failed);
				}
				zval_ptr_dtor(&name);
				zval_ptr_dtor(&validation);
				zval_ptr_dtor(&element_messages);
			}
		}
		zval_ptr_dtor(&validators);
	} ZEND_HASH_FOREACH_END();

	/**
	 * If the validation fails update the messages
	 */
	if (!zend_is_true(&not_failed)) {
		dao_update_property(getThis(), SL("_messages"), &messages);
	}

	/**
	 * Check if there is a method 'afterValidation'
	 */
	if (dao_method_exists_ex(getThis(), SL("aftervalidation")) == SUCCESS) {
		DAO_CALL_METHOD(NULL, getThis(), "aftervalidation", &messages);
	}
	zval_ptr_dtor(&messages);

	/**
	 * Return the validation status
	 */

	RETURN_CTOR(&not_failed);
}

/**
 * Returns the messages generated in the validation
 *
 * @param boolean $byItemName
 * @return Dao\Validation\Message\Group
 */
PHP_METHOD(Dao_Forms_Form, getMessages){

	zval *by_item_name = NULL, messages ={};

	dao_fetch_params(0, 0, 1, &by_item_name);

	dao_read_property(&messages, getThis(), SL("_messages"), PH_NOISY|PH_READONLY);
	if (by_item_name && zend_is_true(by_item_name)) {
		if (Z_TYPE(messages) != IS_ARRAY) {
			object_init_ex(return_value, dao_validation_message_group_ce);
		} else {
			RETURN_ZVAL(&messages, 1, 0);
		}
	} else {
		object_init_ex(return_value, dao_validation_message_group_ce);

		if (Z_TYPE(messages) == IS_ARRAY) {
			zval *v;
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL(messages), v) {
				DAO_CALL_METHOD(NULL, return_value, "appendmessages", v);
			} ZEND_HASH_FOREACH_END();
		}
	}
}

/**
 * Returns the messages generated for a specific element
 *
 * @return Dao\Validation\Message\Group[]
 */
PHP_METHOD(Dao_Forms_Form, getMessagesFor){

	zval *name, messages = {}, element_messages = {};

	dao_fetch_params(0, 1, 0, &name);

	dao_read_property(&messages, getThis(), SL("_messages"), PH_NOISY|PH_READONLY);
	if (dao_array_isset_fetch(&element_messages, &messages, name, PH_READONLY)) {
		RETURN_CTOR(&element_messages);
	}

	object_init_ex(return_value, dao_validation_message_group_ce);
}

/**
 * Check if messages were generated for a specific element
 *
 * @return boolean
 */
PHP_METHOD(Dao_Forms_Form, hasMessagesFor){

	zval *name, messages = {};

	dao_fetch_params(0, 1, 0, &name);

	dao_read_property(&messages, getThis(), SL("_messages"), PH_NOISY|PH_READONLY);
	RETURN_BOOL(dao_array_isset(&messages, name));
}

/**
 * Adds an element to the form
 *
 * @param Dao\Forms\ElementInterface $element
 * @param string $postion
 * @param bool $type If $type is TRUE, the element wile add before $postion, else is after
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, add){

	zval *element, *pos = NULL, *type = NULL, name = {}, values = {}, elements = {}, elements_indexed = {}, tmp0 = {}, tmp1 = {}, offset = {}, new_elements = {};
	zend_string *str_key;
	ulong idx;
	int found = 0, i = 0;

	dao_fetch_params(0, 1, 2, &element, &pos, &type);
	DAO_VERIFY_INTERFACE_EX(element, dao_forms_elementinterface_ce, dao_forms_exception_ce);

	/**
	 * Gets the element's name
	 */
	DAO_CALL_METHOD(&name, element, "getname");

	/**
	 * Link the element to the form
	 */
	DAO_CALL_METHOD(NULL, element, "setform", getThis());

	if (!pos || Z_TYPE_P(pos) == IS_NULL) {
		/* Append the element by its name */
		dao_update_property_array(getThis(), SL("_elements"), &name, element);
		dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
		dao_array_values(&elements_indexed, &elements);
		dao_update_property(getThis(), SL("_elementsIndexed"), &elements_indexed);
		zval_ptr_dtor(&elements_indexed);
	} else {
		if (type && zend_is_true(type)) {
			i = -1;
		}

		array_init_size(&values, 1);

		dao_array_update(&values, &name, element, PH_COPY);

		dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);

		if (Z_TYPE(elements) == IS_ARRAY) {
			ZEND_HASH_FOREACH_KEY(Z_ARRVAL(elements), idx, str_key) {
				zval key = {};
				if (str_key) {
					ZVAL_STR(&key, str_key);
				} else {
					ZVAL_LONG(&key, idx);
				}
				++i;
				if (dao_is_equal(&key, pos)) {
					found = 1;
					break;
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (!found) {
			DAO_THROW_EXCEPTION_STR(dao_forms_exception_ce, "Array position does not exist");
			return;
		}

		ZVAL_LONG(&offset, i);

		DAO_CALL_FUNCTION(&tmp0, "array_slice", &elements, &DAO_GLOBAL(z_zero), &offset, &DAO_GLOBAL(z_true));
		DAO_CALL_FUNCTION(&tmp1, "array_slice", &elements, &offset, &DAO_GLOBAL(z_null), &DAO_GLOBAL(z_true));

		array_init(&new_elements);

		dao_array_merge_recursive_n(&new_elements, &tmp0);
		dao_array_merge_recursive_n(&new_elements, &values);
		dao_array_merge_recursive_n(&new_elements, &tmp1);
		zval_ptr_dtor(&values);
		zval_ptr_dtor(&tmp0);
		zval_ptr_dtor(&tmp1);

		dao_update_property(getThis(), SL("_elements"), &new_elements);
		dao_array_values(&elements_indexed, &new_elements);
		zval_ptr_dtor(&new_elements);

		dao_update_property(getThis(), SL("_elementsIndexed"), &elements_indexed);
		zval_ptr_dtor(&elements_indexed);
	}
	zval_ptr_dtor(&name);

	RETURN_THIS();
}

/**
 * Renders a specific item in the form
 *
 * @param string $name
 * @param array $attributes
 * @param string $template
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, render){

	zval *name = NULL, *attributes = NULL, *template = NULL, elements = {}, element = {}, label ={}, element_str = {};
	zval replace_pairs = {}, options = {}, *el;

	dao_fetch_params(0, 0, 3, &name, &attributes, &template);

	if (!attributes) {
		attributes = &DAO_GLOBAL(z_null);
	}

	if (!template) {
		template = &DAO_GLOBAL(z_null);
	}

	if (name && DAO_IS_NOT_EMPTY(name)) {
		DAO_ENSURE_IS_STRING(name);

		dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
		if (!dao_array_isset_fetch(&element, &elements, name, PH_READONLY)) {
			zend_throw_exception_ex(dao_forms_exception_ce, 0, "Element with ID=%s is not a part of the form", Z_STRVAL_P(name));
			return;
		}

		if (DAO_IS_NOT_EMPTY(template)) {
			DAO_CALL_METHOD(&label, getThis(), "getlabel", name);
			DAO_CALL_METHOD(&element_str, &element, "render", attributes);

			array_init_size(&replace_pairs, 2);
			dao_array_update_str(&replace_pairs, SL(":label:"), &label, 0);
			dao_array_update_str(&replace_pairs, SL(":element:"), &element_str, 0);

			dao_strtr_array(return_value, template, &replace_pairs);
			zval_ptr_dtor(&replace_pairs);
		} else {
			DAO_CALL_METHOD(return_value, &element, "render", attributes);
		}
	} else {
		dao_read_property(&options, getThis(), SL("_options"), PH_NOISY|PH_READONLY);

		DAO_CALL_CE_STATIC(return_value, dao_tag_ce, "form", &options);

		dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);

		if (Z_TYPE(elements) == IS_ARRAY) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL(elements), el) {
				zval str = {}, el_name = {}, el_label = {}, el_replace_pairs = {};

				DAO_CALL_METHOD(&str, el, "render", attributes);
				if (DAO_IS_NOT_EMPTY(template)) {
					zval tmp = {};
					DAO_CALL_METHOD(&el_name, el, "getname");
					DAO_CALL_METHOD(&el_label, getThis(), "getlabel", &el_name);
					zval_ptr_dtor(&el_name);

					array_init_size(&el_replace_pairs, 2);
					dao_array_update_str(&el_replace_pairs, SL(":label:"), &el_label, 0);
					dao_array_update_str(&el_replace_pairs, SL(":element:"), &str, PH_COPY);

					dao_strtr_array(&tmp, template, &el_replace_pairs);
					zval_ptr_dtor(&el_replace_pairs);
					dao_concat_self(return_value, &tmp);
					zval_ptr_dtor(&tmp);
				} else {
					dao_concat_self(return_value, &str);
				}
				zval_ptr_dtor(&str);
			} ZEND_HASH_FOREACH_END();
		}

		dao_concat_self_str(return_value, SL("</form>"));
	}
}

/**
 * Returns an element added to the form by its name
 *
 * @param string $name
 * @return Dao\Forms\ElementInterface
 */
PHP_METHOD(Dao_Forms_Form, get){

	zval *name, elements = {}, element = {};

	dao_fetch_params(0, 1, 0, &name);

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(&element, &elements, name, PH_READONLY)) {
		DAO_ENSURE_IS_STRING(name);
		zend_throw_exception_ex(dao_forms_exception_ce, 0, "Element with ID=%s is not a part of the form", Z_STRVAL_P(name));
		return;
	}

	RETURN_CTOR(&element);
}

/**
 * Generate the label of a element added to the form including HTML
 *
 * @param string $name
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, label){

	zval *name, *attributes = NULL, elements = {}, element = {};

	dao_fetch_params(0, 1, 1, &name, &attributes);

	if (!attributes) {
		attributes = &DAO_GLOBAL(z_null);
	}

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(&element, &elements, name, PH_READONLY)) {
		DAO_ENSURE_IS_STRING(name);
		zend_throw_exception_ex(dao_forms_exception_ce, 0, "Element with ID=%s is not a part of the form", Z_STRVAL_P(name));
		return;
	}

	DAO_RETURN_CALL_METHOD(&element, "label", attributes);
}

/**
 * Returns a label for an element
 *
 * @param string $name
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, getLabel){

	zval *name, elements = {}, element = {}, label = {};

	dao_fetch_params(0, 1, 0, &name);

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(&element, &elements, name, PH_READONLY)) {
		DAO_ENSURE_IS_STRING(name);
		zend_throw_exception_ex(dao_forms_exception_ce, 0, "Element with ID=%s is not a part of the form", Z_STRVAL_P(name));
		return;
	}

	DAO_CALL_METHOD(&label, &element, "getlabel");

	/* Use the element's name as label if the label is not available */
	if (!zend_is_true(&label)) {
		RETURN_CTOR(name);
	}

	RETURN_ZVAL(&label, 0, 0);
}

/**
 * Gets a value from the internal related entity or from the default value
 *
 * @param string $name
 * @return mixed
 */
PHP_METHOD(Dao_Forms_Form, getValue){

	zval *name, *flag = NULL;

	dao_fetch_params(0, 1, 1, &name, &flag);

	if (!flag) {
		DAO_RETURN_CALL_SELF("getvalues", name, &DAO_GLOBAL(z_zero));
	} else {
		DAO_RETURN_CALL_SELF("getvalues", name, flag);
	}
}

/**
 * Sets a value
 *
 * @param string $name
 * @param mixed $value
 * @return mixed
 */
PHP_METHOD(Dao_Forms_Form, setValue){

	zval *name, *value = NULL, entity = {};

	dao_fetch_params(0, 1, 1, &name, &value);

	if (!value) {
		value = &DAO_GLOBAL(z_null);
	}

	dao_read_property(&entity, getThis(), SL("_entity"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(entity) == IS_OBJECT) {
		zval method = {};
		/**
		 * Check if the entity has a getter
		 */
		DAO_CONCAT_SV(&method, "set", name);
		dao_strtolower_inplace(&method);
		if (dao_method_exists(&entity, &method) == SUCCESS) {
			DAO_CALL_METHOD(NULL, &entity, Z_STRVAL(method), value);
			zval_ptr_dtor(&method);
			return;
		}
		zval_ptr_dtor(&method);

		if (!dao_isset_property_zval(&entity, name) && dao_method_exists_ex(&entity, SL("__set")) == SUCCESS) {
			DAO_CALL_METHOD(NULL, &entity, "__set", name, value);
		} else {
			dao_update_property_zval_zval(&entity, name, value);
		}
		return;
	}

	dao_update_property_array(getThis(), SL("_filterData"), name, value);
}

/**
 * Gets a values
 *
 * @param string $name
 * @return mixed
 */
PHP_METHOD(Dao_Forms_Form, getValues){

	zval *name = NULL, *flag = NULL, data = {}, entity = {}, method = {}, value = {}, filter_data = {};
	int f = 0;

	dao_fetch_params(0, 0, 2, &name, &flag);

	if (!name) {
		name = &DAO_GLOBAL(z_null);
	}

	if (flag) {
		f = dao_get_intval(flag);
	}

	if ((f & DAO_FROM_VALUES_RAW) == DAO_FROM_VALUES_RAW) {
		dao_read_property(&data, getThis(), SL("_data"), PH_NOISY|PH_READONLY);
		if (DAO_IS_EMPTY(name)) {
			RETURN_CTOR(&data);
		}

		if (Z_TYPE(data) == IS_ARRAY) {
			/**
			 * Check if the data is in the data array
			 */
			if (dao_array_isset_fetch(&value, &data, name, PH_READONLY)) {
				RETURN_CTOR(&value);
			}
		}

		RETURN_NULL();
	}

	dao_read_property(&entity, getThis(), SL("_entity"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(entity) == IS_OBJECT) {
		if (DAO_IS_EMPTY(name)) {
			if ((f & DAO_FROM_VALUES_AS_ARRAY) == DAO_FROM_VALUES_AS_ARRAY) {
				dao_get_object_vars(return_value, &entity, 1);
				return;
			} else {
				RETURN_CTOR(&entity);
			}
		}

		/**
		 * Check if the entity has a getter
		 */
		DAO_CONCAT_SV(&method, "get", name);
		dao_strtolower_inplace(&method);
		if (dao_method_exists(&entity, &method) == SUCCESS) {
			DAO_RETURN_CALL_METHOD(&entity, Z_STRVAL(method));
			zval_ptr_dtor(&method);
			return;
		}
		zval_ptr_dtor(&method);

		/**
		 * Check if the entity has a public property
		 */
		if (dao_property_isset_fetch_zval(&value, &entity, name, PH_READONLY)) {
			RETURN_CTOR(&value);
		}
	}

	dao_read_property(&filter_data, getThis(), SL("_filterData"), PH_NOISY|PH_READONLY);

	if (DAO_IS_EMPTY(name)) {
		RETURN_CTOR(&filter_data);
	} else if (Z_TYPE(filter_data) == IS_ARRAY) {
		if (dao_array_isset_fetch(&value, &filter_data, name, PH_READONLY)) {
			RETURN_CTOR(&value);
		}
	}

	RETURN_NULL();
}

/**
 * Check if the form contains an element
 *
 * @param string $name
 * @return boolean
 */
PHP_METHOD(Dao_Forms_Form, has){

	zval *name, elements = {};

	dao_fetch_params(0, 1, 0, &name);

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
	RETURN_BOOL(dao_array_isset(&elements, name));
}

/**
 * Removes an element from the form
 *
 * @param string $name
 * @return boolean
 */
PHP_METHOD(Dao_Forms_Form, remove){

	zval *name, elements = {}, elements_indexed = {};

	dao_fetch_params(0, 1, 0, &name);

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);

	if (dao_isset_property_array(getThis(), SL("_elements"), name)) {
		dao_unset_property_array(getThis(), SL("_elements"), name);

		dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
		dao_array_values(&elements_indexed, &elements);
		dao_update_property(getThis(), SL("_elementsIndexed"), &elements_indexed);
		zval_ptr_dtor(&elements_indexed);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

/**
 * Clears every element in the form to its default value
 *
 * @param array $fields
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, clear){

	zval *fields = NULL, elements = {}, *element, name = {};

	dao_fetch_params(0, 0, 1, &fields);

	if (!fields) {
		fields = &DAO_GLOBAL(z_null);
	}

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(elements) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(elements), element) {
			if (Z_TYPE_P(fields) != IS_ARRAY) {
				DAO_CALL_METHOD(NULL, element, "clear");
			} else {
				DAO_CALL_METHOD(&name, element, "getname");
				if (dao_fast_in_array(&name, fields)) {
					DAO_CALL_METHOD(NULL, element, "clear");
				}
				zval_ptr_dtor(&name);
			}
		} ZEND_HASH_FOREACH_END();

	}

	RETURN_THIS();
}

/**
 * Returns the number of elements in the form
 *
 * @return int
 */
PHP_METHOD(Dao_Forms_Form, count){

	zval elements = {};
	long int count;

	dao_read_property(&elements, getThis(), SL("_elementsIndexed"), PH_NOISY|PH_READONLY);
	count = (Z_TYPE(elements) == IS_ARRAY) ? zend_hash_num_elements(Z_ARRVAL(elements)) : 0;

	RETURN_LONG(count);
}

/**
 * Rewinds the internal iterator
 */
PHP_METHOD(Dao_Forms_Form, rewind){

	zval elements = {}, elements_indexed = {};

	dao_read_property(&elements, getThis(), SL("_elementsIndexed"), PH_NOISY|PH_READONLY);
	dao_update_property_long(getThis(), SL("_position"), 0);
	dao_array_values(&elements_indexed, &elements);
	dao_update_property(getThis(), SL("_elementsIndexed"), &elements_indexed);
	zval_ptr_dtor(&elements_indexed);
}

/**
 * Returns the current element in the iterator
 *
 * @return Dao\Forms\ElementInterface
 */
PHP_METHOD(Dao_Forms_Form, current){

	zval elements = {}, position = {};

	dao_read_property(&elements, getThis(), SL("_elementsIndexed"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(elements) == IS_ARRAY) {
		dao_read_property(&position, getThis(), SL("_position"), PH_NOISY|PH_READONLY);
		if (dao_array_isset_fetch(return_value, &elements, &position, PH_COPY)) {
			return;
		}
	}

	RETURN_NULL();
}

/**
 * Returns the current position/key in the iterator
 *
 * @return int
 */
PHP_METHOD(Dao_Forms_Form, key){

	RETURN_MEMBER(getThis(), "_position");
}

/**
 * Moves the internal iteration pointer to the next position
 *
 */
PHP_METHOD(Dao_Forms_Form, next){

	dao_property_incr(getThis(), SL("_position"));
}

/**
 * Check if the current element in the iterator is valid
 *
 * @return boolean
 */
PHP_METHOD(Dao_Forms_Form, valid){

	zval elements = {}, position = {};

	dao_read_property(&elements, getThis(), SL("_elementsIndexed"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(elements) == IS_ARRAY) {
		dao_read_property(&position, getThis(), SL("_position"), PH_NOISY|PH_READONLY);
		if (dao_array_isset(&elements, &position)) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}

/**
 * Appends a message to the form
 *
 *<code>
 * $form->appendMessage('email', new Dao\Validation\Message('Must be not empty '));
 *</code>
 *
 * @param string $field
 * @param Dao\Validation\Message $message
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, appendMessage){

	zval *filed, *message, current_messages = {}, element_messages = {};

	dao_fetch_params(0, 2, 0, &filed, &message);

	dao_read_property(&current_messages, getThis(), SL("_messages"), PH_COPY);
	if (Z_TYPE(current_messages) != IS_ARRAY) {
		array_init(&current_messages);
	}

	if (!dao_array_isset_fetch(&element_messages, &current_messages, filed, PH_COPY)) {
		object_init_ex(&element_messages, dao_validation_message_group_ce);
		DAO_CALL_METHOD(NULL, &element_messages, "__construct");
	}

	DAO_CALL_METHOD(NULL, &element_messages, "appendmessage", message);

	dao_array_update(&current_messages, filed, &element_messages, 0);
	dao_update_property(getThis(), SL("_messages"), &current_messages);
	zval_ptr_dtor(&current_messages);

	RETURN_THIS();
}

/**
 * Appends a messages to the form
 *
 *<code>
 * $form->appendMessages('email', array(new Dao\Validation\Message('Must be not empty '), new Dao\Validation\Message('Must be an email address')));
 *</code>
 *
 * @param string $field
 * @param Dao\Validation\MessageInterface[] $messages
 * @return Dao\Forms\Form
 */
PHP_METHOD(Dao_Forms_Form, appendMessages){

	zval *filed, *messages, current_messages = {}, element_messages = {};

	dao_fetch_params(0, 2, 0, &filed, &messages);

	dao_read_property(&current_messages, getThis(), SL("_messages"), PH_READONLY);
	if (Z_TYPE(current_messages) != IS_ARRAY) {
		array_init(&current_messages);
		dao_update_property(getThis(), SL("_messages"), &current_messages);
		zval_ptr_dtor(&current_messages);
	}

	if (!dao_array_isset_fetch(&element_messages, &current_messages, filed, PH_READONLY)) {
		object_init_ex(&element_messages, dao_validation_message_group_ce);
		DAO_CALL_METHOD(NULL, &element_messages, "__construct");
		dao_array_update(&current_messages, filed, &element_messages, 0);
	}

	DAO_CALL_METHOD(NULL, &element_messages, "appendmessages", messages);

	RETURN_THIS();
}

/**
 * Returns a form elements as an array
 *
 * @return array
 */
PHP_METHOD(Dao_Forms_Form, toArray){

	zval options = {}, elements = {}, *element;

	array_init(return_value);

	dao_read_property(&options, getThis(), SL("_options"), PH_NOISY|PH_READONLY);
	if (!DAO_IS_EMPTY(&options)) {
		dao_array_update_str(return_value, SL("options"), &options, PH_COPY);
	}

	dao_read_property(&elements, getThis(), SL("_elements"), PH_NOISY|PH_READONLY);

	if (Z_TYPE(elements) == IS_ARRAY) {
		zval index = {};
		ZVAL_STRING(&index, "elements");
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(elements), element) {
			zval name ={}, value = {};

			DAO_CALL_METHOD(&name, element, "getname");
			DAO_CALL_METHOD(&value, element, "toarray");

			dao_array_update_multi_2(return_value, &index, &name, &value, 0);
			zval_ptr_dtor(&name);
		} ZEND_HASH_FOREACH_END();
		zval_ptr_dtor(&index);
	}
}

/**
 * Renders the form html
 *
 * @return string
 */
PHP_METHOD(Dao_Forms_Form, __toString){

	DAO_CALL_METHOD(return_value, getThis(), "render");
}
