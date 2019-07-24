# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require 'yaml'
require 'fileutils'
require '../vendor/unity/auto/generate_test_runner'
require '../vendor/unity/auto/unity_test_summary'
require '../vendor/unity/auto/colour_reporter.rb'
require './system/systest_generator'

module RakefileHelpers

  SYSTEST_GENERATED_FILES_PATH   = './system/generated/'
  SYSTEST_BUILD_FILES_PATH       = './system/build/'
  SYSTEST_COMPILE_MOCKABLES_PATH = './system/test_compilation/'
  C_EXTENSION = '.c'
  RESULT_EXTENSION = '.result'

  def load_configuration(config_file)
    $cfg_file = config_file
    $cfg = YAML.load(File.read('./targets/' + $cfg_file))
    $colour_output = false unless $cfg['colour']
  end

  def configure_clean
    CLEAN.include(SYSTEST_GENERATED_FILES_PATH + '*.*')
    CLEAN.include(SYSTEST_BUILD_FILES_PATH + '*.*')
  end

  def configure_toolchain(config_file)
    load_configuration(config_file)
    configure_clean
  end

  def get_local_include_dirs
    include_dirs = $cfg['compiler']['includes']['items'].dup
    include_dirs.delete_if {|dir| dir.is_a?(Array)}
    return include_dirs
  end

  def extract_headers(filename)
    includes = []
    lines = File.readlines(filename)
    lines.each do |line|
      m = line.match(/^\s*#include\s+\"\s*(.+\.[hH])\s*\"/)
      if not m.nil?
        includes << m[1]
      end
    end
    return includes
  end

  def find_source_file(header, paths)
    paths.each do |dir|
      src_file = dir + header.ext(C_EXTENSION)
      if (File.exists?(src_file))
        return src_file
      end
    end
    return nil
  end

  def squash(prefix, items)
    result = ''
    items.each { |item| result += " #{prefix}#{tackit(item)}" }
    return result
  end

  def build_compiler_fields
    command  = tackit($cfg['compiler']['path'])
    if $cfg['compiler']['defines']['items'].nil?
      defines  = ''
    else
      defines  = squash($cfg['compiler']['defines']['prefix'], $cfg['compiler']['defines']['items'])
    end
    options  = squash('', $cfg['compiler']['options'])
    includes = squash($cfg['compiler']['includes']['prefix'], $cfg['compiler']['includes']['items'])
    includes = includes.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)
    return {:command => command, :defines => defines, :options => options, :includes => includes}
  end

  def compile(file, defines=[])
    compiler = build_compiler_fields
    cmd_str = "#{compiler[:command]}#{compiler[:defines]}#{defines.inject(''){|all, a| ' -D'+a+all }}#{compiler[:options]}#{compiler[:includes]} #{file} " +
      "#{$cfg['compiler']['object_files']['prefix']}#{$cfg['compiler']['object_files']['destination']}"
    obj_file = "#{File.basename(file, C_EXTENSION)}#{$cfg['compiler']['object_files']['extension']}"
    execute(cmd_str + obj_file)
    return obj_file
  end

  def build_linker_fields
    command  = tackit($cfg['linker']['path'])
    if $cfg['linker']['options'].nil?
      options  = ''
    else
      options  = squash('', $cfg['linker']['options'])
    end
    if ($cfg['linker']['includes'].nil? || $cfg['linker']['includes']['items'].nil?)
      includes = ''
    else
      includes = squash($cfg['linker']['includes']['prefix'], $cfg['linker']['includes']['items'])
    end
    includes = includes.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)
    return {:command => command, :options => options, :includes => includes}
  end

  def link_it(exe_name, obj_list)
    linker = build_linker_fields
    cmd_str = "#{linker[:command]}#{linker[:options]}#{linker[:includes]} " +
      (obj_list.map{|obj|"#{$cfg['linker']['object_files']['path']}#{obj} "}).uniq.join +
      $cfg['linker']['bin_files']['prefix'] + ' ' +
      $cfg['linker']['bin_files']['destination'] +
      exe_name + $cfg['linker']['bin_files']['extension']
    execute(cmd_str)
  end

  def build_simulator_fields
    return nil if $cfg['simulator'].nil?
    if $cfg['simulator']['path'].nil?
      command = ''
    else
      command = (tackit($cfg['simulator']['path']) + ' ')
    end
    if $cfg['simulator']['pre_support'].nil?
      pre_support = ''
    else
      pre_support = squash('', $cfg['simulator']['pre_support'])
    end
    if $cfg['simulator']['post_support'].nil?
      post_support = ''
    else
      post_support = squash('', $cfg['simulator']['post_support'])
    end
    return {:command => command, :pre_support => pre_support, :post_support => post_support}
  end

  def execute(command_string, verbose=true, raise_on_failure=true)
    #report command_string
    output = `#{command_string}`.chomp
    report(output) if (verbose && !output.nil? && (output.length > 0))
    if ($?.exitstatus != 0) and (raise_on_failure)
      raise "#{command_string} failed. (Returned #{$?.exitstatus})"
    end
    return output
  end

  def tackit(strings)
    case(strings)
      when Array
        "\"#{strings.join}\""
      when /^-/
        strings
      when /\s/
        "\"#{strings}\""
      else
        strings
    end
  end

  def report_summary
    summary = UnityTestSummary.new
    summary.root = File.expand_path(File.dirname(__FILE__)) + '/'
    results_glob = "#{$cfg['compiler']['build_path']}*.test*"
    results_glob.gsub!(/\\/, '/')
    results = Dir[results_glob]
    summary.targets = results
    summary.run
    fail_out "FAIL: There were failures" if (summary.failures > 0)
  end

  def run_system_test_interactions(test_case_files)
    load '../lib/cmock.rb'

    SystemTestGenerator.new.generate_files(test_case_files)
    test_files = FileList.new(SYSTEST_GENERATED_FILES_PATH + 'test*.c')

    load_configuration($cfg_file)
    $cfg['compiler']['defines']['items'] = [] if $cfg['compiler']['defines']['items'].nil?

    include_dirs = get_local_include_dirs

    # Build and execute each unit test
    test_files.each do |test|

      obj_list = []

      test_base    = File.basename(test, C_EXTENSION)
      cmock_config = test_base.gsub(/test_/, '') + '_cmock.yml'

      report "Executing system tests in #{File.basename(test)}..."

      # Detect dependencies and build required required modules
      extract_headers(test).each do |header|

        # Generate any needed mocks
        if header =~ /^mock_(.*)\.h/i
          module_name = $1
          cmock = CMock.new(SYSTEST_GENERATED_FILES_PATH + cmock_config)
          cmock.setup_mocks("#{$cfg['compiler']['source_path']}#{module_name}.h")
        end
        # Compile corresponding source file if it exists
        src_file = find_source_file(header, include_dirs)
        if !src_file.nil?
          obj_list << compile(src_file)
        end
      end

      # Generate and build the test suite runner
      runner_name = test_base + '_runner.c'
      runner_path = $cfg['compiler']['source_path'] + runner_name
      UnityTestRunnerGenerator.new(SYSTEST_GENERATED_FILES_PATH + cmock_config).run(test, runner_path)
      obj_list << compile(runner_path)

      # Build the test module
      obj_list << compile(test)

      # Link the test executable
      link_it(test_base, obj_list)

      # Execute unit test and generate results file
      simulator = build_simulator_fields
      executable = $cfg['linker']['bin_files']['destination'] + test_base + $cfg['linker']['bin_files']['extension']
      if simulator.nil?
        cmd_str = executable
      else
        cmd_str = "#{simulator[:command]} #{simulator[:pre_support]} #{executable} #{simulator[:post_support]}"
      end
      output = execute(cmd_str, false, false)
      test_results = $cfg['compiler']['build_path'] + test_base + RESULT_EXTENSION
      File.open(test_results, 'w') { |f| f.print output }
    end

    # Parse and report test results
    total_tests = 0
    total_failures = 0
    failure_messages = []

    test_case_files.each do |test_case|
      tests = (YAML.load_file(test_case))[:systest][:tests][:units]
      total_tests += tests.size

      test_file    = 'test_' + File.basename(test_case).ext(C_EXTENSION)
      result_file  = test_file.ext(RESULT_EXTENSION)
      test_results = File.readlines(SYSTEST_BUILD_FILES_PATH + result_file).reject {|line| line.size < 10 } # we're rejecting lines that are too short to be realistic, which handles line ending problems
      tests.each_with_index do |test, index|
        # compare test's intended pass/fail state with pass/fail state in actual results;
        # if they don't match, the system test has failed
        this_failed = case(test[:pass])
          when :ignore
            (test_results[index] =~ /:IGNORE/).nil?
          when true
            (test_results[index] =~ /:PASS/).nil?
          when false
            (test_results[index] =~ /:FAIL/).nil?
        end
        if (this_failed)
          total_failures += 1
          test_results[index] =~ /test#{index+1}:(.+)/
          failure_messages << "#{test_file}:test#{index+1}:should #{test[:should]}:#{$1}"
        end
        # some tests have additional requirements to check for (checking the actual output message)
        if (test[:verify_error]) and not (test_results[index] =~ /test#{index+1}:.*#{test[:verify_error]}/)
          total_failures += 1
          failure_messages << "#{test_file}:test#{index+1}:should #{test[:should]}:should have output matching '#{test[:verify_error]}' but was '#{test_results[index]}'"
        end
      end
    end

    report "\n"
    report "------------------------------------\n"
    report "SYSTEM TEST MOCK INTERACTION SUMMARY\n"
    report "------------------------------------\n"
    report "#{total_tests} Tests #{total_failures} Failures 0 Ignored\n"
    report "\n"

    if (failure_messages.size > 0)
      report 'System test failures:'
      failure_messages.each do |failure|
        report failure
      end
    end

    report ''

    return total_failures
  end

  def profile_this(filename)
    profile = true
    begin
      require 'ruby-prof'
      RubyProf.start
    rescue
      profile = false
    end

    yield

    if (profile)
      profile_result = RubyProf.stop
      File.open("Profile_#{filename}.html", 'w') do |f|
        RubyProf::GraphHtmlPrinter.new(profile_result).print(f)
      end
    end
  end

  def run_system_test_compilations(mockables)
    load '../lib/cmock.rb'

    load_configuration($cfg_file)
    $cfg['compiler']['defines']['items'] = [] if $cfg['compiler']['defines']['items'].nil?

    report "\n"
    report "------------------------------------\n"
    report "SYSTEM TEST MOCK COMPILATION SUMMARY\n"
    report "------------------------------------\n"
    mockables.each do |header|
      mock_filename = 'mock_' + File.basename(header).ext('.c')
      CMock.new(SYSTEST_COMPILE_MOCKABLES_PATH + 'config.yml').setup_mocks(header)
      report "Compiling #{mock_filename}..."
      compile(SYSTEST_GENERATED_FILES_PATH + mock_filename)
    end
  end

  def run_system_test_profiles(mockables)
    load '../lib/cmock.rb'

    load_configuration($cfg_file)
    $cfg['compiler']['defines']['items'] = [] if $cfg['compiler']['defines']['items'].nil?

    report "\n"
    report "--------------------------\n"
    report "SYSTEM TEST MOCK PROFILING\n"
    report "--------------------------\n"
    mockables.each do |header|
      mock_filename = 'mock_' + File.basename(header).ext('.c')
      profile_this(mock_filename.gsub('.c','')) do
        10.times do
          CMock.new(SYSTEST_COMPILE_MOCKABLES_PATH + 'config.yml').setup_mocks(header)
        end
      end
      report "Compiling #{mock_filename}..."
      compile(SYSTEST_GENERATED_FILES_PATH + mock_filename)
    end
  end

  def build_and_test_c_files
    report "\n"
    report "----------------\n"
    report "UNIT TEST C CODE\n"
    report "----------------\n"
    errors = false
    FileList.new("c/*.yml").each do |yaml_file|
      test = YAML.load(File.read(yaml_file))
      report "\nTesting #{yaml_file.sub('.yml','')}"
      report "(#{test[:options].join(', ')})"
      test[:files].each { |f| compile(f, test[:options]) }
      obj_files = test[:files].map { |f| f.gsub!(/.*\//,'').gsub!(C_EXTENSION, $cfg['compiler']['object_files']['extension']) }
      link_it('TestCMockC', obj_files)
      if $cfg['simulator'].nil?
        execute($cfg['linker']['bin_files']['destination'] + 'TestCMockC' + $cfg['linker']['bin_files']['extension'])
      else
        execute(tackit($cfg['simulator']['path'].join) + ' ' +
            $cfg['simulator']['pre_support'].map{|o| tackit(o)}.join(' ') + ' ' +
            $cfg['linker']['bin_files']['destination'] +
            'TestCMockC' +
            $cfg['linker']['bin_files']['extension'] + ' ' +
            $cfg['simulator']['post_support'].map{|o| tackit(o)}.join(' ') )
      end
    end
  end

  def fail_out(msg)
    puts msg
    exit(-1)
  end
end

