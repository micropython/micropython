# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require 'yaml'
require 'fileutils'
require HERE + '../../auto/unity_test_summary'
require HERE + '../../auto/generate_test_runner'
require HERE + '../../auto/colour_reporter'

module RakefileHelpers
  C_EXTENSION = '.c'.freeze

  def load_configuration(config_file)
    return if $configured

    $cfg_file = HERE + "../../test/targets/#{config_file}" unless config_file =~ /[\\|\/]/
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

  def build_compiler_fields
    command = tackit($cfg['compiler']['path'])
    defines = if $cfg['compiler']['defines']['items'].nil?
                ''
              else
                squash($cfg['compiler']['defines']['prefix'], $cfg['compiler']['defines']['items'] + ['UNITY_OUTPUT_CHAR=UnityOutputCharSpy_OutputChar'] + ['UNITY_OUTPUT_CHAR_HEADER_DECLARATION=UnityOutputCharSpy_OutputChar\(int\)'])
              end
    options  = squash('', $cfg['compiler']['options'])
    includes = squash($cfg['compiler']['includes']['prefix'], $cfg['compiler']['includes']['items'])
    includes = includes.gsub(/\\ /, ' ').gsub(/\\\"/, '"').gsub(/\\$/, '') # Remove trailing slashes (for IAR)

    { command: command, defines: defines, options: options, includes: includes }
  end

  def compile(file, _defines = [])
    compiler = build_compiler_fields
    unity_include = $cfg['compiler']['includes']['prefix'] + '../../src'
    cmd_str = "#{compiler[:command]}#{compiler[:defines]}#{compiler[:options]}#{compiler[:includes]} #{unity_include} #{file} " \
              "#{$cfg['compiler']['object_files']['prefix']}#{$cfg['compiler']['object_files']['destination']}" \
              "#{File.basename(file, C_EXTENSION)}#{$cfg['compiler']['object_files']['extension']}"

    execute(cmd_str)
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

    { command: command, options: options, includes: includes }
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
    { command: command, pre_support: pre_support, post_support: post_support }
  end

  def execute(command_string, verbose = true)
    report command_string
    output = `#{command_string}`.chomp
    report(output) if verbose && !output.nil? && !output.empty?
    raise "Command failed. (Returned #{$?.exitstatus})" if $?.exitstatus != 0
    output
  end

  def report_summary
    summary = UnityTestSummary.new
    summary.root = HERE
    results_glob = "#{$cfg['compiler']['build_path']}*.test*"
    results_glob.tr!('\\', '/')
    results = Dir[results_glob]
    summary.targets = results
    summary.run
  end

  def run_tests
    report 'Running Unity system tests...'

    # Tack on TEST define for compiling unit tests
    load_configuration($cfg_file)
    test_defines = ['TEST']
    $cfg['compiler']['defines']['items'] = [] if $cfg['compiler']['defines']['items'].nil?

    # Get a list of all source files needed
    src_files  = Dir[HERE + 'src/*.c']
    src_files += Dir[HERE + 'test/*.c']
    src_files += Dir[HERE + 'test/main/*.c']
    src_files << '../../src/unity.c'

    # Build object files
    src_files.each { |f| compile(f, test_defines) }
    obj_list = src_files.map { |f| File.basename(f.ext($cfg['compiler']['object_files']['extension'])) }

    # Link the test executable
    test_base = 'framework_test'
    link_it(test_base, obj_list)

    # Execute unit test and generate results file
    simulator = build_simulator_fields
    executable = $cfg['linker']['bin_files']['destination'] + test_base + $cfg['linker']['bin_files']['extension']
    cmd_str = if simulator.nil?
                executable + ' -v -r'
              else
                "#{simulator[:command]} #{simulator[:pre_support]} #{executable} #{simulator[:post_support]}"
              end
    output = execute(cmd_str)
    test_results = $cfg['compiler']['build_path'] + test_base
    test_results += if output.match(/OK$/m).nil?
                      '.testfail'
                    else
                      '.testpass'
                    end
    File.open(test_results, 'w') { |f| f.print output }
  end
end
