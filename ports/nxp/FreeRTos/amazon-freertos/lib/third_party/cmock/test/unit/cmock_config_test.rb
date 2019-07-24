# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================


require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_config'


describe CMockConfig, "Verify CMockConfig Module" do

  it "use default settings when no parameters are specified" do
    config = CMockConfig.new
    assert_equal(CMockConfig::CMockDefaultOptions[:mock_path],             config.mock_path)
    assert_equal(CMockConfig::CMockDefaultOptions[:includes],              config.includes)
    assert_equal(CMockConfig::CMockDefaultOptions[:attributes],            config.attributes)
    assert_equal(CMockConfig::CMockDefaultOptions[:plugins],               config.plugins)
    assert_equal(CMockConfig::CMockDefaultOptions[:treat_externs],         config.treat_externs)
  end

  it "replace only options specified in a hash" do
    test_includes = ['hello']
    test_attributes = ['blah', 'bleh']
    config = CMockConfig.new(:includes => test_includes, :attributes => test_attributes)
    assert_equal(CMockConfig::CMockDefaultOptions[:mock_path],              config.mock_path)
    assert_equal(test_includes,                                             config.includes)
    assert_equal(test_attributes,                                           config.attributes)
    assert_equal(CMockConfig::CMockDefaultOptions[:plugins],                config.plugins)
    assert_equal(CMockConfig::CMockDefaultOptions[:treat_externs],          config.treat_externs)
  end

  it "replace only options specified in a yaml file" do
    test_plugins = [:soda, :pizza]
    config = CMockConfig.new("#{File.expand_path(File.dirname(__FILE__))}/cmock_config_test.yml")
    assert_equal(CMockConfig::CMockDefaultOptions[:mock_path],              config.mock_path)
    assert_equal(CMockConfig::CMockDefaultOptions[:includes],               config.includes)
    assert_equal(test_plugins,                                              config.plugins)
    assert_equal(:include,                                                  config.treat_externs)
  end

  it "populate treat_as map with internal standard_treat_as_map defaults, redefine defaults, and add custom values" do

    user_treat_as1 = {
      'BOOL'          => 'UINT8', # redefine standard default
      'unsigned long' => 'INT',   # redefine standard default
      'U8'            => 'UINT8', # custom value
      'U16'           => 'UINT16' # custom value
    }
    user_treat_as2 = {
      'BOOL'          => 'INT16', # redefine standard default
      'U16'           => 'HEX16'  # custom value
    }

    config1 = CMockConfig.new({:treat_as => user_treat_as1})
    config2 = CMockConfig.new({:treat_as => user_treat_as2})

    # ----- USER SET 1
    # standard defaults
    assert_equal('INT',      config1.treat_as['BOOL_T'])
    assert_equal('HEX32',    config1.treat_as['unsigned int'])
    assert_equal('HEX8_ARRAY',config1.treat_as['void*'])
    assert_equal('STRING',   config1.treat_as['CSTRING'])
    assert_equal('STRING',   config1.treat_as['char*'])
    assert_equal('HEX8',     config1.treat_as['unsigned char'])
    assert_equal('INT',      config1.treat_as['long'])
    assert_equal('INT16',    config1.treat_as['short'])

    # overrides
    assert_equal('UINT8',    config1.treat_as['BOOL'])
    assert_equal('INT',      config1.treat_as['unsigned long'])

    # added custom values
    assert_equal('UINT8',    config1.treat_as['U8'])
    assert_equal('UINT16',   config1.treat_as['U16'])

    # standard_treat_as_map: unchanged
    assert_equal('INT',      config1.standard_treat_as_map['BOOL'])
    assert_equal('HEX32',    config1.standard_treat_as_map['unsigned long'])
    assert_equal('STRING',   config1.standard_treat_as_map['char*'])

    # ----- USER SET 2
    # standard defaults
    assert_equal('INT',      config2.treat_as['BOOL_T'])
    assert_equal('HEX32',    config2.treat_as['unsigned int'])
    assert_equal('HEX8_ARRAY',config2.treat_as['void*'])
    assert_equal('STRING',   config2.treat_as['CSTRING'])
    assert_equal('STRING',   config2.treat_as['char*'])
    assert_equal('HEX8',     config2.treat_as['unsigned char'])
    assert_equal('INT',      config2.treat_as['long'])
    assert_equal('INT16',    config2.treat_as['short'])
    assert_equal('HEX32',    config2.treat_as['unsigned long'])

    # overrides
    assert_equal('INT16',    config2.treat_as['BOOL'])

    # added custom values
    assert_equal('HEX16',    config2.treat_as['U16'])

    # standard_treat_as_map: unchanged
    assert_equal('INT',      config2.standard_treat_as_map['BOOL'])
    assert_equal('HEX32',    config2.standard_treat_as_map['unsigned long'])
    assert_equal('STRING',   config2.standard_treat_as_map['char*'])
  end

  it "standard treat_as map should be incorruptable" do
    config = CMockConfig.new({})

    assert_equal('INT', config.standard_treat_as_map['BOOL_T'])

    local = config.standard_treat_as_map
    local['BOOL_T'] = "U8"

    assert_equal('INT', config.standard_treat_as_map['BOOL_T'])
  end

end
