# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

HERE = File.expand_path(File.dirname(__FILE__)) + '/'

require 'rake'
require 'rake/clean'
require 'rake/testtask'
require HERE + 'rakefile_helper'

TEMP_DIRS = [
  File.join(HERE, 'build')
].freeze

TEMP_DIRS.each do |dir|
  directory(dir)
  CLOBBER.include(dir)
end

task prepare_for_tests: TEMP_DIRS

include RakefileHelpers

# Load default configuration, for now
DEFAULT_CONFIG_FILE = 'gcc_auto_stdint.yml'.freeze
configure_toolchain(DEFAULT_CONFIG_FILE)

task unit: [:prepare_for_tests] do
  run_tests
end

desc 'Build and test Unity Framework'
task all: %i(clean unit)
task default: %i(clobber all)
task ci: %i(no_color default)
task cruise: %i(no_color default)

desc 'Load configuration'
task :config, :config_file do |_t, args|
  configure_toolchain(args[:config_file])
end

task :no_color do
  $colour_output = false
end
