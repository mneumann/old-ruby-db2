rb_define_const(mDB2CLI, "SQL_SUCCESS", INT2NUM(SQL_SUCCESS));
rb_define_const(mDB2CLI, "SQL_SUCCESS_WITH_INFO", INT2NUM(SQL_SUCCESS_WITH_INFO));
rb_define_const(mDB2CLI, "SQL_INVALID_HANDLE", INT2NUM(SQL_INVALID_HANDLE));
rb_define_const(mDB2CLI, "SQL_STILL_EXECUTING", INT2NUM(SQL_STILL_EXECUTING));
rb_define_const(mDB2CLI, "SQL_ERROR", INT2NUM(SQL_ERROR));
rb_define_const(mDB2CLI, "SQL_NO_DATA_FOUND", INT2NUM(SQL_NO_DATA_FOUND));
rb_define_const(mDB2CLI, "SQL_NULL_DATA", INT2NUM(SQL_NULL_DATA));
rb_define_const(mDB2CLI, "SQL_HANDLE_ENV", INT2NUM(SQL_HANDLE_ENV));
rb_define_const(mDB2CLI, "SQL_HANDLE_DBC", INT2NUM(SQL_HANDLE_DBC));
rb_define_const(mDB2CLI, "SQL_HANDLE_STMT", INT2NUM(SQL_HANDLE_STMT));
rb_define_const(mDB2CLI, "SQL_HANDLE_DESC", INT2NUM(SQL_HANDLE_DESC));
rb_define_const(mDB2CLI, "SQL_NULL_HANDLE", INT2NUM(SQL_NULL_HANDLE));
rb_define_const(mDB2CLI, "SQL_NULL_HENV", INT2NUM(SQL_NULL_HENV));
rb_define_const(mDB2CLI, "SQL_NULL_HDBC", INT2NUM(SQL_NULL_HDBC));
rb_define_const(mDB2CLI, "SQL_NULL_HSTMT", INT2NUM(SQL_NULL_HSTMT));
rb_define_const(mDB2CLI, "SQL_NULL_HDESC", INT2NUM(SQL_NULL_HDESC));
rb_define_const(mDB2CLI, "SQL_NO_NULLS", INT2NUM(SQL_NO_NULLS));
rb_define_const(mDB2CLI, "SQL_NULLABLE", INT2NUM(SQL_NULLABLE));
rb_define_const(mDB2CLI, "SQL_COMMIT", INT2NUM(SQL_COMMIT));
rb_define_const(mDB2CLI, "SQL_ROLLBACK", INT2NUM(SQL_ROLLBACK));
rb_define_const(mDB2CLI, "SQL_TRUE", INT2NUM(SQL_TRUE));
rb_define_const(mDB2CLI, "SQL_FALSE", INT2NUM(SQL_FALSE));
rb_define_const(mDB2CLI, "SQL_MAX_DSN_LENGTH", INT2NUM(SQL_MAX_DSN_LENGTH));
rb_define_const(mDB2CLI, "SQL_BIGINT", INT2NUM(SQL_BIGINT));
rb_define_const(mDB2CLI, "SQL_BLOB", INT2NUM(SQL_BLOB));
rb_define_const(mDB2CLI, "SQL_BLOB_LOCATOR", INT2NUM(SQL_BLOB_LOCATOR));
rb_define_const(mDB2CLI, "SQL_CHAR", INT2NUM(SQL_CHAR));
rb_define_const(mDB2CLI, "SQL_BINARY", INT2NUM(SQL_BINARY));
rb_define_const(mDB2CLI, "SQL_CLOB", INT2NUM(SQL_CLOB));
rb_define_const(mDB2CLI, "SQL_CLOB_LOCATOR", INT2NUM(SQL_CLOB_LOCATOR));
rb_define_const(mDB2CLI, "SQL_TYPE_DATE", INT2NUM(SQL_TYPE_DATE));
rb_define_const(mDB2CLI, "SQL_DBCLOB", INT2NUM(SQL_DBCLOB));
rb_define_const(mDB2CLI, "SQL_DBCLOB_LOCATOR", INT2NUM(SQL_DBCLOB_LOCATOR));
rb_define_const(mDB2CLI, "SQL_DECIMAL", INT2NUM(SQL_DECIMAL));
rb_define_const(mDB2CLI, "SQL_DOUBLE", INT2NUM(SQL_DOUBLE));
rb_define_const(mDB2CLI, "SQL_FLOAT", INT2NUM(SQL_FLOAT));
rb_define_const(mDB2CLI, "SQL_GRAPHIC", INT2NUM(SQL_GRAPHIC));
rb_define_const(mDB2CLI, "SQL_INTEGER", INT2NUM(SQL_INTEGER));
rb_define_const(mDB2CLI, "SQL_LONGVARCHAR", INT2NUM(SQL_LONGVARCHAR));
rb_define_const(mDB2CLI, "SQL_LONGVARBINARY", INT2NUM(SQL_LONGVARBINARY));
rb_define_const(mDB2CLI, "SQL_LONGVARGRAPHIC", INT2NUM(SQL_LONGVARGRAPHIC));
rb_define_const(mDB2CLI, "SQL_NUMERIC", INT2NUM(SQL_NUMERIC));
rb_define_const(mDB2CLI, "SQL_REAL", INT2NUM(SQL_REAL));
rb_define_const(mDB2CLI, "SQL_SMALLINT", INT2NUM(SQL_SMALLINT));
rb_define_const(mDB2CLI, "SQL_TYPE_TIME", INT2NUM(SQL_TYPE_TIME));
rb_define_const(mDB2CLI, "SQL_TYPE_TIMESTAMP", INT2NUM(SQL_TYPE_TIMESTAMP));
rb_define_const(mDB2CLI, "SQL_VARCHAR", INT2NUM(SQL_VARCHAR));
rb_define_const(mDB2CLI, "SQL_VARBINARY", INT2NUM(SQL_VARBINARY));
rb_define_const(mDB2CLI, "SQL_VARGRAPHIC", INT2NUM(SQL_VARGRAPHIC));
rb_define_const(mDB2CLI, "SQL_FETCH_NEXT", INT2NUM(SQL_FETCH_NEXT));
rb_define_const(mDB2CLI, "SQL_FETCH_PRIOR", INT2NUM(SQL_FETCH_PRIOR));
rb_define_const(mDB2CLI, "SQL_FETCH_RELATIVE", INT2NUM(SQL_FETCH_RELATIVE));
rb_define_const(mDB2CLI, "SQL_FETCH_ABSOLUTE", INT2NUM(SQL_FETCH_ABSOLUTE));
rb_define_const(mDB2CLI, "SQL_FETCH_FIRST", INT2NUM(SQL_FETCH_FIRST));
rb_define_const(mDB2CLI, "SQL_FETCH_LAST", INT2NUM(SQL_FETCH_LAST));
rb_define_const(mDB2CLI, "SQL_FETCH_BOOKMARK", INT2NUM(SQL_FETCH_BOOKMARK));
rb_define_const(mDB2CLI, "SQL_CLOSE", INT2NUM(SQL_CLOSE));
rb_define_const(mDB2CLI, "SQL_DROP", INT2NUM(SQL_DROP));
rb_define_const(mDB2CLI, "SQL_UNBIND", INT2NUM(SQL_UNBIND));
rb_define_const(mDB2CLI, "SQL_RESET_PARAMS", INT2NUM(SQL_RESET_PARAMS));
rb_define_const(mDB2CLI, "SQL_DESC_AUTO_UNIQUE_VALUE", INT2NUM(SQL_DESC_AUTO_UNIQUE_VALUE));
rb_define_const(mDB2CLI, "SQL_DESC_CASE_SENSITIVE", INT2NUM(SQL_DESC_CASE_SENSITIVE));
rb_define_const(mDB2CLI, "SQL_DESC_CATALOG_NAME", INT2NUM(SQL_DESC_CATALOG_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_COUNT", INT2NUM(SQL_DESC_COUNT));
rb_define_const(mDB2CLI, "SQL_DESC_DISPLAY_SIZE", INT2NUM(SQL_DESC_DISPLAY_SIZE));
rb_define_const(mDB2CLI, "SQL_DESC_LABEL", INT2NUM(SQL_DESC_LABEL));
rb_define_const(mDB2CLI, "SQL_DESC_DISTINCT_TYPE", INT2NUM(SQL_DESC_DISTINCT_TYPE));
rb_define_const(mDB2CLI, "SQL_DESC_OCTET_LENGTH", INT2NUM(SQL_DESC_OCTET_LENGTH));
rb_define_const(mDB2CLI, "SQL_DESC_FIXED_PREC_SCALE", INT2NUM(SQL_DESC_FIXED_PREC_SCALE));
rb_define_const(mDB2CLI, "SQL_DESC_NAME", INT2NUM(SQL_DESC_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_NULLABLE", INT2NUM(SQL_DESC_NULLABLE));
rb_define_const(mDB2CLI, "SQL_DESC_SCHEMA_NAME", INT2NUM(SQL_DESC_SCHEMA_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_PRECISION", INT2NUM(SQL_DESC_PRECISION));
rb_define_const(mDB2CLI, "SQL_DESC_SCALE", INT2NUM(SQL_DESC_SCALE));
rb_define_const(mDB2CLI, "SQL_DESC_SEARCHABLE", INT2NUM(SQL_DESC_SEARCHABLE));
rb_define_const(mDB2CLI, "SQL_DESC_TABLE_NAME", INT2NUM(SQL_DESC_TABLE_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_TYPE", INT2NUM(SQL_DESC_TYPE));
rb_define_const(mDB2CLI, "SQL_DESC_TYPE_NAME", INT2NUM(SQL_DESC_TYPE_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_UNSIGNED", INT2NUM(SQL_DESC_UNSIGNED));
rb_define_const(mDB2CLI, "SQL_DESC_UPDATABLE", INT2NUM(SQL_DESC_UPDATABLE));
rb_define_const(mDB2CLI, "SQL_DESC_BASE_COLUMN_NAME", INT2NUM(SQL_DESC_BASE_COLUMN_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_BASE_TABLE_NAME", INT2NUM(SQL_DESC_BASE_TABLE_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_CONCISE_TYPE", INT2NUM(SQL_DESC_CONCISE_TYPE));
rb_define_const(mDB2CLI, "SQL_DESC_LENGTH", INT2NUM(SQL_DESC_LENGTH));
rb_define_const(mDB2CLI, "SQL_DESC_LITERAL_PREFIX", INT2NUM(SQL_DESC_LITERAL_PREFIX));
rb_define_const(mDB2CLI, "SQL_DESC_LITERAL_SUFFIX", INT2NUM(SQL_DESC_LITERAL_SUFFIX));
rb_define_const(mDB2CLI, "SQL_DESC_LOCAL_TYPE_NAME", INT2NUM(SQL_DESC_LOCAL_TYPE_NAME));
rb_define_const(mDB2CLI, "SQL_DESC_NUM_PREC_RADIX", INT2NUM(SQL_DESC_NUM_PREC_RADIX));
rb_define_const(mDB2CLI, "SQL_DESC_UNNAMED", INT2NUM(SQL_DESC_UNNAMED));

rb_define_const(mDB2CLI, "SQL_WLONGVARCHAR", INT2NUM(SQL_WLONGVARCHAR));
rb_define_const(mDB2CLI, "SQL_TINYINT", INT2NUM(SQL_TINYINT));
rb_define_const(mDB2CLI, "SQL_BIT", INT2NUM(SQL_BIT));
rb_define_const(mDB2CLI, "SQL_WVARCHAR", INT2NUM(SQL_WVARCHAR));
rb_define_const(mDB2CLI, "SQL_DATALINK", INT2NUM(SQL_DATALINK));
rb_define_const(mDB2CLI, "SQL_WCHAR", INT2NUM(SQL_WCHAR));

rb_define_const(mDB2CLI, "SQL_ATTR_AUTOCOMMIT", INT2NUM(SQL_ATTR_AUTOCOMMIT));
rb_define_const(mDB2CLI, "SQL_AUTOCOMMIT_ON", INT2NUM(SQL_AUTOCOMMIT_ON));
rb_define_const(mDB2CLI, "SQL_AUTOCOMMIT_OFF", INT2NUM(SQL_AUTOCOMMIT_OFF));
