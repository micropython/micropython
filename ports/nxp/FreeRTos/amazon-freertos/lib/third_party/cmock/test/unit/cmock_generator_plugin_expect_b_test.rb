# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_expect'

describe CMockGeneratorPluginExpect, "Verify CMockGeneratorPluginExpect Module with Global Ordering" do

  before do
    create_mocks :config, :utils

    @config = create_stub(
      :when_ptr => :compare_data,
      :enforce_strict_ordering => true,
      :respond_to? => true,
      :plugins => [ :expect, :expect_any_args ] )

    @utils.expect :helpers, {}
    @cmock_generator_plugin_expect = CMockGeneratorPluginExpect.new(@config, @utils)
  end

  after do
  end

  it "have set up internal priority on init" do
    assert_equal(nil,     @cmock_generator_plugin_expect.unity_helper)
    assert_equal(5,       @cmock_generator_plugin_expect.priority)
  end

  it "not include any additional include files" do
    assert(!@cmock_generator_plugin_expect.respond_to?(:include_files))
  end

  it "add to typedef structure mock needs of functions of style 'void func(void)' and global ordering" do
    function = {:name => "Oak", :args => [], :return => test_return[:void]}
    expected = "  int CallOrder;\n"
    returned = @cmock_generator_plugin_expect.instance_typedefs(function)
    assert_equal(expected, returned)
  end

  it "add to typedef structure mock needs of functions of style 'int func(void)'" do
    function = {:name => "Elm", :args => [], :return => test_return[:int]}
    expected = "  int ReturnVal;\n  int CallOrder;\n"
    returned = @cmock_generator_plugin_expect.instance_typedefs(function)
    assert_equal(expected, returned)
  end

  it "add to typedef structure mock needs of functions of style 'void func(int chicken, char* pork)'" do
    function = {:name => "Cedar", :args => [{ :name => "chicken", :type => "int"}, { :name => "pork", :type => "char*"}], :return => test_return[:void]}
    expected = "  int CallOrder;\n  int Expected_chicken;\n  char* Expected_pork;\n"
    returned = @cmock_generator_plugin_expect.instance_typedefs(function)
    assert_equal(expected, returned)
  end

  it "add to typedef structure mock needs of functions of style 'int func(float beef)'" do
    function = {:name => "Birch", :args => [{ :name => "beef", :type => "float"}], :return => test_return[:int]}
    expected = "  int ReturnVal;\n  int CallOrder;\n  float Expected_beef;\n"
    returned = @cmock_generator_plugin_expect.instance_typedefs(function)
    assert_equal(expected, returned)
  end

  it "add mock function declaration for functions of style 'void func(void)'" do
    function = {:name => "Maple", :args => [], :return => test_return[:void]}
    expected = "#define Maple_Expect() Maple_CMockExpect(__LINE__)\n" +
               "void Maple_CMockExpect(UNITY_LINE_TYPE cmock_line);\n"
    returned = @cmock_generator_plugin_expect.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "add mock function declaration for functions of style 'int func(void)'" do
    function = {:name => "Spruce", :args => [], :return => test_return[:int]}
    expected = "#define Spruce_ExpectAndReturn(cmock_retval) Spruce_CMockExpectAndReturn(__LINE__, cmock_retval)\n" +
               "void Spruce_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);\n"
    returned = @cmock_generator_plugin_expect.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "add mock function declaration for functions of style 'const char* func(int tofu)'" do
    function = {:name => "Pine", :args => ["int tofu"], :args_string => "int tofu", :args_call => 'tofu', :return => test_return[:string]}
    expected = "#define Pine_ExpectAndReturn(tofu, cmock_retval) Pine_CMockExpectAndReturn(__LINE__, tofu, cmock_retval)\n" +
               "void Pine_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, int tofu, const char* cmock_to_return);\n"
    returned = @cmock_generator_plugin_expect.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "add mock function implementation for functions of style 'void func(void)'" do
    function = {:name => "Apple", :args => [], :return => test_return[:void]}
    expected = ""
    returned = @cmock_generator_plugin_expect.mock_implementation(function)
    assert_equal(expected, returned)
  end

  it "add mock function implementation for functions of style 'int func(int veal, unsigned int sushi)'" do
    function = {:name => "Cherry", :args => [ { :type => "int", :name => "veal" }, { :type => "unsigned int", :name => "sushi" } ], :return => test_return[:int]}

    @utils.expect :code_verify_an_arg_expectation, "mocked_retval_1\n", [function, function[:args][0]]
    @utils.expect :code_verify_an_arg_expectation, "mocked_retval_2\n", [function, function[:args][1]]
    expected = "  if (cmock_call_instance->IgnoreMode != CMOCK_ARG_NONE)\n" +
               "  {\n" +
               "mocked_retval_1\n" +
               "mocked_retval_2\n" +
               "  }\n"
    returned = @cmock_generator_plugin_expect.mock_implementation(function)
    assert_equal(expected, returned)
  end

  it "add mock function implementation using ordering if needed" do
    function = {:name => "Apple", :args => [], :return => test_return[:void]}
    expected = ""
    @cmock_generator_plugin_expect.ordered = true
    returned = @cmock_generator_plugin_expect.mock_implementation(function)
    assert_equal(expected, returned)
  end

  it "add mock function implementation for functions of style 'void func(int worm)' and strict ordering" do
    function = {:name => "Apple", :args => [{ :type => "int", :name => "worm" }], :return => test_return[:void]}
    @utils.expect :code_verify_an_arg_expectation, "mocked_retval_0\n", [function, function[:args][0]]
    expected = "  if (cmock_call_instance->IgnoreMode != CMOCK_ARG_NONE)\n" +
               "  {\n" +
               "mocked_retval_0\n" +
               "  }\n"
    @cmock_generator_plugin_expect.ordered = true
    returned = @cmock_generator_plugin_expect.mock_implementation(function)
    assert_equal(expected, returned)
  end

  it "add mock interfaces for functions of style 'void func(void)'" do
    function = {:name => "Pear", :args => [], :args_string => "void", :return => test_return[:void]}
    @utils.expect :code_add_base_expectation, "mock_retval_0\n", ["Pear"]
    @utils.expect :code_call_argument_loader, "mock_retval_1\n", [function]
    expected = ["void Pear_CMockExpect(UNITY_LINE_TYPE cmock_line)\n",
                "{\n",
                "mock_retval_0\n",
                "mock_retval_1\n",
                "  UNITY_CLR_DETAILS();\n",
                "}\n\n"
               ].join
    returned = @cmock_generator_plugin_expect.mock_interfaces(function)
    assert_equal(expected, returned)
  end

  it "add mock interfaces for functions of style 'int func(void)'" do
    function = {:name => "Orange", :args => [], :args_string => "void", :return => test_return[:int]}
    @utils.expect :code_add_base_expectation, "mock_retval_0\n", ["Orange"]
    @utils.expect :code_call_argument_loader, "mock_retval_1\n", [function]
    @utils.expect :code_assign_argument_quickly, "mock_retval_2\n", ["cmock_call_instance->ReturnVal", function[:return]]
    expected = ["void Orange_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return)\n",
                "{\n",
                "mock_retval_0\n",
                "mock_retval_1\n",
                "mock_retval_2\n",
                "  UNITY_CLR_DETAILS();\n",
                "}\n\n"
               ].join
    returned = @cmock_generator_plugin_expect.mock_interfaces(function)
    assert_equal(expected, returned)
  end

  it "add mock interfaces for functions of style 'int func(char* pescado)'" do
    function = {:name => "Lemon", :args => [{ :type => "char*", :name => "pescado"}], :args_string => "char* pescado", :return => test_return[:int]}
    @utils.expect :code_add_base_expectation, "mock_retval_0\n", ["Lemon"]
    @utils.expect :code_call_argument_loader, "mock_retval_1\n", [function]
    @utils.expect :code_assign_argument_quickly, "mock_retval_2\n", ["cmock_call_instance->ReturnVal", function[:return]]
    expected = ["void Lemon_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, char* pescado, int cmock_to_return)\n",
                "{\n",
                "mock_retval_0\n",
                "mock_retval_1\n",
                "mock_retval_2\n",
                "  UNITY_CLR_DETAILS();\n",
                "}\n\n"
               ].join
    returned = @cmock_generator_plugin_expect.mock_interfaces(function)
    assert_equal(expected, returned)
  end

  it "add mock interfaces for functions when using ordering" do
    function = {:name => "Pear", :args => [], :args_string => "void", :return => test_return[:void]}
    @utils.expect :code_add_base_expectation, "mock_retval_0\n", ["Pear"]
    @utils.expect :code_call_argument_loader, "mock_retval_1\n", [function]
    expected = ["void Pear_CMockExpect(UNITY_LINE_TYPE cmock_line)\n",
                "{\n",
                "mock_retval_0\n",
                "mock_retval_1\n",
                "  UNITY_CLR_DETAILS();\n",
                "}\n\n"
               ].join
    @cmock_generator_plugin_expect.ordered = true
    returned = @cmock_generator_plugin_expect.mock_interfaces(function)
    assert_equal(expected, returned)
  end

  it "add mock verify lines" do
    function = {:name => "Banana" }
    expected = "  UNITY_SET_DETAIL(CMockString_Banana);\n" +
               "  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Banana_CallInstance, cmock_line, CMockStringCalledLess);\n"
    returned = @cmock_generator_plugin_expect.mock_verify(function)
    assert_equal(expected, returned)
  end

end
