#!/usr/bin/ruby
#
# unity_to_junit.rb
#
require 'fileutils'
require 'optparse'
require 'ostruct'
require 'set'

require 'pp'

VERSION = 1.0

class ArgvParser
  #
  # Return a structure describing the options.
  #
  def self.parse(args)
    # The options specified on the command line will be collected in *options*.
    # We set default values here.
    options = OpenStruct.new
    options.results_dir = '.'
    options.root_path = '.'
    options.out_file = 'results.xml'

    opts = OptionParser.new do |o|
      o.banner = 'Usage: unity_to_junit.rb [options]'

      o.separator ''
      o.separator 'Specific options:'

      o.on('-r', '--results <dir>', 'Look for Unity Results files here.') do |results|
        # puts "results #{results}"
        options.results_dir = results
      end

      o.on('-p', '--root_path <path>', 'Prepend this path to files in results.') do |root_path|
        options.root_path = root_path
      end

      o.on('-o', '--output <filename>', 'XML file to generate.') do |out_file|
        # puts "out_file: #{out_file}"
        options.out_file = out_file
      end

      o.separator ''
      o.separator 'Common options:'

      # No argument, shows at tail.  This will print an options summary.
      o.on_tail('-h', '--help', 'Show this message') do
        puts o
        exit
      end

      # Another typical switch to print the version.
      o.on_tail('--version', 'Show version') do
        puts "unity_to_junit.rb version #{VERSION}"
        exit
      end
    end

    opts.parse!(args)
    options
  end # parse()
end # class OptparseExample

class UnityToJUnit
  include FileUtils::Verbose
  attr_reader :report, :total_tests, :failures, :ignored
  attr_writer :targets, :root, :out_file

  def initialize
    @report = ''
    @unit_name = ''
  end

  def run
    # Clean up result file names
    results = @targets.map { |target| target.tr('\\', '/') }
    # puts "Output File: #{@out_file}"
    f = File.new(@out_file, 'w')
    write_xml_header(f)
    write_suites_header(f)
    results.each do |result_file|
      lines = File.readlines(result_file).map(&:chomp)

      raise "Empty test result file: #{result_file}" if lines.empty?

      result_output = get_details(result_file, lines)
      tests, failures, ignored = parse_test_summary(lines)
      result_output[:counts][:total] = tests
      result_output[:counts][:failed] = failures
      result_output[:counts][:ignored] = ignored
      result_output[:counts][:passed] = (result_output[:counts][:total] - result_output[:counts][:failed] - result_output[:counts][:ignored])

      # use line[0] from the test output to get the test_file path and name
      test_file_str = lines[0].tr('\\', '/')
      test_file_str = test_file_str.split(':')
      test_file = if test_file_str.length < 2
                    result_file
                  else
                    test_file_str[0] + ':' + test_file_str[1]
                  end
      result_output[:source][:path] = File.dirname(test_file)
      result_output[:source][:file] = File.basename(test_file)

      # save result_output
      @unit_name = File.basename(test_file, '.*')

      write_suite_header(result_output[:counts], f)
      write_failures(result_output, f)
      write_tests(result_output, f)
      write_ignored(result_output, f)
      write_suite_footer(f)
    end
    write_suites_footer(f)
    f.close
  end

  def usage(err_msg = nil)
    puts "\nERROR: "
    puts err_msg if err_msg
    puts 'Usage: unity_to_junit.rb [options]'
    puts ''
    puts 'Specific options:'
    puts '    -r, --results <dir>              Look for Unity Results files here.'
    puts '    -p, --root_path <path>           Prepend this path to files in results.'
    puts '    -o, --output <filename>          XML file to generate.'
    puts ''
    puts 'Common options:'
    puts '    -h, --help                       Show this message'
    puts '        --version                    Show version'

    exit 1
  end

  protected

  def get_details(_result_file, lines)
    results = results_structure
    lines.each do |line|
      line = line.tr('\\', '/')
      _src_file, src_line, test_name, status, msg = line.split(/:/)
      case status
      when 'IGNORE' then results[:ignores] << { test: test_name, line: src_line, message: msg }
      when 'FAIL'   then results[:failures] << { test: test_name, line: src_line, message: msg }
      when 'PASS'   then results[:successes] << { test: test_name, line: src_line, message: msg }
      end
    end
    results
  end

  def parse_test_summary(summary)
    raise "Couldn't parse test results: #{summary}" unless summary.find { |v| v =~ /(\d+) Tests (\d+) Failures (\d+) Ignored/ }
    [Regexp.last_match(1).to_i, Regexp.last_match(2).to_i, Regexp.last_match(3).to_i]
  end

  def here
    File.expand_path(File.dirname(__FILE__))
  end

  private

  def results_structure
    {
      source: { path: '', file: '' },
      successes: [],
      failures: [],
      ignores: [],
      counts: { total: 0, passed: 0, failed: 0, ignored: 0 },
      stdout: []
    }
  end

  def write_xml_header(stream)
    stream.puts "<?xml version='1.0' encoding='utf-8' ?>"
  end

  def write_suites_header(stream)
    stream.puts '<testsuites>'
  end

  def write_suite_header(counts, stream)
    stream.puts "\t<testsuite errors=\"0\" skipped=\"#{counts[:ignored]}\" failures=\"#{counts[:failed]}\" tests=\"#{counts[:total]}\" name=\"unity\">"
  end

  def write_failures(results, stream)
    result = results[:failures]
    result.each do |item|
      filename = File.join(results[:source][:path], File.basename(results[:source][:file], '.*'))
      stream.puts "\t\t<testcase classname=\"#{@unit_name}\" name=\"#{item[:test]}\" time=\"0\">"
      stream.puts "\t\t\t<failure message=\"#{item[:message]}\" type=\"Assertion\"/>"
      stream.puts "\t\t\t<system-err>&#xD;[File] #{filename}&#xD;[Line] #{item[:line]}&#xD;</system-err>"
      stream.puts "\t\t</testcase>"
    end
  end

  def write_tests(results, stream)
    result = results[:successes]
    result.each do |item|
      stream.puts "\t\t<testcase classname=\"#{@unit_name}\" name=\"#{item[:test]}\" time=\"0\" />"
    end
  end

  def write_ignored(results, stream)
    result = results[:ignores]
    result.each do |item|
      filename = File.join(results[:source][:path], File.basename(results[:source][:file], '.*'))
      puts "Writing ignored tests for test harness: #{filename}"
      stream.puts "\t\t<testcase classname=\"#{@unit_name}\" name=\"#{item[:test]}\" time=\"0\">"
      stream.puts "\t\t\t<skipped message=\"#{item[:message]}\" type=\"Assertion\"/>"
      stream.puts "\t\t\t<system-err>&#xD;[File] #{filename}&#xD;[Line] #{item[:line]}&#xD;</system-err>"
      stream.puts "\t\t</testcase>"
    end
  end

  def write_suite_footer(stream)
    stream.puts "\t</testsuite>"
  end

  def write_suites_footer(stream)
    stream.puts '</testsuites>'
  end
end # UnityToJUnit

if __FILE__ == $0
  # parse out the command options
  options = ArgvParser.parse(ARGV)

  # create an instance to work with
  utj = UnityToJUnit.new
  begin
    # look in the specified or current directory for result files
    targets = "#{options.results_dir.tr('\\', '/')}**/*.test*"

    results = Dir[targets]
    raise "No *.testpass, *.testfail, or *.testresults files found in '#{targets}'" if results.empty?
    utj.targets = results

    # set the root path
    utj.root = options.root_path

    # set the output XML file name
    # puts "Output File from options: #{options.out_file}"
    utj.out_file = options.out_file

    # run the summarizer
    puts utj.run
  rescue StandardError => e
    utj.usage e.message
  end
end
