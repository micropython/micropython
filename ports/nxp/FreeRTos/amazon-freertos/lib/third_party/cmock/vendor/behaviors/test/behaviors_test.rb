require 'test/unit'
require File.expand_path(File.dirname(__FILE__)) + '/../lib/behaviors'
require 'stringio'

loading_developer_test_class_stdout = StringIO.new
saved_stdout = $stdout.dup
$stdout = loading_developer_test_class_stdout

class DeveloperTest
  extend Behaviors
  attr_accessor :flunk_msg, :tested_code

  should "test their code" do
    @tested_code = true
  end
  should "go to meetings"
end

$stdout = saved_stdout
loading_developer_test_class_stdout.rewind
$loading_developer_test_class_output = loading_developer_test_class_stdout.read

class BehaviorsTest < Test::Unit::TestCase


  def setup
    @target = DeveloperTest.new
    assert_nil @target.tested_code, "block called too early"
  end

  #
  # TESTS
  #
  def test_should_called_with_a_block_defines_a_test
    assert @target.methods.include?("test_should_test their code"), "Missing test method"

    @target.send("test_should_test their code")

    assert @target.tested_code, "block not called"
  end

  def test_should_called_without_a_block_does_not_create_a_test_method
    assert !@target.methods.include?("test_should_go to meetings"), "Should not have method"
  end

  def test_should_called_without_a_block_will_give_unimplemented_output_when_class_loads
    unimplemented_output = "UNIMPLEMENTED CASE: Developer should go to meetings"
    assert_match(/#{unimplemented_output}/, $loading_developer_test_class_output)
  end
end
