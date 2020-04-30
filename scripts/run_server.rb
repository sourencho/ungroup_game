#!/usr/bin/env ruby

require 'socket'

open_port = false
if ARGV.length > 0 && (ARGV[0] == "--open-port")
  open_port = true
end

# use this trick to find an open tcp port
server = TCPServer.new('127.0.0.1', 0)
port = server.addr[1]
server.close
puts "Using port: #{port}"

# for dev or prod (not local), we probably want to open the port
if open_port
  # make sure we have closed any ports previously opened by this script
  `firewall-cmd --reload`

  # open firewall port, runs on Fedora Linux only
  `firewall-cmd --zone=FedoraServer --add-port=#{port}/tcp`
end

# run the server on the given port
`./build/src/server/ug-server -p #{port}`
