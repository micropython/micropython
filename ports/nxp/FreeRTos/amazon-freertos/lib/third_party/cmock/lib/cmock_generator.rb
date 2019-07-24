# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class CMockGenerator

  attr_accessor :config, :file_writer, :module_name, :clean_mock_name, :mock_name, :utils, :plugins, :weak, :ordered

  def initialize(config, file_writer, utils, plugins)
    @file_writer = file_writer
    @utils       = utils
    @plugins     = plugins
    @config      = config
    @prefix      = @config.mock_prefix
    @suffix      = @config.mock_suffix
    @weak        = @config.weak
    @ordered     = @config.enforce_strict_ordering
    @framework   = @config.framework.to_s
    @fail_on_unexpected_calls = @config.fail_on_unexpected_calls

    @subdir      = @config.subdir

    @includes_h_pre_orig_header  = (@config.includes || @config.includes_h_pre_orig_header || []).map{|h| h =~ /</ ? h : "\"#{h}\""}
    @includes_h_post_orig_header = (@config.includes_h_post_orig_header || []).map{|h| h =~ /</ ? h : "\"#{h}\""}
    @includes_c_pre_header       = (@config.includes_c_pre_header || []).map{|h| h =~ /</ ? h : "\"#{h}\""}
    @includes_c_post_header      = (@config.includes_c_post_header || []).map{|h| h =~ /</ ? h : "\"#{h}\""}

    here = File.dirname __FILE__
    unity_path_in_ceedling = "#{here}/../../unity" # path to Unity from within Ceedling
    unity_path_in_cmock = "#{here}/../vendor/unity" # path to Unity from within CMock
    # path to Unity as specified by env var
    unity_path_in_env = ENV.has_key?("UNITY_DIR") ? File.expand_path(ENV.fetch("UNITY_DIR")) : nil

    if unity_path_in_env and File.exist? unity_path_in_env
      require "#{unity_path_in_env}/auto/type_sanitizer"
    elsif File.exist? unity_path_in_ceedling
      require "#{unity_path_in_ceedling}/auto/type_sanitizer"
    elsif File.exist? unity_path_in_cmock
      require "#{unity_path_in_cmock}/auto/type_sanitizer"
    else
      raise "Failed to find an instance of Unity to pull in type_sanitizer module!"
    end

  end

  def create_mock(module_name, parsed_stuff)
    @module_name = module_name
    @mock_name   = @prefix + @module_name + @suffix
    @clean_mock_name = TypeSanitizer.sanitize_c_identifier(@mock_name)
    create_mock_subdir()
    create_mock_header_file(parsed_stuff)
    create_mock_source_file(parsed_stuff)
    create_mock_internal_source_file(parsed_stuff)
  end

  private if $ThisIsOnlyATest.nil? ##############################

  def create_mock_subdir()
    if @subdir
      @file_writer.create_subdir(@subdir)
    end
  end

  def create_mock_header_file(parsed_stuff)
    @file_writer.create_file(@mock_name + ".h", @subdir) do |file, filename|
      create_mock_header_header(file, filename)
      create_mock_header_service_call_declarations(file)
      create_typedefs(file, parsed_stuff[:typedefs])
      parsed_stuff[:functions].each do |function|
        file << "/**\n"
        file << " * @defgroup MOCK_#{function[:name]} Mock functions for #{function[:name]}.\n"
        file << " */\n"
        file << "/** @{ */\n"
        file << @plugins.run(:mock_function_declarations, function)
        file << "/** @} */\n\n"
      end
      create_mock_header_footer(file)
    end
  end

  def create_mock_source_file(parsed_stuff)
    @file_writer.create_file(@mock_name + ".c", @subdir) do |file, filename|
      create_source_header_section(file, filename, parsed_stuff[:functions])
      create_instance_structure(file, parsed_stuff[:functions])
      create_extern_declarations(file)
      create_mock_verify_function(file, parsed_stuff[:functions])
      create_mock_init_function(file)
      create_mock_destroy_function(file, parsed_stuff[:functions])
      parsed_stuff[:functions].each do |function|
        create_mock_implementation(file, function)
        create_mock_interfaces(file, function)
      end
    end
  end

  def create_mock_internal_source_file(parsed_stuff)
    @file_writer.create_file(@mock_name + "_internal.c", @subdir) do |file, filename|
      orig_filename = (@subdir ? @subdir + "/" : "") + @module_name + ".h"
      file << @config.orig_header_include_fmt.gsub(/%s/, "#{orig_filename}") + "\n\n"
      parsed_stuff[:functions].each do |function|
        # prepare return value and arguments
        function_mod_and_rettype = (function[:modifier].empty? ? '' : "#{function[:modifier]} ") +
                                   (function[:return][:type]) +
                                   (function[:c_calling_convention] ? " #{function[:c_calling_convention]}" : '')
        args_string = function[:args_string]
        args_string += (", " + function[:var_arg]) unless (function[:var_arg].nil?)
        file << "/* Defined in " + @mock_name + ".h. */\n"
        file << "#{function_mod_and_rettype} __cmock__#{function[:name]}(#{args_string});\n\n"
        file << "static #{function_mod_and_rettype} #{function[:name]}(#{args_string})\n"
        file << "{\n"
        if (function[:return][:void?])
          file << "  __cmock__#{function[:name]}(#{function[:args].map{|m| m[:name]}.join(', ')});\n"
        else
          file << "  return __cmock__#{function[:name]}(#{function[:args].map{|m| m[:name]}.join(', ')});\n"
        end
        file << "}\n\n"
      end
    end
  end

  def create_mock_header_header(file, filename)
    define_name   = @clean_mock_name.upcase
    orig_filename = (@subdir ? @subdir + "/" : "") + @module_name + ".h"
    file << "/* AUTOGENERATED FILE. DO NOT EDIT. */\n"
    file << "#ifndef _#{define_name}_H\n"
    file << "#define _#{define_name}_H\n\n"
    @includes_h_pre_orig_header.each {|inc| file << "#include #{inc}\n"}
    file << @config.orig_header_include_fmt.gsub(/%s/, "#{orig_filename}") + "\n"
    @includes_h_post_orig_header.each {|inc| file << "#include #{inc}\n"}
    plugin_includes = @plugins.run(:include_files)
    file << plugin_includes if (!plugin_includes.empty?)
    file << "\n"
    file << "/* Ignore the following warnings, since we are copying code */\n"
    file << "#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)\n"
    file << "#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))\n"
    file << "#pragma GCC diagnostic push\n"
    file << "#endif\n"
    file << "#if !defined(__clang__)\n"
    file << "#pragma GCC diagnostic ignored \"-Wpragmas\"\n"
    file << "#endif\n"
    file << "#pragma GCC diagnostic ignored \"-Wunknown-pragmas\"\n"
    file << "#pragma GCC diagnostic ignored \"-Wduplicate-decl-specifier\"\n"
    file << "#endif\n"
    file << "\n"
  end

  def create_typedefs(file, typedefs)
    file << "\n"
    typedefs.each {|typedef| file << "#{typedef}\n" }
    file << "\n\n"
  end

  def create_mock_header_service_call_declarations(file)
    file << "void #{@clean_mock_name}_Init(void);\n"
    file << "void #{@clean_mock_name}_Destroy(void);\n"
    file << "void #{@clean_mock_name}_Verify(void);\n\n"
  end

  def create_mock_header_footer(header)
    header << "\n"
    header << "#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)\n"
    header << "#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))\n"
    header << "#pragma GCC diagnostic pop\n"
    header << "#endif\n"
    header << "#endif\n"
    header << "\n"
    header << "#endif\n"
  end

  def create_source_header_section(file, filename, functions)
    header_file = (@subdir ? @subdir + '/' : '') + filename.gsub(".c",".h")
    file << "/* AUTOGENERATED FILE. DO NOT EDIT. */\n"
    file << "#include <string.h>\n"
    file << "#include <stdlib.h>\n"
    file << "#include <setjmp.h>\n"
    file << "#include \"#{@framework}.h\"\n"
    file << "#include \"cmock.h\"\n"
    @includes_c_pre_header.each {|inc| file << "#include #{inc}\n"}
    file << "#include \"#{header_file}\"\n"
    @includes_c_post_header.each {|inc| file << "#include #{inc}\n"}
    file << "\n"
    strs = []
    functions.each do |func|
      strs << func[:name]
      func[:args].each {|arg| strs << arg[:name] }
    end
    strs.uniq.sort.each do |str|
      file << "static const char* CMockString_#{str} = \"#{str}\";\n"
    end
    file << "\n"
  end

  def create_instance_structure(file, functions)
    functions.each do |function|
      file << "typedef struct _CMOCK_#{function[:name]}_CALL_INSTANCE\n{\n"
      file << "  UNITY_LINE_TYPE LineNumber;\n"
      file << @plugins.run(:instance_typedefs, function)
      file << "\n} CMOCK_#{function[:name]}_CALL_INSTANCE;\n\n"
    end
    file << "static struct #{@clean_mock_name}Instance\n{\n"
    if (functions.size == 0)
      file << "  unsigned char placeHolder;\n"
    end
    functions.each do |function|
      file << @plugins.run(:instance_structure, function)
      file << "  CMOCK_MEM_INDEX_TYPE #{function[:name]}_CallInstance;\n"
    end
    file << "} Mock;\n\n"
  end

  def create_extern_declarations(file)
    file << "extern jmp_buf AbortFrame;\n"
    if (@ordered)
      file << "extern int GlobalExpectCount;\n"
      file << "extern int GlobalVerifyOrder;\n"
    end
    file << "\n"
  end

  def create_mock_verify_function(file, functions)
    file << "void #{@clean_mock_name}_Verify(void)\n{\n"
    verifications = functions.collect {|function| @plugins.run(:mock_verify, function)}.join
    file << "  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;\n" unless verifications.empty?
    file << verifications
    file << "}\n\n"
  end

  def create_mock_init_function(file)
    file << "void #{@clean_mock_name}_Init(void)\n{\n"
    file << "  #{@clean_mock_name}_Destroy();\n"
    file << "}\n\n"
  end

  def create_mock_destroy_function(file, functions)
    file << "void #{@clean_mock_name}_Destroy(void)\n{\n"
    file << "  CMock_Guts_MemFreeAll();\n"
    file << "  memset(&Mock, 0, sizeof(Mock));\n"
    file << functions.collect {|function| @plugins.run(:mock_destroy, function)}.join

    unless (@fail_on_unexpected_calls)
      file << functions.collect {|function| @plugins.run(:mock_ignore, function)}.join
    end

    if (@ordered)
      file << "  GlobalExpectCount = 0;\n"
      file << "  GlobalVerifyOrder = 0;\n"
    end
    file << "}\n\n"
  end

  def create_mock_implementation(file, function)
    # prepare return value and arguments
    function_mod_and_rettype = (function[:modifier].empty? ? '' : "#{function[:modifier]} ") +
                               (function[:return][:type]) +
                               (function[:c_calling_convention] ? " #{function[:c_calling_convention]}" : '')
    args_string = function[:args_string]
    args_string += (", " + function[:var_arg]) unless (function[:var_arg].nil?)

    # Create mock function
    if (not @weak.empty?)
        file << "#if defined (__IAR_SYSTEMS_ICC__)\n"
        file << "#pragma weak __cmock__#{function[:name]}\n"
        file << "#else\n"
        file << "#{function_mod_and_rettype} __cmock__#{function[:name]}(#{args_string}) #{weak};\n"
        file << "#endif\n\n"
    end
    file << "#{function_mod_and_rettype} __cmock__#{function[:name]}(#{args_string})\n"
    file << "{\n"
    file << "  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;\n"
    file << "  CMOCK_#{function[:name]}_CALL_INSTANCE* cmock_call_instance;\n"
    file << "  UNITY_SET_DETAIL(CMockString_#{function[:name]});\n"
    file << "  cmock_call_instance = (CMOCK_#{function[:name]}_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.#{function[:name]}_CallInstance);\n"
    file << "  Mock.#{function[:name]}_CallInstance = CMock_Guts_MemNext(Mock.#{function[:name]}_CallInstance);\n"
    file << @plugins.run(:mock_implementation_precheck, function)
    file << "  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);\n"
    file << "  cmock_line = cmock_call_instance->LineNumber;\n"
    if (@ordered)
      file << "  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)\n"
      file << "    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);\n"
      file << "  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)\n"
      file << "    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);\n"
    end
    file << @plugins.run(:mock_implementation, function)
    file << "  UNITY_CLR_DETAILS();\n"
    file << "  return cmock_call_instance->ReturnVal;\n" unless (function[:return][:void?])
    file << "}\n\n"
  end

  def create_mock_interfaces(file, function)
    file << @utils.code_add_argument_loader(function)
    file << @plugins.run(:mock_interfaces, function)
  end
end
