
require "readline"
require "db2"

class ReadlineControl
  
  def initialize
    @keywords = []
    set_prompt
    Readline.completion_proc = proc {|str| complete(str) }
  end

  def add_keywords(arr)
    @keywords += arr
  end

  def complete(str)
    @keywords.grep(/^#{Regexp.escape(str)}/i)
  end

  def set_prompt(prompt="> ")
    @prompt = prompt
  end

  def readline
    Readline.readline(@prompt, true)
  end

end

if ARGV.size < 1 or ARGV.size > 3
  puts
  puts "USAGE: #{$0} database [, user [, password] ]"
  puts
  exit 1
else
  DB   = ARGV.shift
  USER = ARGV.shift || ""
  PASS = ARGV.shift || ""
end

puts
begin
  print "  CONNECT TO #{DB}" 
  print " USER '#{USER}'" unless USER.empty?
  print " PASSWORD '#{PASS}'" unless PASS.empty?
  puts

  Env  = DB2::Environment.new
  Conn = Env.createConnection("SAMPLE")

rescue DB2::CLIError => err
    puts
    puts err.message
    puts
end

puts


PROMPT      = "db2 => "
PROMPT_CONT = "db2 =| "

SQL_KEYWORDS = %w(
  INSERT DELETE UPDATE SELECT FROM WHERE IN LIKE SET VALUES INTO
  CREATE TABLE DROP 
  COMMIT ROLLBACK
  CHAR VARCHAR INT INTEGER NUMBER FLOAT REAL LONG CLOB BLOB DECIMAL DBCLOB DBBLOB
)

rd = ReadlineControl.new
rd.add_keywords SQL_KEYWORDS
rd.set_prompt(PROMPT)


def output_table(rows)
  collength = [] 
 
  rows.each {|r| r.each_with_index {|c,i|  
    r[i] = c.to_s.chomp("\000")
    c = r[i]
    collength[i] = c.size if collength[i].nil? or c.size > collength[i] 
  } }

  split_line = " +"
  collength.each {|col|
    split_line << "-" * (col+2) + "+"
  }
  
  puts
  puts split_line

  # output table
  rows.each_with_index {|r, rn|
    print " |"
    r.each_with_index {|c,i|
      print sprintf(" %*2$s |", c.to_s, -(collength[i]))
    }
    puts
    puts split_line if rn == 0
  }
  puts split_line
  puts
end


loop {
  line = rd.readline
  line.chomp!
  next if line.empty?
 
  begin

    if line =~ /^\\/ then
      if line =~ /^\\list tables/i then
        stmt = Conn.createStatement.tables
      elsif line =~ /^\\list views/i then
        stmt = Conn.createStatement.tables("%", "%", "VIEW")
      elsif line =~ /^\\quit/i then
        break 
      else
        puts
        puts "Unknown command!"
        puts
        next
      end
    else
      # multi-line
      if line[-1].chr == "\\" then
        line.chop!
        rd.set_prompt(PROMPT_CONT)
        loop {
          ln = rd.readline
          line.chomp!
          next if line.empty?
           
          if ln[-1].chr == "\\" then
            line += ln.chop
          else
            line += ln
            break
          end
        }
      end

      rd.set_prompt(PROMPT)
      stmt = Conn.createStatement.execDirect(line)
    end

    head = stmt.colNames 
    next if head.empty? 
    rows = stmt.fetchAll 
    rows[0,0] = [head]

    output_table(rows)
  rescue DB2::CLIError => err
    puts
    puts err.message
    puts
  end
}

Conn.disconnect
Conn.free
Env.free


