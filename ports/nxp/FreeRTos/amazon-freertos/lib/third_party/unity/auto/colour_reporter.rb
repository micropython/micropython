# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require "#{File.expand_path(File.dirname(__FILE__))}/colour_prompt"

$colour_output = true

def report(message)
  if !$colour_output
    $stdout.puts(message)
  else
    message = message.join('\n') if message.class == Array
    message.each_line do |line|
      line.chomp!
      colour = case line
               when /(?:total\s+)?tests:?\s+(\d+)\s+(?:total\s+)?failures:?\s+\d+\s+Ignored:?/i
                 Regexp.last_match(1).to_i.zero? ? :green : :red
               when /PASS/
                 :green
               when /^OK$/
                 :green
               when /(?:FAIL|ERROR)/
                 :red
               when /IGNORE/
                 :yellow
               when /^(?:Creating|Compiling|Linking)/
                 :white
               else
                 :silver
               end
      colour_puts(colour, line)
    end
  end
  $stdout.flush
  $stderr.flush
end
