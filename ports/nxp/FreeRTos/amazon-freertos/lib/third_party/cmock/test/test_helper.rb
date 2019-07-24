# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================


require 'minitest/autorun'

def create_mocks(*mocks)
  mocks.each do |mock|
    eval "@#{mock} = Minitest::Mock.new"
  end
end

def create_stub(funcs)
  stub = Class.new
  blob = "class << stub\n"
  funcs.each_pair {|k,v| blob += "def #{k.to_s}(unused=nil)\n #{v.inspect}\nend\n" }
  blob += "end"
  eval blob
  stub
end

def test_return
  {
    :int     => {:type => "int",         :name => 'cmock_to_return', :ptr? => false, :const? => false, :void? => false, :str => 'int cmock_to_return'},
    :int_ptr => {:type => "int*",        :name => 'cmock_to_return', :ptr? => true,  :const? => false, :void? => false, :str => 'int* cmock_to_return'},
    :void    => {:type => "void",        :name => 'cmock_to_return', :ptr? => false, :const? => false, :void? => true,  :str => 'void cmock_to_return'},
    :string  => {:type => "const char*", :name => 'cmock_to_return', :ptr? => false, :const? => true,  :void? => false, :str => 'const char* cmock_to_return'},
  }
end

def test_arg
  {
    :int        => {:type => "int",         :name => 'MyInt',       :ptr? => false, :const? => false, :const_ptr? => false},
    :int_ptr    => {:type => "int*",        :name => 'MyIntPtr',    :ptr? => true,  :const? => false, :const_ptr? => false},
    :const_ptr  => {:type => "int*",        :name => 'MyConstPtr',  :ptr? => true,  :const? => false, :const_ptr? => true},
    :double_ptr => {:type => "int const**", :name => 'MyDoublePtr', :ptr? => true,  :const? => true,  :const_ptr? => false},
    :mytype     => {:type => "MY_TYPE",     :name => 'MyMyType',    :ptr? => false, :const? => true,  :const_ptr? => false},
    :mytype_ptr => {:type => "MY_TYPE*",    :name => 'MyMyTypePtr', :ptr? => true,  :const? => false, :const_ptr? => false},
    :string     => {:type => "const char*", :name => 'MyStr',       :ptr? => false, :const? => true,  :const_ptr? => false},
  }
end
