
require "db2/db2cli.rb"

module DB2

  module DB2Util
    include DB2CLI

    def free
      SQLFreeHandle(@handle_type, @handle)
    end

    def handle
      @handle
    end

    def check_rc(rc)
      if rc != SQL_SUCCESS and rc != SQL_SUCCESS_WITH_INFO
         case rc
            when SQL_NEED_DATA
                 rcText = 'SQL_NEED_DATA'
            when SQL_NO_DATA_FOUND
                 rcText = 'SQL_NO_DATA'
            when SQL_STILL_EXECUTING
                 rcText = 'SQL_STILL_EXECUTING'
            when SQL_ERROR
                 rcText = 'SQL_ERROR'
            when SQL_INVALID_HANDLE
                 rcText = 'SQL_INVALID_HANDLE'
         end
         puts "DB2 return code is #{rcText}"
         rec = 1
         loop do
           a = SQLGetDiagRec(@handle_type, @handle, rec, 500)
           break if a[0] != SQL_SUCCESS
           puts a[3] if a[3] != nil and a[3] != ''          #display message
           rec += 1
         end
         raise "ERROR From DB2"
      end
    end
  end # module DB2Util

  class Environment
    include DB2Util

    def initialize
      @handle_type = SQL_HANDLE_ENV
      rc, @handle = SQLAllocHandle(@handle_type, SQL_NULL_HANDLE)
      check_rc(rc)
    end

    def data_sources(buffer_length = 1024)
      retval = []
      max_buffer_length = buffer_length

      a = SQLDataSources(@handle, SQL_FETCH_FIRST, SQL_MAX_DSN_LENGTH+1, buffer_length)
      retval << [a[1], a[3]]
      max_buffer_length = [max_buffer_length, a[4]].max

      loop do
        a = SQLDataSources(@handle, SQL_FETCH_NEXT, SQL_MAX_DSN_LENGTH+1, buffer_length)
        break if a[0] == SQL_NO_DATA_FOUND

        retval << [a[1], a[3]]
        max_buffer_length = [max_buffer_length, a[4]].max
      end

      if max_buffer_length > buffer_length then
        get_data_sources(max_buffer_length)
      else
        retval
      end
    end

  end # class Environment


  class Connection
    include DB2Util

    def initialize(environment)
      @env = environment
      @handle_type = SQL_HANDLE_DBC
      rc, @handle = SQLAllocHandle(@handle_type, @env.handle)
      check_rc(rc)
    end

    def connect(server_name, user_name="", auth="")
      rc = SQLConnect(@handle, server_name, user_name, auth)
      check_rc(rc)
    end

    def set_auto_commit(switch)
      switch.upcase!
      if switch != 'ON' and switch != 'OFF'
         raise 'set_auto_commit: Specify either "ON" or "OFF"'
      end

      if switch == 'ON'
         rc = SQLSetConnectAttr(@handle, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON)
      else
         rc = SQLSetConnectAttr(@handle, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF)
      end
      check_rc(rc)
    end

    def set_connect_attr(attr, value)
      if value.type == String
         value += "\0"
      end
      rc = SQLSetConnectAttr(@handle, attr, value)
      check_rc(rc)
    end

    def disconnect
      rc = SQLDisconnect(@handle)
      check_rc(rc)
    end

    def rollback
      rc = SQLEndTran(@handle_type, @handle, SQL_ROLLBACK)
      check_rc(rc)
    end

    def commit
      rc = SQLEndTran(@handle_type, @handle, SQL_COMMIT)
      check_rc(rc)
    end


  end # class Connection


  class Statement
    include DB2Util

    def initialize(connection)
      @conn = connection
      @handle_type = SQL_HANDLE_STMT
      @parms = []                           #yun
      @sql = ''                             #yun
      @numParms = 0                         #yun
      @prepared = false                     #yun
      @parmArray = []                       #yun. attributes of the parameter markers
      rc, @handle = SQLAllocHandle(@handle_type, @conn.handle)
      check_rc(rc)
    end

    def prepare(sql)
      @sql = sql
      rc = SQLPrepare(@handle, sql)
      check_rc(rc)
      rc, @numParms = SQLNumParams(@handle) #number of question marks
      check_rc(rc)
      #--------------------------------------------------------------------------
      # parameter attributes are stored in instance variable @parmArray so that
      # they are available when execute method is called.
      #--------------------------------------------------------------------------
      if @numParms > 0            #get parameter marker attributes
         1.upto(@numParms) do     #parameter number starts from 1
            | i |
            rc, type, size, decimalDigits = SQLDescribeParam(@handle, i)
            check_rc(rc)
            @parmArray << Parameter.new(type, size, decimalDigits)
         end
      end
      @prepared = true;
      self
    end

    def execute(*parms)
      if @prepared == false
         raise "The statement was not prepared"
      end

      if parms.size == 1 and parms[0].type == Array
         parms = parms[0]
      end

      if (@numParms != parms.size)
         raise "Number of parameters supplied does not match with the SQL statement"
      end

      if @numParms > 0            #need to bind parameters
         #--------------------------------------------------------------------
         #calling bindParms may not be safe. Look comment below.
         #--------------------------------------------------------------------
         #bindParms(parms)        #may not be safe

         valueArray = []
         1.upto(@numParms) do     #parameter number starts from 1
            | i |
            type = @parmArray[i - 1].type
            size = @parmArray[i - 1].size
            decimalDigits = @parmArray[i - 1].decimalDigits

            if parms[i - 1].type == String
               valueArray << parms[i - 1]
            else
               valueArray << parms[i - 1].to_s
            end

            rc = SQLBindParameter(@handle, i, type, size, decimalDigits, valueArray[i - 1])
            check_rc(rc)
         end
      end

      rc = SQLExecute(@handle)
      check_rc(rc)

      if @numParms != 0
         SQLFreeStmt(@handle, SQL_RESET_PARAMS)    #reset parameters
         check_rc(rc)
      end

      self
    end

    #-------------------------------------------------------------------------------
    # The last argument(value) to SQLBindParameter is a deferred argument, that is,
    # it should be available when SQLExecute is called. Even though "value" is
    # local to bindParms method, it seems that it is available when SQLExecute
    # is called. I am not sure whether it would still work if garbage collection
    # is done between bindParms call and SQLExecute call inside the execute method
    # above.
    #-------------------------------------------------------------------------------
    def bindParms(parms)        #This is the real thing. It uses SQLBindParms
       1.upto(@numParms) do     #parameter number starts from 1
          | i |
          rc, dataType, parmSize, decimalDigits = SQLDescribeParam(@handle, i)
          check_rc(rc)
          if parms[i - 1].type == String
             value = parms[i - 1]
          else
             value = parms[i - 1].to_s
          end
          rc = SQLBindParameter(@handle, i, dataType, parmSize, decimalDigits, value)
          check_rc(rc)
       end
    end
 
    #------------------------------------------------------------------------------
    # bind method does not use DB2's SQLBindParams, but replaces "?" in the
    # SQL statement with the value before passing the SQL statement to DB2.
    # It is not efficient and can handle only strings since it puts everything in
    # quotes.
    #------------------------------------------------------------------------------
    def bind(sql, args)                #does not use SQLBindParams
      arg_index = 0
      result = ""
      tokens(sql).each { |part|
        case part
        when '?'
          result << "'" + (args[arg_index]) + "'"  #put it into quotes
          arg_index += 1
        when '??'
          result << "?"
        else
          result << part
        end
      }
      if arg_index < args.size
        raise "Too many SQL parameters"
      elsif arg_index > args.size
        raise "Not enough SQL parameters"
      end
      result
    end

    ## Break the sql string into parts.
    #
    # This is NOT a full lexer for SQL.  It just breaks up the SQL
    # string enough so that question marks, double question marks and
    # quoted strings are separated.  This is used when binding
    # arguments to "?" in the SQL string.  Note: comments are not
    # handled.
    #
    def tokens(sql)
      toks = sql.scan(/('([^'\\]|''|\\.)*'|"([^"\\]|""|\\.)*"|\?\??|[^'"?]+)/)
      toks.collect {|t| t[0]}
    end


    def exec_direct(sql)
      rc = SQLExecDirect(@handle, sql)
      check_rc(rc)
      self
    end


    def set_cursor_name(name)
      rc = SQLSetCursorName(@handle, name)
      check_rc(rc)
      self
    end

    def get_cursor_name
      rc, name = SQLGetCursorName(@handle)
      check_rc(rc)
      name
    end

    def row_count
      rc, rowcount = SQLRowCount(@handle)
      check_rc(rc)
      rowcount
    end

    def num_result_cols
      rc, cols = SQLNumResultCols(@handle)
      check_rc(rc)
      cols
    end

    def fetch_all
      if block_given?
        while (row=fetch) do
          yield row
        end
      else
        res = []
        while (row=fetch) do
          res << row
        end
        res
      end
    end


    def fetch
      rc, nr_cols = SQLNumResultCols(@handle)

      cols = (1..nr_cols).collect do |c|
        rc, name, bl, type, col_sz = SQLDescribeCol(@handle, c, 1024)
        [name, type, col_sz]
      end

      rc = SQLFetch(@handle)

      if rc == SQL_NO_DATA_FOUND
         #SQLCloseCursor(@handle)
         SQLFreeStmt(@handle, SQL_CLOSE)           #close cursor
         return nil
      end

      raise "ERROR" unless rc == SQL_SUCCESS

      retval = []
      cols.each_with_index do |c, i|
        rc, content = SQLGetData(@handle, i+1, c[1], c[2] + 1) #yun added 1 to c[2]
        retval << content
      end

      return retval
    end

    def fetch_as_hash
      rc, nr_cols = SQLNumResultCols(@handle)

      cols = (1..nr_cols).collect do |c|
        rc, name, bl, type, col_sz = SQLDescribeCol(@handle, c, 1024)
        [name, type, col_sz]
      end

      rc = SQLFetch(@handle)

      if rc == SQL_NO_DATA_FOUND
         #SQLCloseCursor(@handle)
         SQLFreeStmt(@handle, SQL_CLOSE)           #close cursor
         SQLFreeStmt(@handle, SQL_RESET_PARAMS)    #reset parameters
         return nil
      end

      raise "ERROR" unless rc == SQL_SUCCESS

      retval = {}
      cols.each_with_index do |c, i|
        rc, content = SQLGetData(@handle, i+1, c[1], c[2] + 1)   #yun added 1 to c[2]
        retval[c[0]] = content
      end

      return retval
    end

  end # class Statement

  class Parameter
     attr_reader :type, :size, :decimalDigits
     def initialize(type, size, decimalDigits)
        @type = type
        @size = size
        @decimalDigits = decimalDigits
     end
  end

end # module DB2
