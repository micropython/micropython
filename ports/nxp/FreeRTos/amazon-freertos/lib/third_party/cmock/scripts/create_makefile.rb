require 'fileutils'
ABS_ROOT = FileUtils.pwd
CMOCK_DIR = File.expand_path(ENV.fetch('CMOCK_DIR', File.join(ABS_ROOT, '..', '..')))
require "#{CMOCK_DIR}/lib/cmock"
UNITY_DIR = File.join(CMOCK_DIR, 'vendor', 'unity')
require "#{UNITY_DIR}/auto/generate_test_runner"

SRC_DIR =  ENV.fetch('SRC_DIR',  './src')
TEST_DIR = ENV.fetch('TEST_DIR', './test')
UNITY_SRC = File.join(UNITY_DIR, 'src')
CMOCK_SRC = File.join(CMOCK_DIR, 'src')
BUILD_DIR = ENV.fetch('BUILD_DIR', './build')
TEST_BUILD_DIR = ENV.fetch('TEST_BUILD_DIR', File.join(BUILD_DIR, 'test'))
OBJ_DIR = File.join(TEST_BUILD_DIR, 'obj')
UNITY_OBJ = File.join(OBJ_DIR, 'unity.o')
CMOCK_OBJ = File.join(OBJ_DIR, 'cmock.o')
RUNNERS_DIR = File.join(TEST_BUILD_DIR, 'runners')
MOCKS_DIR = File.join(TEST_BUILD_DIR, 'mocks')
TEST_BIN_DIR = TEST_BUILD_DIR
MOCK_PREFIX = ENV.fetch('TEST_MOCK_PREFIX', 'mock_')
MOCK_SUFFIX = ENV.fetch('TEST_MOCK_SUFFIX', '')
TEST_MAKEFILE = ENV.fetch('TEST_MAKEFILE', File.join(TEST_BUILD_DIR, 'MakefileTestSupport'))
MOCK_MATCHER = /#{MOCK_PREFIX}[A-Za-z_][A-Za-z0-9_\-\.]+#{MOCK_SUFFIX}/

[TEST_BUILD_DIR, OBJ_DIR, RUNNERS_DIR, MOCKS_DIR, TEST_BIN_DIR].each do |dir|
  FileUtils.mkdir_p dir
end

all_headers_to_mock = []

suppress_error = !ARGV.nil? && !ARGV.empty? && (ARGV[0].upcase == "--SILENT")

File.open(TEST_MAKEFILE, "w") do |mkfile|

  # Define make variables
  mkfile.puts "CC ?= gcc"
  mkfile.puts "BUILD_DIR ?= ./build"
  mkfile.puts "SRC_DIR ?= ./src"
  mkfile.puts "TEST_DIR ?= ./test"
  mkfile.puts "TEST_CFLAGS ?= -DTEST"
  mkfile.puts "CMOCK_DIR ?= #{CMOCK_DIR}"
  mkfile.puts "UNITY_DIR ?= #{UNITY_DIR}"
  mkfile.puts "TEST_BUILD_DIR ?= ${BUILD_DIR}/test"
  mkfile.puts "TEST_MAKEFILE = ${TEST_BUILD_DIR}/MakefileTestSupport"
  mkfile.puts "OBJ ?= ${BUILD_DIR}/obj"
  mkfile.puts "OBJ_DIR = ${OBJ}"
  mkfile.puts ""

  # Build Unity
  mkfile.puts "#{UNITY_OBJ}: #{UNITY_SRC}/unity.c"
  mkfile.puts "\t${CC} -o $@ -c $< -I #{UNITY_SRC}"
  mkfile.puts ""

  # Build CMock
  mkfile.puts "#{CMOCK_OBJ}: #{CMOCK_SRC}/cmock.c"
  mkfile.puts "\t${CC} -o $@ -c $< -I #{UNITY_SRC} -I #{CMOCK_SRC}"
  mkfile.puts ""

  test_sources = Dir["#{TEST_DIR}/**/test_*.c"]
  test_targets = []
  generator = UnityTestRunnerGenerator.new
  all_headers = Dir["#{SRC_DIR}/**/{[!#{MOCK_PREFIX}]}*{[!#{MOCK_SUFFIX}]}.h"]  #headers that begin with prefix or end with suffix are not included
  makefile_targets = []

  test_sources.each do |test|
    module_name = File.basename(test, '.c')
    src_module_name = module_name.sub(/^test_/, '')
    test_obj = File.join(OBJ_DIR, "#{module_name}.o")
    runner_source = File.join(RUNNERS_DIR, "runner_#{module_name}.c")
    runner_obj = File.join(OBJ_DIR, "runner_#{module_name}.o")
    test_bin = File.join(TEST_BIN_DIR, module_name)
    test_results = File.join(TEST_BIN_DIR, module_name + '.testresult')

    cfg = {
      src: test,
      includes: generator.find_includes(File.readlines(test).join(''))
    }

    # Build main project modules, with TEST defined
    module_src = File.join(SRC_DIR, "#{src_module_name}.c")
    module_obj = File.join(OBJ_DIR, "#{src_module_name}.o")
    if not makefile_targets.include? module_obj
        makefile_targets.push(module_obj)
        mkfile.puts "#{module_obj}: #{module_src}"
        mkfile.puts "\t${CC} -o $@ -c $< ${TEST_CFLAGS} -I #{SRC_DIR} ${INCLUDE_PATH}"
        mkfile.puts ""
    end

    # process link-only files
    linkonly = cfg[:includes][:linkonly]
    linkonly_objs = []
    linkonly.each do |linkonlyfile|
        linkonlybase = File.basename(linkonlyfile)
        linkonlymodule_src = File.join(SRC_DIR, "#{linkonlyfile}.c")
        linkonlymodule_obj = File.join(OBJ_DIR, "#{linkonlybase}.o")
        linkonly_objs.push(linkonlymodule_obj)
        #only create the target if we didn't already
        if not makefile_targets.include? linkonlymodule_obj
            makefile_targets.push(linkonlymodule_obj)
            mkfile.puts "#{linkonlymodule_obj}: #{linkonlymodule_src}"
            mkfile.puts "\t${CC} -o $@ -c $< ${TEST_CFLAGS} -I #{SRC_DIR} ${INCLUDE_PATH}"
            mkfile.puts ""
        end
    end

    # Create runners
    mkfile.puts "#{runner_source}: #{test}"
    mkfile.puts "\t@UNITY_DIR=${UNITY_DIR} ruby ${CMOCK_DIR}/scripts/create_runner.rb #{test} #{runner_source}"
    mkfile.puts ""

    # Build runner
    mkfile.puts "#{runner_obj}: #{runner_source}"
    mkfile.puts "\t${CC} -o $@ -c $< ${TEST_CFLAGS} -I #{SRC_DIR} -I #{MOCKS_DIR} -I #{UNITY_SRC} -I #{CMOCK_SRC} ${INCLUDE_PATH}"
    mkfile.puts ""

    # Collect mocks to generate
    system_mocks = cfg[:includes][:system].select{|name| name =~ MOCK_MATCHER}
    raise "Mocking of system headers is not yet supported!" if !system_mocks.empty?
    local_mocks = cfg[:includes][:local].select{|name| name =~ MOCK_MATCHER}

    module_names_to_mock = local_mocks.map{|name| "#{name.sub(/#{MOCK_PREFIX}/,'')}.h"}
    headers_to_mock = []
    module_names_to_mock.each do |name|
      header_to_mock = nil
      all_headers.each do |header|
        if (header =~ /[\/\\]?#{name}$/)
          header_to_mock = header
          break
        end
      end
      raise "Module header '#{name}' not found to mock!" unless header_to_mock
      headers_to_mock << header_to_mock
    end

    all_headers_to_mock += headers_to_mock
    mock_objs = headers_to_mock.map do |hdr|
      mock_name = MOCK_PREFIX + File.basename(hdr, '.h')
      File.join(MOCKS_DIR, mock_name + '.o')
    end
    all_headers_to_mock.uniq!

    # Build test suite
    mkfile.puts "#{test_obj}: #{test} #{module_obj} #{mock_objs.join(' ')}"
    mkfile.puts "\t${CC} -o $@ -c $< ${TEST_CFLAGS} -I #{SRC_DIR} -I #{UNITY_SRC} -I #{CMOCK_SRC} -I #{MOCKS_DIR} ${INCLUDE_PATH}"
    mkfile.puts ""

    # Build test suite executable
    test_objs = "#{test_obj} #{runner_obj} #{module_obj} #{mock_objs.join(' ')} #{linkonly_objs.join(' ')} #{UNITY_OBJ} #{CMOCK_OBJ}"
    mkfile.puts "#{test_bin}: #{test_objs}"
    mkfile.puts "\t${CC} -o $@ ${LDFLAGS} #{test_objs}"
    mkfile.puts ""

    # Run test suite and generate report
    mkfile.puts "#{test_results}: #{test_bin}"
    mkfile.puts "\t-#{test_bin} > #{test_results} 2>&1"
    mkfile.puts ""

    test_targets << test_bin
  end

  # Generate and build mocks
  all_headers_to_mock.each do |hdr|
    mock_name = MOCK_PREFIX + File.basename(hdr, '.h')
    mock_header = File.join(MOCKS_DIR, mock_name + '.h')
    mock_src = File.join(MOCKS_DIR, mock_name + '.c')
    mock_obj = File.join(MOCKS_DIR, mock_name + '.o')

    mkfile.puts "#{mock_src}: #{hdr}"
    mkfile.puts "\t@CMOCK_DIR=${CMOCK_DIR} ruby ${CMOCK_DIR}/scripts/create_mock.rb #{hdr}"
    mkfile.puts ""

    mkfile.puts "#{mock_obj}: #{mock_src} #{mock_header}"
    mkfile.puts "\t${CC} -o $@ -c $< ${TEST_CFLAGS} -I #{MOCKS_DIR} -I #{SRC_DIR} -I #{UNITY_SRC} -I #{CMOCK_SRC} ${INCLUDE_PATH}"
    mkfile.puts ""
  end

  # Create test summary task
  mkfile.puts "test_summary:"
  mkfile.puts "\t@UNITY_DIR=${UNITY_DIR} ruby ${CMOCK_DIR}/scripts/test_summary.rb #{suppress_error ? '--silent' : ''}"
  mkfile.puts ""
  mkfile.puts ".PHONY: test_summary"
  mkfile.puts ""

  # Create target to run all tests
  mkfile.puts "test: #{test_targets.map{|t| t + '.testresult'}.join(' ')} test_summary"
  mkfile.puts ""

end
