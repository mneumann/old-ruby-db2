require "db2.rb"
include DB2

env = Environment.new
conn = env.createConnection("SAMPLE")


stmt = conn.createStatement
stmt.prepare("SELECT * FROM EMPLOYEE").execute
p stmt.fetchAll

conn.disconnect
