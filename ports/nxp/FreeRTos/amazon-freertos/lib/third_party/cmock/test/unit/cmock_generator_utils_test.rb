# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_utils'

describe CMockGeneratorUtils, "Verify CMockGeneratorUtils Module" do

  before do
    create_mocks :config, :unity_helper, :unity_helper

    @config.expect :when_ptr, :compare_ptr
    @config.expect :enforce_strict_ordering, false
    @config.expect :plugins, []
    @config.expect :plugins, []
    @config.expect :plugins, []
    @config.expect :plugins, []
    @config.expect :plugins, []
    @config.expect :plugins, []
    @config.expect :treat_as, {'int' => 'INT','short' => 'INT16','long' => 'INT','char' => 'INT8','const char*' => 'STRING'}
    @cmock_generator_utils_simple = CMockGeneratorUtils.new(@config, {:unity_helper => @unity_helper})

    @config.expect :when_ptr, :smart
    @config.expect :enforce_strict_ordering, true
    @config.expect :plugins, [:array, :cexception, :return_thru_ptr, :ignore_arg, :ignore]
    @config.expect :plugins, [:array, :cexception, :return_thru_ptr, :ignore_arg, :ignore]
    @config.expect :plugins, [:array, :cexception, :return_thru_ptr, :ignore_arg, :ignore]
    @config.expect :plugins, [:array, :cexception, :return_thru_ptr, :ignore_arg, :ignore]
    @config.expect :plugins, [:array, :cexception, :return_thru_ptr, :ignore_arg, :ignore]
    @config.expect :plugins, [:array, :cexception, :return_thru_ptr, :ignore_arg, :ignore]
    @config.expect :treat_as, {'int' => 'INT','short' => 'INT16','long' => 'INT','char' => 'INT8','uint32_t' => 'HEX32','const char*' => 'STRING'}
    @cmock_generator_utils_complex = CMockGeneratorUtils.new(@config, {:unity_helper => @unity_helper, :A=>1, :B=>2})
  end

  after do
  end

  it "have set up internal accessors correctly on init" do
    assert_equal(false,   @cmock_generator_utils_simple.arrays)
    assert_equal(false,   @cmock_generator_utils_simple.cexception)
  end

  it "have set up internal accessors correctly on init, complete with passed helpers" do
    assert_equal(true, @cmock_generator_utils_complex.arrays)
    assert_equal(true, @cmock_generator_utils_complex.cexception)
  end

  it "detect pointers and strings" do
    assert_equal(false, @cmock_generator_utils_simple.ptr_or_str?('int'))
    assert_equal(true, @cmock_generator_utils_simple.ptr_or_str?('int*'))
    assert_equal(true, @cmock_generator_utils_simple.ptr_or_str?('char*'))
  end

  it "add code for a base expectation with no plugins" do
    expected =
      "  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Apple_CALL_INSTANCE));\n" +
      "  CMOCK_Apple_CALL_INSTANCE* cmock_call_instance = (CMOCK_Apple_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);\n" +
      "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);\n" +
      "  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));\n" +
      "  Mock.Apple_CallInstance = CMock_Guts_MemChain(Mock.Apple_CallInstance, cmock_guts_index);\n" +
      "  cmock_call_instance->LineNumber = cmock_line;\n"
    output = @cmock_generator_utils_simple.code_add_base_expectation("Apple")
    assert_equal(expected, output)
  end

  it "add code for a base expectation with all plugins" do
    expected =
      "  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Apple_CALL_INSTANCE));\n" +
      "  CMOCK_Apple_CALL_INSTANCE* cmock_call_instance = (CMOCK_Apple_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);\n" +
      "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);\n" +
      "  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));\n" +
      "  Mock.Apple_CallInstance = CMock_Guts_MemChain(Mock.Apple_CallInstance, cmock_guts_index);\n" +
      "  Mock.Apple_IgnoreBool = (int)0;\n" +
      "  cmock_call_instance->LineNumber = cmock_line;\n" +
      "  cmock_call_instance->CallOrder = ++GlobalExpectCount;\n" +
      "  cmock_call_instance->ExceptionToThrow = CEXCEPTION_NONE;\n"
    output = @cmock_generator_utils_complex.code_add_base_expectation("Apple", true)
    assert_equal(expected, output)
  end

  it "add code for a base expectation with all plugins and ordering not supported" do
    expected =
      "  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Apple_CALL_INSTANCE));\n" +
      "  CMOCK_Apple_CALL_INSTANCE* cmock_call_instance = (CMOCK_Apple_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);\n" +
      "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);\n" +
      "  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));\n" +
      "  Mock.Apple_CallInstance = CMock_Guts_MemChain(Mock.Apple_CallInstance, cmock_guts_index);\n" +
      "  Mock.Apple_IgnoreBool = (int)0;\n" +
      "  cmock_call_instance->LineNumber = cmock_line;\n" +
      "  cmock_call_instance->ExceptionToThrow = CEXCEPTION_NONE;\n"
    output = @cmock_generator_utils_complex.code_add_base_expectation("Apple", false)
    assert_equal(expected, output)
  end

  it "add argument expectations for values when no array plugin" do
    arg1 = { :name => "Orange", :const? => false, :type => 'int', :ptr? => false }
    expected1 = "  cmock_call_instance->Expected_Orange = Orange;\n"

    arg2 = { :name => "Lemon", :const? => true, :type => 'const char*', :ptr? => false }
    expected2 = "  cmock_call_instance->Expected_Lemon = Lemon;\n"

    arg3 = { :name => "Kiwi", :const? => false, :type => 'KIWI_T*', :ptr? => true }
    expected3 = "  cmock_call_instance->Expected_Kiwi = Kiwi;\n"

    arg4 = { :name => "Lime", :const? => false, :type => 'LIME_T', :ptr? => false }
    expected4 = "  memcpy(&cmock_call_instance->Expected_Lime, &Lime, sizeof(LIME_T));\n"

    assert_equal(expected1, @cmock_generator_utils_simple.code_add_an_arg_expectation(arg1))
    assert_equal(expected2, @cmock_generator_utils_simple.code_add_an_arg_expectation(arg2))
    assert_equal(expected3, @cmock_generator_utils_simple.code_add_an_arg_expectation(arg3))
    assert_equal(expected4, @cmock_generator_utils_simple.code_add_an_arg_expectation(arg4))
  end

  it "add argument expectations for values when array plugin enabled" do
    arg1 = { :name => "Orange", :const? => false, :type => 'int', :ptr? => false }
    expected1 = "  cmock_call_instance->Expected_Orange = Orange;\n" +
                "  cmock_call_instance->IgnoreArg_Orange = 0;\n"

    arg2 = { :name => "Lemon", :const? => true, :type => 'const char*', :ptr? => false }
    expected2 = "  cmock_call_instance->Expected_Lemon = Lemon;\n" +
                "  cmock_call_instance->Expected_Lemon_Depth = Lemon_Depth;\n" +
                "  cmock_call_instance->IgnoreArg_Lemon = 0;\n"

    arg3 = { :name => "Kiwi", :const? => false, :type => 'KIWI_T*', :ptr? => true }
    expected3 = "  cmock_call_instance->Expected_Kiwi = Kiwi;\n" +
                "  cmock_call_instance->Expected_Kiwi_Depth = Kiwi_Depth;\n" +
                "  cmock_call_instance->IgnoreArg_Kiwi = 0;\n" +
                "  cmock_call_instance->ReturnThruPtr_Kiwi_Used = 0;\n"

    arg4 = { :name => "Lime", :const? => false, :type => 'LIME_T', :ptr? => false }
    expected4 = "  memcpy(&cmock_call_instance->Expected_Lime, &Lime, sizeof(LIME_T));\n" +
                "  cmock_call_instance->IgnoreArg_Lime = 0;\n"

    assert_equal(expected1, @cmock_generator_utils_complex.code_add_an_arg_expectation(arg1))
    assert_equal(expected2, @cmock_generator_utils_complex.code_add_an_arg_expectation(arg2, 'Lemon_Depth'))
    assert_equal(expected3, @cmock_generator_utils_complex.code_add_an_arg_expectation(arg3, 'Lemon_Depth'))
    assert_equal(expected4, @cmock_generator_utils_complex.code_add_an_arg_expectation(arg4))
  end

  it 'not have an argument loader when the function has no arguments' do
    function = { :name => "Melon", :args_string => "void" }

    assert_equal("", @cmock_generator_utils_complex.code_add_argument_loader(function))
  end

  it 'create an argument loader when the function has arguments' do
    function = { :name => "Melon",
                 :args_string => "stuff",
                 :args => [test_arg[:int_ptr], test_arg[:mytype], test_arg[:string]]
    }
    expected = "void CMockExpectParameters_Melon(CMOCK_Melon_CALL_INSTANCE* cmock_call_instance, stuff)\n{\n" +
               "  cmock_call_instance->Expected_MyIntPtr = MyIntPtr;\n" +
               "  memcpy(&cmock_call_instance->Expected_MyMyType, &MyMyType, sizeof(MY_TYPE));\n" +
               "  cmock_call_instance->Expected_MyStr = MyStr;\n" +
               "}\n\n"
    assert_equal(expected, @cmock_generator_utils_simple.code_add_argument_loader(function))
  end

  it 'create an argument loader when the function has arguments supporting arrays' do
    function = { :name => "Melon",
                 :args_string => "stuff",
                 :args => [test_arg[:int_ptr], test_arg[:mytype], test_arg[:string]]
    }
    expected = "void CMockExpectParameters_Melon(CMOCK_Melon_CALL_INSTANCE* cmock_call_instance, int* MyIntPtr, int MyIntPtr_Depth, const MY_TYPE MyMyType, const char* MyStr)\n{\n" +
               "  cmock_call_instance->Expected_MyIntPtr = MyIntPtr;\n" +
               "  cmock_call_instance->Expected_MyIntPtr_Depth = MyIntPtr_Depth;\n" +
               "  cmock_call_instance->IgnoreArg_MyIntPtr = 0;\n" +
               "  cmock_call_instance->ReturnThruPtr_MyIntPtr_Used = 0;\n" +
               "  memcpy(&cmock_call_instance->Expected_MyMyType, &MyMyType, sizeof(MY_TYPE));\n" +
               "  cmock_call_instance->IgnoreArg_MyMyType = 0;\n" +
               "  cmock_call_instance->Expected_MyStr = MyStr;\n" +
               "  cmock_call_instance->IgnoreArg_MyStr = 0;\n" +
               "}\n\n"
    assert_equal(expected, @cmock_generator_utils_complex.code_add_argument_loader(function))
  end

  it 'create an argument loader when the function has pointer arguments supporting arrays' do
    function = { :name => "Melon",
                 :args_string => "stuff",
                 :args => [test_arg[:const_ptr], test_arg[:double_ptr]]
    }
    expected = "void CMockExpectParameters_Melon(CMOCK_Melon_CALL_INSTANCE* cmock_call_instance, int* const MyConstPtr, int MyConstPtr_Depth, int const** MyDoublePtr, int MyDoublePtr_Depth)\n{\n" +
               "  cmock_call_instance->Expected_MyConstPtr = MyConstPtr;\n" +
               "  cmock_call_instance->Expected_MyConstPtr_Depth = MyConstPtr_Depth;\n" +
               "  cmock_call_instance->IgnoreArg_MyConstPtr = 0;\n" +
               "  cmock_call_instance->ReturnThruPtr_MyConstPtr_Used = 0;\n" +
               "  cmock_call_instance->Expected_MyDoublePtr = MyDoublePtr;\n" +
               "  cmock_call_instance->Expected_MyDoublePtr_Depth = MyDoublePtr_Depth;\n" +
               "  cmock_call_instance->IgnoreArg_MyDoublePtr = 0;\n" +
               "}\n\n"
    assert_equal(expected, @cmock_generator_utils_complex.code_add_argument_loader(function))
  end

  it "not call argument loader if there are no arguments to actually use for this function" do
    function = { :name => "Pineapple", :args_string => "void" }

    assert_equal("", @cmock_generator_utils_complex.code_call_argument_loader(function))
  end

  it 'call an argument loader when the function has arguments' do
    function = { :name => "Pineapple",
                 :args_string => "stuff",
                 :args => [test_arg[:int_ptr], test_arg[:mytype], test_arg[:string]]
    }
    expected = "  CMockExpectParameters_Pineapple(cmock_call_instance, MyIntPtr, MyMyType, MyStr);\n"
    assert_equal(expected, @cmock_generator_utils_simple.code_call_argument_loader(function))
  end

  it 'call an argument loader when the function has arguments with arrays' do
    function = { :name => "Pineapple",
                 :args_string => "stuff",
                 :args => [test_arg[:int_ptr], test_arg[:mytype], test_arg[:string]]
    }
    expected = "  CMockExpectParameters_Pineapple(cmock_call_instance, MyIntPtr, 1, MyMyType, MyStr);\n"
    assert_equal(expected, @cmock_generator_utils_complex.code_call_argument_loader(function))
  end

  it 'handle a simple assert when requested' do
    function = { :name => 'Pear' }
    arg      = test_arg[:int]
    expected = "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyInt);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_INT(cmock_call_instance->Expected_MyInt, MyInt, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_INT', ''],  ['int']
    assert_equal(expected, @cmock_generator_utils_simple.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle a pointer comparison when configured to do so' do
    function = { :name => 'Pear' }
    arg      = test_arg[:int_ptr]
    expected = "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyIntPtr);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_PTR(cmock_call_instance->Expected_MyIntPtr, MyIntPtr, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    assert_equal(expected, @cmock_generator_utils_simple.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle const char as string compares ' do
    function = { :name => 'Pear' }
    arg      = test_arg[:string]
    expected = "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyStr);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_STRING(cmock_call_instance->Expected_MyStr, MyStr, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_STRING',''], ['const char*']
    assert_equal(expected, @cmock_generator_utils_simple.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle custom types as memory compares when we have no better way to do it' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype]
    expected = "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyType);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(&cmock_call_instance->Expected_MyMyType), (void*)(&MyMyType), sizeof(MY_TYPE), cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MEMORY','&'], ['MY_TYPE']
    assert_equal(expected, @cmock_generator_utils_simple.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle custom types with custom handlers when available, even if they do not support the extra message' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype]
    expected = "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyType);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_MY_TYPE(cmock_call_instance->Expected_MyMyType, MyMyType, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MY_TYPE',''], ['MY_TYPE']
    assert_equal(expected, @cmock_generator_utils_simple.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle pointers to custom types with array handlers, even if the array extension is turned off' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype]
    expected = "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyType);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_MY_TYPE_ARRAY(&cmock_call_instance->Expected_MyMyType, &MyMyType, 1, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MY_TYPE_ARRAY','&'], ['MY_TYPE']
    assert_equal(expected, @cmock_generator_utils_simple.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle a simple assert when requested with array plugin enabled' do
    function = { :name => 'Pear' }
    arg      = test_arg[:int]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyInt)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyInt);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_INT(cmock_call_instance->Expected_MyInt, MyInt, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_INT',''], ['int']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle an array comparison with array plugin enabled' do
    function = { :name => 'Pear' }
    arg      = test_arg[:int_ptr]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyIntPtr)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyIntPtr);\n" +
               "    if (cmock_call_instance->Expected_MyIntPtr == NULL)\n" +
               "      { UNITY_TEST_ASSERT_NULL(MyIntPtr, cmock_line, CMockStringExpNULL); }\n" +
               "    else if (cmock_call_instance->Expected_MyIntPtr_Depth == 0)\n" +
               "      { UNITY_TEST_ASSERT_EQUAL_PTR(cmock_call_instance->Expected_MyIntPtr, MyIntPtr, cmock_line, CMockStringMismatch); }\n" +
               "    else\n" +
               "      { UNITY_TEST_ASSERT_EQUAL_INT_ARRAY(cmock_call_instance->Expected_MyIntPtr, MyIntPtr, cmock_call_instance->Expected_MyIntPtr_Depth, cmock_line, CMockStringMismatch); }\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_INT_ARRAY',''], ['int*']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle const char as string compares with array plugin enabled' do
    function = { :name => 'Pear' }
    arg      = test_arg[:string]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyStr)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyStr);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_STRING(cmock_call_instance->Expected_MyStr, MyStr, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_STRING',''], ['const char*']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle custom types as memory compares when we have no better way to do it with array plugin enabled' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyMyType)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyType);\n" +
               "    if (cmock_call_instance->Expected_MyMyType == NULL)\n" +
               "      { UNITY_TEST_ASSERT_NULL(MyMyType, cmock_line, CMockStringExpNULL); }\n" +
               "    else\n" +
               "      { UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY((void*)(cmock_call_instance->Expected_MyMyType), (void*)(MyMyType), sizeof(MY_TYPE), 1, cmock_line, CMockStringMismatch); }\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY', ''],  ['MY_TYPE']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle custom types with custom handlers when available, even if they do not support the extra message with array plugin enabled' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyMyType)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyType);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_MY_TYPE(cmock_call_instance->Expected_MyMyType, MyMyType, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MY_TYPE',  ''], ['MY_TYPE']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle custom types with array handlers when array plugin is enabled' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype_ptr]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyMyTypePtr)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyTypePtr);\n" +
               "    if (cmock_call_instance->Expected_MyMyTypePtr == NULL)\n" +
               "      { UNITY_TEST_ASSERT_NULL(MyMyTypePtr, cmock_line, CMockStringExpNULL); }\n" +
               "    else if (cmock_call_instance->Expected_MyMyTypePtr_Depth == 0)\n" +
               "      { UNITY_TEST_ASSERT_EQUAL_PTR(cmock_call_instance->Expected_MyMyTypePtr, MyMyTypePtr, cmock_line, CMockStringMismatch); }\n" +
               "    else\n" +
               "      { UNITY_TEST_ASSERT_EQUAL_MY_TYPE_ARRAY(cmock_call_instance->Expected_MyMyTypePtr, MyMyTypePtr, cmock_call_instance->Expected_MyMyTypePtr_Depth, cmock_line, CMockStringMismatch); }\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MY_TYPE_ARRAY', ''], ['MY_TYPE*']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end

  it 'handle custom types with array handlers when array plugin is enabled for non-array types' do
    function = { :name => 'Pear' }
    arg      = test_arg[:mytype]
    expected = "  if (!cmock_call_instance->IgnoreArg_MyMyType)\n" +
               "  {\n" +
               "    UNITY_SET_DETAILS(CMockString_Pear,CMockString_MyMyType);\n" +
               "    UNITY_TEST_ASSERT_EQUAL_MY_TYPE_ARRAY(&cmock_call_instance->Expected_MyMyType, &MyMyType, 1, cmock_line, CMockStringMismatch);\n" +
               "  }\n"
    @unity_helper.expect :nil?, false
    @unity_helper.expect :get_helper, ['UNITY_TEST_ASSERT_EQUAL_MY_TYPE_ARRAY', '&'], ['MY_TYPE']
    assert_equal(expected, @cmock_generator_utils_complex.code_verify_an_arg_expectation(function, arg))
  end
end
