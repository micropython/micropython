# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require 'yaml'

SYS_TEST_GEN_ROOT = File.expand_path( File.dirname( __FILE__ ) ) + '/'
GENERATED_PATH    = SYS_TEST_GEN_ROOT + 'generated/'
BUILD_PATH        = SYS_TEST_GEN_ROOT + 'build/'
CASES_PATH        = SYS_TEST_GEN_ROOT + 'cases/'

TYPES_H          = 'types.h'
UNITY_H          = 'unity.h'
CMOCK_H          = 'cmock.h'
UNITY_HELPER_H   = 'unity_helper.h'
UNITY_HELPER_C   = 'unity_helper.c'
MOCKABLE_H       = 'mockable.h'

YAML_EXTENSION = '.yml'
TEST_PREFIX    = 'test_'
MOCK_PREFIX    = 'mock_'
H_EXTENSION    = '.h'
C_EXTENSION    = '.c'


class SystemTestGenerator

  def generate_files(test_cases)
    test_cases.each do |filename|
      yaml_hash = YAML.load_file(filename)

      name = File.basename(filename, YAML_EXTENSION)
      namix = "#{name}_"

      generate_cmock_config(yaml_hash, namix)
      generate_code(yaml_hash, namix, name)
    end
  end

  private

  def generate_cmock_config(yaml_hash, namix)
    cmock_yaml = yaml_hash.clone
    cmock_yaml.delete(:systest)
    cmock = cmock_yaml[:cmock]

    cmock[:mock_path]   = GENERATED_PATH
    cmock[:includes]    = (cmock[:includes] || []) + [namix + TYPES_H]
    cmock[:mock_prefix] = MOCK_PREFIX
    if not yaml_hash[:systest][:unity_helper].nil?
      cmock[:includes]     << namix + UNITY_HELPER_H
      cmock[:unity_helper_path]  = GENERATED_PATH + namix + UNITY_HELPER_H
    end

    File.open(GENERATED_PATH + namix + 'cmock' + YAML_EXTENSION, 'w') do |out|
      YAML.dump(cmock_yaml, out)
    end
  end

  def generate_code(yaml_hash, namix, name)
    generate_types_file(yaml_hash, namix)
    generate_mockable_file(yaml_hash, namix)
    generate_unity_helper_files(yaml_hash, namix)

    generate_test_file(yaml_hash, namix, name)
    generate_source_file(yaml_hash, namix, name)
  end

  def generate_types_file(yaml_hash, namix)
    types = yaml_hash[:systest][:types]
    return if types.nil?

    write_header_file(GENERATED_PATH + namix + TYPES_H, namix.upcase + 'TYPES_H') do |out|
      out.puts(types)
    end
  end

  def generate_mockable_file(yaml_hash, namix)
    mockable = yaml_hash[:systest][:mockable]
    return if mockable.nil?

    write_header_file(GENERATED_PATH + namix + MOCKABLE_H, namix.upcase + 'MOCKABLE_H', [namix + TYPES_H]) do |out|
      out.puts(mockable)
    end
  end

  def generate_unity_helper_files(yaml_hash, namix)
    unity_helper = yaml_hash[:systest][:unity_helper]
    return if unity_helper.nil?

    write_header_file(GENERATED_PATH + namix + UNITY_HELPER_H, namix.upcase + 'UNITY_HELPER_H', [namix + TYPES_H]) do |out|
      out.puts(unity_helper[:header])
    end

    write_source_file(GENERATED_PATH + namix + UNITY_HELPER_C, ["unity.h", namix + UNITY_HELPER_H]) do |out|
      out.puts(unity_helper[:code])
    end
  end

  def generate_test_file(yaml_hash, namix, name)
    tests = yaml_hash[:systest][:tests]
    return if tests.nil?

    includes = [UNITY_H, CMOCK_H]
    includes << (namix + UNITY_HELPER_H) if not yaml_hash[:systest][:unity_helper].nil?
    includes << [MOCK_PREFIX + namix + MOCKABLE_H]
    includes << [name + H_EXTENSION]

    write_source_file(GENERATED_PATH + TEST_PREFIX + name + C_EXTENSION, includes.flatten) do |out|
      out.puts(tests[:common])
      out.puts('')

      tests[:units].each_with_index do |test, index|
        out.puts('// should ' + test[:should])
        out.puts(test[:code].gsub!(/test\(\)/, "void test#{index+1}(void)"))
        out.puts('')
      end
    end
  end

  def generate_source_file(yaml_hash, namix, name)
    source = yaml_hash[:systest][:source]
    return if source.nil?

    header_file = name + H_EXTENSION

    includes = yaml_hash[:systest][:types].nil? ? [] : [(namix + TYPES_H)]

    write_header_file(GENERATED_PATH + header_file, name.upcase, includes) do |out|
      out.puts(source[:header])
    end

    includes = []
    includes << (namix + TYPES_H) if not yaml_hash[:systest][:types].nil?
    includes << (namix + MOCKABLE_H) if not yaml_hash[:systest][:mockable].nil?
    includes << header_file

    write_source_file(GENERATED_PATH + name + C_EXTENSION, includes.flatten) do |out|
      out.puts(source[:code])
    end
  end

  def write_header_file(filename, upcase_name, include_list=[])
    File.open(filename, 'w') do |out|
      out.puts("#ifndef _#{upcase_name}")
      out.puts("#define _#{upcase_name}")
      out.puts('')
      include_list.each do |include|
        out.puts("#include \"#{include}\"")
      end
      out.puts('')
      out.puts("#if defined(__GNUC__) && !defined(__ICC)")
      out.puts("#if !defined(__clang__)")
      out.puts("#pragma GCC diagnostic ignored \"-Wpragmas\"")
      out.puts("#endif")
      out.puts('#pragma GCC diagnostic ignored "-Wunknown-pragmas"')
      out.puts('#pragma GCC diagnostic ignored "-Wduplicate-decl-specifier"')
      out.puts("#endif")
      out.puts('')
      yield(out)
      out.puts('')
      out.puts("#endif // _#{upcase_name}")
      out.puts('')
    end
  end

  def write_source_file(filename, include_list=[])
    File.open(filename, 'w') do |out|
      include_list.each do |include|
        out.puts("#include \"#{include}\"")
      end
      out.puts('')
      out.puts("#if defined(__GNUC__) && !defined(__ICC)")
      out.puts("#if !defined(__clang__)")
      out.puts("#pragma GCC diagnostic ignored \"-Wpragmas\"")
      out.puts("#endif")
      out.puts('#pragma GCC diagnostic ignored "-Wunknown-pragmas"')
      out.puts('#pragma GCC diagnostic ignored "-Wduplicate-decl-specifier"')
      out.puts("#endif")
      out.puts('')
      yield(out)
      out.puts('')
    end
  end

end


if ($0 == __FILE__)
  SystemTestGenerator.new.generate_files(Dir[CASES_PATH + "*#{YAML_EXTENSION}"])
end

