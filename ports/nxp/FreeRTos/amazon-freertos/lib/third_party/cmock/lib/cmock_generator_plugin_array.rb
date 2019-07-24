# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class CMockGeneratorPluginArray

  attr_reader :priority
  attr_accessor :config, :utils, :unity_helper, :ordered
  def initialize(config, utils)
    @config       = config
    @ptr_handling = @config.when_ptr
    @ordered      = @config.enforce_strict_ordering
    @utils        = utils
    @unity_helper = @utils.helpers[:unity_helper]
    @priority     = 8
  end

  def instance_typedefs(function)
    function[:args].inject("") do |all, arg|
      (arg[:ptr?]) ? all + "  int Expected_#{arg[:name]}_Depth;\n" : all
    end
  end

  def mock_function_declarations(function)
    return nil unless function[:contains_ptr?]
    args_call   = function[:args].map{|m| m[:ptr?] ? "#{m[:name]}, #{m[:name]}_Depth" : "#{m[:name]}"}.join(', ')
    args_string = function[:args].map do |m|
      type = @utils.arg_type_with_const(m)
      m[:ptr?] ? "#{type} #{m[:name]}, int #{m[:name]}_Depth" : "#{type} #{m[:name]}"
    end.join(', ')
    if (function[:return][:void?])
      return "#define #{function[:name]}_ExpectWithArray(#{args_call}) #{function[:name]}_CMockExpectWithArray(__LINE__, #{args_call})\n" +
             "void #{function[:name]}_CMockExpectWithArray(UNITY_LINE_TYPE cmock_line, #{args_string});\n\n"
    else
      return "#define #{function[:name]}_ExpectWithArrayAndReturn(#{args_call}, cmock_retval) #{function[:name]}_CMockExpectWithArrayAndReturn(__LINE__, #{args_call}, cmock_retval)\n" +
             "void #{function[:name]}_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, #{args_string}, #{function[:return][:str]});\n\n"
    end
  end

  def mock_interfaces(function)
    return nil unless function[:contains_ptr?]
    lines = []
    func_name = function[:name]
    args_string = function[:args].map do |m|
      type = @utils.arg_type_with_const(m)
      m[:ptr?] ? "#{type} #{m[:name]}, int #{m[:name]}_Depth" : "#{type} #{m[:name]}"
    end.join(', ')
    call_string = function[:args].map{|m| m[:ptr?] ? "#{m[:name]}, #{m[:name]}_Depth" : m[:name]}.join(', ')
    if (function[:return][:void?])
      lines << "void #{func_name}_CMockExpectWithArray(UNITY_LINE_TYPE cmock_line, #{args_string})\n"
    else
      lines << "void #{func_name}_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, #{args_string}, #{function[:return][:str]})\n"
    end
    lines << "{\n"
    lines << @utils.code_add_base_expectation(func_name)
    lines << "  CMockExpectParameters_#{func_name}(cmock_call_instance, #{call_string});\n"
    lines << "  cmock_call_instance->ReturnVal = cmock_to_return;\n" unless (function[:return][:void?])
    lines << "}\n\n"
  end

end
