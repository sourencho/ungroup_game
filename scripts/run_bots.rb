#!/usr/bin/ruby

require "resolv"

if ARGV.length != 2
  puts "Please pass two arguments: hostname and number of bots"
  puts "Example: ./run_bots.rb localhost 10"
  exit
end

num_bots = ARGV[1].to_i
if num_bots < 0 || num_bots > 50
  puts "Invalid arguments for number of bots. It should be between 0 and 50. Exiting."
  exit
end
server_name = ARGV[0]
server_ip = Resolv.getaddress server_name
server_ip_valid = !!(server_ip =~ Resolv::IPv4::Regex)
if !server_ip_valid
  puts "Invalid arguments for number of bots. It should be between 0 and 50. Exiting."
  exit
end

begin
  pids = []
  for _ in 1..num_bots do
    j = fork do
      exec "./build/src/client/ug-client -x -b -s 0 -a #{server_ip}"
      exit
    end
    pids.push j
    Process.detach(j)
  end
  sleep(1000000)
rescue SystemExit, Interrupt
  puts "Terminating..."
  STDOUT.flush
  for pid in pids do
    Process.kill("TERM", pid)
  end
  raise
end
