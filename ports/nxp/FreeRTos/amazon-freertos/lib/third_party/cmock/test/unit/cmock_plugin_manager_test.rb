# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_plugin_manager'
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_expect'
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_ignore'
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_generator_plugin_cexception'

describe CMockPluginManager, "Verify CMockPluginManager Module" do

  before do
    create_mocks :utils, :pluginA, :pluginB
    @config = create_stub(
      :respond_to => true,
      :when_ptr => :compare_data,
      :enforce_strict_ordering => false,
      :ignore => :args_and_calls
    )

    eval "class << @config\ndef plugins\n@plugins||[]\nend\ndef plugins=(val)\n@plugins=val\nend\nend\n"

  end

  after do
  end

  it "return all plugins by default" do
    @config.plugins = ['cexception','ignore']
    @utils.expect :helpers, {}

    @cmock_plugins = CMockPluginManager.new(@config, @utils)

    test_plugins = @cmock_plugins.plugins
    contained = { :expect => false, :ignore => false, :cexception => false }
    test_plugins.each do |plugin|
      contained[:expect]     = true   if plugin.instance_of?(CMockGeneratorPluginExpect)
      contained[:ignore]     = true   if plugin.instance_of?(CMockGeneratorPluginIgnore)
      contained[:cexception] = true   if plugin.instance_of?(CMockGeneratorPluginCexception)
    end
    assert_equal(true, contained[:expect])
    assert_equal(true, contained[:ignore])
    assert_equal(true, contained[:cexception])
  end

  it "return restricted plugins based on config" do
    @utils.expect :helpers, {}

    @cmock_plugins = CMockPluginManager.new(@config, @utils)

    test_plugins = @cmock_plugins.plugins
    contained = { :expect => false, :ignore => false, :cexception => false }
    test_plugins.each do |plugin|
      contained[:expect]     = true   if plugin.instance_of?(CMockGeneratorPluginExpect)
      contained[:ignore]     = true   if plugin.instance_of?(CMockGeneratorPluginIgnore)
      contained[:cexception] = true   if plugin.instance_of?(CMockGeneratorPluginCexception)
    end
    assert_equal(true, contained[:expect])
    assert_equal(false,contained[:ignore])
    assert_equal(false,contained[:cexception])
  end

  it "run a desired method over each plugin requested and return the results" do
    @utils.expect :helpers, {}
    @cmock_plugins = CMockPluginManager.new(@config, @utils)

    @pluginA = create_stub(:test_method => ["This Is An Awesome Test-"])
    @pluginB = create_stub(:test_method => ["And This is Part 2-","Of An Awesome Test"])
    @cmock_plugins.plugins = [@pluginA, @pluginB]

    expected = "This Is An Awesome Test-And This is Part 2-Of An Awesome Test"
    output   = @cmock_plugins.run(:test_method)
    assert_equal(expected, output)
  end

  it "run a desired method and arg list over each plugin requested and return the results" do
    @utils.expect :helpers, {}
    @cmock_plugins = CMockPluginManager.new(@config, @utils)

    @pluginA = create_stub(:test_method => ["This Is An Awesome Test-"])
    @pluginB = create_stub(:test_method => ["And This is Part 2-","Of An Awesome Test"])
    @cmock_plugins.plugins = [@pluginA, @pluginB]

    expected = "This Is An Awesome Test-And This is Part 2-Of An Awesome Test"
    output   = @cmock_plugins.run(:test_method, "chickenpotpie")
    assert_equal(expected, output)
  end
end
