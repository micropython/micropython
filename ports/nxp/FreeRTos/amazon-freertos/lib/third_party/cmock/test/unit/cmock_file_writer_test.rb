# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_file_writer'

describe CMockFileWriter, "Verify CMockFileWriter Module" do

  before do
    create_mocks :config
    @cmock_file_writer = CMockFileWriter.new(@config)
  end

  after do
  end

  it "complain if a block was not specified when calling create" do
    begin
      @cmock_file_writer.create_file("test.txt")
      assert false, "Should Have Thrown An Error When Calling Without A Block"
    rescue
    end
  end
end
