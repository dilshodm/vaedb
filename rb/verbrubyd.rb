#!/usr/bin/env ruby

THIS_FILE = File.symlink?(__FILE__) ? File.readlink(__FILE__) : __FILE__
Dir.glob(File.dirname(THIS_FILE) + '/lib/vendor/*/lib').each { |dir| $:.unshift(dir) }
$:.unshift File.dirname(THIS_FILE) + '/lib'
$stderr.reopen $stdout

require 'verbrubyd'

Verbrubyd.new.run