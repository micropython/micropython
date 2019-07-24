require 'test/unit'
require 'fileutils'

class BehaviorsTasksTest < Test::Unit::TestCase
  include FileUtils

  def setup
    @here = File.expand_path(File.dirname(__FILE__))
    @base_cmd = RUBY_PLATFORM[/mswin/] ? 'rake.cmd ' : 'rake '
  end 

  #
  # HELPERS
  #
  def run_behaviors_task
    run_cmd "behaviors"
  end

  def run_behaviors_html_task
    run_cmd "behaviors_html"
  end

  def run_cmd(cmd)
    cd "#{@here}/tasks_test" do
      @report = %x[ #{@base_cmd} #{cmd} ]
    end
  end

  def see_html_task_output_message
    @html_output_filename = "#{@here}/tasks_test/behaviors_doc/behaviors.html"
    assert_match(/Wrote #{@html_output_filename}/, @report)
  end

  def see_that_html_report_file_exits
    assert File.exists?(@html_output_filename), "html output file should exist"
  end

  def html_report_file_should_contain(user_behaviors)
    file_contents = File.read(@html_output_filename)
    user_behaviors.each do |line|
      assert_match(/#{line}/, file_contents)
    end
    rm_rf File.dirname(@html_output_filename)
  end

  #
  # TESTS
  #
  def test_that_behaviors_tasks_should_list_behavioral_definitions_for_the_classes_under_test
    run_behaviors_task
    user_behaviors = [
    "User should:",
    " - be able set user name and age during construction",
    " - be able to get user name and age",
    " - be able to ask if a user is an adult"
    ]
    assert_match(/#{user_behaviors.join("\n")}/, @report)
  end

  def test_that_behaviors_tasks_should_list_behavioral_definitions_for_the_classes_under_test_in_html_output
    run_behaviors_html_task
    see_html_task_output_message
    see_that_html_report_file_exits
    user_behaviors = [
      "User should:",
      "be able set user name and age during construction",
      "be able to get user name and age",
      "be able to ask if a user is an adult"
    ]
    html_report_file_should_contain user_behaviors
  end

end
