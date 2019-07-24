# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

$ThisIsOnlyATest = true

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_header_parser'

describe CMockHeaderParser, "Verify CMockHeaderParser Module" do

  before do
    create_mocks :config
    @test_name = 'test_file.h'
    @config.expect :strippables, ["STRIPPABLE"]
    @config.expect :attributes, ['__ramfunc', 'funky_attrib', 'SQLITE_API']
    @config.expect :c_calling_conventions, ['__stdcall']
    @config.expect :treat_as_void, ['MY_FUNKY_VOID']
    @config.expect :treat_as, { "BANJOS" => "INT", "TUBAS" => "HEX16"}
    @config.expect :when_no_prototypes, :error
    @config.expect :verbosity, 1
    @config.expect :treat_externs, :exclude

    @parser = CMockHeaderParser.new(@config)
  end

  after do
  end

  it "create and initialize variables to defaults appropriately" do
    assert_equal([], @parser.funcs)
    assert_equal(['const', '__ramfunc', 'funky_attrib', 'SQLITE_API'], @parser.c_attributes)
    assert_equal(['void','MY_FUNKY_VOID'], @parser.treat_as_void)
  end

  it "strip out line comments" do
    source =
      " abcd;\n" +
      "// hello;\n" +
      "who // is you\n"

    expected =
    [
      "abcd",
      "who"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove block comments" do
    source =
      " no_comments;\n" +
      "// basic_line_comment;\n" +
      "/* basic_block_comment;*/\n" +
      "pre_block; /* start_of_block_comment;\n" +
      "// embedded_line_comment_in_block_comment; */\n" +
      "// /* commented_out_block_comment_line\n" +
      "shown_because_block_comment_invalid_from_line_comment;\n" +
      "// */\n" +
      "//* shorter_commented_out_block_comment_line; \n" +
      "shown_because_block_comment_invalid_from_shorter_line_comment;\n" +
      "/*/\n" +
      "not_shown_because_line_above_started_comment;\n" +
      "//*/\n" +
      "/* \n" +
      "not_shown_because_block_comment_started_this_time;\n" +
      "/*/\n" +
      "shown_because_line_above_ended_comment_this_time;\n" +
      "//*/\n"

    expected =
    [
      "no_comments",
      "pre_block",
      "shown_because_block_comment_invalid_from_line_comment",
      "shown_because_block_comment_invalid_from_shorter_line_comment",
      "shown_because_line_above_ended_comment_this_time"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove strippables from the beginning or end of function declarations" do
    source =
      "void* my_calloc(size_t, size_t) STRIPPABLE;\n" +
      "void\n" +
      "  my_realloc(void*, size_t) STRIPPABLE;\n" +
      "extern int\n" +
      "  my_printf (void *my_object, const char *my_format, ...)\n" +
      "  STRIPPABLE;\n" +
      "  void STRIPPABLE universal_handler ();\n"

    expected =
    [
      "void* my_calloc(size_t, size_t)",
      "void my_realloc(void*, size_t)",
      "void universal_handler()"
    ]

    assert_equal(expected, @parser.import_source(source))
  end

  it "remove gcc's function __attribute__'s" do
    source =
      "void* my_calloc(size_t, size_t) __attribute__((alloc_size(1,2)));\n" +
      "void\n" +
      "  my_realloc(void*, size_t) __attribute__((alloc_size(2)));\n" +
      "extern int\n" +
      "  my_printf (void *my_object, const char *my_format, ...)\n" +
      "  __attribute__ ((format (printf, 2, 3)));\n" +
      "  void __attribute__ ((interrupt)) universal_handler ();\n"

    expected =
    [
      "void* my_calloc(size_t, size_t)",
      "void my_realloc(void*, size_t)",
      "void universal_handler()"
    ]

    assert_equal(expected, @parser.import_source(source))
  end

  it "remove preprocessor directives" do
    source =
      "#when stuff_happens\n" +
      "#ifdef _TEST\n" +
      "#pragma stack_switch"

    expected = []

    assert_equal(expected, @parser.import_source(source))
  end


  it "remove assembler pragma sections" do
    source =
      " #pragma\tasm\n" +
      "  .foo\n" +
      "  lda %m\n" +
      "  nop\n" +
      "# pragma  endasm \n" +
      "foo"

    expected = ["foo"]

    assert_equal(expected, @parser.import_source(source))
  end


  it "smush lines together that contain continuation characters" do
    source =
      "hoo hah \\\n" +
      "when \\ \n"

    expected =
    [
      "hoo hah when"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove C macro definitions" do
    source =
      "#define this is the first line\\\n" +
      "and the second\\\n" +
      "and the third that should be removed\n" +
      "but I'm here\n"

    expected = ["but I'm here"]

    assert_equal(expected, @parser.import_source(source))
  end


  it "remove typedef statements" do
    source =
      "typedef uint32 (unsigned int);\n" +
      "const typedef int INT;\n" +
      "int notatypedef;\n" +
      "int typedef_isnt_me;\n" +
      " typedef who cares what really comes here \n" + # exercise multiline typedef
      "   continuation;\n" +
      "this should remain!;\n" +
      "typedef blah bleh;\n" +
      "typedef struct shell_command_struct {\n" +
      "  char_ptr COMMAND;\n" +
      "  int_32 (*SHELL_FUNC)(int_32 argc);\n" +
      "} SHELL_COMMAND_STRUCT, * SHELL_COMMAND_PTR;\n" +
      "typedef struct shell_command_struct  {\n" +
      "  char_ptr  COMMAND;\n" +
      "  int_32      (*SHELL_FUNC)(int_32 argc, char_ptr argv[]);\n" +
      "} SHELL_COMMAND_STRUCT, * SHELL_COMMAND_PTR;\n" +
      "typedef struct shell_command_struct {\n" +
      " char_ptr COMMAND;\n" +
      " int_32 (*SHELL_FUNC)(int_32 argc);\n" +
      "};\n"

    expected =
    [
      "int notatypedef",
      "int typedef_isnt_me",
      "this should remain!"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove enum statements" do
    source =
      "enum _NamedEnum {\n" +
      " THING1 = (0x0001),\n" +
      " THING2 = (0x0001 << 5),\n" +
      "}ListOValues;\n\n" +
      "don't delete me!!\n" +
      " modifier_str enum _NamedEnum {THING1 = (0x0001), THING2 = (0x0001 << 5)} ListOValues;\n\n" +
      "typedef enum {\n" +
      " THING1,\n" +
      " THING2,\n" +
      "} Thinger;\n" +
      "or me!!\n"

    assert_equal(["don't delete me!! or me!!"], @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove union statements" do
    source =
      "union _NamedDoohicky {\n" +
      " unsigned int a;\n" +
      " char b;\n" +
      "} Doohicky;\n\n" +
      "I want to live!!\n" +
      "some_modifier union { unsigned int a; char b;} Whatever;\n" +
      "typedef union {\n" +
      " unsigned int a;\n" +
      " char b;\n" +
      "} Whatever;\n" +
      "me too!!\n"

    assert_equal(["I want to live!! me too!!"], @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove struct statements" do
    source =
      "struct _NamedStruct1 {\n" +
      " unsigned int a;\n" +
      " signed long int b;\n" +
      "} Thing ;\n\n" +
      "extern struct ForwardDeclared_t TestDataType1;\n" +
      "void foo(void);\n" +
      "struct\n"+
      "   MultilineForwardDeclared_t\n" +
      "   TestDataType2;\n" +
      "struct THINGER foo(void);\n" +
      "typedef struct {\n" +
      " unsigned int a;\n" +
      " signed char b;\n" +
      "}Thinger;\n" +
      "I want to live!!\n"

    assert_equal(["void foo(void)", "struct THINGER foo(void)", "I want to live!!"],
                 @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove externed and inline functions" do
    source =
      " extern uint32 foobar(unsigned int);\n" +
      "uint32 extern_name_func(unsigned int);\n" +
      "uint32 funcinline(unsigned int);\n" +
      "extern void bar(unsigned int);\n" +
      "inline void bar(unsigned int);\n" +
      "extern\n" +
      "void kinda_ugly_on_the_next_line(unsigned int);\n"

    expected =
    [
      "uint32 extern_name_func(unsigned int)",
      "uint32 funcinline(unsigned int)"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove function definitions but keep function declarations" do
    source =
      "uint32 func_with_decl_a(unsigned int);\n" +
      "uint32 func_with_decl_a(unsigned int a) { return a; }\n" +
      "uint32 func_with_decl_b(unsigned int);\n" +
      "uint32 func_with_decl_b(unsigned int a)\n" +
      "{\n" +
      "    bar((unsigned int) a);\n" +
      "    stripme(a);\n" +
      "}\n"

    expected =
    [
      "uint32 func_with_decl_a(unsigned int)",
      "uint32 func_with_decl_a",                 #okay. it's not going to be interpretted as another function
      "uint32 func_with_decl_b(unsigned int)",
      "uint32 func_with_decl_b",                 #okay. it's not going to be interpretted as another function
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove function definitions with nested braces but keep function declarations" do
    source =
      "uint32 func_with_decl_a(unsigned int);\n" +
      "uint32 func_with_decl_a(unsigned int a) {\n" +
      "  while (stuff) {\n" +
      "    not_a_definition1(void);\n" +
      "  }\n" +
      "  not_a_definition2(blah, bleh);\n" +
      "  return a;\n" +
      "}\n" +
      "uint32 func_with_decl_b(unsigned int);\n" +
      "uint32 func_with_decl_b(unsigned int a)\n" +
      "{\n" +
      "    bar((unsigned int) a);\n" +
      "    stripme(a);\n" +
      "}\n" +
      "uint32 func_with_decl_c(unsigned int);\n" +
      "uint32 func_with_decl_c(unsigned int a)\n" +
      "{\n" +
      "    if(a > 0)\n" +
      "    {\n" +
      "       return 1;\n" +
      "    }\n" +
      "    else\n"+
      "    {\n" +
      "       return 2;\n" +
      "    }\n" +
      "}\n"

    expected =
    [
      "uint32 func_with_decl_a(unsigned int)",
      "uint32 func_with_decl_a",                 #okay. it's not going to be interpretted as another function
      "uint32 func_with_decl_b(unsigned int)",
      "uint32 func_with_decl_b",                 #okay. it's not going to be interpretted as another function
      "uint32 func_with_decl_c(unsigned int)",
      "uint32 func_with_decl_c",                 #okay. it's not going to be interpretted as another function
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove a fully defined inline function" do
    source =
      "inline void foo(unsigned int a) { oranges = a; }\n" +
      "inline void bar(unsigned int a) { apples = a; };\n" +
      "inline void bar(unsigned int a)\n" +
      "{" +
      "  bananas = a;\n" +
      "}"

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "remove a fully defined inline function that is multiple lines" do
    source =
      "inline void bar(unsigned int a)\n" +
      "{" +
      "  bananas = a;\n" +
      "  grapes = a;\n" +
      "  apples(bananas, grapes);\n" +
      "}"

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "remove a fully defined inline function that contains nested braces" do
    source =
      "inline void bar(unsigned int a)\n" +
      "{" +
      "  apples(bananas, grapes);\n" +
      "  if (bananas == a)\n" +
      "  {\n" +
      "    oranges(a);\n" +
      "    grapes = a;\n" +
      "  }\n" +
      "  grapefruit(bananas, grapes);\n" +
      "}"

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "remove just inline functions if externs to be included" do
    source =
      " extern uint32 foobar(unsigned int);\n" +
      "uint32 extern_name_func(unsigned int);\n" +
      "uint32 funcinline(unsigned int);\n" +
      "extern void bar(unsigned int);\n" +
      "inline void bar(unsigned int);\n" +
      "extern\n" +
      "void kinda_ugly_on_the_next_line(unsigned int);\n"

    expected =
    [ "extern uint32 foobar(unsigned int)",
      "uint32 extern_name_func(unsigned int)",
      "uint32 funcinline(unsigned int)",
      "extern void bar(unsigned int)",
      "extern void kinda_ugly_on_the_next_line(unsigned int)"
    ]

    @parser.treat_externs = :include
    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove defines" do
    source =
      "#define whatever you feel like defining\n" +
      "void hello(void);\n" +
      "#DEFINE I JUST DON'T CARE\n" +
      "#deFINE\n" +
      "#define get_foo() \\\n   ((Thing)foo.bar)" # exercise multiline define

    expected =
    [
      "void hello(void)",
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove keywords that would keep things from going smoothly in the future" do
    source =
      "const int TheMatrix(register int Trinity, unsigned int *restrict Neo)"

    expected =
    [
      "const int TheMatrix(int Trinity, unsigned int * Neo)",
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  # some code actually typedef's void even though it's not ANSI C and is, frankly, weird
  # since cmock treats void specially, we can't let void be obfuscated
  it "handle odd case of typedef'd void returned" do
    source = "MY_FUNKY_VOID FunkyVoidReturned(int a)"
    expected = { :var_arg=>nil,
                 :name=>"FunkyVoidReturned",
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[{:type=>"int", :name=>"a", :ptr? => false, :const? => false, :const_ptr? => false}],
                 :args_string=>"int a",
                 :args_call=>"a"}
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "handle odd case of typedef'd void as arg" do
    source = "int FunkyVoidAsArg(MY_FUNKY_VOID)"
    expected = { :var_arg=>nil,
                 :name=>"FunkyVoidAsArg",
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[],
                 :args_string=>"void",
                 :args_call=>"" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "handle odd case of typedef'd void as arg pointer" do
    source = "char FunkyVoidPointer(MY_FUNKY_VOID* bluh)"
    expected = { :var_arg=>nil,
                 :name=>"FunkyVoidPointer",
                 :return=>{ :type   => "char",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "char cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[{:type=>"MY_FUNKY_VOID*", :name=>"bluh", :ptr? => true, :const? => false, :const_ptr? => false}],
                 :args_string=>"MY_FUNKY_VOID* bluh",
                 :args_call=>"bluh" }
    assert_equal(expected, @parser.parse_declaration(source))
  end


  it "strip default values from function parameter lists" do
    source =
      "void Foo(int a = 57, float b=37.52, char c= 'd', char* e=\"junk\");\n"

    expected =
    [
      "void Foo(int a, float b, char c, char* e)"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "raise upon empty file" do
    source = ''

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "clean up module names that contain spaces, dashes, and such" do
    source = 'void meh(int (*func)(int));'

    retval = @parser.parse("C:\Ugly Module-Name", source)
    assert (retval[:typedefs][0] =~ /CUglyModuleName/)
  end

  it "raise upon no function prototypes found in file" do
    source =
      "typedef void SILLY_VOID_TYPE1;\n" +
      "typedef (void) SILLY_VOID_TYPE2 ;\n" +
      "typedef ( void ) (*FUNCPTR)(void);\n\n" +
      "#define get_foo() \\\n   ((Thing)foo.bar)"

    # ensure it's expected type of exception
    assert_raises(RuntimeError) do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end


  it "raise upon prototype parsing failure" do
    source = "void (int, )"

    # ensure it's expected type of exception
    assert_raises(RuntimeError) do
      @parser.parse("module", source)
    end

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert(e.message.include?("Failed Parsing Declaration Prototype!"))
    end
  end

  it "extract and return function declarations with retval and args" do

    source = "int Foo(int a, unsigned int b)"
    expected = { :var_arg=>nil,
                 :name=>"Foo",
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"int", :name=>"a", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int", :name=>"b", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int a, unsigned int b",
                 :args_call=>"a, b" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract and return function declarations with no retval" do

    source = "void    FunkyChicken(    uint la,  int     de, bool da)"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyChicken",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"uint", :name=>"la", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int",  :name=>"de", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"bool", :name=>"da", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"uint la, int     de, bool da",
                 :args_call=>"la, de, da" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract and return function declarations with implied voids" do

    source = "void tat()"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"tat",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ ],
                 :args_string=>"void",
                 :args_call=>"" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract modifiers properly" do

    source = "const int TheMatrix(int Trinity, unsigned int * Neo)"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => true,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :name=>"TheMatrix",
                 :modifier=>"const",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int*", :name=>"Neo",     :ptr? => true,  :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int Trinity, unsigned int* Neo",
                 :args_call=>"Trinity, Neo" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract c calling conventions properly" do

    source = "const int __stdcall TheMatrix(int Trinity, unsigned int * Neo)"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => true,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :name=>"TheMatrix",
                 :modifier=>"const",
                 :c_calling_convention=>"__stdcall",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int*", :name=>"Neo",     :ptr? => true,  :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int Trinity, unsigned int* Neo",
                 :args_call=>"Trinity, Neo" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "fully parse multiple prototypes" do

    source = "const int TheMatrix(int Trinity, unsigned int * Neo);\n" +
             "int Morpheus(int, unsigned int*);\n"

    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => true,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"TheMatrix",
                  :modifier=>"const",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"unsigned int*", :name=>"Neo",     :ptr? => true,  :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Trinity, unsigned int* Neo",
                 :args_call=>"Trinity, Neo" },
                { :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"Morpheus",
                  :modifier=>"",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"int",           :name=>"cmock_arg1", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"unsigned int*", :name=>"cmock_arg2", :ptr? => true,  :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int cmock_arg1, unsigned int* cmock_arg2",
                 :args_call=>"cmock_arg1, cmock_arg2"
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "not extract for mocking multiply defined prototypes" do

    source = "const int TheMatrix(int Trinity, unsigned int * Neo);\n" +
             "const int TheMatrix(int, unsigned int*);\n"

    expected = [{ :var_arg=>nil,
                  :name=>"TheMatrix",
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => true,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :modifier=>"const",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"unsigned int*", :name=>"Neo", :ptr? => true,      :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Trinity, unsigned int* Neo",
                  :args_call=>"Trinity, Neo"
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "should properly handle const before return type" do
    sources = [
      "const int * PorkRoast(void);\n",
      "const int* PorkRoast(void);\n",
      "const int *PorkRoast(void);\n"
    ]

    expected = [{ :var_arg => nil,
                  :name    => "PorkRoast",
                  :return  => { :type       => "const int*",
                                :name       => 'cmock_to_return',
                                :ptr?       => true,
                                :const?     => true,
                                :const_ptr? => false,
                                :str        => "const int* cmock_to_return",
                                :void?      => false
                              },
                  :modifier      => "",
                  :contains_ptr? => false,
                  :args          => [],
                  :args_string   => "void",
                  :args_call     => ""
                }]

    sources.each do |source|
      assert_equal(expected, @parser.parse("module", source)[:functions])
    end
  end

  it "should properly handle const before return type" do
    sources = [
      "int const * PorkRoast(void);\n",
      "int const* PorkRoast(void);\n",
      "int const *PorkRoast(void);\n"
    ]

    expected = [{ :var_arg => nil,
                  :name    => "PorkRoast",
                  :return  => { :type       => "int const*",
                                :name       => 'cmock_to_return',
                                :ptr?       => true,
                                :const?     => true,
                                :const_ptr? => false,
                                :str        => "int const* cmock_to_return",
                                :void?      => false
                              },
                  :modifier      => "",
                  :contains_ptr? => false,
                  :args          => [],
                  :args_string   => "void",
                  :args_call     => ""
                }]

    sources.each do |source|
      assert_equal(expected, @parser.parse("module", source)[:functions])
    end
  end

  it "should properly handle const applied after asterisk in return type (not legal C, but sometimes used)" do

    source = "int * const PorkRoast(void);\n"

    expected = [{ :var_arg=>nil,
                  :name=>"PorkRoast",
                  :return=> { :type   => "int*",
                              :name   => 'cmock_to_return',
                              :ptr?   => true,
                              :const? => false,
                              :const_ptr? => true,
                              :str    => "int* cmock_to_return",
                              :void?  => false
                            },
                  :modifier=>"const",
                  :contains_ptr? => false,
                  :args=>[],
                  :args_string=>"void",
                  :args_call=>""
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "properly parse const and pointer argument types with no arg names" do

    source = "void foo(int const*, int*const, const int*, const int*const, int const*const, int*, int, const int);\n"

    expected = [{ :name => "foo",
                  :modifier => "",
                  :return => { :type       => "void",
                               :name       => "cmock_to_return",
                               :str        => "void cmock_to_return",
                               :void?      => true,
                               :ptr?       => false,
                               :const?     => false,
                               :const_ptr? => false
                             },
                  :var_arg => nil,
                  :args_string => "int const* cmock_arg1, int* const cmock_arg2, const int* cmock_arg3, const int* const cmock_arg4, " +
                                  "int const* const cmock_arg5, int* cmock_arg6, int cmock_arg7, const int cmock_arg8",
                  :args => [{ :type=>"int const*", :name => "cmock_arg1", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"int*",       :name => "cmock_arg2", :ptr? => true,  :const? => false, :const_ptr? => true  },
                            { :type=>"const int*", :name => "cmock_arg3", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"const int*", :name => "cmock_arg4", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int const*", :name => "cmock_arg5", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int*",       :name => "cmock_arg6", :ptr? => true,  :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "cmock_arg7", :ptr? => false, :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "cmock_arg8", :ptr? => false, :const? => true,  :const_ptr? => false }],
                  :args_call => "cmock_arg1, cmock_arg2, cmock_arg3, cmock_arg4, cmock_arg5, cmock_arg6, cmock_arg7, cmock_arg8",
                  :contains_ptr? => true
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "properly parse const and pointer argument types with arg names" do

    source = "void bar(int const* param1, int*const param2, const int* param3, const int*const param4,\n" +
             "         int const*const param5, int*param6, int param7, const int param8);\n"

    expected = [{ :name => "bar",
                  :modifier => "",
                  :return => { :type       => "void",
                               :name       => "cmock_to_return",
                               :str        => "void cmock_to_return",
                               :void?      => true,
                               :ptr?       => false,
                               :const?     => false,
                               :const_ptr? => false
                             },
                  :var_arg => nil,
                  :args_string => "int const* param1, int* const param2, const int* param3, const int* const param4, " +
                                  "int const* const param5, int* param6, int param7, const int param8",
                  :args => [{ :type=>"int const*", :name => "param1", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"int*",       :name => "param2", :ptr? => true,  :const? => false, :const_ptr? => true  },
                            { :type=>"const int*", :name => "param3", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"const int*", :name => "param4", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int const*", :name => "param5", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int*",       :name => "param6", :ptr? => true,  :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "param7", :ptr? => false, :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "param8", :ptr? => false, :const? => true,  :const_ptr? => false }],
                  :args_call => "param1, param2, param3, param4, param5, param6, param7, param8",
                  :contains_ptr? => true
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "properly detect typedef'd variants of void and use those" do

    source = "typedef (void) FUNKY_VOID_T;\n" +
             "typedef void CHUNKY_VOID_T;\n" +
             "FUNKY_VOID_T DrHorrible(int SingAlong);\n" +
             "int CaptainHammer(CHUNKY_VOID_T);\n"

    expected = [{ :var_arg=>nil,
                  :name=>"DrHorrible",
                  :return  => { :type   => "void",
                                :name   => 'cmock_to_return',
                                :ptr?   => false,
                                :const? => false,
                                :const_ptr? => false,
                                :str    => "void cmock_to_return",
                                :void?  => true
                              },
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"SingAlong", :ptr? => false, :const? => false, :const_ptr? => false} ],
                  :args_string=>"int SingAlong",
                  :args_call=>"SingAlong"
                },
                { :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"CaptainHammer",
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ ],
                  :args_string=>"void",
                  :args_call=>""
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "be ok with structs inside of function declarations" do

    source = "int DrHorrible(struct SingAlong Blog);\n" +
             "void Penny(struct const _KeepYourHeadUp_ * const BillyBuddy);\n" +
             "struct TheseArentTheHammer CaptainHammer(void);\n"

    expected = [{ :var_arg=>nil,
                  :return =>{ :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"DrHorrible",
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"struct SingAlong", :name=>"Blog", :ptr? => false, :const? => false, :const_ptr? => false} ],
                  :args_string=>"struct SingAlong Blog",
                  :args_call=>"Blog"
                },
                { :var_arg=>nil,
                  :return=> { :type   => "void",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "void cmock_to_return",
                              :void?  => true
                            },
                  :name=>"Penny",
                  :modifier=>"",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"struct const _KeepYourHeadUp_*", :name=>"BillyBuddy", :ptr? => true, :const? => true, :const_ptr? => true} ],
                  :args_string=>"struct const _KeepYourHeadUp_* const BillyBuddy",
                  :args_call=>"BillyBuddy"
                },
                { :var_arg=>nil,
                  :return=> { :type   => "struct TheseArentTheHammer",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "struct TheseArentTheHammer cmock_to_return",
                              :void?  => false
                            },
                  :name=>"CaptainHammer",
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ ],
                  :args_string=>"void",
                  :args_call=>""
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions containing unions with union specifier" do
    source = "void OrangePeel(union STARS_AND_STRIPES * a, union AFL_CIO b)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"OrangePeel",
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"union STARS_AND_STRIPES*", :name=>"a", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"union AFL_CIO", :name=>"b", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"union STARS_AND_STRIPES* a, union AFL_CIO b",
                 :args_call=>"a, b" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "not be thwarted by variables named with primitive types as part of the name" do
    source = "void ApplePeel(const unsigned int const_param, int int_param, int integer, char character, int* const constant)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"ApplePeel",
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=> "unsigned int", :name=>"const_param", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"int", :name=>"int_param", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int", :name=>"integer", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"char", :name=>"character", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int*", :name=>"constant", :ptr? => true, :const? => false, :const_ptr? => true}
                        ],
                 :args_string=>"const unsigned int const_param, int int_param, int integer, char character, int* const constant",
                 :args_call=>"const_param, int_param, integer, character, constant" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "not be thwarted by custom types named similarly to primitive types" do
    source = "void LemonPeel(integer param, character thing, longint * junk, constant value, int32_t const number)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"LemonPeel",
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"integer", :name=>"param", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"character", :name=>"thing", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"longint*", :name=>"junk", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"constant", :name=>"value", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int32_t", :name=>"number", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"integer param, character thing, longint* junk, constant value, int32_t const number",
                 :args_call=>"param, thing, junk, value, number" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "handle some of those chains of C name specifiers naturally" do
    source = "void CoinOperated(signed char abc, const unsigned long int xyz_123, unsigned int const abc_123, long long arm_of_the_law)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"CoinOperated",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"signed char", :name=>"abc", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned long int", :name=>"xyz_123", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"unsigned int", :name=>"abc_123", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"long long", :name=>"arm_of_the_law", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"signed char abc, const unsigned long int xyz_123, unsigned int const abc_123, long long arm_of_the_law",
                 :args_call=>"abc, xyz_123, abc_123, arm_of_the_law" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "handle custom types of various formats" do
    source = "void CardOperated(CUSTOM_TYPE abc, CUSTOM_TYPE* xyz_123, CUSTOM_TYPE const abcxyz, struct CUSTOM_TYPE const * const abc123)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"CardOperated",
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"CUSTOM_TYPE", :name=>"abc", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"CUSTOM_TYPE*", :name=>"xyz_123", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"CUSTOM_TYPE", :name=>"abcxyz", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"struct CUSTOM_TYPE const*", :name=>"abc123", :ptr? => true, :const? => true, :const_ptr? => true}
                        ],
                 :args_string=>"CUSTOM_TYPE abc, CUSTOM_TYPE* xyz_123, CUSTOM_TYPE const abcxyz, struct CUSTOM_TYPE const* const abc123",
                 :args_call=>"abc, xyz_123, abcxyz, abc123" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "handle arrays and treat them as pointers or strings" do
    source = "void KeyOperated(CUSTOM_TYPE thing1[], int thing2 [ ], char thing3 [][2 ][ 3], int* thing4[4])"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"KeyOperated",
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"CUSTOM_TYPE*", :name=>"thing1", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"int*", :name=>"thing2", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"char*", :name=>"thing3", :ptr? => false, :const? => false, :const_ptr? => false},  #THIS one will likely change in the future when we improve multidimensional array support
                          {:type=>"int**", :name=>"thing4", :ptr? => true, :const? => false, :const_ptr? => false}    #THIS one will likely change in the future when we improve multidimensional array support
                        ],
                 :args_string=>"CUSTOM_TYPE* thing1, int* thing2, char* thing3, int** thing4",
                 :args_call=>"thing1, thing2, thing3, thing4" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "give a reasonable guess when dealing with weird combinations of custom types and modifiers" do
    source = "void Cheese(unsigned CUSTOM_TYPE abc, unsigned xyz, CUSTOM_TYPE1 CUSTOM_TYPE2 pdq)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"Cheese",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"unsigned CUSTOM_TYPE", :name=>"abc", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned", :name=>"xyz", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"CUSTOM_TYPE1 CUSTOM_TYPE2", :name=>"pdq", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"unsigned CUSTOM_TYPE abc, unsigned xyz, CUSTOM_TYPE1 CUSTOM_TYPE2 pdq",
                 :args_call=>"abc, xyz, pdq" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "extract functions containing a function pointer" do
    source = "void FunkyTurkey(unsigned int (*func_ptr)(int, char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a function pointer with a void" do
    source = "void FunkyTurkey(void (*func_ptr)(void))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(void);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a function pointer with an implied void" do
    source = "void FunkyTurkey(unsigned int (*func_ptr)())"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)();"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a constant function pointer and a pointer in the nested arg list" do
    source = "void FunkyChicken(unsigned int (* const func_ptr)(unsigned long int * , char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyChicken",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 const func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(unsigned long int* , char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  # it "extract functions containing a function pointer taking a vararg" do
    # source = "void FunkyParrot(unsigned int (*func_ptr)(int, char, ...))"
    # expected = [{ :var_arg=>nil,
                 # :return=>{ :type   => "void",
                            # :name   => 'cmock_to_return',
                            # :ptr?   => false,
                            # :const? => false,
                            # :const_ptr? => false,
                            # :str    => "void cmock_to_return",
                            # :void?  => true
                          # },
                 # :name=>"FunkyParrot",
                 # :modifier=>"",
                 # :contains_ptr? => false,
                 # :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        # ],
                 # :args_string=>"cmock_module_func_ptr1 func_ptr",
                 # :args_call=>"func_ptr" }]
    # typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char, ...);"]
    # result = @parser.parse("module", source)
    # assert_equal(expected, result[:functions])
    # assert_equal(typedefs, result[:typedefs])
  # end

  it "extract functions containing a function pointer with extra parenthesis and two sets" do
    source = "void FunkyBudgie(int (((* func_ptr1)(int, char))), void (*func_ptr2)(void))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyBudgie",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr1", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"cmock_module_func_ptr2", :name=>"func_ptr2", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr1, cmock_module_func_ptr2 func_ptr2",
                 :args_call=>"func_ptr1, func_ptr2" }]
    typedefs = ["typedef int(*cmock_module_func_ptr1)(int, char);", "typedef void(*cmock_module_func_ptr2)(void);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a function pointers, structs and other things" do
    source = "struct mytype *FunkyRobin(uint16_t num1, uint16_t num2, void (*func_ptr1)(uint16_t num3, struct mytype2 *s));"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "struct mytype*",
                            :name   => 'cmock_to_return',
                            :ptr?   => true,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "struct mytype* cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyRobin",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"uint16_t", :name=>"num1", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"uint16_t", :name=>"num2", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"cmock_module_func_ptr1", :name=>"func_ptr1", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"uint16_t num1, uint16_t num2, cmock_module_func_ptr1 func_ptr1",
                 :args_call=>"num1, num2, func_ptr1" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(uint16_t num3, struct mytype2* s);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing an anonymous function pointer" do
    source = "void FunkyFowl(unsigned int (* const)(int, char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyFowl",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"cmock_arg1", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 const cmock_arg1",
                 :args_call=>"cmock_arg1" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer" do
    source = "unsigned short (*FunkyPidgeon( const char op_code ))( int, long int )"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyPidgeon",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"char", :name=>"op_code", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"const char op_code",
                 :args_call=>"op_code" }]
    typedefs = ["typedef unsigned short(*cmock_module_func_ptr1)( int, long int );"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer with implied void" do
    source = "unsigned short (*FunkyTweetie())()"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyTweetie",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[],
                 :args_string=>"void",
                 :args_call=>"" }]
    typedefs = ["typedef unsigned short(*cmock_module_func_ptr1)();"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer where everything is a void" do
    source = "void (*   FunkySeaGull(void))(void)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkySeaGull",
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[],
                 :args_string=>"void",
                 :args_call=>"" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(void);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer with some pointer nonsense" do
    source = "unsigned int * (* FunkyMacaw(double* foo, THING *bar))(unsigned int)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyMacaw",
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"double*", :name=>"foo", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"THING*", :name=>"bar", :ptr? => true, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"double* foo, THING* bar",
                 :args_call=>"foo, bar" }]
    typedefs = ["typedef unsigned int *(*cmock_module_func_ptr1)(unsigned int);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract this SQLite3 function with an anonymous function pointer arg (regression test)" do
    source = "SQLITE_API int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int n, void(*)(void*))"
    expected = [{ :var_arg=>nil,
                  :return=>{ :type   => "int",
                             :name   => "cmock_to_return",
                             :ptr?   => false,
                             :const? => false,
                             :const_ptr? => false,
                             :str    => "int cmock_to_return",
                             :void?  => false
                           },
                   :name=>"sqlite3_bind_text",
                   :modifier=>"SQLITE_API",
                   :contains_ptr? => true,
                   :args=>[ {:type=>"sqlite3_stmt*", :name=>"cmock_arg2", :ptr? => true, :const? => false, :const_ptr? => false},
                            {:type=>"int", :name=>"cmock_arg3", :ptr? => false, :const? => false, :const_ptr? => false},
                            {:type=>"const char*", :name=>"cmock_arg4", :ptr? => false, :const? => true, :const_ptr? => false},
                            {:type=>"int", :name=>"n", :ptr? => false, :const? => false, :const_ptr? => false},
                            {:type=>"cmock_module_func_ptr1", :name=>"cmock_arg1", :ptr? => false, :const? => false, :const_ptr? => false}
                          ],
                   :args_string=>"sqlite3_stmt* cmock_arg2, int cmock_arg3, const char* cmock_arg4, int n, cmock_module_func_ptr1 cmock_arg1",
                   :args_call=>"cmock_arg2, cmock_arg3, cmock_arg4, n, cmock_arg1" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(void*);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions with varargs" do
    source = "int XFiles(int Scully, int Mulder, ...);\n"
    expected = [{ :var_arg=>"...",
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"XFiles",
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Scully", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Mulder", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Scully, int Mulder",
                  :args_call=>"Scully, Mulder"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions with void pointers" do
    source = "void* MoreSillySongs(void* stuff);\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "void*",
                              :name   => 'cmock_to_return',
                              :ptr?   => true,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "void* cmock_to_return",
                              :void?  => false
                            },
                  :name=>"MoreSillySongs",
                  :modifier=>"",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"void*", :name=>"stuff", :ptr? => true, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"void* stuff",
                  :args_call=>"stuff"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions with strippable confusing junk like gcc attributes" do
    source = "int LaverneAndShirley(int Lenny, int Squiggy) __attribute__((weak)) __attribute__ ((deprecated));\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"LaverneAndShirley",
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Lenny", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Squiggy", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Lenny, int Squiggy",
                  :args_call=>"Lenny, Squiggy"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions with strippable confusing junk like gcc attributes with parenthesis" do
    source = "int TheCosbyShow(int Cliff, int Claire) __attribute__((weak, alias (\"__f\"));\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"TheCosbyShow",
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Cliff", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Claire", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Cliff, int Claire",
                  :args_call=>"Cliff, Claire"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "divines all permutations of ptr, const, and const_ptr correctly" do
    truth_table = [
      # argument                                           ptr    const  const_ptr
      [ "constNOTconst constNOTconst",                     false, false, false ],
      [ "const constNOTconst constNOTconst",               false, true,  false ],
      [ "constNOTconst const constNOTconst",               false, true,  false ],
      [ "constNOTconst *constNOTconst",                    true,  false, false ],
      [ "const constNOTconst *constNOTconst",              true,  true,  false ],
      [ "constNOTconst const *constNOTconst",              true,  true,  false ],
      [ "constNOTconst *const constNOTconst",              true,  false, true ],
      [ "const constNOTconst *const constNOTconst",        true,  true,  true ],
      [ "constNOTconst const *const constNOTconst",        true,  true,  true ],
      [ "constNOTconst **constNOTconst",                   true,  false, false ],
      [ "const constNOTconst **constNOTconst",             true,  false, false ],
      [ "constNOTconst const **constNOTconst",             true,  false, false ],
      [ "constNOTconst *const *constNOTconst",             true,  true,  false ],
      [ "const constNOTconst *const *constNOTconst",       true,  true,  false ],
      [ "constNOTconst const *const *constNOTconst",       true,  true,  false ],
      [ "constNOTconst **const constNOTconst",             true,  false, true ],
      [ "const constNOTconst **const constNOTconst",       true,  false, true ],
      [ "constNOTconst const **const constNOTconst",       true,  false, true ],
      [ "constNOTconst *const *const constNOTconst",       true,  true,  true ],
      [ "const constNOTconst *const *const constNOTconst", true,  true,  true ],
      [ "constNOTconst const *const *const constNOTconst", true,  true,  true ]
    ]

    truth_table.each do |entry|
      assert_equal(@parser.divine_ptr(entry[0]), entry[1])
      assert_equal(@parser.divine_const(entry[0]), entry[2])
      assert_equal(@parser.divine_ptr_and_const(entry[0]),
        { ptr?: entry[1], const?: entry[2], const_ptr?: entry[3] })
    end
  end

  it "divines ptr correctly for string types" do
    truth_table = [
      # argument                      ptr
      [ "char s",                     false ],
      [ "const char s",               false ],
      [ "char const s",               false ],
      [ "char *s",                    false ],
      [ "const char *s",              false ],
      [ "char const *s",              false ],
      [ "char *const s",              false ],
      [ "const char *const s",        false ],
      [ "char const *const s",        false ],
      [ "char **s",                   true  ],
      [ "const char **s",             true  ],
      [ "char const **s",             true  ],
      [ "char *const *s",             true  ],
      [ "const char *const *s",       true  ],
      [ "char const *const *s",       true  ],
      [ "char **const s",             true  ],
      [ "const char **const s",       true  ],
      [ "char const **const s",       true  ],
      [ "char *const *const s",       true  ],
      [ "const char *const *const s", true  ],
      [ "char const *const *const s", true  ]
    ]

    truth_table.each do |entry|
      assert_equal(@parser.divine_ptr(entry[0]), entry[1])
    end
  end

end
