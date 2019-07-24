# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_expect_any_args.rb'

describe CMockGeneratorPluginExpectAnyArgs, "Verify CMockGeneratorPluginExpectAnyArgs Module" do

  before do
    create_mocks :config, :utils
    @config = create_stub(:respond_to? => true)
    @cmock_generator_plugin_expect_any_args = CMockGeneratorPluginExpectAnyArgs.new(@config, @utils)
  end

  after do
  end

  it "have set up internal priority" do
    assert_equal(3, @cmock_generator_plugin_expect_any_args.priority)
  end

  it "not have any additional include file requirements" do
    assert(!@cmock_generator_plugin_expect_any_args.respond_to?(:include_files))
  end

  it "handle function declarations for functions without return values" do
    function = {:name => "Mold", :args_string => "void", :return => test_return[:void]}
    expected = "#define Mold_ExpectAnyArgs() Mold_CMockExpectAnyArgs(__LINE__)\nvoid Mold_CMockExpectAnyArgs(UNITY_LINE_TYPE cmock_line);\n"
    returned = @cmock_generator_plugin_expect_any_args.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "handle function declarations for functions that returns something" do
    function = {:name => "Fungus", :args_string => "void", :return => test_return[:string]}
    expected = "#define Fungus_ExpectAnyArgsAndReturn(cmock_retval) Fungus_CMockExpectAnyArgsAndReturn(__LINE__, cmock_retval)\n"+
               "void Fungus_CMockExpectAnyArgsAndReturn(UNITY_LINE_TYPE cmock_line, const char* cmock_to_return);\n"
    returned = @cmock_generator_plugin_expect_any_args.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "should not respond to implementation requests" do
    assert(!@cmock_generator_plugin_expect_any_args.respond_to?(:mock_implementation))
  end

  it "add a new mock interface for ignoring when function had no return value" do
    function = {:name => "Slime", :args => [], :args_string => "void", :return => test_return[:void]}
    expected = ["void Slime_CMockExpectAnyArgs(UNITY_LINE_TYPE cmock_line)\n",
                "{\n",
                "mock_return_1",
                "  cmock_call_instance->IgnoreMode = CMOCK_ARG_NONE;\n",
                "}\n\n"
               ].join
    @utils.expect :code_add_base_expectation, "mock_return_1", ["Slime", true]
    returned = @cmock_generator_plugin_expect_any_args.mock_interfaces(function)
    assert_equal(expected, returned)
  end
end
