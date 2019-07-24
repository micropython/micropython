
require '../auto/generate_module.rb'
require 'fileutils'

def touch_src(file)
  FileUtils.touch "sandbox/src/#{file}"
end

def touch_test(file)
  FileUtils.touch "sandbox/test/#{file}"
end

def create_src_with_known_content(file)
  File.open("sandbox/src/#{file}", "w") {|f| f.write("the original #{file}")}
end

def create_test_with_known_content(file)
  File.open("sandbox/test/#{file}", "w") {|f| f.write("the original #{file}")}
end

def expect_src_content_didnt_change(file)
  expect(File.read("sandbox/src/#{file}")).to eq("the original #{file}")
end

def expect_test_content_didnt_change(file)
  expect(File.read("sandbox/test/#{file}")).to eq("the original #{file}")
end

def expect_src_file_to_exist(file)
  expect(File.exist?("sandbox/src/#{file}")).to be true
end

def expect_test_file_to_exist(file)
  expect(File.exist?("sandbox/test/#{file}")).to be true
end

describe "UnityModuleGenerator" do

  before do
    # clean sandbox and setup our "project" folders
    FileUtils.rm_rf "sandbox"
    FileUtils.mkdir_p "sandbox"
    FileUtils.mkdir_p "sandbox/src"
    FileUtils.mkdir_p "sandbox/test"

    @options = {
      :path_src => "sandbox/src",
      :path_tst => "sandbox/test",
    }
  end

  context "with src pattern" do
    before do
      @options[:pattern] = "src"
    end

    it "fails when all files already exist" do
      # create an existing triad of files
      touch_src "meh.c"
      touch_src "meh.h"
      touch_test "Testmeh.c"
      expect {
        UnityModuleGenerator.new(@options).generate("meh")
      }.to raise_error("ERROR: File meh already exists. Exiting.")
    end

    it "creates the test file if the source and header files exist" do
      # Create the existing files.
      touch_src "meh.c"
      touch_src "meh.h"

      UnityModuleGenerator.new(@options).generate("meh")

      expect_test_file_to_exist "Testmeh.c"
    end

    it "does not alter existing files" do
      # Create some files with known content.
      create_src_with_known_content "meh.c"
      create_src_with_known_content "meh.h"

      UnityModuleGenerator.new(@options).generate("meh")

      expect_src_content_didnt_change "meh.c"
      expect_src_content_didnt_change "meh.c"
    end

    it "does not alter existing test files" do
      # Create some files with known content.
      create_test_with_known_content "Testmeh.c"

      UnityModuleGenerator.new(@options).generate("meh")

      expect_test_content_didnt_change "Testmeh.c"
    end

  end

  context "with mch pattern" do
    before do
      @options[:pattern] = "mch"
    end

    it "fails when all files exist" do
        touch_src "meh_model.c"
        touch_src "meh_conductor.c"
        touch_src "meh_hardware.c"
        touch_src "meh_model.h"
        touch_src "meh_conductor.h"
        touch_src "meh_hardware.h"
        touch_test "Testmeh_model.c"
        touch_test "Testmeh_conductor.c"
        touch_test "Testmeh_hardware.c"
        expect {
          UnityModuleGenerator.new(@options).generate("meh")
        }.to raise_error("ERROR: File meh_model already exists. Exiting.")
    end

    it "creates files that don't exist" do
      touch_src "meh_model.c"
      touch_src "meh_conductor.c"
      touch_src "meh_hardware.c"
      touch_src "meh_model.h"
      touch_src "meh_conductor.h"

      UnityModuleGenerator.new(@options).generate("meh")

      expect_src_file_to_exist "meh_hardware.h"
      expect_test_file_to_exist "Testmeh_model.c"
      expect_test_file_to_exist "Testmeh_conductor.c"
      expect_test_file_to_exist "Testmeh_hardware.c"
    end

    it "does not alter existing source files" do
      create_src_with_known_content "meh_model.c"
      create_src_with_known_content "meh_model.c"
      create_src_with_known_content "meh_model.c"
      create_src_with_known_content "meh_model.h"
      create_src_with_known_content "meh_model.c"

      UnityModuleGenerator.new(@options).generate("meh")

      expect_src_content_didnt_change "meh_model.c"
      expect_src_content_didnt_change "meh_model.c"
      expect_src_content_didnt_change "meh_model.c"
      expect_src_content_didnt_change "meh_model.c"
    end

    it "does not alter existing test files" do
      create_test_with_known_content "Testmeh_model.c"

      UnityModuleGenerator.new(@options).generate("meh")

      expect_test_content_didnt_change "Testmeh_model.c"
    end

  end
end
