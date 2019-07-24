# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_ignore_arg'

describe CMockGeneratorPluginIgnoreArg, "Verify CMockGeneratorPluginIgnoreArg Module" do

  before do
    create_mocks :config, :utils

    # int *Oak(void)"
    @void_func = {:name => "Oak", :args => [], :return => test_return[:int_ptr]}

    # void Pine(int chicken, const int beef, int *tofu)
    @complex_func = {:name => "Pine",
                     :args => [{ :type => "int",
                                 :name => "chicken",
                                 :ptr? => false,
                               },
                               { :type   => "const int*",
                                 :name   => "beef",
                                 :ptr?   => true,
                                 :const? => true,
                               },
                               { :type => "int*",
                                 :name => "tofu",
                                 :ptr? => true,
                               }],
                     :return => test_return[:void],
                     :contains_ptr? => true }

    #no strict ordering
    @cmock_generator_plugin_ignore_arg = CMockGeneratorPluginIgnoreArg.new(@config, @utils)
  end

  after do
  end

  it "have set up internal priority correctly on init" do
    assert_equal(10,      @cmock_generator_plugin_ignore_arg.priority)
  end

  it "not include any additional include files" do
    assert(!@cmock_generator_plugin_ignore_arg.respond_to?(:include_files))
  end

  it "not add to typedef structure for functions with no args" do
    returned = @cmock_generator_plugin_ignore_arg.instance_typedefs(@void_func)
    assert_equal("", returned)
  end

  it "add to tyepdef structure mock needs of functions of style 'void func(int chicken, int* pork)'" do
    expected = "  int IgnoreArg_chicken;\n" +
               "  int IgnoreArg_beef;\n" +
               "  int IgnoreArg_tofu;\n"
    returned = @cmock_generator_plugin_ignore_arg.instance_typedefs(@complex_func)
    assert_equal(expected, returned)
  end

  it "add mock function declarations for all arguments" do
    expected =
      "#define Pine_IgnoreArg_chicken()" +
      " Pine_CMockIgnoreArg_chicken(__LINE__)\n" +
      "void Pine_CMockIgnoreArg_chicken(UNITY_LINE_TYPE cmock_line);\n" +

      "#define Pine_IgnoreArg_beef()" +
      " Pine_CMockIgnoreArg_beef(__LINE__)\n" +
      "void Pine_CMockIgnoreArg_beef(UNITY_LINE_TYPE cmock_line);\n" +

      "#define Pine_IgnoreArg_tofu()" +
      " Pine_CMockIgnoreArg_tofu(__LINE__)\n" +
      "void Pine_CMockIgnoreArg_tofu(UNITY_LINE_TYPE cmock_line);\n"

    returned = @cmock_generator_plugin_ignore_arg.mock_function_declarations(@complex_func)
    assert_equal(expected, returned)
  end

  it "add mock interfaces for all arguments" do
    expected =
      "void Pine_CMockIgnoreArg_chicken(UNITY_LINE_TYPE cmock_line)\n" +
      "{\n" +
      "  CMOCK_Pine_CALL_INSTANCE* cmock_call_instance = " +
      "(CMOCK_Pine_CALL_INSTANCE*)CMock_Guts_GetAddressFor(CMock_Guts_MemEndOfChain(Mock.Pine_CallInstance));\n" +
      "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringIgnPreExp);\n" +
      "  cmock_call_instance->IgnoreArg_chicken = 1;\n" +
      "}\n\n" +

      "void Pine_CMockIgnoreArg_beef(UNITY_LINE_TYPE cmock_line)\n" +
      "{\n" +
      "  CMOCK_Pine_CALL_INSTANCE* cmock_call_instance = " +
      "(CMOCK_Pine_CALL_INSTANCE*)CMock_Guts_GetAddressFor(CMock_Guts_MemEndOfChain(Mock.Pine_CallInstance));\n" +
      "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringIgnPreExp);\n" +
      "  cmock_call_instance->IgnoreArg_beef = 1;\n" +
      "}\n\n" +

      "void Pine_CMockIgnoreArg_tofu(UNITY_LINE_TYPE cmock_line)\n" +
      "{\n" +
      "  CMOCK_Pine_CALL_INSTANCE* cmock_call_instance = " +
      "(CMOCK_Pine_CALL_INSTANCE*)CMock_Guts_GetAddressFor(CMock_Guts_MemEndOfChain(Mock.Pine_CallInstance));\n" +
      "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringIgnPreExp);\n" +
      "  cmock_call_instance->IgnoreArg_tofu = 1;\n" +
      "}\n\n"

    returned = @cmock_generator_plugin_ignore_arg.mock_interfaces(@complex_func).join("")
    assert_equal(expected, returned)
  end

  it "not add a mock implementation" do
    assert(!@cmock_generator_plugin_ignore_arg.respond_to?(:mock_implementation))
  end

end
