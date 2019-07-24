# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_array'
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_utils'

class UtilsStub
  def helpers
    {}
  end
  def arg_type_with_const(arg)
    CMockGeneratorUtils.arg_type_with_const(arg)
  end
  def code_add_base_expectation(func)
    "mock_retval_0"
  end
end

describe CMockGeneratorPluginArray, "Verify CMockPGeneratorluginArray Module" do
  before do
    #no strict ordering
    @config = create_stub(
      :when_ptr => :compare_data,
      :enforce_strict_ordering => false,
      :respond_to? => true )

    @utils = UtilsStub.new

    @cmock_generator_plugin_array = CMockGeneratorPluginArray.new(@config, @utils)
  end

  after do
  end

  it "have set up internal priority" do
    assert_equal(nil,     @cmock_generator_plugin_array.unity_helper)
    assert_equal(8,       @cmock_generator_plugin_array.priority)
  end

  it "not include any additional include files" do
    assert(!@cmock_generator_plugin_array.respond_to?(:include_files))
  end

  it "not add to typedef structure for functions of style 'int* func(void)'" do
    function = {:name => "Oak", :args => [], :return => test_return[:int_ptr]}
    returned = @cmock_generator_plugin_array.instance_typedefs(function)
    assert_equal("", returned)
  end

  it "add to tyepdef structure mock needs of functions of style 'void func(int chicken, int* pork)'" do
    function = {:name => "Cedar", :args => [{ :name => "chicken", :type => "int", :ptr? => false}, { :name => "pork", :type => "int*", :ptr? => true}], :return => test_return[:void]}
    expected = "  int Expected_pork_Depth;\n"
    returned = @cmock_generator_plugin_array.instance_typedefs(function)
    assert_equal(expected, returned)
  end

  it "not add an additional mock interface for functions not containing pointers" do
    function = {:name => "Maple", :args_string => "int blah", :return  => test_return[:string], :contains_ptr? => false}
    returned = @cmock_generator_plugin_array.mock_function_declarations(function)
    assert_nil(returned)
  end

  it "add another mock function declaration for functions of style 'void func(int* tofu)'" do
    function = {:name => "Pine",
                :args => [{ :type => "int*",
                           :name => "tofu",
                           :ptr? => true,
                         }],
                :return => test_return[:void],
                :contains_ptr? => true }

    expected = "#define #{function[:name]}_ExpectWithArray(tofu, tofu_Depth) #{function[:name]}_CMockExpectWithArray(__LINE__, tofu, tofu_Depth)\n" +
               "void #{function[:name]}_CMockExpectWithArray(UNITY_LINE_TYPE cmock_line, int* tofu, int tofu_Depth);\n"
    returned = @cmock_generator_plugin_array.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "add another mock function declaration for functions of style 'const char* func(int* tofu)'" do
    function = {:name => "Pine",
                :args => [{ :type => "int*",
                           :name => "tofu",
                           :ptr? => true,
                         }],
                :return => test_return[:string],
                :contains_ptr? => true }

    expected = "#define #{function[:name]}_ExpectWithArrayAndReturn(tofu, tofu_Depth, cmock_retval) #{function[:name]}_CMockExpectWithArrayAndReturn(__LINE__, tofu, tofu_Depth, cmock_retval)\n" +
               "void #{function[:name]}_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, int* tofu, int tofu_Depth, const char* cmock_to_return);\n"
    returned = @cmock_generator_plugin_array.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "add another mock function declaration for functions of style 'const char* func(const int* tofu)'" do
    function = {:name => "Pine",
                :args => [{ :type   => "const int*",
                            :name   => "tofu",
                            :ptr?   => true,
                            :const? => true,
                         }],
                :return => test_return[:string],
                :contains_ptr? => true }

    expected = "#define #{function[:name]}_ExpectWithArrayAndReturn(tofu, tofu_Depth, cmock_retval) #{function[:name]}_CMockExpectWithArrayAndReturn(__LINE__, tofu, tofu_Depth, cmock_retval)\n" +
               "void #{function[:name]}_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, const int* tofu, int tofu_Depth, const char* cmock_to_return);\n"
    returned = @cmock_generator_plugin_array.mock_function_declarations(function)
    assert_equal(expected, returned)
  end

  it "not have a mock function implementation" do
    assert(!@cmock_generator_plugin_array.respond_to?(:mock_implementation))
  end

  it "not have a mock interfaces for functions of style 'int* func(void)'" do
    function = {:name => "Pear", :args => [], :args_string => "void", :return => test_return[:int_ptr]}
    returned = @cmock_generator_plugin_array.mock_interfaces(function)
    assert_nil(returned)
  end

  it "add mock interfaces for functions of style 'int* func(int* pescado, int pes)'" do
    function = {:name => "Lemon",
                :args => [{ :type => "int*", :name => "pescado", :ptr? => true}, { :type => "int", :name => "pes", :ptr? => false}],
                :args_string => "int* pescado, int pes",
                :return  => test_return[:int_ptr],
                :contains_ptr? => true }

    expected = ["void Lemon_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, int* pescado, int pescado_Depth, int pes, int* cmock_to_return)\n",
                "{\n",
                "mock_retval_0",
                "  CMockExpectParameters_Lemon(cmock_call_instance, pescado, pescado_Depth, pes);\n",
                "  cmock_call_instance->ReturnVal = cmock_to_return;\n",
                "}\n\n"
               ].join
    returned = @cmock_generator_plugin_array.mock_interfaces(function).join
    assert_equal(expected, returned)
  end

end
