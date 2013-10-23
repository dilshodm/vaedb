$:.push('../gen-rb')

require 'thrift'
require 'vae_db'

example_str = <<-EOH
method_call(get):
  i32:1319955805
  i32:1
  string:"/shop_banners"
  map<string,string>:{"page":"1","paginate":"1",}
method_call(openSession):
  string:"tendeep"
  string:"9e0894576f05ff094f747ba9af42735e85179141"
  bool:0
  i32:1319955805
method_call(openSession):
  string:"saturdaysnyc"
  string:"6bf980c51abe34034edf00a6924824a48b1ea64f"
  bool:0
  i32:2103457087
EOH

class String
  def gets()
    arr = to_a
    line = arr[0]
    rest = arr[1..-1]
    replace(arr[1..-1].join) if rest
    line
  end
end

class VaeReplayBot
  R_MAP_S_S_LIST  = /"([^"\\]*(?:\\.[^"\\]*)*)":"([^"\\]*(?:\\.[^"\\]*)*)",/
  R_METHOD_CALL   = /method_call\((.+)\):/
  R_VALUE_STRING  = /string:"(.*)"/
  R_VALUE_BOOL    = /bool:(0|1)/
  R_VALUE_I32     = /i32:(.*)/
  R_VALUE_MAP_S_S = /map<string,string>:\{((#{R_MAP_S_S_LIST})*)\}/
  R_EMPTY         = /\A\Z/
  R_ANY           = /(.*)/

  LEX_RULES = [
    [:method_call, R_METHOD_CALL],
    [:value_string, R_VALUE_STRING],
    [:value_bool, R_VALUE_BOOL],
    [:value_i32, R_VALUE_I32],
    [:value_map_s_s, R_VALUE_MAP_S_S],
    [:empty, R_EMPTY],
    [:any, R_ANY]
  ]

  def initialize(vaedb)
    @vaedb = vaedb
  end

  def method_call(method_name, args)
    begin
      puts "#{method_name} <- #{args.join(',')}"
      @vaedb.send(method_name.to_sym, *args)
    rescue VaeDbInternalError => e
      puts "  VaeDbInternalError: #{e.message}"
    end
  end

  def classify_line(line)
    LEX_RULES.each { |tag, rexp|
      lexeme = line.scan(rexp)[0]
      return [tag, lexeme[0], lexeme] if lexeme
    }
  end

  def process_stream(s)
    method_name = nil
    args = []

    line_num = 0
    while(line = s.gets)
      line_num +=1
      tag, lexeme, match = classify_line(line.strip)

      case tag
        when :method_call
          if !method_name.nil?
            method_call(method_name, args)
            args = []
          end
          method_name = lexeme

        when :value_string
          args.push(lexeme)

        when :value_bool
          args.push(lexeme == "1")

        when :value_i32
          args.push(lexeme.to_i)

        when :value_map_s_s
          args.push(Hash[*lexeme.scan(R_MAP_S_S_LIST).flatten])

        when :empty

        else
          puts "error on line #{line_num}: don't know what to do with input '#{lexeme}'"
          return false
      end
    end

    method_call(method_name, args) if method_name

  end

end

begin
  if ARGV.count != 3
    puts "usage: bot.rb {hostname} {port} {inputfile}"
    exit(-1)
  end

  host = ARGV[0]
  port = ARGV[1]
  log_filename = ARGV[2]

  socket = Thrift::Socket.new(host, port)
  transport = Thrift::BufferedTransport.new(socket)
  protocol = Thrift::BinaryProtocol.new(transport)
  client = VaeDb::Client.new(protocol)

  transport.open()
 
  bot = VaeReplayBot.new(client)
  File.open(log_filename, "r") do 
    |file| bot.process_stream(file)
  end
    
  transport.close()
rescue
    puts $!
end
