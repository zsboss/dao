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
  | Authors: Remi Collet <remi@famillecollet.com>                          |
  +------------------------------------------------------------------------+
*/

/* placeholder for non-free jsminifier.h */

#ifndef DAO_ASSETS_FILTERS_JSMINIFIER_H
#define DAO_ASSETS_FILTERS_JSMINIFIER_H

#include <Zend/zend.h>

int dao_jsmin(zval *return_value, zval *script);

#endif /* DAO_ASSETS_FILTERS_JSMINIFIER_H */
