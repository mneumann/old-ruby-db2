
require "db2/db2cli.rb"

module DB2

  FINALIZE_INFO = {}
  FINALIZE_PROC = proc do |id|
    info =  FINALIZE_INFO[id]
    if $DEBUG then
      $stderr.puts "freeing handle <#{info[1]}> (#{info[0]})"
    end
    DB2CLI::SQLFreeHandle(info[0], info[1]) 
  end 


  class CLIError < Exception
    attr_reader :sql_state, :native_error, :message_text

    def initialize(sql_state, native_error, message_text)
      @sql_state    = sql_state
      @native_error = native_error
      @message_text = message_text
    end
    
    def message
      @message_text
    end    
  end

  class Handle
    include DB2CLI

    def initialize(handle_type, parent_handle)
      @handle_type = handle_type
      @handle = parent_handle

      rc, handle = SQLAllocHandle(@handle_type, @handle)
      check_rc(rc)
      @handle = handle
      add_finalizer
     end

    def free
      rc = SQLFreeHandle(@handle_type, @handle)
      check_rc(rc)
    end

    def handle
      @handle
    end

    private

    def add_finalizer
      DB2::FINALIZE_INFO[self.id] = [@handle_type.dup, @handle.dup]  
      ObjectSpace.define_finalizer(self, DB2::FINALIZE_PROC)
    end

    def check_rc(rc)
      if rc != SQL_SUCCESS and rc != SQL_SUCCESS_WITH_INFO then 
        err = SQLGetDiagRec(@handle_type, @handle, 1, 1024)
        raise CLIError.new(err[1], err[2], err[3]) 
      end
    end
  end # class Handle


  class Environment < Handle

    def initialize
      super(SQL_HANDLE_ENV, SQL_NULL_HANDLE)
    end

    def dataSources
      data_sources_buffer
    end


    def createConnection(*a)
      Connection.new(self, *a)
    end


    private

    def data_sources_buffer(buffer_length = 1024)
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
        data_sources_buffer(max_buffer_length)
      else
        retval
      end
    end


  end # class Environment



  class Connection < Handle

    def initialize(environment, *conn_params)
      @env = environment

      super(SQL_HANDLE_DBC, @env.handle)

      @connected = false

      unless conn_params.empty?
        connect(*conn_params)
      end
    end

    def connect(server_name, user_name="", auth="")
      rc = SQLConnect(@handle, server_name, user_name, auth) 
      check_rc(rc)
      @connected = true
    end

    def disconnect
      rc = SQLDisconnect(@handle)
      check_rc(rc)
      @connected = false
    end

    def rollback
      rc = SQLEndTran(@handle_type, @handle, SQL_ROLLBACK)
      check_rc(rc)
    end

    def commit
      rc = SQLEndTran(@handle_type, @handle, SQL_COMMIT)
      check_rc(rc)
    end

    def createStatement
      Statement.new(self)
    end

  end # class Connection


  class Statement < Handle

    def initialize(connection)
      @conn = connection
      super(SQL_HANDLE_STMT, @conn.handle)
    end

    def prepare(sql)
      rc = SQLPrepare(@handle, sql)
      check_rc(rc)
      self
    end

    def freeStmt(options)
      rc = SQLFreeStmt(@handle, options) 
      ckeck_rc(rc)
    end

    def execute
      rc = SQLExecute(@handle)
      check_rc(rc)
      self
    end

    def execDirect(sql)
      rc = SQLExecDirect(@handle, sql)
      check_rc(rc)
      self
    end

    def tables(schema_name="%", table_name="%", table_type="TABLE") 
      rc = SQLTables(@handle, "", schema_name, table_name, table_type)
      check_rc(rc)
      self 
    end

    def rowCount
      rc, rowcount = SQLRowCount(@handle)
      check_rc(rc)
      rowcount
    end

    def numResultCols
      rc, cols = SQLNumResultCols(@handle)
      check_rc(rc)
      cols
    end

    def colNames
      rc, nr_cols = SQLNumResultCols(@handle)
    
      (1..nr_cols).collect do |c| 
        rc, name, bl, type, col_sz = SQLDescribeCol(@handle, c, 1024)
        name
      end 
    end

    def fetchAll
      if block_given?
        while (row=fetchRow) do
          yield row
        end
      else
        res = []
        while (row=fetchRow) do
          res << row
        end
        res
      end
    end

    def fetchRow(orientation=SQL_FETCH_NEXT, offset=0)
      rc, nr_cols = SQLNumResultCols(@handle)
    
      cols = (1..nr_cols).collect do |c| 
        rc, name, bl, type, col_sz = SQLDescribeCol(@handle, c, 1024)
        [name, type, col_sz]
      end 

      rc = SQLFetchScroll(@handle, orientation, offset)

      return nil if rc == SQL_NO_DATA_FOUND
      raise "ERROR" unless rc == SQL_SUCCESS 
    
      retval = [] 
      cols.each_with_index do |c, i|
        rc, content = SQLGetData(@handle, i+1, c[1], c[2]) 
        retval << content
      end 

      return retval
    end

    alias :fetch :fetchRow

  end # class Statement


end # module DB2
