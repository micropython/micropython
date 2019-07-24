# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class CMockGeneratorUtils

  attr_accessor :config, :helpers, :ordered, :ptr_handling, :arrays, :cexception

  def initialize(config, helpers={})
    @config = config
    @ptr_handling = @config.when_ptr
    @ordered      = @config.enforce_strict_ordering
    @arrays       = @config.plugins.include? :array
    @cexception   = @config.plugins.include? :cexception
    @expect_any   = @config.plugins.include? :expect_any_args
    @return_thru_ptr = @config.plugins.include? :return_thru_ptr
    @ignore_arg   = @config.plugins.include? :ignore_arg
    @ignore       = @config.plugins.include? :ignore
    @treat_as     = @config.treat_as
	  @helpers = helpers
  end

  def self.arg_type_with_const(arg)
    # Restore any "const" that was removed in header parsing
    if arg[:type].include?('*')
      arg[:const_ptr?] ? "#{arg[:type]} const" : arg[:type]
    else
      arg[:const?] ? "const #{arg[:type]}" : arg[:type]
    end
  end

  def arg_type_with_const(arg)
    self.class.arg_type_with_const(arg)
  end

  def code_verify_an_arg_expectation(function, arg)
    if (@arrays)
      case(@ptr_handling)
        when :smart        then code_verify_an_arg_expectation_with_smart_arrays(function, arg)
        when :compare_data then code_verify_an_arg_expectation_with_normal_arrays(function, arg)
        when :compare_ptr  then raise "ERROR: the array plugin doesn't enjoy working with :compare_ptr only.  Disable one option."
      end
    else
      code_verify_an_arg_expectation_with_no_arrays(function, arg)
    end
  end

  def code_add_base_expectation(func_name, global_ordering_supported=true)
    lines =  "  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_#{func_name}_CALL_INSTANCE));\n"
    lines << "  CMOCK_#{func_name}_CALL_INSTANCE* cmock_call_instance = (CMOCK_#{func_name}_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);\n"
    lines << "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);\n"
    lines << "  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));\n"
    lines << "  Mock.#{func_name}_CallInstance = CMock_Guts_MemChain(Mock.#{func_name}_CallInstance, cmock_guts_index);\n"
    lines << "  Mock.#{func_name}_IgnoreBool = (int)0;\n" if (@ignore)
    lines << "  cmock_call_instance->LineNumber = cmock_line;\n"
    lines << "  cmock_call_instance->CallOrder = ++GlobalExpectCount;\n" if (@ordered and global_ordering_supported)
    lines << "  cmock_call_instance->ExceptionToThrow = CEXCEPTION_NONE;\n" if (@cexception)
    lines << "  cmock_call_instance->IgnoreMode = CMOCK_ARG_ALL;\n" if (@expect_any)
    lines
  end

  def code_add_an_arg_expectation(arg, depth=1)
    lines =  code_assign_argument_quickly("cmock_call_instance->Expected_#{arg[:name]}", arg)
    lines << "  cmock_call_instance->Expected_#{arg[:name]}_Depth = #{arg[:name]}_Depth;\n" if (@arrays and (depth.class == String))
    lines << "  cmock_call_instance->IgnoreArg_#{arg[:name]} = 0;\n" if (@ignore_arg)
    lines << "  cmock_call_instance->ReturnThruPtr_#{arg[:name]}_Used = 0;\n" if (@return_thru_ptr and ptr_or_str?(arg[:type]) and not arg[:const?])
    lines
  end

  def code_assign_argument_quickly(dest, arg)
    if (arg[:ptr?] or @treat_as.include?(arg[:type]))
      "  #{dest} = #{arg[:name]};\n"
    else
      "  memcpy(&#{dest}, &#{arg[:name]}, sizeof(#{arg[:type]}));\n"
    end
  end

  def code_add_argument_loader(function)
    if (function[:args_string] != "void")
      if (@arrays)
        args_string = function[:args].map do |m|
          type = arg_type_with_const(m)
          m[:ptr?] ? "#{type} #{m[:name]}, int #{m[:name]}_Depth" : "#{type} #{m[:name]}"
        end.join(', ')
        "void CMockExpectParameters_#{function[:name]}(CMOCK_#{function[:name]}_CALL_INSTANCE* cmock_call_instance, #{args_string})\n{\n" +
        function[:args].inject("") { |all, arg| all + code_add_an_arg_expectation(arg, (arg[:ptr?] ? "#{arg[:name]}_Depth" : 1) ) } +
        "}\n\n"
      else
        "void CMockExpectParameters_#{function[:name]}(CMOCK_#{function[:name]}_CALL_INSTANCE* cmock_call_instance, #{function[:args_string]})\n{\n" +
        function[:args].inject("") { |all, arg| all + code_add_an_arg_expectation(arg) } +
        "}\n\n"
      end
    else
      ""
    end
  end

  def code_call_argument_loader(function)
    if (function[:args_string] != "void")
      args = function[:args].map do |m|
        (@arrays and m[:ptr?]) ? "#{m[:name]}, 1" : m[:name]
      end
      "  CMockExpectParameters_#{function[:name]}(cmock_call_instance, #{args.join(', ')});\n"
    else
      ""
    end
  end

  def ptr_or_str?(arg_type)
    return (arg_type.include? '*' or
            @treat_as.fetch(arg_type, "").include? '*')
  end

  #private ######################

  def lookup_expect_type(function, arg)
    c_type     = arg[:type]
    arg_name   = arg[:name]
    expected   = "cmock_call_instance->Expected_#{arg_name}"
    ignore     = "cmock_call_instance->IgnoreArg_#{arg_name}"
    unity_func = if ((arg[:ptr?]) and ((c_type =~ /\*\*/) or (@ptr_handling == :compare_ptr)))
                   ['UNITY_TEST_ASSERT_EQUAL_PTR', '']
                 else
                   (@helpers.nil? or @helpers[:unity_helper].nil?) ? ["UNITY_TEST_ASSERT_EQUAL",''] : @helpers[:unity_helper].get_helper(c_type)
                 end
    return c_type, arg_name, expected, ignore, unity_func[0], unity_func[1]
  end

  def code_verify_an_arg_expectation_with_no_arrays(function, arg)
    c_type, arg_name, expected, ignore, unity_func, pre = lookup_expect_type(function, arg)
    lines = ""
    lines << "  if (!#{ignore})\n" if @ignore_arg
    lines << "  {\n"
    lines << "    UNITY_SET_DETAILS(CMockString_#{function[:name]},CMockString_#{arg_name});\n"
    case(unity_func)
      when "UNITY_TEST_ASSERT_EQUAL_MEMORY"
        c_type_local = c_type.gsub(/\*$/,'')
        lines << "    UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type_local}), cmock_line, CMockStringMismatch);\n"
      when "UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY"
        if (pre == '&')
          lines << "    UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type.sub('*','')}), cmock_line, CMockStringMismatch);\n"
        else
          lines << "    if (#{pre}#{expected} == NULL)\n"
          lines << "      { UNITY_TEST_ASSERT_NULL(#{pre}#{arg_name}, cmock_line, CMockStringExpNULL); }\n"
          lines << "    else\n"
          lines << "      { UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type.sub('*','')}), cmock_line, CMockStringMismatch); }\n"
        end
      when /_ARRAY/
        if (pre == '&')
          lines << "    #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, 1, cmock_line, CMockStringMismatch);\n"
        else
          lines << "    if (#{pre}#{expected} == NULL)\n"
          lines << "      { UNITY_TEST_ASSERT_NULL(#{pre}#{arg_name}, cmock_line, CMockStringExpNULL); }\n"
          lines << "    else\n"
          lines << "      { #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, 1, cmock_line, CMockStringMismatch); }\n"
        end
      else
        lines << "    #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, cmock_line, CMockStringMismatch);\n"
    end
    lines << "  }\n"
    lines
  end

  def code_verify_an_arg_expectation_with_normal_arrays(function, arg)
    c_type, arg_name, expected, ignore, unity_func, pre = lookup_expect_type(function, arg)
    depth_name = (arg[:ptr?]) ? "cmock_call_instance->Expected_#{arg_name}_Depth" : 1
    lines = ""
    lines << "  if (!#{ignore})\n" if @ignore_arg
    lines << "  {\n"
    lines << "    UNITY_SET_DETAILS(CMockString_#{function[:name]},CMockString_#{arg_name});\n"
    case(unity_func)
      when "UNITY_TEST_ASSERT_EQUAL_MEMORY"
        c_type_local = c_type.gsub(/\*$/,'')
        lines << "    UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type_local}), cmock_line, CMockStringMismatch);\n"
      when "UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY"
        if (pre == '&')
          lines << "    UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type.sub('*','')}), cmock_line, CMockStringMismatch);\n"
        else
          lines << "    if (#{pre}#{expected} == NULL)\n"
          lines << "      { UNITY_TEST_ASSERT_NULL(#{pre}#{arg_name}, cmock_line, CMockStringExpNULL); }\n"
          lines << "    else\n"
          lines << "      { UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type.sub('*','')}), #{depth_name}, cmock_line, CMockStringMismatch); }\n"
        end
      when /_ARRAY/
        if (pre == '&')
          lines << "    #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, #{depth_name}, cmock_line, CMockStringMismatch);\n"
        else
          lines << "    if (#{pre}#{expected} == NULL)\n"
          lines << "      { UNITY_TEST_ASSERT_NULL(#{pre}#{arg_name}, cmock_line, CMockStringExpNULL); }\n"
          lines << "    else\n"
          lines << "      { #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, #{depth_name}, cmock_line, CMockStringMismatch); }\n"
        end
      else
        lines << "    #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, cmock_line, CMockStringMismatch);\n"
    end
    lines << "  }\n"
    lines
  end

  def code_verify_an_arg_expectation_with_smart_arrays(function, arg)
    c_type, arg_name, expected, ignore, unity_func, pre = lookup_expect_type(function, arg)
    depth_name = (arg[:ptr?]) ? "cmock_call_instance->Expected_#{arg_name}_Depth" : 1
    lines = ""
    lines << "  if (!#{ignore})\n" if @ignore_arg
    lines << "  {\n"
    lines << "    UNITY_SET_DETAILS(CMockString_#{function[:name]},CMockString_#{arg_name});\n"
    case(unity_func)
      when "UNITY_TEST_ASSERT_EQUAL_MEMORY"
        c_type_local = c_type.gsub(/\*$/,'')
        lines << "    UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type_local}), cmock_line, CMockStringMismatch);\n"
      when "UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY"
        if (pre == '&')
          lines << "    UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type.sub('*','')}), #{depth_name}, cmock_line, CMockStringMismatch);\n"
        else
          lines << "    if (#{pre}#{expected} == NULL)\n"
          lines << "      { UNITY_TEST_ASSERT_NULL(#{arg_name}, cmock_line, CMockStringExpNULL); }\n"
          lines << ((depth_name != 1) ? "    else if (#{depth_name} == 0)\n      { UNITY_TEST_ASSERT_EQUAL_PTR(#{pre}#{expected}, #{pre}#{arg_name}, cmock_line, CMockStringMismatch); }\n" : "")
          lines << "    else\n"
          lines << "      { UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY((void*)(#{pre}#{expected}), (void*)(#{pre}#{arg_name}), sizeof(#{c_type.sub('*','')}), #{depth_name}, cmock_line, CMockStringMismatch); }\n"
        end
      when /_ARRAY/
        if (pre == '&')
          lines << "    #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, #{depth_name}, cmock_line, CMockStringMismatch);\n"
        else
          lines << "    if (#{pre}#{expected} == NULL)\n"
          lines << "      { UNITY_TEST_ASSERT_NULL(#{pre}#{arg_name}, cmock_line, CMockStringExpNULL); }\n"
          lines << ((depth_name != 1) ? "    else if (#{depth_name} == 0)\n      { UNITY_TEST_ASSERT_EQUAL_PTR(#{pre}#{expected}, #{pre}#{arg_name}, cmock_line, CMockStringMismatch); }\n" : "")
          lines << "    else\n"
          lines << "      { #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, #{depth_name}, cmock_line, CMockStringMismatch); }\n"
        end
      else
        lines << "    #{unity_func}(#{pre}#{expected}, #{pre}#{arg_name}, cmock_line, CMockStringMismatch);\n"
    end
    lines << "  }\n"
    lines
  end

end
