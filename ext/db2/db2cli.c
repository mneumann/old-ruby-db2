/*
 * IBM DB2 CLI 5.0 (Call Level Interface) Module for Ruby
 *
 * File:   db2cli.c
 *
 * Author: Michael Neumann (mneumann@ntecs.de)
 *
 * Contributors: 
 *
 *   Songsu Yun (yuns@us.ibm.com)
 *
 *   Stephen R. Veit (sveit@tradeharbor.com)
 *
 *
 * Copyright (c) 2001, 2002, 2003 by Michael Neumann.
 *
 * Released under the same terms as Ruby itself.
 *
 * $Id: db2cli.c,v 1.4 2003/11/22 16:56:46 mneumann Exp $
 *
 */



/**********************************************************************
 Description:
 ======================================================================
 - SQLRETURN is always returned as Ruby-Integer (Fixnum or Bignum) 
 - any SQLHANDLE-type (SQLHENV, SQLHDBC, SQLHSTMT, SQLHDESC) is given
   or returned as Ruby-Integer (Fixnum or Bignum)
 - SQLSMALLINT etc. are all Ruby-Integer
**********************************************************************/

/**********************************************************************
 Strange Things:
 ======================================================================
 - after allocating an Environment-Handle "p 5.7" gives "5,7.0" on my
   system (in Germany correct would be "5,7"). Fault of Linux ????
**********************************************************************/



/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "sqlcli1.h"
#include "ruby.h"



/* Macros */
#define TO_C_INT(val)    NUM2INT(val)
#define TO_RUBY_INT(val) INT2NUM(val)
#define MUST_BE_STRING(val) Check_Type(val, T_STRING)

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* Global Variables */
static VALUE mDB2CLI;
static VALUE cDate, cTime, cTimestamp;
static VALUE objNull; /* object for a SQL NULL value */



/* Functions */


/*******************************************************
 SQLAllocHandle
 =======================================================
 PARAMS:   handle_type, input_handle : Integer
 RETURNS:  rc, output_handle : Integer
********************************************************/

static VALUE
db2_SQLAllocHandle(self, handle_type, input_handle)
  VALUE self;
  VALUE handle_type, input_handle;
{
  SQLRETURN rc;
  SQLHANDLE output_handle;

  rc = SQLAllocHandle(
    (SQLSMALLINT) TO_C_INT(handle_type),
    (SQLHANDLE)   TO_C_INT(input_handle),
    (SQLHANDLE*)  &output_handle
  );

  return rb_ary_new3(2, TO_RUBY_INT(rc), TO_RUBY_INT(output_handle));
}


/*******************************************************
 SQLFreeHandle
 =======================================================
 PARAMS:   handle_type, handle : Integer
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLFreeHandle(self, handle_type, handle)
  VALUE self;
  VALUE handle_type, handle;
{
  SQLRETURN rc;

  rc = SQLFreeHandle(
    (SQLSMALLINT) TO_C_INT(handle_type),
    (SQLHANDLE)   TO_C_INT(handle)
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLDataSources
 =======================================================
 PARAMS:   environment_handle : Integer,
           direction          : Integer,
           server_name_length : Integer,   (buffer-length)
           description_length : Integer    (buffer-length)

 RETURNS:  rc                 : Integer,
           server_name        : String,
           server_name_length : Integer,   (bytes available)
           description        : String,
           description_length : Integer    (bytes available)

********************************************************/

static VALUE
db2_SQLDataSources(self, environment_handle, direction,
                   server_name_length, description_length)
  VALUE self;
  VALUE environment_handle, direction;
  VALUE server_name_length, description_length;
{
  SQLRETURN rc;
  SQLCHAR*    server_name;
  SQLCHAR*    description;
  SQLSMALLINT sl;          /* server_name_length      */
  SQLSMALLINT dl;          /* description_length      */
  SQLSMALLINT sn_length;   /* real server_name_length */
  SQLSMALLINT ds_length;   /* real description_length */
  VALUE       retval;

  sl = TO_C_INT(server_name_length);
  dl = TO_C_INT(description_length);

  server_name = (SQLCHAR*) ALLOC_N(SQLCHAR, sl);
  description = (SQLCHAR*) ALLOC_N(SQLCHAR, dl);

  rc = SQLDataSources(
    (SQLHENV)      TO_C_INT(environment_handle),
    (SQLUSMALLINT) TO_C_INT(direction),
    (SQLCHAR*)     server_name,
    (SQLSMALLINT)  sl,
    (SQLSMALLINT*) &sn_length,

    (SQLCHAR*)     description,
    (SQLSMALLINT)  dl,
    (SQLSMALLINT*) &ds_length
 );

  if (rc != 0) {                  /* added by yun */
     sn_length = 0;
     ds_length = 0;
  }

  /* Remove null-termination if it exists. Added by sveit. */
  if (sn_length > 0 && server_name[sn_length - 1] == 0)
      sn_length--;
  if (ds_length > 0 && description[ds_length - 1] == 0)
      ds_length--;

  retval = rb_ary_new3(
    5,
    TO_RUBY_INT(rc),
    rb_str_new(server_name, MIN(sl, sn_length)),
    TO_RUBY_INT(sn_length),
    rb_str_new(description, MIN(dl, ds_length)),
    TO_RUBY_INT(ds_length)
  );

  free((void*)description);
  free((void*)server_name);

  return retval;
}


/*******************************************************
 SQLConnect
 =======================================================
 PARAMS:   connection_handle : Integer,
           server_name, user_name, auth : String
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLConnect(self, connection_handle, server_name, user_name, auth)
  VALUE self;
  VALUE connection_handle, server_name, user_name, auth;
{
  SQLRETURN rc;

  MUST_BE_STRING(server_name);
  MUST_BE_STRING(user_name);
  MUST_BE_STRING(auth);

  rc = SQLConnect(
    (SQLHDBC)      TO_C_INT(connection_handle),
    (SQLCHAR *FAR) RSTRING(server_name)->ptr,
    (SQLSMALLINT)  RSTRING(server_name)->len,
    (SQLCHAR *FAR) RSTRING(user_name)->ptr,
    (SQLSMALLINT)  RSTRING(user_name)->len,
    (SQLCHAR *FAR) RSTRING(auth)->ptr,
    (SQLSMALLINT)  RSTRING(auth)->len
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLSetConnectAttr     added by yun
 =======================================================
 PARAMS:   connection_handle : Integer,
           attribute:          integer
           value:              either string or integer
 RETURNS:  rc : Integer
 NOTE: When passing a string as value, the string might 
       have to be null-terminated.
********************************************************/

static VALUE
db2_SQLSetConnectAttr(self, connection_handle, attribute, value)
  VALUE self;
  VALUE connection_handle, attribute, value;
{
  SQLRETURN rc;

  if (TYPE(value) == T_STRING) {
     rc = SQLSetConnectAttr(
       (SQLHDBC)      TO_C_INT(connection_handle),
       (SQLINTEGER)   TO_C_INT(attribute),
       (SQLPOINTER)   RSTRING(value)->ptr,
       (SQLINTEGER)   RSTRING(value)->len
     );
  } else {
     rc = SQLSetConnectAttr(
       (SQLHDBC)      TO_C_INT(connection_handle),
       (SQLINTEGER)   TO_C_INT(attribute),
       (SQLPOINTER)   TO_C_INT(value),
       (SQLINTEGER)   SQL_IS_INTEGER
     );
  }

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLDisconnect
 =======================================================
 PARAMS:   connection_handle : Integer
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLDisconnect(self, connection_handle)
  VALUE self;
  VALUE connection_handle;
{
  SQLRETURN rc;

  rc = SQLDisconnect(
    (SQLHDBC) TO_C_INT(connection_handle)
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLPrepare
 =======================================================
 PARAMS:   statement_handle : Integer,
           statement_text : String
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLPrepare(self, statement_handle, statement_text)
  VALUE self;
  VALUE statement_handle, statement_text;
{
  SQLRETURN rc;

  MUST_BE_STRING(statement_text);

  rc = SQLPrepare(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLCHAR *FAR) RSTRING(statement_text)->ptr,
    (SQLINTEGER)   RSTRING(statement_text)->len
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLNumResultCols
 =======================================================
 PARAMS:   statement_handle : Integer
 RETURNS:  rc, column_count : Integer
********************************************************/

static VALUE
db2_SQLNumResultCols(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN   rc;
  SQLSMALLINT column_count;

  rc = SQLNumResultCols(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLSMALLINT*) &column_count
  );

  return rb_ary_new3(2, TO_RUBY_INT(rc), TO_RUBY_INT(column_count));
}

/*******************************************************
 SQLNumParams                            added by yun
 =======================================================
 PARAMS:   statement_handle : Integer
 RETURNS:  rc, param_count : Integer
********************************************************/

static VALUE
db2_SQLNumParams(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN   rc;
  SQLSMALLINT param_count;

  rc = SQLNumParams(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLSMALLINT*) &param_count
  );

  return rb_ary_new3(2, TO_RUBY_INT(rc), TO_RUBY_INT(param_count));
}


/*******************************************************
 SQLDescribeCol
 =======================================================
 PARAMS:   statement_handle : Integer,
           column_number    : Integer,
           buffer_length    : Integer (for column_name)

 RETURNS:  rc               : Integer,
           column_name      : String | nil,
           name_length      : Integer,
           data_type        : Integer,
           column_size      : Integer,
           decimal_digits   : Integer,
           nullable         : Integer
********************************************************/

static VALUE
db2_SQLDescribeCol(self, statement_handle, column_number, buffer_length)
  VALUE self;
  VALUE statement_handle, column_number, buffer_length;
{
  SQLRETURN rc;
  SQLCHAR*  colname_ptr;
  SQLSMALLINT name_length, data_type, decimal_digits, nullable;
  SQLUINTEGER column_size;
  SQLSMALLINT bl;  /* buffer_length */

  VALUE retval;

  bl          = TO_C_INT(buffer_length);
  colname_ptr = (SQLCHAR*) ALLOC_N(SQLCHAR, bl);

  rc = SQLDescribeCol(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLUSMALLINT) TO_C_INT(column_number),
    (SQLCHAR *)    colname_ptr,
    (SQLSMALLINT)  bl,
    (SQLSMALLINT*) &name_length,
    (SQLSMALLINT*) &data_type,
    (SQLUINTEGER*) &column_size,
    (SQLSMALLINT*) &decimal_digits,
    (SQLSMALLINT*) &nullable
  );


  retval = rb_ary_new3(
    7,
    TO_RUBY_INT(rc),
    colname_ptr == NULL ? Qnil : rb_str_new((const char *)colname_ptr, MIN(name_length, bl)),
    TO_RUBY_INT(name_length),
    TO_RUBY_INT(data_type),
    TO_RUBY_INT(column_size),
    TO_RUBY_INT(decimal_digits),
    TO_RUBY_INT(nullable)
  );

  free(colname_ptr);

  return retval;
}


/*******************************************************
 SQLDescribeParam                added by yun
 =======================================================
 PARAMS:   statement_handle : Integer,
           parameter_number : Integer,

 RETURNS:  rc               : Integer,
           parameter data type   : Integer,
           parameter size   : Integer,
           decimal_digits   : Integer,
           nullable         : Integer
********************************************************/

static VALUE
db2_SQLDescribeParam(self, statement_handle, param_number)
  VALUE self;
  VALUE statement_handle, param_number;
{
  SQLRETURN rc;
  SQLSMALLINT data_type, decimal_digits, nullable;
  SQLUINTEGER param_size;

  VALUE retval;

  rc = SQLDescribeParam(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLUSMALLINT) TO_C_INT(param_number),
    (SQLSMALLINT*) &data_type,
    (SQLUINTEGER*) &param_size,
    (SQLSMALLINT*) &decimal_digits,
    (SQLSMALLINT*) &nullable
  );


  retval = rb_ary_new3(
    5,
    TO_RUBY_INT(rc),
    TO_RUBY_INT(data_type),
    TO_RUBY_INT(param_size),
    TO_RUBY_INT(decimal_digits),
    TO_RUBY_INT(nullable)
  );

  return retval;
}

/*******************************************************
 SQLColAttribute
 =======================================================
 PARAMS:   statement_handle    : Integer,
           column_number       : Integer,
           field_identifier    : Integer,
           buffer_length       : Integer | nil (nil == numeric_attribute)

 RETURNS:  if numeric_attribute (buffer_length==nil):
             rc                  : Integer,
             numeric_attribute   : Integer
           elsif character_attribute (buffer_length != nil):
             rc                  : Integer,
             character_attribute : String,
             string_length       : Integer

********************************************************/

static VALUE
db2_SQLColAttribute(self, statement_handle, column_number, field_identifier, buffer_length)
  VALUE self;
  VALUE statement_handle, column_number, field_identifier, buffer_length;
{
  SQLRETURN   rc;
  SQLSMALLINT bl;  /* buffer_length */
  SQLPOINTER  character_attr_ptr;
  SQLSMALLINT string_length;
  signed long numeric_attribute;
  VALUE retval;


  if (NIL_P(buffer_length) != 0) {
    /* numeric_attribute */
    bl = 0;
    character_attr_ptr = NULL;
  }
  else {
    /* character_attribute */
    bl = TO_C_INT(buffer_length);
    character_attr_ptr = (SQLPOINTER) ALLOC_N(char, bl);
  }

  rc = SQLColAttribute(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLUSMALLINT) TO_C_INT(column_number),
    (SQLSMALLINT)  TO_C_INT(field_identifier),
    (SQLPOINTER)   character_attr_ptr,
    (SQLSMALLINT)  bl,
    (SQLSMALLINT*) &string_length,
    (SQLPOINTER)   &numeric_attribute
  );


  if (NIL_P(buffer_length) != 0) {
    /* numeric_attribute */
    retval = rb_ary_new3(
      2,
      TO_RUBY_INT(rc),
      TO_RUBY_INT(numeric_attribute)
    );
  }
  else {
    /* character_attribute */
    retval = rb_ary_new3(
      3,
      TO_RUBY_INT(rc),
      rb_str_new(character_attr_ptr, MIN(string_length, bl)),
      TO_RUBY_INT(string_length)
    );
    free(character_attr_ptr);
  }

  return retval;
}


/*******************************************************
 SQLExecDirect
 =======================================================
 PARAMS:   statement_handle : Integer,
           statement_text : String
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLExecDirect(self, statement_handle, statement_text)
  VALUE self;
  VALUE statement_handle, statement_text;
{
  SQLRETURN rc;

  MUST_BE_STRING(statement_text);

  rc = SQLExecDirect(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLCHAR *FAR) RSTRING(statement_text)->ptr,
    (SQLINTEGER)   RSTRING(statement_text)->len
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLExecute
 =======================================================
 PARAMS:   statement_handle : Integer
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLExecute(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN rc;

  rc = SQLExecute(
    (SQLHSTMT) TO_C_INT(statement_handle)
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLFreeStmt                          added by yun
 =======================================================
 PARAMS:   statement_handle : Integer
           option: option
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLFreeStmt(self, statement_handle, option)
  VALUE self;
  VALUE statement_handle;
  VALUE option;
{
  SQLRETURN rc;

  rc = SQLFreeStmt(
    (SQLHSTMT) TO_C_INT(statement_handle),
    (SQLUSMALLINT) TO_C_INT(option)
  );

  return TO_RUBY_INT(rc);
}

/*******************************************************
 SQLCloseCursor                      added by yun
 =======================================================
 PARAMS:   statement_handle : Integer
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLCloseCursor(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN rc;

  rc = SQLCloseCursor(
    (SQLHSTMT) TO_C_INT(statement_handle)
  );

  return TO_RUBY_INT(rc);
}

/*******************************************************
 SQLRowCount
 =======================================================
 PARAMS:   statement_handle : Integer
 RETURNS:  rc, row_count : Integer
********************************************************/

static VALUE
db2_SQLRowCount(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN  rc;
  SQLINTEGER row_count;

  rc = SQLRowCount(
    (SQLHSTMT)    TO_C_INT(statement_handle),
    (SQLINTEGER*) &row_count
  );

  return rb_ary_new3(2, TO_RUBY_INT(rc), TO_RUBY_INT(row_count));
}


/*******************************************************
 SQLFetch
 =======================================================
 PARAMS:   statement_handle : Integer
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLFetch(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN rc;

  rc = SQLFetch(
    (SQLHSTMT) TO_C_INT(statement_handle)
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLFetchScroll
 =======================================================
 PARAMS:   statement_handle  : Integer,
           fetch_orientation : Integer,
           fetch_offset      : Integer

 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLFetchScroll(self, statement_handle, fetch_orientation, fetch_offset)
  VALUE self;
  VALUE statement_handle, fetch_orientation, fetch_offset;
{
  SQLRETURN rc;

  rc = SQLFetchScroll(
    (SQLHSTMT)    TO_C_INT(statement_handle),
    (SQLSMALLINT) TO_C_INT(fetch_orientation),
    (SQLINTEGER)  TO_C_INT(fetch_offset)
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLGetData
 =======================================================
 PARAMS:   statement_handle    : Integer,
           column_number       : Integer,
           target_type         : Integer,    (e.g. SQL_BLOB)
           buffer_length = nil : Integer     (nil for e.g. SQL_INTEGER)

 RETURNS:  rc                  : Integer,
           column              : ?,
           strlen_or_indptr    :
********************************************************/

static VALUE
db2_SQLGetData(argc, argv, self)
  int argc;
  VALUE *argv;
  VALUE self;
{
  VALUE statement_handle, column_number, target_type, buffer_length;

  SQLRETURN  rc;
  SQLINTEGER strlen_or_indptr;
  SQLHSTMT sh;     /* statement_handle */
  SQLUSMALLINT cn; /* column_number    */
  SQLINTEGER bl;   /* buffer_length    */
  VALUE retval;

  union {
    SQLDOUBLE        dbl;
    SQLREAL          real;
    SQLINTEGER       integer;
    SQLSMALLINT      smallint;
    DATE_STRUCT      date;
    TIME_STRUCT      time;
    TIMESTAMP_STRUCT timestamp;
  } ptr_value;
  SQLPOINTER ptr;


  rb_scan_args(argc, argv, "31", &statement_handle, &column_number,
               &target_type, &buffer_length);

  sh = TO_C_INT(statement_handle);
  cn = TO_C_INT(column_number);

  if (NIL_P(buffer_length) != 0) {
    bl = 0;
  }
  else {
    bl = TO_C_INT(buffer_length);
  }


  #define CALL_SQL_GET_DATA(ptr, type, len)  \
    rc = SQLGetData(                         \
      (SQLHSTMT)     sh,                     \
      (SQLUSMALLINT) cn,                     \
      (SQLSMALLINT)  type,                   \
      (SQLPOINTER)   ptr,                    \
      (SQLINTEGER)   len,                    \
      (SQLINTEGER*)  &strlen_or_indptr       \
    );

  #define RETVAL(val)                                        \
    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)    \
      retval =                                               \
        (strlen_or_indptr == SQL_NULL_DATA) ? objNull : val; \
    else                                                     \
      retval = Qnil;                                         \



  switch (TO_C_INT(target_type)) {
    case SQL_DOUBLE:
    case SQL_FLOAT:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_DOUBLE, sizeof(SQLDOUBLE));
      RETVAL( rb_float_new(ptr_value.dbl) );
      break;

    case SQL_REAL:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_FLOAT, sizeof(SQLREAL));
      RETVAL( rb_float_new(ptr_value.real) );
      break;

    case SQL_INTEGER:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_LONG, sizeof(SQLINTEGER));
      RETVAL( TO_RUBY_INT(ptr_value.integer) );
      break;

    case SQL_SMALLINT:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_SHORT, sizeof(SQLSMALLINT));
      RETVAL( TO_RUBY_INT(ptr_value.smallint) );
      break;


    case SQL_TYPE_DATE:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_TYPE_DATE, sizeof(DATE_STRUCT));
      RETVAL( rb_funcall( cDate, rb_intern("new"), 3,
                          TO_RUBY_INT(ptr_value.date.year),
                          TO_RUBY_INT(ptr_value.date.month),
                          TO_RUBY_INT(ptr_value.date.day) ) );
      break;

    case SQL_TYPE_TIME:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_TYPE_TIME, sizeof(TIME_STRUCT));
      RETVAL( rb_funcall( cTime, rb_intern("new"), 3,
                          TO_RUBY_INT(ptr_value.time.hour),
                          TO_RUBY_INT(ptr_value.time.minute),
                          TO_RUBY_INT(ptr_value.time.second) ) );
      break;

    case SQL_TYPE_TIMESTAMP:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_TYPE_TIMESTAMP, sizeof(TIMESTAMP_STRUCT));
      RETVAL( rb_funcall( cTimestamp, rb_intern("new"), 7,
                          TO_RUBY_INT(ptr_value.timestamp.year),
                          TO_RUBY_INT(ptr_value.timestamp.month),
                          TO_RUBY_INT(ptr_value.timestamp.day),
                          TO_RUBY_INT(ptr_value.timestamp.hour),
                          TO_RUBY_INT(ptr_value.timestamp.minute),
                          TO_RUBY_INT(ptr_value.timestamp.second),
                          TO_RUBY_INT(ptr_value.timestamp.fraction) ) );
      break;



    case SQL_CHAR:
    case SQL_CLOB:
    case SQL_LONGVARCHAR:
    case SQL_VARCHAR:
    /* TODO: should handle SQL_DECIMAL and SQL_NUMERIC different ? */
    case SQL_DECIMAL:
    case SQL_NUMERIC:

      ptr = (SQLPOINTER) ALLOC_N(SQLCHAR, bl);
      CALL_SQL_GET_DATA(ptr, SQL_C_CHAR, bl);
      RETVAL( rb_str_new(ptr, MIN(bl, strlen_or_indptr)) );

      free((void*)ptr);
      break;



    case SQL_BLOB:
    case SQL_BINARY:
    case SQL_LONGVARBINARY:
    case SQL_VARBINARY:

      ptr = (SQLPOINTER) ALLOC_N(SQLCHAR, bl);
      CALL_SQL_GET_DATA(ptr, SQL_C_BINARY, bl);
      RETVAL( rb_str_new(ptr, MIN(bl, strlen_or_indptr)) );

      free((void*)ptr);
      break;


    case SQL_BLOB_LOCATOR:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_BLOB_LOCATOR, sizeof(SQLINTEGER));
      RETVAL( TO_RUBY_INT(ptr_value.integer) );
      break;

    case SQL_CLOB_LOCATOR:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_CLOB_LOCATOR, sizeof(SQLINTEGER));
      RETVAL( TO_RUBY_INT(ptr_value.integer) );
      break;

    case SQL_DBCLOB_LOCATOR:
      CALL_SQL_GET_DATA(&ptr_value, SQL_C_DBCLOB_LOCATOR, sizeof(SQLINTEGER));
      RETVAL( TO_RUBY_INT(ptr_value.integer) );
      break;


    case SQL_BIGINT:
      /* TODO: How large can a BIGINT be? ==> expect 200 bytes, should be enought? */
      ptr = (SQLPOINTER) ALLOC_N(SQLCHAR, MAX(bl,200));
      CALL_SQL_GET_DATA(ptr, SQL_C_CHAR, bl);

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
        if (strlen_or_indptr == SQL_NULL_DATA) {
          retval = objNull;
        }
        else {
          /* convert string to integer */
          retval = rb_str_new(ptr, MIN(bl, strlen_or_indptr));
          retval = rb_funcall(retval, rb_intern("to_i"), 0); 
        }
      }
      else {
        retval = Qnil;
      }

      free((void*)ptr);
      break;





    case SQL_DBCLOB:
    case SQL_GRAPHIC:
    case SQL_LONGVARGRAPHIC:
    case SQL_VARGRAPHIC:

      ptr = (SQLPOINTER) ALLOC_N(SQLCHAR, bl);  /* NOTE: not SQLDBCHAR */
      CALL_SQL_GET_DATA(ptr, SQL_C_DBCHAR, bl);
      RETVAL( rb_str_new(ptr, MIN(bl, strlen_or_indptr)) );

      free((void*)ptr);
      break;




    default:
      rb_raise(rb_eTypeError, "Wrong parameter for target_type!");
  };

  #undef RETVAL
  #undef CALL_SQL_GET_DATA

  return rb_ary_new3(3, TO_RUBY_INT(rc), retval, TO_RUBY_INT(strlen_or_indptr));
}


/*******************************************************
 SQLEndTran
 =======================================================
 PARAMS:   handle_type, handle,
           completion_type : Integer
 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLEndTran(self, handle_type, handle, completion_type)
  VALUE self;
  VALUE handle_type, handle, completion_type;
{
  SQLRETURN rc;

  rc = SQLEndTran(
    (SQLSMALLINT) TO_C_INT(handle_type),
    (SQLHANDLE)   TO_C_INT(handle),
    (SQLSMALLINT) TO_C_INT(completion_type)
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLGetDiagRec
 =======================================================
 PARAMS:   handle_type, handle, rec_number,
           buffer_length : Integer

 RETURNS:  rc           : Integer,
           sql_state    : String,
           native_error : Integer,
           message_text : String,
           text_length  : Integer
********************************************************/

static VALUE
db2_SQLGetDiagRec(self, handle_type, handle, rec_number, buffer_length)
  VALUE self;
  VALUE handle_type, handle, rec_number, buffer_length;
{
  SQLRETURN   rc;
  SQLCHAR     sql_state[5];
  SQLINTEGER  native_error;
  SQLCHAR*    message_text;
  SQLINTEGER  bl;           /* buffer_length */
  SQLSMALLINT text_length;

  VALUE       retval;


  bl           = TO_C_INT(buffer_length);
  message_text = (SQLCHAR*) ALLOC_N(SQLCHAR, bl);

  rc = SQLGetDiagRec(
    (SQLSMALLINT)  TO_C_INT(handle_type),
    (SQLHANDLE)    TO_C_INT(handle),
    (SQLSMALLINT)  TO_C_INT(rec_number),
    (SQLCHAR*)     sql_state,
    (SQLINTEGER*)  &native_error,
    (SQLCHAR*)     message_text,
    (SQLINTEGER)   bl,
    (SQLSMALLINT*) &text_length
  );

  retval = rb_ary_new3(
    5,
    TO_RUBY_INT(rc),
    rb_str_new((const char *)sql_state, 5),
    TO_RUBY_INT(native_error),
    rb_str_new((const char *)message_text, MIN(bl, text_length)),
    TO_RUBY_INT(text_length)
  );

  free(message_text);

  return retval;
}





/*******************************************************
 SQLTables
 =======================================================
 PARAMS:   statement_handle : Integer,
           catalog_name     : String,    (must be set to "")
           schema_name      : String,
           table_name       : String,
           table_type       : String     (e.g. "TABLE, VIEW")

 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLTables(self, statement_handle, catalog_name, schema_name,
              table_name, table_type)
  VALUE self;
  VALUE statement_handle, catalog_name, schema_name;
  VALUE table_name, table_type;
{
  SQLRETURN rc;

  MUST_BE_STRING(catalog_name);
  MUST_BE_STRING(schema_name);
  MUST_BE_STRING(table_name);
  MUST_BE_STRING(table_type);

  rc = SQLTables(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLCHAR *FAR) RSTRING(catalog_name)->ptr,
    (SQLSMALLINT)  RSTRING(catalog_name)->len,
    (SQLCHAR *FAR) RSTRING(schema_name)->ptr,
    (SQLSMALLINT)  RSTRING(schema_name)->len,
    (SQLCHAR *FAR) RSTRING(table_name)->ptr,
    (SQLSMALLINT)  RSTRING(table_name)->len,
    (SQLCHAR *FAR) RSTRING(table_type)->ptr,
    (SQLSMALLINT)  RSTRING(table_type)->len
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLBindParameter             added by yun
 =======================================================
 PARAMS:   statement_handle : Integer,
           parameter_number : Integer,
           parameter_type   : Integer,
           parameter_size   : Integer,
           decimal_digits   : Integer
           parameter_value  : String,

 RETURNS:  rc : Integer

 Note
   ValueType parameter to SQLBindParameter call is set to
        SQL_C_CHAR. DB2 will convert it to proper type
        based on ParameterType parameter(parameter_type)
********************************************************/

static VALUE
db2_SQLBindParameter(self, statement_handle, parameter_number, parameter_type,
              parameter_size, decimal_digits, parameter_value)
  VALUE self;
  VALUE statement_handle, parameter_number, parameter_type, parameter_value;
  VALUE parameter_size, decimal_digits;
{
  SQLRETURN rc;

  MUST_BE_STRING(parameter_value);
  rc = SQLBindParameter(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLUSMALLINT) TO_C_INT(parameter_number),
    (SQLSMALLINT)  SQL_PARAM_INPUT,               /* support input parameter only */
    (SQLSMALLINT)  SQL_C_CHAR,                    /* ValueType. Always string */
    (SQLSMALLINT)  TO_C_INT(parameter_type),      /* SQL data type of the parameter */
    (SQLUINTEGER)  TO_C_INT(parameter_size),      /* column size */
    (SQLSMALLINT)  TO_C_INT(decimal_digits),      /* decimal digits */
    (SQLCHAR *FAR) RSTRING(parameter_value)->ptr, /* parameter value pointer */
    (SQLINTEGER)   RSTRING(parameter_value)->len, /* buffer length */
    (SQLINTEGER *FAR)  &(RSTRING(parameter_value)->len) /*StrLen */
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLGetCursorName                added by yun
 =======================================================
 PARAMS:   statement_handle  : Integer,

 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLGetCursorName(self, statement_handle)
  VALUE self;
  VALUE statement_handle;
{
  SQLRETURN rc;
  char name[20];
  short length;
  VALUE retval;

  rc = SQLGetCursorName(
    (SQLHSTMT)      TO_C_INT(statement_handle),
    (SQLCHAR *FAR)  name,
    (SQLSMALLINT)   18,
    (SQLSMALLINT *) &length
  );

  retval = rb_ary_new3(
    2,
    TO_RUBY_INT(rc),
    rb_str_new((const char *)name, length)
  );

  return retval;
}

/*******************************************************
 SQLSetCursorName                added by yun
 =======================================================
 PARAMS:   statement_handle  : Integer,
           name              : String

 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLSetCursorName(self, statement_handle, name)
  VALUE self;
  VALUE statement_handle, name;
{
  SQLRETURN rc;

  MUST_BE_STRING(name);
  rc = SQLSetCursorName(
    (SQLHSTMT)      TO_C_INT(statement_handle),
    (SQLCHAR *FAR)  RSTRING(name)->ptr,
    (SQLSMALLINT)   RSTRING(name)->len
  );

  return TO_RUBY_INT(rc);
}


/*******************************************************
 SQLColumns
 =======================================================
 PARAMS:   statement_handle : Integer,
           catalog_name     : String,    (must be set to "")
           schema_name      : String,
           table_name       : String,
           column_name      : String

 RETURNS:  rc : Integer
********************************************************/

static VALUE
db2_SQLColumns(self, statement_handle, catalog_name, schema_name,
              table_name, column_name)
  VALUE self;
  VALUE statement_handle, catalog_name, schema_name;
  VALUE table_name, column_name;
{
  SQLRETURN rc;

  MUST_BE_STRING(catalog_name);
  MUST_BE_STRING(schema_name);
  MUST_BE_STRING(table_name);
  MUST_BE_STRING(column_name);

  rc = SQLColumns(
    (SQLHSTMT)     TO_C_INT(statement_handle),
    (SQLCHAR *FAR) RSTRING(catalog_name)->ptr,
    (SQLSMALLINT)  RSTRING(catalog_name)->len,
    (SQLCHAR *FAR) RSTRING(schema_name)->ptr,
    (SQLSMALLINT)  RSTRING(schema_name)->len,
    (SQLCHAR *FAR) RSTRING(table_name)->ptr,
    (SQLSMALLINT)  RSTRING(table_name)->len,
    (SQLCHAR *FAR) RSTRING(column_name)->ptr,
    (SQLSMALLINT)  RSTRING(column_name)->len
  );

  return TO_RUBY_INT(rc);
}

/* Init */

void Init_db2cli() {
  mDB2CLI = rb_eval_string("DB2CLI");

  #include "constants.h"

  rb_define_module_function(mDB2CLI, "SQLAllocHandle",   db2_SQLAllocHandle,   2);
  rb_define_module_function(mDB2CLI, "SQLFreeHandle",    db2_SQLFreeHandle,    2);

  rb_define_module_function(mDB2CLI, "SQLDataSources",   db2_SQLDataSources,   4);
  rb_define_module_function(mDB2CLI, "SQLConnect",       db2_SQLConnect,       4);
  rb_define_module_function(mDB2CLI, "SQLSetConnectAttr",db2_SQLSetConnectAttr,3);
  rb_define_module_function(mDB2CLI, "SQLDisconnect",    db2_SQLDisconnect,    1);

  rb_define_module_function(mDB2CLI, "SQLPrepare",       db2_SQLPrepare,       2);
  rb_define_module_function(mDB2CLI, "SQLNumResultCols", db2_SQLNumResultCols, 1);
  rb_define_module_function(mDB2CLI, "SQLNumParams",     db2_SQLNumParams,     1);
  rb_define_module_function(mDB2CLI, "SQLDescribeCol",   db2_SQLDescribeCol,   3);
  rb_define_module_function(mDB2CLI, "SQLDescribeParam", db2_SQLDescribeParam, 2);
  rb_define_module_function(mDB2CLI, "SQLColAttribute",  db2_SQLColAttribute,  4);
  rb_define_module_function(mDB2CLI, "SQLExecDirect",    db2_SQLExecDirect,    2);
  rb_define_module_function(mDB2CLI, "SQLExecute",       db2_SQLExecute,       1);
  rb_define_module_function(mDB2CLI, "SQLCloseCursor",   db2_SQLCloseCursor,   1);
  rb_define_module_function(mDB2CLI, "SQLFreeStmt",      db2_SQLFreeStmt,      2);
  rb_define_module_function(mDB2CLI, "SQLRowCount",      db2_SQLRowCount,      1);
  rb_define_module_function(mDB2CLI, "SQLFetch",         db2_SQLFetch,         1);
  rb_define_module_function(mDB2CLI, "SQLFetchScroll",   db2_SQLFetchScroll,   3);
  rb_define_module_function(mDB2CLI, "SQLGetData",       db2_SQLGetData,      -1); /* 3-4 */

  rb_define_module_function(mDB2CLI, "SQLEndTran",       db2_SQLEndTran,       3);
  rb_define_module_function(mDB2CLI, "SQLGetDiagRec",    db2_SQLGetDiagRec,    4);

  rb_define_module_function(mDB2CLI, "SQLTables",        db2_SQLTables,        5);
  rb_define_module_function(mDB2CLI, "SQLColumns",       db2_SQLColumns,       5);
  rb_define_module_function(mDB2CLI, "SQLBindParameter", db2_SQLBindParameter, 6);
  rb_define_module_function(mDB2CLI, "SQLSetCursorName", db2_SQLSetCursorName, 2);
  rb_define_module_function(mDB2CLI, "SQLGetCursorName", db2_SQLGetCursorName, 1);
  /* Datatype classes or objects */

  cDate      = rb_eval_string("DB2CLI::Date");
  cTime      = rb_eval_string("DB2CLI::Time");
  cTimestamp = rb_eval_string("DB2CLI::Timestamp");
  objNull    = rb_eval_string("DB2CLI::Null");
}


