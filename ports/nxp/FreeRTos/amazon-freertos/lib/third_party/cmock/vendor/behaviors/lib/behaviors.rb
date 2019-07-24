=begin rdoc
= Usage
Behaviors provides a single method: should.
  
Instead of naming test methods like:

 def test_something
 end

You declare test methods like:

 should "perform action" do
 end

You may omit the body of a <tt>should</tt> method to describe unimplemented behavior.

 should "perform other action"

When you run your unit tests, empty <tt>should</tt> methods will appear as an 'UNIMPLEMENTED CASE' along with the described behavior.
This is useful for sketching out planned behavior quickly.

Simply <tt>extend Behaviors</tt> in your <tt>TestCase</tt> to start using behaviors. 

  require 'test/unit'
  require 'behaviors'
  require 'user'

  class UserTest < Test::Unit::TestCase
    extend Behaviors
    ...
  end

= Motivation
Test methods typically focus on the name of the method under test instead of its behavior.
Creating test methods with <tt>should</tt> statements focuses on the behavior of an object.
This helps you to think about the role of the object under test.

Using a behavior-driven approach prevents the danger in assuming a one-to-one mapping of method names to 
test method names.
As always, you get the most value by writing the tests first.

For a more complete BDD framework, try RSpec http://rspec.rubyforge.org/
  
= Rake tasks

You can define a <tt>Behaviors::ReportTask</tt> in your <tt>Rakefile</tt> to generate rake tasks that
summarize the behavior of your project.

These tasks are named <tt>behaviors</tt> and <tt>behaviors_html</tt>. They will output to the
console or an html file in the <tt>doc</tt> directory with a list all of your <tt>should</tt> tests.
  Behaviors::ReportTask.new do |t|
    t.pattern = 'test/**/*_test.rb'
  end

You may also initialize the <tt>ReportTask</tt> with a custom name to associate with a particular suite of tests.
  Behaviors::ReportTask.new(:widget_subsystem) do |t|
    t.pattern = 'test/widgets/*_test.rb'
  end

The html report will be placed in the <tt>doc</tt> directory by default.
You can override this default by setting the <tt>html_dir</tt> in the <tt>ReportTask</tt>.
  Behaviors::ReportTask.new do |t|
    t.pattern = 'test/**/*_test.rb'
    t.html_dir = 'behaviors_html_reports'
  end
=end
module Behaviors
  def should(behave,&block)
    mname = "test_should_#{behave}"
    if block
      define_method mname, &block
    else
      puts ">>> UNIMPLEMENTED CASE: #{name.sub(/Test$/,'')} should #{behave}"
    end
  end
end
