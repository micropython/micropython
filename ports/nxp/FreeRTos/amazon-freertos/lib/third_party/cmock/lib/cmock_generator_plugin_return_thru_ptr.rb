class CMockGeneratorPluginReturnThruPtr
  attr_reader :priority
  attr_accessor :utils

  def initialize(config, utils)
    @utils        = utils
    @priority     = 9
  end

  def instance_typedefs(function)
    lines = ""
    function[:args].each do |arg|
      if (@utils.ptr_or_str?(arg[:type]) and not arg[:const?])
        lines << "  int ReturnThruPtr_#{arg[:name]}_Used;\n"
        lines << "  #{arg[:type]} ReturnThruPtr_#{arg[:name]}_Val;\n"
        lines << "  int ReturnThruPtr_#{arg[:name]}_Size;\n"
      end
    end
    lines
  end

  def mock_function_declarations(function)
    lines = ""
    function[:args].each do |arg|
      if (@utils.ptr_or_str?(arg[:type]) and not arg[:const?])
        lines << "#define #{function[:name]}_ReturnThruPtr_#{arg[:name]}(#{arg[:name]})"
        lines << " #{function[:name]}_CMockReturnMemThruPtr_#{arg[:name]}(__LINE__, #{arg[:name]}, sizeof(*#{arg[:name]}))\n"
        lines << "#define #{function[:name]}_ReturnArrayThruPtr_#{arg[:name]}(#{arg[:name]}, cmock_len)"
        lines << " #{function[:name]}_CMockReturnMemThruPtr_#{arg[:name]}(__LINE__, #{arg[:name]}, (int)(cmock_len * (int)sizeof(*#{arg[:name]})))\n"
        lines << "#define #{function[:name]}_ReturnMemThruPtr_#{arg[:name]}(#{arg[:name]}, cmock_size)"
        lines << " #{function[:name]}_CMockReturnMemThruPtr_#{arg[:name]}(__LINE__, #{arg[:name]}, cmock_size)\n"
        lines << "void #{function[:name]}_CMockReturnMemThruPtr_#{arg[:name]}(UNITY_LINE_TYPE cmock_line, #{arg[:type]} #{arg[:name]}, int cmock_size);\n\n"
      end
    end
    lines
  end

  def mock_interfaces(function)
    lines = []
    func_name = function[:name]
    function[:args].each do |arg|
      arg_name = arg[:name]
      arg_type = arg[:type]
      if (@utils.ptr_or_str?(arg[:type]) and not arg[:const?])
        lines << "void #{func_name}_CMockReturnMemThruPtr_#{arg_name}(UNITY_LINE_TYPE cmock_line, #{arg[:type]} #{arg_name}, int cmock_size)\n"
        lines << "{\n"
        lines << "  CMOCK_#{func_name}_CALL_INSTANCE* cmock_call_instance = " +
          "(CMOCK_#{func_name}_CALL_INSTANCE*)CMock_Guts_GetAddressFor(CMock_Guts_MemEndOfChain(Mock.#{func_name}_CallInstance));\n"
        lines << "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringPtrPreExp);\n"
        lines << "  cmock_call_instance->ReturnThruPtr_#{arg_name}_Used = 1;\n"
        lines << "  cmock_call_instance->ReturnThruPtr_#{arg_name}_Val = #{arg_name};\n"
        lines << "  cmock_call_instance->ReturnThruPtr_#{arg_name}_Size = cmock_size;\n"
        lines << "}\n\n"
      end
    end
    lines
  end

  def mock_implementation(function)
    lines = []
    function[:args].each do |arg|
      arg_name = arg[:name]
      arg_type = arg[:type]
      if (@utils.ptr_or_str?(arg[:type]) and not arg[:const?])
        lines << "  if (cmock_call_instance->ReturnThruPtr_#{arg_name}_Used)\n"
        lines << "  {\n"
        lines << "    UNITY_TEST_ASSERT_NOT_NULL(#{arg_name}, cmock_line, CMockStringPtrIsNULL);\n"
        lines << "    memcpy((void*)#{arg_name}, (void*)cmock_call_instance->ReturnThruPtr_#{arg_name}_Val,\n"
        lines << "      cmock_call_instance->ReturnThruPtr_#{arg_name}_Size);\n"
        lines << "  }\n"
      end
    end
    lines
  end
end
