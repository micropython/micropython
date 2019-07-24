# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class CMockGeneratorPluginExpect

  attr_reader :priority
  attr_accessor :config, :utils, :unity_helper, :ordered

  def initialize(config, utils)
    @config       = config
    @ptr_handling = @config.when_ptr
    @ordered      = @config.enforce_strict_ordering
    @utils        = utils
    @unity_helper = @utils.helpers[:unity_helper]
    @priority     = 5

    if (@config.plugins.include? :expect_any_args)
      alias :mock_implementation :mock_implementation_might_check_args
    else
      alias :mock_implementation :mock_implementation_always_check_args
    end
  end

  def instance_typedefs(function)
    lines = ""
    lines << "  #{function[:return][:type]} ReturnVal;\n"  unless (function[:return][:void?])
    lines << "  int CallOrder;\n"                          if (@ordered)
    function[:args].each do |arg|
      lines << "  #{arg[:type]} Expected_#{arg[:name]};\n"
    end
    lines
  end

  def mock_function_declarations(function)
    if (function[:args].empty?)
      if (function[:return][:void?])
        return "#define #{function[:name]}_Expect() #{function[:name]}_CMockExpect(__LINE__)\n" +
               "void #{function[:name]}_CMockExpect(UNITY_LINE_TYPE cmock_line);\n\n"
      else
        return "#define #{function[:name]}_ExpectAndReturn(cmock_retval) #{function[:name]}_CMockExpectAndReturn(__LINE__, cmock_retval)\n" +
               "void #{function[:name]}_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, #{function[:return][:str]});\n\n"
      end
    else
      if (function[:return][:void?])
        return "#define #{function[:name]}_Expect(#{function[:args_call]}) #{function[:name]}_CMockExpect(__LINE__, #{function[:args_call]})\n" +
               "void #{function[:name]}_CMockExpect(UNITY_LINE_TYPE cmock_line, #{function[:args_string]});\n\n"
      else
        return "#define #{function[:name]}_ExpectAndReturn(#{function[:args_call]}, cmock_retval) #{function[:name]}_CMockExpectAndReturn(__LINE__, #{function[:args_call]}, cmock_retval)\n" +
               "void #{function[:name]}_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, #{function[:args_string]}, #{function[:return][:str]});\n\n"
      end
    end
  end

  def mock_implementation_always_check_args(function)
    lines = ""
    function[:args].each do |arg|
      lines << @utils.code_verify_an_arg_expectation(function, arg)
    end
    lines
  end

  def mock_implementation_might_check_args(function)
    return "" if (function[:args].empty?)
    lines = "  if (cmock_call_instance->IgnoreMode != CMOCK_ARG_NONE)\n  {\n"
    function[:args].each do |arg|
      lines << @utils.code_verify_an_arg_expectation(function, arg)
    end
    lines << "  }\n"
    lines
  end

  def mock_interfaces(function)
    lines = ""
    func_name = function[:name]
    if (function[:return][:void?])
      if (function[:args_string] == "void")
        lines << "void #{func_name}_CMockExpect(UNITY_LINE_TYPE cmock_line)\n{\n"
      else
        lines << "void #{func_name}_CMockExpect(UNITY_LINE_TYPE cmock_line, #{function[:args_string]})\n{\n"
      end
    else
      if (function[:args_string] == "void")
        lines << "void #{func_name}_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, #{function[:return][:str]})\n{\n"
      else
        lines << "void #{func_name}_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, #{function[:args_string]}, #{function[:return][:str]})\n{\n"
      end
    end
    lines << @utils.code_add_base_expectation(func_name)
    lines << @utils.code_call_argument_loader(function)
    lines << @utils.code_assign_argument_quickly("cmock_call_instance->ReturnVal", function[:return]) unless (function[:return][:void?])
    lines << "  UNITY_CLR_DETAILS();\n"
    lines << "}\n\n"
  end

  def mock_verify(function)
    func_name = function[:name]
    "  UNITY_SET_DETAIL(CMockString_#{function[:name]});\n" +
    "  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.#{func_name}_CallInstance, cmock_line, CMockStringCalledLess);\n"
  end

end
