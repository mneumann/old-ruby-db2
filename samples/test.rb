
require "db2/db2cli.rb"

include DB2CLI

rc, env  = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE); p rc

puts "Datasources: "
p SQLDataSources(env, SQL_FETCH_FIRST, SQL_MAX_DSN_LENGTH+1, 200) 

loop { 
  a = SQLDataSources(env, SQL_FETCH_NEXT, SQL_MAX_DSN_LENGTH+1, 200)
  break if a[0] == SQL_NO_DATA_FOUND
  p a
}
puts "--------------------"
readline

rc, dbc  = SQLAllocHandle(SQL_HANDLE_DBC, env); p rc

p SQLConnect(dbc, "SAMPLE", "", "")

rc, stmt = SQLAllocHandle(SQL_HANDLE_STMT, dbc); p rc



p SQLPrepare(stmt, "SELECT * FROM EMPLOYEE")

p SQLExecute(stmt)

DELIM = "  "

rc, COLS = SQLNumResultCols(stmt)

cols = (1..COLS).collect {|c| 
  rc, name, bl, type, col_sz = SQLDescribeCol(stmt, c, 100)
  [name, type, col_sz]
}

cols.each {|c| print c[0], DELIM }
puts

while SQLFetch(stmt) == SQL_SUCCESS do

  cols.each_with_index {|c, i|
    rc, content = SQLGetData(stmt, i+1, c[1], 100) 
    print content, DELIM 
  }
  puts 
end

p SQLFreeStmt(stmt, SQL_CLOSE)

################
def get_row_as_hash(stmt)
  rc, nr_cols = SQLNumResultCols(stmt)
  
  cols = (1..nr_cols).collect do |c| 
    rc, name, bl, type, col_sz = SQLDescribeCol(stmt, c, 100)
    [name, type, col_sz]
  end 

  rc = SQLFetch(stmt)

  return nil if rc == SQL_NO_DATA_FOUND
  raise "ERROR" unless rc == SQL_SUCCESS 
  
  retval = {}
  cols.each_with_index do |c, i|
    rc, content = SQLGetData(stmt, i+1, c[1], c[2]) 
    retval[c[0]] = content
  end 

  return retval
end
################

puts "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
p SQLTables(stmt, "", "", "%", "")
while (c=get_row_as_hash(stmt))
  p c
end
puts "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"


p SQLFreeStmt(stmt, SQL_CLOSE)

p SQLExecDirect(stmt, "SELECT 2, 2.3+3.4, 'Hallo' FROM EMPLOYEE")
#p SQLExecDirect(stmt, "SELECT YEARS FROM STAFF WHERE YEARS IS NULL")

p "*************************************"
p SQLColAttribute(stmt, 2, SQL_DESC_TYPE_NAME, 200)
p SQLColAttribute(stmt, 2, SQL_DESC_TYPE, nil)
p "*************************************"


p SQLFetch(stmt)


puts "=================================="
p SQLGetData(stmt, 1, SQL_INTEGER)
p SQLGetData(stmt, 1, SQL_SMALLINT)

p SQLGetData(stmt, 2, SQL_DOUBLE)
#p SQLGetData(stmt, 2, SQL_FLOAT)
#p SQLGetData(stmt, 2, SQL_REAL)

p SQLGetData(stmt, 3, SQL_INTEGER, 20000)
puts "=================================="


p SQLFreeStmt(stmt, SQL_CLOSE)


puts "-----111111111111111-----------"


p SQLExecDirect(stmt, "INSERT INTO meintable (test,hallo) VALUES (2,3)")

p SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, 1000)

p SQLRowCount(stmt)

p SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_ROLLBACK)
p SQLFreeHandle(SQL_HANDLE_STMT, stmt)




p SQLDisconnect(dbc)
p SQLFreeHandle(SQL_HANDLE_DBC, dbc)
p SQLFreeHandle(SQL_HANDLE_ENV, env)

