$:.unshift File.dirname(THIS_FILE) + '/../gen-rb'
require 'rubygems'
require 'thrift'
require 'vae_rubyd_handler'

class Vaerubyd

  def run
    processor = VaeRubyd::Processor.new(VaeRubydHandler.new)
    transport = Thrift::ServerSocket.new(9090)
    transportFactory = Thrift::BufferedTransportFactory.new
    server = Thrift::ThreadPoolServer.new(processor, transport, transportFactory)
    puts "VaeRubyd Running"
    puts "  Encoding: #{Encoding.default_internal} #{Encoding.default_external}"
    server.serve()
  end

end
