require "mkmf"

if RUBY_PLATFORM =~ /(mswin32|cygwin|mingw)/
  # version 6.1 Windows 95/98/NT
  DB2LIB = "db2cli"   
  DB2DIR = ENV["DB2DIR"] || "C:/SQLLIB"
else
  # version 7.1 Linux 
  DB2LIB = "db2" 
  DB2DIR = ENV["DB2DIR"] || "/usr/IBMdb2/V7.1"
end

dir_config( "db2", DB2DIR + "/include", DB2DIR + "/lib" )

if have_library(DB2LIB, "SQLConnect") and 
   have_header("sqlcli.h")
  create_makefile "db2cli"
else
  puts "ABORT: Could not locate DB2 libraries or headers!"
  puts "Please set DB2DIR to your DB2 directory, e.g. /usr/IBMdb2/V7.1 (UNIX) " + 
       "or C:/SQLLIB (Windows)"
  exit 1
end

