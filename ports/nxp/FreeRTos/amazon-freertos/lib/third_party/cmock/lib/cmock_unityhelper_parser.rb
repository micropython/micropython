# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ========================================== 

class CMockUnityHelperParser
  
  attr_accessor :c_types
  
  def initialize(config)
    @config = config
    @fallback = @config.plugins.include?(:array) ? 'UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY' : 'UNITY_TEST_ASSERT_EQUAL_MEMORY'
    @c_types = map_C_types.merge(import_source)
  end

  def get_helper(ctype)
    lookup = ctype.gsub(/(?:^|(\S?)(\s*)|(\W))const(?:$|(\s*)(\S)|(\W))/,'\1\3\5\6').strip.gsub(/\s+/,'_')
    return [@c_types[lookup], ''] if (@c_types[lookup])
    if (lookup =~ /\*$/)
      lookup = lookup.gsub(/\*$/,'')
      return [@c_types[lookup], '*'] if (@c_types[lookup])
    else
      lookup = lookup + '*'
      return [@c_types[lookup], '&'] if (@c_types[lookup])
    end
    return ['UNITY_TEST_ASSERT_EQUAL_PTR', ''] if (ctype =~ /cmock_\w+_ptr\d+/)
    raise("Don't know how to test #{ctype} and memory tests are disabled!") unless @config.memcmp_if_unknown
    return (lookup =~ /\*$/) ? [@fallback, '&'] : [@fallback, '']
  end
  
  private ###########################
  
  def map_C_types
    c_types = {}
    @config.treat_as.each_pair do |ctype, expecttype|
      c_type = ctype.gsub(/\s+/,'_')
      if (expecttype =~ /\*/)
        c_types[c_type] = "UNITY_TEST_ASSERT_EQUAL_#{expecttype.gsub(/\*/,'')}_ARRAY"
      else
        c_types[c_type] = "UNITY_TEST_ASSERT_EQUAL_#{expecttype}"
        c_types[c_type+'*'] ||= "UNITY_TEST_ASSERT_EQUAL_#{expecttype}_ARRAY"
      end
    end
    c_types
  end
  
  def import_source
    source = @config.load_unity_helper
    return {} if source.nil?
    c_types = {}
    source = source.gsub(/\/\/.*$/, '') #remove line comments
    source = source.gsub(/\/\*.*?\*\//m, '') #remove block comments
     
    #scan for comparison helpers
    match_regex = Regexp.new('^\s*#define\s+(UNITY_TEST_ASSERT_EQUAL_(\w+))\s*\(' + Array.new(4,'\s*\w+\s*').join(',') + '\)')
    pairs = source.scan(match_regex).flatten.compact
    (pairs.size/2).times do |i|
      expect = pairs[i*2]
      ctype = pairs[(i*2)+1]
      c_types[ctype] = expect unless expect.include?("_ARRAY")
    end
      
    #scan for array variants of those helpers
    match_regex = Regexp.new('^\s*#define\s+(UNITY_TEST_ASSERT_EQUAL_(\w+_ARRAY))\s*\(' + Array.new(5,'\s*\w+\s*').join(',') + '\)')
    pairs = source.scan(match_regex).flatten.compact
    (pairs.size/2).times do |i|
      expect = pairs[i*2]
      ctype = pairs[(i*2)+1]
      c_types[ctype.gsub('_ARRAY','*')] = expect
    end
    
    c_types
  end
end
