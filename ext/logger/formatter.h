
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

#ifndef DAO_LOGGER_FORMATTER_H
#define DAO_LOGGER_FORMATTER_H

#include "php_dao.h"

extern zend_class_entry *dao_logger_formatter_ce;

DAO_INIT_CLASS(Dao_Logger_Formatter);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_logger_formatter_gettypestring, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_logger_formatter_interpolate, 0, 0, 2)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

#endif /* DAO_LOGGER_FORMATTER_H */
