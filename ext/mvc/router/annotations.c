
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
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#include "mvc/router/annotations.h"
#include "mvc/router.h"
#include "mvc/router/exception.h"
#include "annotations/adapterinterface.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/concat.h"
#include "kernel/hash.h"
#include "kernel/operators.h"

#include "interned-strings.h"

/**
 * Dao\Mvc\Router\Annotations
 *
 * A router that reads routes annotations from classes/resources
 *
 *<code>
 * $di['router'] = function() {
 *
 *		//Use the annotations router
 *		$router = new \Dao\Mvc\Router\Annotations(false);
 *
 *		//This will do the same as above but only if the handled uri starts with /robots
 * 		$router->addResource('Robots', '/robots');
 *
 * 		return $router;
 *	};
 *</code>
 */
zend_class_entry *dao_mvc_router_annotations_ce;

PHP_METHOD(Dao_Mvc_Router_Annotations, addResource);
PHP_METHOD(Dao_Mvc_Router_Annotations, addModuleResource);
PHP_METHOD(Dao_Mvc_Router_Annotations, handle);
PHP_METHOD(Dao_Mvc_Router_Annotations, processControllerAnnotation);
PHP_METHOD(Dao_Mvc_Router_Annotations, processActionAnnotation);
PHP_METHOD(Dao_Mvc_Router_Annotations, setControllerSuffix);
PHP_METHOD(Dao_Mvc_Router_Annotations, setActionSuffix);
PHP_METHOD(Dao_Mvc_Router_Annotations, getResources);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_addresource, 0, 0, 1)
	ZEND_ARG_INFO(0, handler)
	ZEND_ARG_INFO(0, prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_addmoduleresource, 0, 0, 2)
	ZEND_ARG_INFO(0, module)
	ZEND_ARG_INFO(0, handler)
	ZEND_ARG_INFO(0, prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_handle, 0, 0, 0)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_processcontrollerannotation, 0, 0, 2)
	ZEND_ARG_INFO(0, handler)
	ZEND_ARG_INFO(0, annotation)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_processactionannotation, 0, 0, 5)
	ZEND_ARG_INFO(0, module)
	ZEND_ARG_INFO(0, namespace)
	ZEND_ARG_INFO(0, controller)
	ZEND_ARG_INFO(0, action)
	ZEND_ARG_INFO(0, annotation)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_setcontrollersuffix, 0, 0, 1)
	ZEND_ARG_INFO(0, controllerSuffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_router_annotations_setactionsuffix, 0, 0, 1)
	ZEND_ARG_INFO(0, actionSuffix)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_mvc_router_annotations_method_entry[] = {
	PHP_ME(Dao_Mvc_Router_Annotations, addResource, arginfo_dao_mvc_router_annotations_addresource, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, addModuleResource, arginfo_dao_mvc_router_annotations_addmoduleresource, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, handle, arginfo_dao_mvc_router_annotations_handle, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, processControllerAnnotation, arginfo_dao_mvc_router_annotations_processcontrollerannotation, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, processActionAnnotation, arginfo_dao_mvc_router_annotations_processactionannotation, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, setControllerSuffix, arginfo_dao_mvc_router_annotations_setcontrollersuffix, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, setActionSuffix, arginfo_dao_mvc_router_annotations_setactionsuffix, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Router_Annotations, getResources, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Mvc\Router\Annotations initializer
 */
DAO_INIT_CLASS(Dao_Mvc_Router_Annotations){

	DAO_REGISTER_CLASS_EX(Dao\\Mvc\\Router, Annotations, mvc_router_annotations, dao_mvc_router_ce, dao_mvc_router_annotations_method_entry, 0);

	zend_declare_property_null(dao_mvc_router_annotations_ce, SL("_handlers"), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(dao_mvc_router_annotations_ce, SL("_processed"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_string(dao_mvc_router_annotations_ce, SL("_controllerSuffix"), "Controller", ZEND_ACC_PROTECTED);
	zend_declare_property_string(dao_mvc_router_annotations_ce, SL("_actionSuffix"), "Action", ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_router_annotations_ce, SL("_routePrefix"), ZEND_ACC_PROTECTED);

	return SUCCESS;
}

/**
 * Adds a resource to the annotations handler
 * A resource is a class that contains routing annotations
 *
 * @param string $handler
 * @param string $prefix
 * @return Dao\Mvc\Router\Annotations
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, addResource){

	zval *handler, *prefix = NULL, scope = {};

	dao_fetch_params(0, 1, 1, &handler, &prefix);

	if (!prefix) {
		prefix = &DAO_GLOBAL(z_null);
	}

	if (Z_TYPE_P(handler) != IS_STRING) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_router_exception_ce, "The handler must be a class name");
		return;
	}

	array_init_size(&scope, 2);
	dao_array_append(&scope, prefix, PH_COPY);
	dao_array_append(&scope, handler, PH_COPY);
	dao_update_property_array_append(getThis(), SL("_handlers"), &scope);
	dao_update_property(getThis(), SL("_processed"), &DAO_GLOBAL(z_false));

	RETURN_THIS();
}

/**
 * Adds a resource to the annotations handler
 * A resource is a class that contains routing annotations
 * The class is located in a module
 *
 * @param string $module
 * @param string $handler
 * @param string $prefix
 * @return Dao\Mvc\Router\Annotations
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, addModuleResource){

	zval *module, *handler, *prefix = NULL, scope = {};

	dao_fetch_params(0, 2, 1, &module, &handler, &prefix);

	if (!prefix) {
		prefix = &DAO_GLOBAL(z_null);
	}

	if (Z_TYPE_P(module) != IS_STRING) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_router_exception_ce, "The module is not a valid string");
		return;
	}
	if (Z_TYPE_P(handler) != IS_STRING) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_router_exception_ce, "The handler must be a class name");
		return;
	}

	array_init_size(&scope, 3);
	dao_array_append(&scope, prefix, PH_COPY);
	dao_array_append(&scope, handler, PH_COPY);
	dao_array_append(&scope, module, PH_COPY);
	dao_update_property_array_append(getThis(), SL("_handlers"), &scope);

	dao_update_property(getThis(), SL("_processed"), &DAO_GLOBAL(z_false));

	RETURN_THIS();
}

/**
 * Produce the routing parameters from the rewrite information
 *
 * @param string $uri
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, handle){

	zval *uri = NULL, real_uri = {}, service = {}, annotations_service = {}, processed = {}, handlers = {}, controller_suffix = {}, *scope;
	zend_string *str_key;
	ulong idx;

	dao_fetch_params(0, 0, 1, &uri);

	if (!uri || Z_TYPE_P(uri) == IS_NULL) {
		DAO_CALL_METHOD(&real_uri, getThis(), "getrewriteuri");
	} else {
		ZVAL_COPY_VALUE(&real_uri, uri);
	}

	ZVAL_STR(&service, IS(annotations));

	DAO_CALL_METHOD(&annotations_service, getThis(), "getresolveservice", &service);
	DAO_VERIFY_INTERFACE(&annotations_service, dao_annotations_adapterinterface_ce);

	dao_read_property(&processed, getThis(), SL("_processed"), PH_READONLY);
	if (!zend_is_true(&processed)) {
		dao_read_property(&handlers, getThis(), SL("_handlers"), PH_READONLY);
		if (Z_TYPE(handlers) == IS_ARRAY) {
			dao_read_property(&controller_suffix, getThis(), SL("_controllerSuffix"), PH_READONLY);

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL(handlers), scope) {
				zval prefix = {}, handler = {}, controller_name = {}, namespace_name = {}, module_name = {}, suffixed = {};
				zval handler_annotations = {}, class_annotations = {}, annotations = {}, *annotation, method_annotations = {}, *collection;
				if (Z_TYPE_P(scope) == IS_ARRAY) {
					/**
					 * A prefix (if any) must be in position 0
					 */
					dao_array_fetch_long(&prefix, scope, 0, PH_NOISY|PH_READONLY);
					if (Z_TYPE(prefix) == IS_STRING) {
						if (!dao_start_with(&real_uri, &prefix, NULL)) {
							continue;
						}
					}

					/**
					 * The controller must be in position 1
					 */
					dao_array_fetch_long(&handler, scope, 1, PH_NOISY|PH_READONLY);
					if (dao_memnstr_str(&handler, SL("\\"))) {
						/**
						 * Extract the real class name from the namespaced class
						 */
						dao_get_class_ns(&controller_name, &handler, 0);

						/**
						 * Extract the namespace from the namespaced class
						 */
						dao_get_ns_class(&namespace_name, &handler, 0);
					} else {
						ZVAL_COPY_VALUE(&controller_name, &handler);
					}

					dao_update_property_null(getThis(), SL("_routePrefix"));

					/**
					 * Check if the scope has a module associated
					 */
					if (dao_array_isset_long(scope, 2)) {
						dao_array_fetch_long(&module_name, scope, 2, PH_NOISY|PH_READONLY);
					} else {
						ZVAL_NULL(&module_name);
					}

					DAO_CONCAT_VV(&suffixed, &handler, &controller_suffix);

					/**
					 * Get the annotations from the class
					 */
					DAO_CALL_METHOD(&handler_annotations, &annotations_service, "get", &suffixed);

					/**
					 * Process class annotations
					 */
					DAO_CALL_METHOD(&class_annotations, &handler_annotations, "getclassannotations");
					if (Z_TYPE(class_annotations) == IS_OBJECT) {
						/**
						 * Process class annotations
						 */
						DAO_CALL_METHOD(&annotations, &class_annotations, "getannotations");
						if (Z_TYPE(annotations) == IS_ARRAY) {
							ZEND_HASH_FOREACH_VAL(Z_ARRVAL(annotations), annotation) {
								DAO_CALL_METHOD(NULL, getThis(), "processcontrollerannotation", &controller_name, annotation);
							} ZEND_HASH_FOREACH_END();

						}
					}

					/**
					 * Process method annotations
					 */
					DAO_CALL_METHOD(&method_annotations, &handler_annotations, "getmethodsannotations");
					if (Z_TYPE(method_annotations) == IS_ARRAY) {
						ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(method_annotations), idx, str_key, collection) {
							zval method = {};
							if (str_key) {
								ZVAL_STR(&method, str_key);
							} else {
								ZVAL_LONG(&method, idx);
							}
							if (Z_TYPE_P(collection) == IS_OBJECT) {
								DAO_CALL_METHOD(&annotations, collection, "getannotations");
								ZEND_HASH_FOREACH_VAL(Z_ARRVAL(annotations), annotation) {
									DAO_CALL_METHOD(NULL, getThis(), "processactionannotation", &module_name, &namespace_name, &controller_name, &method, annotation);
								} ZEND_HASH_FOREACH_END();

							}
						} ZEND_HASH_FOREACH_END();
					}
				}
			} ZEND_HASH_FOREACH_END();
		}

		dao_update_property_bool(getThis(), SL("_processed"), 1);
	}

	/**
	 * Call the parent handle method()
	 */
	DAO_CALL_PARENT(NULL, dao_mvc_router_annotations_ce, getThis(), "handle", &real_uri);
}

/**
 * Checks for annotations in the controller docblock
 *
 * @param string $handler
 * @param Dao\Annotations\AdapterInterface
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, processControllerAnnotation){

	zval *handler, *annotation, name = {}, value = {};

	dao_fetch_params(0, 2, 0, &handler, &annotation);

	DAO_CALL_METHOD(&name, annotation, "getname");

	/**
	 * @RoutePrefix add a prefix for all the routes defined in the model
	 */
	if (DAO_IS_STRING(&name, "RoutePrefix")) {
		DAO_CALL_METHOD(&value, annotation, "getargument", &DAO_GLOBAL(z_zero));
		dao_update_property(getThis(), SL("_routePrefix"), &value);
	}
}

/**
 * Checks for annotations in the public methods of the controller
 *
 * @param string $module
 * @param string $namespace
 * @param string $controller
 * @param string $action
 * @param Dao\Annotations\Annotation $annotation
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, processActionAnnotation){

	zval *module, *namespace, *controller, *action, *annotation, name = {}, methods = {}, action_suffix = {}, route_prefix = {}, empty_str = {}, real_action_name = {}, action_name = {};
	zval parameter = {}, paths = {}, position = {}, value = {}, uri = {}, route = {}, converts = {}, *convert, route_name = {};
	zend_string *str_key;
	ulong idx;
	int is_route;

	dao_fetch_params(0, 5, 0, &module, &namespace, &controller, &action, &annotation);

	DAO_CALL_METHOD(&name, annotation, "getname");

	/* Find if the route is for adding routes */
	if (DAO_IS_STRING(&name, "Route")) {
		is_route = 1;
	} else if (DAO_IS_STRING(&name, "Get")) {
		is_route = 1;
		ZVAL_STR(&methods, IS(GET));
	} else if (DAO_IS_STRING(&name, "Post")) {
		is_route = 1;
		ZVAL_STR(&methods, IS(POST));
	} else if (DAO_IS_STRING(&name, "Put")) {
		is_route = 1;
		ZVAL_STR(&methods, IS(PUT));
	} else if (DAO_IS_STRING(&name, "Delete")) {
		is_route = 1;
		ZVAL_STR(&methods, IS(DELETE));
	} else if (DAO_IS_STRING(&name, "Options")) {
		is_route = 1;
		ZVAL_STR(&methods, IS(OPTIONS));
	} else {
		is_route = 0;
	}

	if (is_route) {
		dao_read_property(&action_suffix, getThis(), SL("_actionSuffix"), PH_READONLY);
		dao_read_property(&route_prefix, getThis(), SL("_routePrefix"), PH_READONLY);

		ZVAL_EMPTY_STRING(&empty_str);

		DAO_STR_REPLACE(&real_action_name, &action_suffix, &empty_str, action);

		dao_fast_strtolower(&action_name, &real_action_name);

		ZVAL_STR(&parameter, IS(paths));

		/**
		 * Check for existing paths in the annotation
		 */
		DAO_CALL_METHOD(&paths, annotation, "getargument", &parameter);
		if (Z_TYPE(paths) != IS_ARRAY) {
			array_init(&paths);
		}

		/**
		 * Update the module if any
		 */
		if (Z_TYPE_P(module) == IS_STRING) {
			dao_array_update_string(&paths, IS(module), module, PH_COPY);
		}

		/**
		 * Update the namespace if any
		 */
		if (Z_TYPE_P(namespace) == IS_STRING) {
			dao_array_update_string(&paths, IS(namespace), namespace, PH_COPY);
		}

		dao_array_update_string(&paths, IS(controller), controller, PH_COPY);
		dao_array_update_string(&paths, IS(action), &real_action_name, PH_COPY);
		dao_array_update_str(&paths, SL("\0exact"), &DAO_GLOBAL(z_true), PH_COPY);

		ZVAL_LONG(&position, 0);

		DAO_CALL_METHOD(&value, annotation, "getargument", &position);

		/**
		 * Create the route using the prefix
		 */
		if (Z_TYPE(value) != IS_NULL) {
			if (!DAO_IS_STRING(&value, "/")) {
				DAO_CONCAT_VV(&uri, &route_prefix, &value);
			} else {
				if (Z_TYPE(route_prefix) != IS_NULL) {
					ZVAL_COPY_VALUE(&uri, &route_prefix);
				} else {
					ZVAL_COPY_VALUE(&uri, &value);
				}
			}
		} else {
			DAO_CONCAT_VV(&uri, &route_prefix, &action_name);
		}

		/**
		 * Add the route to the router
		 */
		DAO_CALL_METHOD(&route, getThis(), "add", &uri, &paths);
		if (Z_TYPE(methods) <= IS_NULL) {
			ZVAL_STRING(&parameter, "methods");

			DAO_CALL_METHOD(&methods, annotation, "getargument", &parameter);

			if (Z_TYPE(methods) == IS_ARRAY || Z_TYPE(methods) == IS_STRING) {
				DAO_CALL_METHOD(NULL, &route, "via", &methods);
			}
		} else {
			DAO_CALL_METHOD(NULL, &route, "via", &methods);
		}

		ZVAL_STRING(&parameter, "converts");

		DAO_CALL_METHOD(&converts, annotation, "getargument", &parameter);
		if (Z_TYPE(converts) == IS_ARRAY) {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(converts), idx, str_key, convert) {
				zval param = {};
				if (str_key) {
					ZVAL_STR(&param, str_key);
				} else {
					ZVAL_LONG(&param, idx);
				}
				DAO_CALL_METHOD(NULL, &route, "convert", &param, convert);
			} ZEND_HASH_FOREACH_END();
		}

		ZVAL_STRING(&parameter, "conversors");

		DAO_CALL_METHOD(&converts, annotation, "getargument", &parameter);
		if (Z_TYPE(converts) == IS_ARRAY) {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(converts), idx, str_key, convert) {
				zval conversor_param = {};
				if (str_key) {
					ZVAL_STR(&conversor_param, str_key);
				} else {
					ZVAL_LONG(&conversor_param, idx);
				}
				DAO_CALL_METHOD(NULL, &route, "convert", &conversor_param, convert);
			} ZEND_HASH_FOREACH_END();
		}

		ZVAL_STR(&parameter, IS(name));

		DAO_CALL_METHOD(&route_name, annotation, "getargument", &parameter);
		if (Z_TYPE(route_name) == IS_STRING) {
			DAO_CALL_METHOD(NULL, &route, "setname", &route_name);
		}

		RETURN_TRUE;
	}
}

/**
 * Changes the controller class suffix
 *
 * @param string $controllerSuffix
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, setControllerSuffix){

	zval *controller_suffix;

	dao_fetch_params(0, 1, 0, &controller_suffix);

	dao_update_property(getThis(), SL("_controllerSuffix"), controller_suffix);

}

/**
 * Changes the action method suffix
 *
 * @param string $actionSuffix
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, setActionSuffix){

	zval *action_suffix;

	dao_fetch_params(0, 1, 0, &action_suffix);

	dao_update_property(getThis(), SL("_actionSuffix"), action_suffix);

}

/**
 * Return the registered resources
 *
 * @return array
 */
PHP_METHOD(Dao_Mvc_Router_Annotations, getResources){


	RETURN_MEMBER(getThis(), "_handlers");
}
