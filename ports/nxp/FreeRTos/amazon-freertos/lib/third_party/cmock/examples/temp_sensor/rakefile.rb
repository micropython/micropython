HERE = File.expand_path(File.dirname(__FILE__)) + '/'

require 'rake'
require 'rake/clean'
require 'rake/testtask'
require './rakefile_helper'

include RakefileHelpers

REQUIRED_DIRS = [ './build', './build/mocks' ]
REQUIRED_DIRS.each do |v|
  directory v
end

# Load default configuration, for now
DEFAULT_CONFIG_FILE = 'gcc.yml'
configure_toolchain(DEFAULT_CONFIG_FILE)

task :unit do
  run_tests(get_unit_test_files)
end

desc "Generate test summary"
task :summary do
  report_summary
end

desc "Build and test Unity"
task :all => [:clean, :unit, :summary]
task :default => REQUIRED_DIRS + [:clobber, :all]
task :ci => [:default]
task :cruise => [:default]

desc "Load configuration"
task :config, :config_file do |t, args|
  configure_toolchain(args[:config_file])
end

desc "Return error on Failures"
task :strict do
  $return_error_on_failures = true
end
