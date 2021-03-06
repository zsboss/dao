
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

#ifndef DAO_EVENTS_MANAGERINTERFACE_H
#define DAO_EVENTS_MANAGERINTERFACE_H

#include "php_dao.h"

extern zend_class_entry *dao_events_managerinterface_ce;

DAO_INIT_CLASS(Dao_Events_ManagerInterface);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_managerinterface_attach, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, eventType, IS_STRING, 0)
	ZEND_ARG_INFO(0, handler)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_managerinterface_detach, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, eventType, IS_STRING, 0)
	ZEND_ARG_INFO(0, handler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_managerinterface_detachall, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, eventType, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_managerinterface_createevent, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, eventType, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_OBJECT, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_TYPE_INFO(0, cancelable, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, flag, _IS_BOOL, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_managerinterface_fire, 0, 0, 2)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_TYPE_INFO(0, source, IS_OBJECT, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_TYPE_INFO(0, cancelable, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, flag, _IS_BOOL, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_managerinterface_getlisteners, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, eventType, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, full, _IS_BOOL, 1)
ZEND_END_ARG_INFO()

#endif /* DAO_EVENTS_MANAGERINTERFACE_H */
