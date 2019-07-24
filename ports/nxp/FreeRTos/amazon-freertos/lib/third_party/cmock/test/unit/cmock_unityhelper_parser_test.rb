# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_unityhelper_parser'

describe CMockUnityHelperParser, "Verify CMockUnityHelperParser Module" do

  before do
    create_mocks :config
  end

  after do
  end

  it "ignore lines that are commented out" do
    source =
      " abcd;\n" +
      "// #define UNITY_TEST_ASSERT_EQUAL_CHICKENS(a,b,line,msg) {...};\n" +
      "or maybe // #define UNITY_TEST_ASSERT_EQUAL_CHICKENS(a,b,line,msg) {...};\n\n"
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, source
    @parser = CMockUnityHelperParser.new(@config)
    expected = {}

    assert_equal(expected, @parser.c_types)
  end

  it "ignore stuff in block comments" do
    source =
      " abcd; /*\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_CHICKENS(a,b,line,msg) {...};\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_CHICKENS(a,b,line,msg) {...};\n */\n"
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, source
    @parser = CMockUnityHelperParser.new(@config)
    expected = {}

    assert_equal(expected, @parser.c_types)
  end

  it "notice equal helpers in the proper form and ignore others" do
    source =
      "abcd;\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_TURKEYS_T(a,b,line,msg) {...};\n" +
      "abcd;\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_WRONG_NUM_ARGS(a,b,c,d,e) {...};\n" +
      "#define UNITY_TEST_ASSERT_WRONG_NAME_EQUAL(a,b,c,d) {...};\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_unsigned_funky_rabbits(a,b,c,d) {...};\n" +
      "abcd;\n"
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, source
    @parser = CMockUnityHelperParser.new(@config)
    expected = {
      'TURKEYS_T' => "UNITY_TEST_ASSERT_EQUAL_TURKEYS_T",
      'unsigned_funky_rabbits' => "UNITY_TEST_ASSERT_EQUAL_unsigned_funky_rabbits"
    }

    assert_equal(expected, @parser.c_types)
  end

  it "notice equal helpers that contain arrays" do
    source =
      "abcd;\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_TURKEYS_ARRAY(a,b,c,d,e) {...};\n" +
      "abcd;\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_WRONG_NUM_ARGS_ARRAY(a,b,c,d,e,f) {...};\n" +
      "#define UNITY_TEST_ASSERT_WRONG_NAME_EQUAL_ARRAY(a,b,c,d,e) {...};\n" +
      "#define UNITY_TEST_ASSERT_EQUAL_unsigned_funky_rabbits_ARRAY(a,b,c,d,e) {...};\n" +
      "abcd;\n"
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, source
    @parser = CMockUnityHelperParser.new(@config)
    expected = {
      'TURKEYS*' => "UNITY_TEST_ASSERT_EQUAL_TURKEYS_ARRAY",
      'unsigned_funky_rabbits*' => "UNITY_TEST_ASSERT_EQUAL_unsigned_funky_rabbits_ARRAY"
    }

    assert_equal(expected, @parser.c_types)
  end

  it "pull in the standard set of helpers and add them to my list" do
    pairs = {
      "UINT"          => "HEX32",
      "unsigned long" => "HEX64",
    }
    expected = {
      "UINT"          => "UNITY_TEST_ASSERT_EQUAL_HEX32",
      "unsigned_long" => "UNITY_TEST_ASSERT_EQUAL_HEX64",
      "UINT*"         => "UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY",
      "unsigned_long*"=> "UNITY_TEST_ASSERT_EQUAL_HEX64_ARRAY",
    }
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, pairs
    @config.expect :load_unity_helper, nil
    @parser = CMockUnityHelperParser.new(@config)

    assert_equal(expected, @parser.c_types)
  end

  it "pull in the user specified set of helpers and add them to my list" do
    pairs = {
      "char*"         => "STRING",
      "unsigned  int" => "HEX32",
    }
    expected = {
      "char*"         => "UNITY_TEST_ASSERT_EQUAL_STRING",
      "unsigned_int"  => "UNITY_TEST_ASSERT_EQUAL_HEX32",
      "char**"        => "UNITY_TEST_ASSERT_EQUAL_STRING_ARRAY",
      "unsigned_int*" => "UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY",
    }
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, pairs
    @config.expect :load_unity_helper, nil
    @parser = CMockUnityHelperParser.new(@config)

    assert_equal(expected, @parser.c_types)
  end

  it "be able to fetch helpers on my list" do
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, ""
    @parser = CMockUnityHelperParser.new(@config)
    @parser.c_types = {
      'UINT8'     => "UNITY_TEST_ASSERT_EQUAL_UINT8",
      'UINT16*'   => "UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY",
      'SPINACH'   => "UNITY_TEST_ASSERT_EQUAL_SPINACH",
      'LONG_LONG' => "UNITY_TEST_ASSERT_EQUAL_LONG_LONG"
    }

    [["UINT8","UINT8"],
     ["UINT16*","UINT16_ARRAY"],
     ["const SPINACH","SPINACH"],
     ["LONG LONG","LONG_LONG"] ].each do |ctype, exptype|
      assert_equal(["UNITY_TEST_ASSERT_EQUAL_#{exptype}",''], @parser.get_helper(ctype))
    end
  end

  it "return memory comparison when asked to fetch helper of types not on my list" do
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, ""
    @parser = CMockUnityHelperParser.new(@config)
    @parser.c_types = {
      'UINT8'   => "UNITY_TEST_ASSERT_EQUAL_UINT8",
      'UINT16*' => "UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY",
      'SPINACH' => "UNITY_TEST_ASSERT_EQUAL_SPINACH",
    }

    ["UINT32","SPINACH_T","SALAD","PINEAPPLE"].each do |ctype|
      @config.expect :memcmp_if_unknown, true
      assert_equal(["UNITY_TEST_ASSERT_EQUAL_MEMORY",'&'], @parser.get_helper(ctype))
    end
  end

  it "return memory array comparison when asked to fetch helper of types not on my list" do
    @config.expect :plugins, [:array]
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, ""
    @parser = CMockUnityHelperParser.new(@config)
    @parser.c_types = {
      'UINT8'   => "UNITY_TEST_ASSERT_EQUAL_UINT8",
      'UINT16*' => "UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY",
      'SPINACH' => "UNITY_TEST_ASSERT_EQUAL_SPINACH",
    }

    ["UINT32*","SPINACH_T*"].each do |ctype|
      @config.expect :memcmp_if_unknown, true
      assert_equal(["UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY",''], @parser.get_helper(ctype))
    end
  end

  it "return the array handler if we cannot find the normal handler" do
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, ""
    @parser = CMockUnityHelperParser.new(@config)
    @parser.c_types = {
      'UINT8'   => "UNITY_TEST_ASSERT_EQUAL_UINT8",
      'UINT16*' => "UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY",
      'SPINACH' => "UNITY_TEST_ASSERT_EQUAL_SPINACH",
    }

      assert_equal(["UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY",'&'], @parser.get_helper("UINT16"))
  end

  it "return the normal handler if we cannot find the array handler" do
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, ""
    @parser = CMockUnityHelperParser.new(@config)
    @parser.c_types = {
      'UINT8'   => "UNITY_TEST_ASSERT_EQUAL_UINT8",
      'UINT16'  => "UNITY_TEST_ASSERT_EQUAL_UINT16",
      'SPINACH' => "UNITY_TEST_ASSERT_EQUAL_SPINACH",
    }

      assert_equal(["UNITY_TEST_ASSERT_EQUAL_UINT8",'*'], @parser.get_helper("UINT8*"))
  end

  it "raise error when asked to fetch helper of type not on my list and not allowed to mem check" do
    @config.expect :plugins, [] #not :array
    @config.expect :treat_as, {}
    @config.expect :load_unity_helper, ""
    @config.expect :memcmp_if_unknown, false
    @parser = CMockUnityHelperParser.new(@config)
    @parser.c_types = {
      'UINT8'   => "UNITY_TEST_ASSERT_EQUAL_UINT8",
      'UINT32*' => "UNITY_TEST_ASSERT_EQUAL_UINT32_ARRAY",
      'SPINACH' => "UNITY_TEST_ASSERT_EQUAL_SPINACH",
    }

    assert_raises (RuntimeError) { @parser.get_helper("UINT16") }
  end
end
