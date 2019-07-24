# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class CMockHeaderParser

  attr_accessor :funcs, :c_attr_noconst, :c_attributes, :treat_as_void, :treat_externs

  def initialize(cfg)
    @funcs = []
    @c_strippables = cfg.strippables
    @c_attr_noconst = cfg.attributes.uniq - ['const']
    @c_attributes = ['const'] + c_attr_noconst
    @c_calling_conventions = cfg.c_calling_conventions.uniq
    @treat_as_void = (['void'] + cfg.treat_as_void).uniq
    @declaration_parse_matcher = /([\d\w\s\*\(\),\[\]]+??)\(([\d\w\s\*\(\),\.\[\]+-]*)\)$/m
    @standards = (['int','short','char','long','unsigned','signed'] + cfg.treat_as.keys).uniq
    @when_no_prototypes = cfg.when_no_prototypes
    @local_as_void = @treat_as_void
    @verbosity = cfg.verbosity
    @treat_externs = cfg.treat_externs
    @c_strippables += ['extern'] if (@treat_externs == :include) #we'll need to remove the attribute if we're allowing externs
  end

  def parse(name, source)
    @module_name = name.gsub(/\W/,'')
    @typedefs = []
    @funcs = []
    function_names = []

    parse_functions( import_source(source) ).map do |decl|
      func = parse_declaration(decl)
      unless (function_names.include? func[:name])
        @funcs << func
        function_names << func[:name]
      end
    end

    { :includes  => nil,
      :functions => @funcs,
      :typedefs  => @typedefs
    }
  end

  private if $ThisIsOnlyATest.nil? ################

  def import_source(source)

    # let's clean up the encoding in case they've done anything weird with the characters we might find
    source = source.force_encoding("ISO-8859-1").encode("utf-8", :replace => nil)

    # void must be void for cmock _ExpectAndReturn calls to process properly, not some weird typedef which equates to void
    # to a certain extent, this action assumes we're chewing on pre-processed header files, otherwise we'll most likely just get stuff from @treat_as_void
    @local_as_void = @treat_as_void
    void_types = source.scan(/typedef\s+(?:\(\s*)?void(?:\s*\))?\s+([\w\d]+)\s*;/)
    if void_types
      @local_as_void += void_types.flatten.uniq.compact
    end

    # smush multiline macros into single line (checking for continuation character at end of line '\')
    source.gsub!(/\s*\\\s*/m, ' ')

    #remove comments (block and line, in three steps to ensure correct precedence)
    source.gsub!(/\/\/(?:.+\/\*|\*(?:$|[^\/])).*$/, '')  # remove line comments that comment out the start of blocks
    source.gsub!(/\/\*.*?\*\//m, '')                     # remove block comments
    source.gsub!(/\/\/.*$/, '')                          # remove line comments (all that remain)

    # remove assembler pragma sections
    source.gsub!(/^\s*#\s*pragma\s+asm\s+.*?#\s*pragma\s+endasm/m, '')

    # remove gcc's __attribute__ tags
    source.gsub!(/__attribute(?:__)?\s*\(\(+.*\)\)+/, '')

    # remove preprocessor statements and extern "C"
    source.gsub!(/^\s*#.*/, '')
    source.gsub!(/extern\s+\"C\"\s*\{/, '')

    # enums, unions, structs, and typedefs can all contain things (e.g. function pointers) that parse like function prototypes, so yank them
    # forward declared structs are removed before struct definitions so they don't mess up real thing later. we leave structs keywords in function prototypes
    source.gsub!(/^[\w\s]*struct[^;\{\}\(\)]+;/m, '')                                      # remove forward declared structs
    source.gsub!(/^[\w\s]*(enum|union|struct|typedef)[\w\s]*\{[^\}]+\}[\w\s\*\,]*;/m, '')  # remove struct, union, and enum definitions and typedefs with braces
    source.gsub!(/(\W)(?:register|auto|static|restrict)(\W)/, '\1\2')                      # remove problem keywords
    source.gsub!(/\s*=\s*['"a-zA-Z0-9_\.]+\s*/, '')                                        # remove default value statements from argument lists
    source.gsub!(/^(?:[\w\s]*\W)?typedef\W[^;]*/m, '')                                     # remove typedef statements
    source.gsub!(/\)(\w)/, ') \1')                                                         # add space between parenthese and alphanumeric
    source.gsub!(/(^|\W+)(?:#{@c_strippables.join('|')})(?=$|\W+)/,'\1') unless @c_strippables.empty? # remove known attributes slated to be stripped

    #scan for functions which return function pointers, because they are a pain
    source.gsub!(/([\w\s\*]+)\(*\(\s*\*([\w\s\*]+)\s*\(([\w\s\*,]*)\)\)\s*\(([\w\s\*,]*)\)\)*/) do |m|
      functype = "cmock_#{@module_name}_func_ptr#{@typedefs.size + 1}"
      @typedefs << "typedef #{$1.strip}(*#{functype})(#{$4});"
      "#{functype} #{$2.strip}(#{$3});"
    end

    # remove nested pairs of braces because no function declarations will be inside of them (leave outer pair for function definition detection)
    if (RUBY_VERSION.split('.')[0].to_i > 1)
      #we assign a string first because (no joke) if Ruby 1.9.3 sees this line as a regex, it will crash.
      r = "\\{([^\\{\\}]*|\\g<0>)*\\}"
      source.gsub!(/#{r}/m, '{ }')
    else
      while source.gsub!(/\{[^\{\}]*\{[^\{\}]*\}[^\{\}]*\}/m, '{ }')
      end
    end

    # remove function definitions by stripping off the arguments right now
    source.gsub!(/\([^\)]*\)\s*\{[^\}]*\}/m, ";")

    #drop extra white space to make the rest go faster
    source.gsub!(/^\s+/, '')          # remove extra white space from beginning of line
    source.gsub!(/\s+$/, '')          # remove extra white space from end of line
    source.gsub!(/\s*\(\s*/, '(')     # remove extra white space from before left parens
    source.gsub!(/\s*\)\s*/, ')')     # remove extra white space from before right parens
    source.gsub!(/\s+/, ' ')          # remove remaining extra white space

    #split lines on semicolons and remove things that are obviously not what we are looking for
    src_lines = source.split(/\s*;\s*/).uniq
    src_lines.delete_if {|line| line.strip.length == 0}                            # remove blank lines
    src_lines.delete_if {|line| !(line =~ /[\w\s\*]+\(+\s*\*[\*\s]*[\w\s]+(?:\[[\w\s]*\]\s*)+\)+\s*\((?:[\w\s\*]*,?)*\s*\)/).nil?}     #remove function pointer arrays
    if (@treat_externs == :include)
      src_lines.delete_if {|line| !(line =~ /(?:^|\s+)(?:inline)\s+/).nil?}        # remove inline functions
    else
      src_lines.delete_if {|line| !(line =~ /(?:^|\s+)(?:extern|inline)\s+/).nil?} # remove inline and extern functions
    end
    src_lines.delete_if {|line| line.empty? } #drop empty lines
  end

  def parse_functions(source)
    funcs = []
    source.each {|line| funcs << line.strip.gsub(/\s+/, ' ') if (line =~ @declaration_parse_matcher)}
    if funcs.empty?
      case @when_no_prototypes
        when :error
          raise "ERROR: No function prototypes found!"
        when :warn
          puts "WARNING: No function prototypes found!" unless (@verbosity < 1)
      end
    end
    return funcs
  end

  def parse_type_and_name(arg)
    # Split up words and remove known attributes.  For pointer types, make sure
    # to remove 'const' only when it applies to the pointer itself, not when it
    # applies to the type pointed to.  For non-pointer types, remove any
    # occurrence of 'const'.
    arg.gsub!(/(\w)\*/,'\1 *') # pull asterisks away from preceding word
    arg.gsub!(/\*(\w)/,'* \1') # pull asterisks away from following word
    arg_array = arg.split
    arg_info = divine_ptr_and_const(arg)
    arg_info[:name] = arg_array[-1]

    attributes = arg.include?('*') ? @c_attr_noconst : @c_attributes
    attr_array = []
    type_array = []

    arg_array[0..-2].each do |word|
      if attributes.include?(word)
        attr_array << word
      elsif @c_calling_conventions.include?(word)
        arg_info[:c_calling_convention] = word
      else
        type_array << word
      end
    end

    if arg_info[:const_ptr?]
      attr_array << 'const'
      type_array.delete_at(type_array.rindex('const'))
    end

    arg_info[:modifier] = attr_array.join(' ')
    arg_info[:type] = type_array.join(' ').gsub(/\s+\*/,'*') # remove space before asterisks
    return arg_info
  end

  def parse_args(arg_list)
    args = []
    arg_list.split(',').each do |arg|
      arg.strip!
      return args if (arg =~ /^\s*((\.\.\.)|(void))\s*$/)   # we're done if we reach void by itself or ...

      arg_info = parse_type_and_name(arg)
      arg_info.delete(:modifier)             # don't care about this
      arg_info.delete(:c_calling_convention) # don't care about this
      args << arg_info
    end
    return args
  end

  def divine_ptr(arg)
    return false unless arg.include? '*'
    # treat "const char *" and similar as a string, not a pointer
    return false if /(^|\s)(const\s+)?char(\s+const)?\s*\*(?!.*\*)/ =~ arg
    return true
  end

  def divine_const(arg)
    # a non-pointer arg containing "const" is a constant
    # an arg containing "const" before the last * is a pointer to a constant
    return ( arg.include?('*') ? (/(^|\s|\*)const(\s(\w|\s)*)?\*(?!.*\*)/ =~ arg)
                               : (/(^|\s)const(\s|$)/ =~ arg) ) ? true : false
  end

  def divine_ptr_and_const(arg)
    divination = {}

    divination[:ptr?] = divine_ptr(arg)
    divination[:const?] = divine_const(arg)

    # an arg containing "const" after the last * is a constant pointer
    divination[:const_ptr?] = (/\*(?!.*\*)\s*const(\s|$)/ =~ arg) ? true : false

    return divination
  end

  def clean_args(arg_list)
    if ((@local_as_void.include?(arg_list.strip)) or (arg_list.empty?))
      return 'void'
    else
      c=0
      arg_list.gsub!(/(\w+)(?:\s*\[\s*\(*[\s\d\w+-]*\)*\s*\])+/,'*\1')  # magically turn brackets into asterisks, also match for parentheses that come from macros
      arg_list.gsub!(/\s+\*/,'*')                                       # remove space to place asterisks with type (where they belong)
      arg_list.gsub!(/\*(\w)/,'* \1')                                   # pull asterisks away from arg to place asterisks with type (where they belong)

      #scan argument list for function pointers and replace them with custom types
      arg_list.gsub!(/([\w\s\*]+)\(+\s*\*[\*\s]*([\w\s]*)\s*\)+\s*\(((?:[\w\s\*]*,?)*)\s*\)*/) do |m|

        functype = "cmock_#{@module_name}_func_ptr#{@typedefs.size + 1}"
        funcret  = $1.strip
        funcname = $2.strip
        funcargs = $3.strip
        funconst = ''
        if (funcname.include? 'const')
          funcname.gsub!('const','').strip!
          funconst = 'const '
        end
        @typedefs << "typedef #{funcret}(*#{functype})(#{funcargs});"
        funcname = "cmock_arg#{c+=1}" if (funcname.empty?)
        "#{functype} #{funconst}#{funcname}"
      end

      #automatically name unnamed arguments (those that only had a type)
      arg_list.split(/\s*,\s*/).map { |arg|
        parts = (arg.split - ['struct', 'union', 'enum', 'const', 'const*'])
        if ((parts.size < 2) or (parts[-1][-1].chr == '*') or (@standards.include?(parts[-1])))
          "#{arg} cmock_arg#{c+=1}"
        else
          arg
        end
      }.join(', ')
    end
  end

  def parse_declaration(declaration)
    decl = {}

    regex_match = @declaration_parse_matcher.match(declaration)
    raise "Failed parsing function declaration: '#{declaration}'" if regex_match.nil?

    #grab argument list
    args = regex_match[2].strip

    #process function attributes, return type, and name
    parsed = parse_type_and_name(regex_match[1])

    decl[:name] = parsed[:name]
    decl[:modifier] = parsed[:modifier]
    unless parsed[:c_calling_convention].nil?
      decl[:c_calling_convention] = parsed[:c_calling_convention]
    end

    rettype = parsed[:type]
    rettype = 'void' if (@local_as_void.include?(rettype.strip))
    decl[:return] = { :type       => rettype,
                      :name       => 'cmock_to_return',
                      :str        => "#{rettype} cmock_to_return",
                      :void?      => (rettype == 'void'),
                      :ptr?       => parsed[:ptr?],
                      :const?     => parsed[:const?],
                      :const_ptr? => parsed[:const_ptr?]
                    }

    #remove default argument statements from mock definitions
    args.gsub!(/=\s*[a-zA-Z0-9_\.]+\s*/, ' ')

    #check for var args
    if (args =~ /\.\.\./)
      decl[:var_arg] = args.match( /[\w\s]*\.\.\./ ).to_s.strip
      if (args =~ /\,[\w\s]*\.\.\./)
        args = args.gsub!(/\,[\w\s]*\.\.\./,'')
      else
        args = 'void'
      end
    else
      decl[:var_arg] = nil
    end
    args = clean_args(args)
    decl[:args_string] = args
    decl[:args] = parse_args(args)
    decl[:args_call] = decl[:args].map{|a| a[:name]}.join(', ')
    decl[:contains_ptr?] = decl[:args].inject(false) {|ptr, arg| arg[:ptr?] ? true : ptr }

    if (decl[:return][:type].nil?   or decl[:name].nil?   or decl[:args].nil? or
        decl[:return][:type].empty? or decl[:name].empty?)
      raise "Failed Parsing Declaration Prototype!\n" +
        "  declaration: '#{declaration}'\n" +
        "  modifier: '#{decl[:modifier]}'\n" +
        "  return: #{prototype_inspect_hash(decl[:return])}\n" +
        "  function: '#{decl[:name]}'\n" +
        "  args: #{prototype_inspect_array_of_hashes(decl[:args])}\n"
    end

    return decl
  end

  def prototype_inspect_hash(hash)
    pairs = []
    hash.each_pair { |name, value| pairs << ":#{name} => #{"'" if (value.class == String)}#{value}#{"'" if (value.class == String)}" }
    return "{#{pairs.join(', ')}}"
  end

  def prototype_inspect_array_of_hashes(array)
    hashes = []
    array.each { |hash| hashes << prototype_inspect_hash(hash) }
    case (array.size)
    when 0
      return "[]"
    when 1
      return "[#{hashes[0]}]"
    else
      return "[\n    #{hashes.join("\n    ")}\n  ]\n"
    end
  end

end
