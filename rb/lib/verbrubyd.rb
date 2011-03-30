$:.unshift File.dirname(THIS_FILE) + '/../gen-rb'
require 'rubygems'
require 'thrift'
require 'verb_rubyd_handler'

class Verbrubyd
  
  def run
    processor = VerbRubyd::Processor.new(VerbRubydHandler.new)
    transport = Thrift::ServerSocket.new(9090)
    transportFactory = Thrift::BufferedTransportFactory.new
    server = Thrift::ThreadPoolServer.new(processor, transport, transportFactory)
    puts "VerbRubyd Running"
    server.serve()
  end
  
end