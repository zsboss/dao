
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

#ifndef DAO_DISPATCHERINTERFACE_H
#define DAO_DISPATCHERINTERFACE_H

#include "php_dao.h"

extern zend_class_entry *dao_dispatcherinterface_ce;

DAO_INIT_CLASS(Dao_DispatcherInterface);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setactionsuffix, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, actionSuffix, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_sethandlersuffix, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, handlerSuffix, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setdefaultmodule, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, module, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setdefaultnamespace, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setdefaulthandler, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, handlerName, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setdefaultaction, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, actionName, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setmodulename, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, module, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setnamespacename, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_sethandlername, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, handlerName, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setactionname, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, actionName, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setlogicbinding, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setparams, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_hasparam, 0, 0, 1)
	ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_setparam, 0, 0, 2)
	ZEND_ARG_INFO(0, param)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_getparam, 0, 0, 1)
	ZEND_ARG_INFO(0, param)
	ZEND_ARG_INFO(0, filters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_forward, 0, 0, 1)
	ZEND_ARG_INFO(0, forward)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_camelizenamespace, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, camelize, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_camelizecontroller, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, camelize, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_seterrorhandler, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, exception_code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_dispatcherinterface_geterrorhandler, 0, 0, 1)
	ZEND_ARG_INFO(0, exception_code)
ZEND_END_ARG_INFO()

#endif /* DAO_DISPATCHERINTERFACE_H */
