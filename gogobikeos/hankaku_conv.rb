#!/usr/bin/env ruby
# frozen_string_literal: true

require 'optparse'

def cmdline
  args = {}
  OptionParser.new do |parser|
    parser.on('-o VALUE') { |v| args[:output] = v }
    args[:input] = parser.parse!(ARGV)

    raise 'input file must be a file' unless args[:input].size == 1

    args[:input] = args[:input][0]
  end
  args
end

def convert2c(input, output)
  fonts = parse_hankaku(input)
  char_lines = ''
  fonts.each_value do |font|
    char_lines += format(
      "    %s\n",
      font[0..7].join(', ') << ','
    )
    char_lines += format(
      "    %s\n",
      font[8..15].join(', ') << ','
    )
  end
  source = format(
    "char hankaku[4096] = {\n%s};",
    char_lines
  )
  writetext(source, output)
end

def parse_hankaku(filename)
  lines = parse_lines(filename)
  fonts = {}
  lines.each do |k, v|
    bin = []
    fonts[k] = []
    v.each { |t| bin.push text2bin(t) }
    bin.each { |b| fonts[k].push bin2hex(b) }
  end
  fonts
end

def text2bin(text)
  text.gsub('.', '0').gsub('*', '1')
end

def bin2hex(bin)
  format('0x%02x', bin.to_i(2))
end

def parse_lines(filename)
  font_lines = {}
  File.open(filename, 'r') do |f|
    until f.eof?
      line = f.readline(chomp: true)
      next if line.empty? || !line.start_with?('char ')

      key = line.chomp.split(' ')[1]
      font_lines[key] = []
      16.times do
        font_lines[key].push f.readline(chomp: true)
      end
    end
  end
  font_lines
end

def writetext(string, filename)
  File.open(filename, 'w') do |f|
    f.write(string)
  end
end

def main
  args = cmdline
  convert2c(args[:input], args[:output])
end

main if __FILE__ == $PROGRAM_NAME
