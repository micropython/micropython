# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_ignore'

describe CMockGeneratorPluginIgnore, "Verify CMockGeneratorPluginIgnore Module" do

  before do
    create_mocks :config, :utils
    @config = create_stub(:respond_to? => true)
    @cmock_generator_plugin_ignore = CMockGeneratorPluginIgnore.new(@config, @utils)
  end

  after do
  end

  it "have set up internal priority" do
    assert_equal(2, @cmock_generator_plugin_ignore.priority)
  end

  it "not have any additional include file requirements" do
    assert(!@cmock_generator_plugin_ignore.respond_to?(:include_files))
  end

  it "add a required variable to the instance structure" do
    function = {:name => "Grass", :args => [], :return => test_return[:void]}
    expected = "  int Grass_IgnoreBool;\n"
    returned = @cmock_generator_plugin_ignore.instance_structure(function)
    assert_equal(expected, returned)
  end

  it "handle function declarations for functions without return values" do
    function = {:name => "Mold", :args_string => "void", :return => test_return[:void]}
    expected = "#define Mold_Ignore() Mold_CMockIgnore()\nvoid Mold_CMockIgnore(void);\n"
    returned = @cmock_generator_plugin_ignore.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "handle function declarations for functions that returns something" do
    function = {:name => "Fungus", :args_string => "void", :return => test_return[:string]}
    expected = "#define Fungus_IgnoreAndReturn(cmock_retval) Fungus_CMockIgnoreAndReturn(__LINE__, cmock_retval)\n"+
               "void Fungus_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, const char* cmock_to_return);\n"
    returned = @cmock_generator_plugin_ignore.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "add required code to implementation precheck with void function" do
    function = {:name => "Mold", :args_string => "void", :return => test_return[:void]}
    expected = ["  if (Mock.Mold_IgnoreBool)\n",
                "  {\n",
                "    UNITY_CLR_DETAILS();\n",
                "    return;\n",
                "  }\n"
               ].join
    returned = @cmock_generator_plugin_ignore.mock_implementation_precheck(function)
    assert_equal(expected, returned)
  end

  it "add required code to implementation precheck with return functions" do
    function = {:name => "Fungus", :args_string => "void", :return => test_return[:int]}
    retval = test_return[:int].merge({ :name => "cmock_call_instance->ReturnVal"})
    @utils.expect :code_assign_argument_quickly, '  mock_retval_0', ["Mock.Fungus_FinalReturn", retval]
    expected = ["  if (Mock.Fungus_IgnoreBool)\n",
                "  {\n",
                "    UNITY_CLR_DETAILS();\n",
                "    if (cmock_call_instance == NULL)\n",
                "      return Mock.Fungus_FinalReturn;\n",
                "    mock_retval_0",
                "    return cmock_call_instance->ReturnVal;\n",
                "  }\n"
               ].join
    returned = @cmock_generator_plugin_ignore.mock_implementation_precheck(function)
    assert_equal(expected, returned)
  end

  it "add a new mock interface for ignoring when function had no return value" do
    function = {:name => "Slime", :args => [], :args_string => "void", :return => test_return[:void]}
    expected = ["void Slime_CMockIgnore(void)\n",
                "{\n",
                "  Mock.Slime_IgnoreBool = (int)1;\n",
                "}\n\n"
               ].join
    returned = @cmock_generator_plugin_ignore.mock_interfaces(function)
    assert_equal(expected, returned)
  end

  it "add a new mock interface for ignoring when function has return value" do
    function = {:name => "Slime", :args => [], :args_string => "void", :return => test_return[:int]}
    @utils.expect :code_add_base_expectation, "mock_return_1", ["Slime", false]
    expected = ["void Slime_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return)\n",
                "{\n",
                "mock_return_1",
                "  cmock_call_instance->ReturnVal = cmock_to_return;\n",
                "  Mock.Slime_IgnoreBool = (int)1;\n",
                "}\n\n"
               ].join
    returned = @cmock_generator_plugin_ignore.mock_interfaces(function)
    assert_equal(expected, returned)
  end

end
