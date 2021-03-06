
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

#include "mvc/model/metadata/strategy/annotations.h"
#include "mvc/model/metadata.h"
#include "mvc/model/exception.h"
#include "db/column.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/fcall.h"
#include "kernel/object.h"
#include "kernel/concat.h"
#include "kernel/file.h"
#include "kernel/hash.h"
#include "kernel/operators.h"
#include "kernel/array.h"

#include "interned-strings.h"

/**
 * Dao\Mvc\Model\MetaData\Strategy\Annotations
 *
 * Queries the table meta-data in order to instrospect the model's metadata
 */
zend_class_entry *dao_mvc_model_metadata_strategy_annotations_ce;

PHP_METHOD(Dao_Mvc_Model_MetaData_Strategy_Annotations, getMetaData);
PHP_METHOD(Dao_Mvc_Model_MetaData_Strategy_Annotations, getColumnMaps);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_model_metadata_strategy_annotations_getmetadata, 0, 0, 2)
	ZEND_ARG_INFO(0, model)
	ZEND_ARG_INFO(0, dependencyInjector)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_mvc_model_metadata_strategy_annotations_method_entry[] = {
	PHP_ME(Dao_Mvc_Model_MetaData_Strategy_Annotations, getMetaData, arginfo_dao_mvc_model_metadata_strategy_annotations_getmetadata, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_MetaData_Strategy_Annotations, getColumnMaps, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Mvc\Model\MetaData\Strategy\Annotations initializer
 */
DAO_INIT_CLASS(Dao_Mvc_Model_MetaData_Strategy_Annotations){

	DAO_REGISTER_CLASS(Dao\\Mvc\\Model\\MetaData\\Strategy, Annotations, mvc_model_metadata_strategy_annotations, dao_mvc_model_metadata_strategy_annotations_method_entry, 0);

	zend_declare_property_null(dao_mvc_model_metadata_strategy_annotations_ce, SL("_columnMap"), ZEND_ACC_PROTECTED);

	return SUCCESS;
}

/**
 * The meta-data is obtained by reading the column descriptions from the database information schema
 *
 * @param Dao\Mvc\ModelInterface $model
 * @param Dao\DiInterface $dependencyInjector
 * @return array
 */
PHP_METHOD(Dao_Mvc_Model_MetaData_Strategy_Annotations, getMetaData){

	zval *model, *dependency_injector, service = {}, class_name = {}, annotations = {}, reflection = {}, exception_message = {}, properties_annotations = {};
	zval attributes = {}, primary_keys = {}, non_primary_keys = {}, numeric_typed = {}, not_null = {}, field_types = {}, field_sizes = {}, field_bytes = {}, field_scales = {};
	zval field_bind_types = {}, automatic_create_attributes = {}, automatic_update_attributes = {}, field_default_values = {}, identity_field = {};
	zval column_annot_name = {}, primary_annot_name = {}, id_annot_name = {}, column_map_name = {}, column_type_name = {}, column_size_name = {}, column_bytes_name = {};
	zval column_scale_name = {}, column_default_name = {}, column_nullable_name = {}, *prop_annotations;
	zend_string *str_key;
	ulong idx;

	dao_fetch_params(0, 2, 0, &model, &dependency_injector);

	if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_model_exception_ce, "The dependency injector is invalid");
		return;
	}

	ZVAL_STR(&service, IS(annotations));

	dao_get_class(&class_name, model, 0);

	DAO_CALL_METHOD(&annotations, dependency_injector, "get", &service);
	DAO_CALL_METHOD(&reflection, &annotations, "get", &class_name);
	zval_ptr_dtor(&annotations);

	if (Z_TYPE(reflection) != IS_OBJECT) {
		DAO_CONCAT_SV(&exception_message, "No annotations were found in class ", &class_name);
		DAO_THROW_EXCEPTION_ZVAL(dao_mvc_model_exception_ce, &exception_message);
		zval_ptr_dtor(&class_name);
		return;
	}

	/** 
	 * Get the properties defined in 
	 */
	DAO_CALL_METHOD(&properties_annotations, &reflection, "getpropertiesannotations");
	zval_ptr_dtor(&reflection);

	if (Z_TYPE(properties_annotations) != IS_ARRAY || !dao_fast_count_ev(&properties_annotations)) {
		DAO_CONCAT_SV(&exception_message, "No properties with annotations were found in class ", &class_name);
		DAO_THROW_EXCEPTION_ZVAL(dao_mvc_model_exception_ce, &exception_message);
		zval_ptr_dtor(&class_name);
		return;
	}
	zval_ptr_dtor(&class_name);

	/** 
	 * Initialize meta-data
	 */
	array_init(&attributes);
	array_init(&primary_keys);
	array_init(&non_primary_keys);
	array_init(&numeric_typed);
	array_init(&not_null);
	array_init(&field_types);
	array_init(&field_sizes);
	array_init(&field_bytes);
	array_init(&field_scales);
	array_init(&field_bind_types);
	array_init(&automatic_create_attributes);
	array_init(&automatic_update_attributes);
	array_init(&field_default_values);

	ZVAL_FALSE(&identity_field);

	ZVAL_STRING(&column_annot_name, "Column");
	ZVAL_STRING(&primary_annot_name, "Primary");
	ZVAL_STRING(&id_annot_name, "Identity");
	ZVAL_STRING(&column_map_name, "column");
	ZVAL_STRING(&column_type_name, "type");
	ZVAL_STRING(&column_size_name, "size");
	ZVAL_STRING(&column_bytes_name, "bytes");
	ZVAL_STRING(&column_scale_name, "scale");
	ZVAL_STRING(&column_default_name, "default");
	ZVAL_STRING(&column_nullable_name, "nullable");

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(properties_annotations), idx, str_key, prop_annotations) {
		zval property = {}, has_annotation = {}, column_annotation = {}, real_property = {}, feature = {};

		if (str_key) {
			ZVAL_STR(&property, str_key);
		} else {
			ZVAL_LONG(&property, idx);
		}

		/** 
		 * All columns marked with the 'Column' annotation are considered columns
		 */
		DAO_CALL_METHOD(&has_annotation, prop_annotations, "has", &column_annot_name);
		if (!zend_is_true(&has_annotation)) {
			continue;
		}

		/** 
		 * Fetch the 'column' annotation
		 */
		DAO_CALL_METHOD(&column_annotation, prop_annotations, "get", &column_annot_name);

		/** 
		 * Check column map
		 */
		DAO_CALL_METHOD(&real_property, &column_annotation, "getargument", &column_map_name);

		if (DAO_IS_EMPTY(&real_property)) {
			ZVAL_COPY(&real_property, &property);
		}

		/** 
		 * Check if annotation has the 'type' named parameter
		 */
		DAO_CALL_METHOD(&feature, &column_annotation, "getargument", &column_type_name);

		if (DAO_IS_STRING(&feature, "integer")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_INTEGER, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_INT, 0);
			dao_array_update_zval_bool(&numeric_typed, &real_property, 1, 0);
		} else if (DAO_IS_STRING(&feature, "string")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_VARCHAR, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		} else if (DAO_IS_STRING(&feature, "decimal")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_DECIMAL, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_DECIMAL, 0);
			dao_array_update_zval_bool(&numeric_typed, &real_property, 1, PH_COPY);
		} else if (DAO_IS_STRING(&feature, "boolean")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_BOOLEAN, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_BOOL, 0);
		} else if (DAO_IS_STRING(&feature, "datetime")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_DATETIME, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		} else if (DAO_IS_STRING(&feature, "date")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_DATE, PH_COPY);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		} else if (DAO_IS_STRING(&feature, "time")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_TIMESTAMP, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		} else if (DAO_IS_STRING(&feature, "array")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_ARRAY, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		} else if (DAO_IS_STRING(&feature, "intarray")) {
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_ARRAY, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		} else {
			/**
			 * By default all columns are varchar/string
			 */
			dao_array_update_zval_long(&field_types, &real_property, DAO_DB_COLUMN_TYPE_VARCHAR, 0);
			dao_array_update_zval_long(&field_bind_types, &real_property, DAO_DB_COLUMN_BIND_PARAM_STR, 0);
		}
		zval_ptr_dtor(&feature);

		DAO_CALL_METHOD(&feature, &column_annotation, "getargument", &column_size_name);
		if (!DAO_IS_EMPTY(&feature)) {
			dao_array_update(&field_sizes, &real_property, &feature, PH_COPY);
			dao_array_update(&field_bytes, &real_property, &feature, PH_COPY);
			zval_ptr_dtor(&feature);
		}

		DAO_CALL_METHOD(&feature, &column_annotation, "getargument", &column_bytes_name);
		if (!DAO_IS_EMPTY(&feature)) {
			dao_array_update(&field_bytes, &real_property, &feature, PH_COPY);
			zval_ptr_dtor(&feature);
		}

		DAO_CALL_METHOD(&feature, &column_annotation, "getargument", &column_scale_name);
		if (!DAO_IS_EMPTY(&feature)) {
			dao_array_update(&field_scales, &real_property, &feature, PH_COPY);
			zval_ptr_dtor(&feature);
		}

		DAO_CALL_METHOD(&feature, &column_annotation, "getargument", &column_default_name);
		if (!DAO_IS_EMPTY(&feature)) {
			dao_array_update(&field_default_values, &real_property, &feature, PH_COPY);
			zval_ptr_dtor(&feature);
		}

		/** 
		 * All columns marked with the 'Primary' annotation are considered primary keys
		 */
		DAO_CALL_METHOD(&has_annotation, prop_annotations, "has", &primary_annot_name);
		if (zend_is_true(&has_annotation)) {
			dao_array_append(&primary_keys, &real_property, PH_COPY);
			zval_ptr_dtor(&has_annotation);
		} else {
			dao_array_append(&non_primary_keys, &real_property, PH_COPY);
		}

		/** 
		 * All columns marked with the 'Primary' annotation are considered primary keys
		 */
		DAO_CALL_METHOD(&has_annotation, prop_annotations, "has", &id_annot_name);
		if (zend_is_true(&has_annotation)) {
			ZVAL_COPY(&identity_field, &real_property);
			zval_ptr_dtor(&has_annotation);
		}

		/** 
		 * Check if the column 
		 */
		DAO_CALL_METHOD(&feature, &column_annotation, "getargument", &column_nullable_name);
		if (!zend_is_true(&feature)) {
			dao_array_append(&not_null, &real_property, PH_COPY);
			zval_ptr_dtor(&feature);
		}

		dao_array_append(&attributes, &real_property, 0);
		zval_ptr_dtor(&column_annotation);
	} ZEND_HASH_FOREACH_END();
	zval_ptr_dtor(&properties_annotations);

	zval_ptr_dtor(&column_annot_name);
	zval_ptr_dtor(&primary_annot_name);
	zval_ptr_dtor(&id_annot_name);
	zval_ptr_dtor(&column_map_name);
	zval_ptr_dtor(&column_type_name);
	zval_ptr_dtor(&column_size_name);
	zval_ptr_dtor(&column_bytes_name);
	zval_ptr_dtor(&column_scale_name);
	zval_ptr_dtor(&column_default_name);
	zval_ptr_dtor(&column_nullable_name);
	
	/** 
	 * Create an array using the MODELS_* constants as indexes
	 */
	array_init_size(return_value, 14);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_ATTRIBUTES, &attributes, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_PRIMARY_KEY, &primary_keys, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_NON_PRIMARY_KEY, &non_primary_keys, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_NOT_NULL, &not_null, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_TYPES, &field_types, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_TYPES_NUMERIC, &numeric_typed, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_IDENTITY_COLUMN, &identity_field, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_TYPES_BIND, &field_bind_types, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_AUTOMATIC_DEFAULT_INSERT, &automatic_create_attributes, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_AUTOMATIC_DEFAULT_UPDATE, &automatic_update_attributes, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_DEFAULT_VALUE, &field_default_values, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_SZIE, &field_sizes, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_SCALE, &field_scales, 0);
	dao_array_update_long(return_value, DAO_MVC_MODEL_METADATA_MODELS_DATA_BYTE, &field_bytes, 0);
}

/**
 * Read the model's column map, this can't be inferred
 *
 * @param Dao\Mvc\ModelInterface $model
 * @param Dao\DiInterface $dependencyInjector
 * @return array
 */
PHP_METHOD(Dao_Mvc_Model_MetaData_Strategy_Annotations, getColumnMaps){

	zval *model, *dependency_injector, service = {}, annotations = {}, class_name = {}, reflection = {}, properties_annotations = {}, exception_message = {};
	zval ordered_column_map = {}, reversed_column_map = {}, column_annot_name = {}, column_map_name = {}, *prop_annotations, *column_name;
	zend_string *str_key;
	ulong idx;

	dao_fetch_params(0, 2, 0, &model, &dependency_injector);

	if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_model_exception_ce, "The dependency injector is invalid");
		return;
	}

	ZVAL_STR(&service, IS(annotations));

	DAO_CALL_METHOD(&annotations, dependency_injector, "get", &service);

	dao_get_class(&class_name, model, 0);

	DAO_CALL_METHOD(&reflection, &annotations, "get", &class_name);
	zval_ptr_dtor(&annotations);
	if (Z_TYPE(reflection) != IS_OBJECT) {
		DAO_CONCAT_SV(&exception_message, "No annotations were found in class ", &class_name);
		DAO_THROW_EXCEPTION_ZVAL(dao_mvc_model_exception_ce, &exception_message);
		return;
	}

	/** 
	 * Get the properties defined in 
	 */
	DAO_CALL_METHOD(&properties_annotations, &reflection, "getpropertiesannotations");
	if (Z_TYPE(properties_annotations) != IS_ARRAY || !dao_fast_count_ev(&properties_annotations)) {
		DAO_CONCAT_SV(&exception_message, "No properties with annotations were found in class ", &class_name);
		DAO_THROW_EXCEPTION_ZVAL(dao_mvc_model_exception_ce, &exception_message);
		return;
	}
	zval_ptr_dtor(&reflection);

	/** 
	 * Check for a columnMap() method on the model
	 */
	if (dao_method_exists_ex(model, SL("columnmap")) == SUCCESS) {
		DAO_CALL_METHOD(&ordered_column_map, model, "columnmap");
		if (Z_TYPE(ordered_column_map) != IS_ARRAY) { 
			DAO_THROW_EXCEPTION_STR(dao_mvc_model_exception_ce, "columnMap() not returned an array");
			return;
		}
	} else {
		array_init(&ordered_column_map);
	}
	array_init(&reversed_column_map);

	ZVAL_STRING(&column_annot_name, "Column");
	ZVAL_STRING(&column_map_name, "column");

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(properties_annotations), idx, str_key, prop_annotations) {
		zval property = {}, has_annotation = {}, column_annotation = {}, real_property = {};

		if (str_key) {
			ZVAL_STR(&property, str_key);
		} else {
			ZVAL_LONG(&property, idx);
		}

		/** 
		 * All columns marked with the 'Column' annotation are considered columns
		 */
		DAO_CALL_METHOD(&has_annotation, prop_annotations, "has", &column_annot_name);
		if (!zend_is_true(&has_annotation)) {
			continue;
		}

		/** 
		 * Fetch the 'column' annotation
		 */
		DAO_CALL_METHOD(&column_annotation, prop_annotations, "get", &column_annot_name);

		/** 
		 * Check column map
		 */
		DAO_CALL_METHOD(&real_property, &column_annotation, "getargument", &column_map_name);
		zval_ptr_dtor(&column_annotation);
		if (DAO_IS_NOT_EMPTY(&real_property)) {
			if (!dao_array_isset(&ordered_column_map, &real_property)) {
				dao_array_update(&ordered_column_map, &real_property, &property, PH_COPY);
			}
			zval_ptr_dtor(&real_property);
		} else {
			if (!dao_array_isset(&ordered_column_map, &property)) {
				dao_array_update(&ordered_column_map, &property, &property, PH_COPY);
			}
			dao_array_update(&ordered_column_map, &property, &property, PH_COPY);
		}
	} ZEND_HASH_FOREACH_END();
	zval_ptr_dtor(&column_annot_name);
	zval_ptr_dtor(&column_map_name);
	zval_ptr_dtor(&properties_annotations);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(ordered_column_map), idx, str_key, column_name) {
		zval name = {};
		if (str_key) {
			ZVAL_STR(&name, str_key);
		} else {
			ZVAL_LONG(&name, idx);
		}
		dao_array_update(&reversed_column_map, column_name, &name, PH_COPY);
	} ZEND_HASH_FOREACH_END();

	array_init_size(return_value, 2);
	dao_array_update_long(return_value, 0, &ordered_column_map, 0);
	dao_array_update_long(return_value, 1, &reversed_column_map, 0);
}

