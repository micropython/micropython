# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class CMockGeneratorPluginCexception

  attr_reader :priority
  attr_reader :config, :utils

  def initialize(config, utils)
    @config = config
    @utils = utils
    @priority = 7
  end

  def include_files
    return "#include \"CException.h\"\n"
  end

  def instance_typedefs(function)
    "  CEXCEPTION_T ExceptionToThrow;\n"
  end

  def mock_function_declarations(function)
    if (function[:args_string] == "void")
      return "#define #{function[:name]}_ExpectAndThrow(cmock_to_throw) #{function[:name]}_CMockExpectAndThrow(__LINE__, cmock_to_throw)\n" +
             "void #{function[:name]}_CMockExpectAndThrow(UNITY_LINE_TYPE cmock_line, CEXCEPTION_T cmock_to_throw);\n\n"
    else
      return "#define #{function[:name]}_ExpectAndThrow(#{function[:args_call]}, cmock_to_throw) #{function[:name]}_CMockExpectAndThrow(__LINE__, #{function[:args_call]}, cmock_to_throw)\n" +
             "void #{function[:name]}_CMockExpectAndThrow(UNITY_LINE_TYPE cmock_line, #{function[:args_string]}, CEXCEPTION_T cmock_to_throw);\n\n"
    end
  end

  def mock_implementation(function)
    "  if (cmock_call_instance->ExceptionToThrow != CEXCEPTION_NONE)\n  {\n" +
    "    UNITY_CLR_DETAILS();\n" +
    "    Throw(cmock_call_instance->ExceptionToThrow);\n  }\n"
  end

  def mock_interfaces(function)
    arg_insert = (function[:args_string] == "void") ? "" : "#{function[:args_string]}, "
    call_string = function[:args].map{|m| m[:name]}.join(', ')
    [ "void #{function[:name]}_CMockExpectAndThrow(UNITY_LINE_TYPE cmock_line, #{arg_insert}CEXCEPTION_T cmock_to_throw)\n{\n",
      @utils.code_add_base_expectation(function[:name]),
      @utils.code_call_argument_loader(function),
      "  cmock_call_instance->ExceptionToThrow = cmock_to_throw;\n",
      "}\n\n" ].join
  end

end
