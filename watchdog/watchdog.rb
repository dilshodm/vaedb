#!/usr/bin/env ruby
$:.push(File.join(File.dirname(__FILE__),'../gen-rb'))

require 'rubygems'
require 'optparse'
require 'vae_db'

options = {}
OptionParser.new do |o|
  options[:host] = 'localhost'
  o.on('-h', '--host HOST', String, 'vaedb host') do |h| 
    options[:host] = h
  end

  options[:port] = 9091
  o.on('-p', '--port PORT', Integer, 'vaedb port') do |p| 
    options[:port] = p
  end

  options[:timeout] = 10
  o.on('-t', '--timeout TIMEOUT', Integer, 'time to wait before taking action') do |t|
    options[:timeout] = t
  end

  options[:command] = "svc -t /service/vaedbd"
  o.on('-c', '--command COMMAND', String, 'command to execute if there is trouble with vaedb') do |c|
    options[:command] = c
  end

  o.parse!
end

ping_thread = Thread.new do
  begin
    socket = Thrift::Socket.new(options[:host], options[:port])
    transport = Thrift::BufferedTransport.new(socket)
    protocol = Thrift::BinaryProtocol.new(transport)
    client = VaeDb::Client.new(protocol)

    transport.open()
    client.ping() 
    transport.close()

  rescue
    %x{#{options[:command]}}
  ensure
    exit 1
  end
end

sleep options[:timeout]
ping_thread.raise if ping_thread.alive?

