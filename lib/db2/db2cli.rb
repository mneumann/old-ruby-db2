#
# $Id: db2cli.rb,v 1.1 2002/09/05 09:57:13 mneumann Exp $
#

require "singleton"


module DB2CLI

  class Date
    attr_accessor :year, :month, :day
    def initialize(year=0, month=0, day=0)
      @year, @month, @day = year, month, day
    end
    def to_s
      "#{@year}-#{@month}-#{@day}"
    end
  end

  class Time
    attr_accessor :hour, :minute, :second
    def initialize(hour=0, minute=0, second=0)
      @hour, @minute, @second = hour, minute, second
    end
    def to_s
      "#{@hour}:#{@minute}:#{@second}"
    end
  end

  class Timestamp
    attr_accessor :year, :month, :day
    attr_accessor :hour, :minute, :second, :fraction
    def initialize(year=0, month=0, day=0, hour=0, minute=0, second=0, fraction=0)
      @year, @month, @day = year, month, day
      @hour, @minute, @second, @fraction = hour, minute, second, fraction
    end
    def to_s
      "#{@year}-#{@month}-#{@day} #{@hour}:#{@minute}:#{@second}.#{@fraction}"
    end
  end


  class NullClass
    include Singleton
    def to_s
      "NULL"
    end
  end

  Null = NullClass.instance  

end # module DB2CLI

require "db2/db2cli.so"

