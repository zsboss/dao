
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

#include "mvc/model/transaction.h"
#include "mvc/model/transactioninterface.h"
#include "mvc/model/transaction/exception.h"
#include "mvc/model/transaction/failed.h"
#include "mvc/model/transaction/managerinterface.h"
#include "di/injectable.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/fcall.h"
#include "kernel/object.h"
#include "kernel/array.h"

#include "interned-strings.h"

/**
 * Dao\Mvc\Model\Transaction
 *
 * Transactions are protective blocks where SQL statements are only permanent if they can
 * all succeed as one atomic action. Dao\Transaction is intended to be used with Dao_Model_Base.
 * Dao Transactions should be created using Dao\Transaction\Manager.
 *
 *<code>
 *try {
 *
 *  $manager = new Dao\Mvc\Model\Transaction\Manager();
 *
 *  $transaction = $manager->get();
 *
 *  $robot = new Robots();
 *  $robot->setTransaction($transaction);
 *  $robot->name = 'WALL·E';
 *  $robot->created_at = date('Y-m-d');
 *  if ($robot->save() == false) {
 *    $transaction->rollback("Can't save robot");
 *  }
 *
 *  $robotPart = new RobotParts();
 *  $robotPart->setTransaction($transaction);
 *  $robotPart->type = 'head';
 *  if ($robotPart->save() == false) {
 *    $transaction->rollback("Can't save robot part");
 *  }
 *
 *  $transaction->commit();
 *
 *} catch(Dao\Mvc\Model\Transaction\Failed $e) {
 *  echo 'Failed, reason: ', $e->getMessage();
 *}
 *
 *</code>
 */
zend_class_entry *dao_mvc_model_transaction_ce;

PHP_METHOD(Dao_Mvc_Model_Transaction, __construct);
PHP_METHOD(Dao_Mvc_Model_Transaction, setTransactionManager);
PHP_METHOD(Dao_Mvc_Model_Transaction, begin);
PHP_METHOD(Dao_Mvc_Model_Transaction, commit);
PHP_METHOD(Dao_Mvc_Model_Transaction, rollback);
PHP_METHOD(Dao_Mvc_Model_Transaction, getConnection);
PHP_METHOD(Dao_Mvc_Model_Transaction, setIsNewTransaction);
PHP_METHOD(Dao_Mvc_Model_Transaction, setRollbackOnAbort);
PHP_METHOD(Dao_Mvc_Model_Transaction, isManaged);
PHP_METHOD(Dao_Mvc_Model_Transaction, getMessages);
PHP_METHOD(Dao_Mvc_Model_Transaction, isValid);
PHP_METHOD(Dao_Mvc_Model_Transaction, setRollbackedRecord);
PHP_METHOD(Dao_Mvc_Model_Transaction, getRollbackedRecord);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_model_transaction___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, dependencyInjector)
	ZEND_ARG_INFO(0, autoBegin)
	ZEND_ARG_INFO(0, service)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_mvc_model_transaction_method_entry[] = {
	PHP_ME(Dao_Mvc_Model_Transaction, __construct, arginfo_dao_mvc_model_transaction___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Mvc_Model_Transaction, setTransactionManager, arginfo_dao_mvc_model_transactioninterface_settransactionmanager, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, begin, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, commit, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, rollback, arginfo_dao_mvc_model_transactioninterface_rollback, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, getConnection, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, setIsNewTransaction, arginfo_dao_mvc_model_transactioninterface_setisnewtransaction, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, setRollbackOnAbort, arginfo_dao_mvc_model_transactioninterface_setrollbackonabort, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, isManaged, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, getMessages, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, isValid, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, setRollbackedRecord, arginfo_dao_mvc_model_transactioninterface_setrollbackedrecord, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_Transaction, getRollbackedRecord, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Mvc\Model\Transaction initializer
 */
DAO_INIT_CLASS(Dao_Mvc_Model_Transaction){

	DAO_REGISTER_CLASS_EX(Dao\\Mvc\\Model, Transaction, mvc_model_transaction, dao_di_injectable_ce, dao_mvc_model_transaction_method_entry, 0);

	zend_declare_property_null(dao_mvc_model_transaction_ce, SL("_connection"), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(dao_mvc_model_transaction_ce, SL("_activeTransaction"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_bool(dao_mvc_model_transaction_ce, SL("_isNewTransaction"), 1, ZEND_ACC_PROTECTED);
	zend_declare_property_bool(dao_mvc_model_transaction_ce, SL("_rollbackOnAbort"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_model_transaction_ce, SL("_manager"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_model_transaction_ce, SL("_messages"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_model_transaction_ce, SL("_rollbackRecord"), ZEND_ACC_PROTECTED);

	zend_class_implements(dao_mvc_model_transaction_ce, 1, dao_mvc_model_transactioninterface_ce);

	return SUCCESS;
}

/**
 * Dao\Mvc\Model\Transaction constructor
 *
 * @param Dao\DiInterface $dependencyInjector
 * @param boolean $autoBegin
 * @param string $service
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, __construct){

	zval *dependency_injector, *auto_begin = NULL, *s = NULL, service = {}, connection = {};

	dao_fetch_params(0, 1, 2, &dependency_injector, &auto_begin, &s);

	if (!auto_begin) {
		auto_begin = &DAO_GLOBAL(z_false);
	}

	if (!s || Z_TYPE_P(s) != IS_STRING) {
		zval models_manager = {};
		ZVAL_STR(&service, IS(modelsManager));
		DAO_CALL_METHOD(&models_manager, getThis(), "getservice", &service);
		DAO_CALL_METHOD(&service, &models_manager, "getdefaultwriteconnectionservice");
		zval_ptr_dtor(&models_manager);
	} else {
		ZVAL_COPY(&service, s);
	}

	if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_model_transaction_exception_ce, "A dependency injector container is required to obtain the services related to the ORM");
		return;
	}

	DAO_CALL_METHOD(&connection, dependency_injector, "get", &service);
	zval_ptr_dtor(&service);

	dao_update_property(getThis(), SL("_connection"), &connection);
	if (zend_is_true(auto_begin)) {
		DAO_CALL_METHOD(NULL, &connection, "begin");
	}
	zval_ptr_dtor(&connection);
}

/**
 * Sets transaction manager related to the transaction
 *
 * @param Dao\Mvc\Model\Transaction\ManagerInterface $manager
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, setTransactionManager){

	zval *manager;

	dao_fetch_params(0, 1, 0, &manager);
	DAO_VERIFY_INTERFACE_EX(manager, dao_mvc_model_transaction_managerinterface_ce, dao_mvc_model_transaction_exception_ce);

	dao_update_property(getThis(), SL("_manager"), manager);
}

/**
 * Starts the transaction
 *
 * @return boolean
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, begin){

	zval connection = {};

	dao_read_property(&connection, getThis(), SL("_connection"), PH_NOISY|PH_READONLY);
	DAO_RETURN_CALL_METHOD(&connection, "begin");
}

/**
 * Commits the transaction
 *
 * @return boolean
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, commit){

	zval manager = {}, connection = {};

	dao_read_property(&manager, getThis(), SL("_manager"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(manager) == IS_OBJECT) {
		DAO_CALL_METHOD(NULL, &manager, "notifycommit", getThis());
	}

	dao_read_property(&connection, getThis(), SL("_connection"), PH_NOISY|PH_READONLY);
	DAO_RETURN_CALL_METHOD(&connection, "commit");
}

/**
 * Rolls back the transaction
 *
 * @param  string $rollbackMessage
 * @param  Dao\Mvc\ModelInterface $rollbackRecord
 * @param  int $rollbackCode
 * @param  boolean $noThrowError
 * @return boolean
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, rollback){

	zval *message = NULL, *_rollback_record = NULL, *rollback_code = NULL, *nothrowerror = NULL, rollback_record = {}, rollback_message = {}, manager = {}, connection = {}, success = {}, i0 = {};

	dao_fetch_params(0, 0, 4, &message, &rollback_record, &rollback_code, &nothrowerror);

	if (!message || !zend_is_true(message)) {
		ZVAL_STRING(&rollback_message, "Transaction aborted");
	} else {
		ZVAL_COPY(&rollback_message, message);
	}

	if (_rollback_record && Z_TYPE_P(_rollback_record) != IS_OBJECT) {
		dao_update_property(getThis(), SL("_rollbackRecord"), _rollback_record);
	}

	dao_read_property(&rollback_record, getThis(), SL("_rollbackRecord"), PH_NOISY|PH_READONLY);

	if (!rollback_code) {
		rollback_code = &DAO_GLOBAL(z_zero);
	}

	if (!nothrowerror) {
		nothrowerror = &DAO_GLOBAL(z_false);
	}

	dao_read_property(&manager, getThis(), SL("_manager"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(manager) == IS_OBJECT) {
		DAO_CALL_METHOD(NULL, &manager, "notifyrollback", getThis());
	}

	dao_read_property(&connection, getThis(), SL("_connection"), PH_NOISY|PH_READONLY);

	DAO_CALL_METHOD(&success, &connection, "rollback");

	if (!zend_is_true(nothrowerror) && zend_is_true(&success)) {
		object_init_ex(&i0, dao_mvc_model_transaction_failed_ce);
		DAO_CALL_METHOD(NULL, &i0, "__construct", &rollback_message, &rollback_record, rollback_code);
		dao_throw_exception(&i0);
	}
	zval_ptr_dtor(&rollback_message);

	RETURN_CTOR(&success);
}

/**
 * Returns the connection related to transaction
 *
 * @return Dao\Db\AdapterInterface
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, getConnection){

	zval rollback_on_abort = {}, message = {}, connection = {};

	dao_read_property(&rollback_on_abort, getThis(), SL("_rollbackOnAbort"), PH_NOISY|PH_READONLY);
	if (zend_is_true(&rollback_on_abort)) {

		if (PG(connection_status) & PHP_CONNECTION_ABORTED) {
			ZVAL_STRING(&message, "The request was aborted");
			DAO_CALL_METHOD(NULL, getThis(), "rollback", &message);
			zval_ptr_dtor(&message);
		}
	}

	dao_read_property(&connection, getThis(), SL("_connection"), PH_NOISY|PH_READONLY);

	RETURN_CTOR(&connection);
}

/**
 * Sets if is a reused transaction or new once
 *
 * @param boolean $isNew
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, setIsNewTransaction){

	zval *is_new;

	dao_fetch_params(0, 1, 0, &is_new);

	dao_update_property(getThis(), SL("_isNewTransaction"), is_new);

}

/**
 * Sets flag to rollback on abort the HTTP connection
 *
 * @param boolean $rollbackOnAbort
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, setRollbackOnAbort){

	zval *rollback_on_abort;

	dao_fetch_params(0, 1, 0, &rollback_on_abort);

	dao_update_property(getThis(), SL("_rollbackOnAbort"), rollback_on_abort);

}

/**
 * Checks whether transaction is managed by a transaction manager
 *
 * @return boolean
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, isManaged){

	zval manager = {};

	dao_read_property(&manager, getThis(), SL("_manager"), PH_NOISY|PH_READONLY);
	boolean_not_function(return_value, &manager);
}

/**
 * Returns validations messages from last save try
 *
 * @return array
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, getMessages){


	RETURN_MEMBER(getThis(), "_messages");
}

/**
 * Checks whether internal connection is under an active transaction
 *
 * @return boolean
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, isValid){

	zval connection = {};

	dao_read_property(&connection, getThis(), SL("_connection"), PH_NOISY|PH_READONLY);
	DAO_RETURN_CALL_METHOD(&connection, "isundertransaction");
}

/**
 * Sets object which generates rollback action
 *
 * @param Dao\Mvc\ModelInterface $record
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, setRollbackedRecord){

	zval *record;

	dao_fetch_params(0, 1, 0, &record);

	dao_update_property(getThis(), SL("_rollbackRecord"), record);

}

/**
 * Gets object which generates rollback action
 *
 * @return Dao\Mvc\ModelInterface
 */
PHP_METHOD(Dao_Mvc_Model_Transaction, getRollbackedRecord){


	RETURN_MEMBER(getThis(), "_rollbackRecord");

}
