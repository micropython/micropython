# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require 'yaml'
require 'fileutils'
require UNITY_ROOT + '../auto/unity_test_summary'
require UNITY_ROOT + '../auto/generate_test_runner'
require UNITY_ROOT + '../auto/colour_reporter'

module RakefileHelpers
  C_EXTENSION = '.c'.freeze
  def load_configuration(config_file)
    return if $configured

    $cfg_file = "targets/#{config_file}" unless config_file =~ /[\\|\/]/
    $cfg = YAML.load(File.read($cfg_file))
    $colour_output = false unless $cfg['colour']
    $configured = true if config_file != DEFAULT_CONFIG_FILE
  end

  def configure_clean
    CLEAN.include($cfg['compiler']['build_path'] + '*.*') unless $cfg['compiler']['build_path'].nil?
  end

  def configure_toolchain(config_file = DEFAULT_CONFIG_FILE)
    config_file += '.yml' unless config_file =~ /\.yml$/
    config_file = config_file unless config_file =~ /[\\|\/]/
    load_configuration(config_file)
    configure_clean
  end

  def unit_test_files
    path = $cfg['compiler']['unit_tests_path'] + 'test*' + C_EXTENSION
    path.tr!('\\', '/')
    FileList.new(path)
  end

  def local_include_dirs
    include_dirs = $cfg['compiler']['includes']['items'].dup
    include_dirs.delete_if { |dir| dir.is_a?(Array) }
    include_dirs
  end

  def extract_headers(filename)
    includes = []
    lines = File.readlines(filename)
    lines.each do |line|
      m = line.match(/^\s*#include\s+\"\s*(.+\.[hH])\s*\"/)
      includes << m[1] unless m.nil?
    end
    includes
  end

  def find_source_file(header, paths)
    paths.each do |dir|
      src_file = dir + header.ext(C_EXTENSION)
      return src_file if File.exist?(src_file)
    end
    nil
  end

  def tackit(strings)
    result = if strings.is_a?(Array)
               "\"#{strings.join}\""
             else
               strings
             end
    result
  end

  def squash(prefix, items)
    result = ''
    items.each { |item| result += " #{prefix}#{tackit(item)}" }
    result
  end

  def should(behave, &block)
    if block
      puts 'Should ' + behave
      yield block
    else
      puts "UNIMPLEMENTED CASE: Should #{behave}"
    end
  end

  def build_compiler_fields(inject_defines)
    command = tackit($cfg['compiler']['path'])
    defines = if $cfg['compiler']['defines']['items'].nil?
                ''
              else
                squash($cfg['compiler']['defines']['prefix'], $cfg['compiler']['defines']['items'] + ['UNITY_OUTPUT_CHAR=putcharSpy'] + ['UNITY_OUTPUT_CHAR_HEADER_DECLARATION=putcharSpy\(int\)'] + inject_defines)
              end
    options = squash('', $cfg['compiler']['options'])
    includes = squash($cfg['compiler']['includes']['prefix'], $cfg['compiler']['includes']['items'])
    includes = includes.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)

    { :command => command, :defines => defines, :options => options, :includes => includes }
  end

  def compile(file, defines = [])
    compiler = build_compiler_fields(defines)
    cmd_str  = "#{compiler[:command]}#{compiler[:defines]}#{compiler[:options]}#{compiler[:includes]} #{file} " \
               "#{$cfg['compiler']['object_files']['prefix']}#{$cfg['compiler']['object_files']['destination']}"
    obj_file = "#{File.basename(file, C_EXTENSION)}#{$cfg['compiler']['object_files']['extension']}"
    execute(cmd_str + obj_file)

    obj_file
  end

  def build_linker_fields
    command = tackit($cfg['linker']['path'])
    options = if $cfg['linker']['options'].nil?
                ''
              else
                squash('', $cfg['linker']['options'])
              end
    includes = if $cfg['linker']['includes'].nil? || $cfg['linker']['includes']['items'].nil?
                 ''
               else
                 squash($cfg['linker']['includes']['prefix'], $cfg['linker']['includes']['items'])
               end.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)

    { :command => command, :options => options, :includes => includes }
  end

  def link_it(exe_name, obj_list)
    linker = build_linker_fields
    cmd_str = "#{linker[:command]}#{linker[:options]}#{linker[:includes]} " +
              (obj_list.map { |obj| "#{$cfg['linker']['object_files']['path']}#{obj} " }).join +
              $cfg['linker']['bin_files']['prefix'] + ' ' +
              $cfg['linker']['bin_files']['destination'] +
              exe_name + $cfg['linker']['bin_files']['extension']
    execute(cmd_str)
  end

  def build_simulator_fields
    return nil if $cfg['simulator'].nil?
    command = if $cfg['simulator']['path'].nil?
                ''
              else
                (tackit($cfg['simulator']['path']) + ' ')
              end
    pre_support = if $cfg['simulator']['pre_support'].nil?
                    ''
                  else
                    squash('', $cfg['simulator']['pre_support'])
                  end
    post_support = if $cfg['simulator']['post_support'].nil?
                     ''
                   else
                     squash('', $cfg['simulator']['post_support'])
                   end

    { :command => command, :pre_support => pre_support, :post_support => post_support }
  end

  def run_astyle(style_what)
    report "Styling C Code..."
    command = "AStyle " \
              "--style=allman --indent=spaces=4 --indent-switches --indent-preproc-define --indent-preproc-block " \
              "--pad-oper --pad-comma --unpad-paren --pad-header " \
              "--align-pointer=type --align-reference=name " \
              "--add-brackets --mode=c --suffix=none " \
              "#{style_what}"
    execute(command, false)
    report "Styling C:PASS"
  end

  def execute(command_string, ok_to_fail = false)
    report command_string if $verbose
    output = `#{command_string}`.chomp
    report(output) if $verbose && !output.nil? && !output.empty?
    raise "Command failed. (Returned #{$?.exitstatus})" if !$?.exitstatus.zero? && !ok_to_fail
    output
  end

  def report_summary
    summary = UnityTestSummary.new
    summary.root = UNITY_ROOT
    results_glob = "#{$cfg['compiler']['build_path']}*.test*"
    results_glob.tr!('\\', '/')
    results = Dir[results_glob]
    summary.targets = results
    report summary.run
  end

  def run_tests(test_files)
    report 'Running Unity system tests...'

    # Tack on TEST define for compiling unit tests
    load_configuration($cfg_file)
    test_defines = ['TEST']
    $cfg['compiler']['defines']['items'] ||= []
    $cfg['compiler']['defines']['items'] << 'TEST'

    include_dirs = local_include_dirs

    # Build and execute each unit test
    test_files.each do |test|
      obj_list = []

      unless $cfg['compiler']['aux_sources'].nil?
        $cfg['compiler']['aux_sources'].each do |aux|
          obj_list << compile(aux, test_defines)
        end
      end

      # Detect dependencies and build required modules
      extract_headers(test).each do |header|
        # Compile corresponding source file if it exists
        src_file = find_source_file(header, include_dirs)

        obj_list << compile(src_file, test_defines) unless src_file.nil?
      end

      # Build the test runner (generate if configured to do so)
      test_base = File.basename(test, C_EXTENSION)

      runner_name = test_base + '_Runner.c'

      runner_path = if $cfg['compiler']['runner_path'].nil?
                      $cfg['compiler']['build_path'] + runner_name
                    else
                      $cfg['compiler']['runner_path'] + runner_name
                    end

      options = $cfg[:unity]
      options[:use_param_tests] = test =~ /parameterized/ ? true : false
      UnityTestRunnerGenerator.new(options).run(test, runner_path)
      obj_list << compile(runner_path, test_defines)

      # Build the test module
      obj_list << compile(test, test_defines)

      # Link the test executable
      link_it(test_base, obj_list)

      # Execute unit test and generate results file
      simulator = build_simulator_fields
      executable = $cfg['linker']['bin_files']['destination'] + test_base + $cfg['linker']['bin_files']['extension']
      cmd_str = if simulator.nil?
                  executable
                else
                  "#{simulator[:command]} #{simulator[:pre_support]} #{executable} #{simulator[:post_support]}"
                end
      output = execute(cmd_str)
      test_results = $cfg['compiler']['build_path'] + test_base
      if output.match(/OK$/m).nil?
        test_results += '.testfail'
      else
        report output unless $verbose # Verbose already prints this line, as does a failure
        test_results += '.testpass'
      end
      File.open(test_results, 'w') { |f| f.print output }
    end
  end
end
