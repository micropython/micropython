
suppress_error = !ARGV.nil? && !ARGV.empty? && (ARGV[0].upcase == "--SILENT")

begin
  require "#{ENV['UNITY_DIR']}/auto/unity_test_summary.rb"

  build_dir = ENV.fetch('BUILD_DIR', './build')
  test_build_dir = ENV.fetch('TEST_BUILD_DIR', File.join(build_dir, 'test'))

  results = Dir["#{test_build_dir}/*.testresult"]
  parser = UnityTestSummary.new
  parser.targets = results
  parser.run
  puts parser.report
rescue StandardError => e
  raise e unless suppress_error
end

exit(parser.failures) unless suppress_error
