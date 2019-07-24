CMock: A Summary
================

*[ThrowTheSwitch.org](http://throwtheswitch.org)*

*This documentation is released under a Creative Commons 3.0 Attribution Share-Alike License*


What Exactly Are We Talking About Here?
---------------------------------------

CMock is a nice little tool which takes your header files and creates
a Mock interface for it so that you can more easily unit test modules
that touch other modules. For each function prototype in your
header, like this one:

    int DoesSomething(int a, int b);


...you get an automatically generated DoesSomething function
that you can link to instead of your real DoesSomething function.
By using this Mocked version, you can then verify that it receives
the data you want, and make it return whatever data you desire,
make it throw errors when you want, and more... Create these for
everything your latest real module touches, and you're suddenly
in a position of power: You can control and verify every detail
of your latest creation.

To make that easier, CMock also gives you a bunch of functions
like the ones below, so you can tell that generated DoesSomething
function how to behave for each test:

    void DoesSomething_ExpectAndReturn(int a, int b, int toReturn);
    void DoesSomething_ExpectAndThrow(int a, int b, EXCEPTION_T error);
    void DoesSomething_StubWithCallback(CMOCK_DoesSomething_CALLBACK YourCallback);
    void DoesSomething_IgnoreAndReturn(int toReturn);


You can pile a bunch of these back to back, and it remembers what
you wanted to pass when, like so:

    test_CallsDoesSomething_ShouldDoJustThat(void)
    {
        DoesSomething_ExpectAndReturn(1,2,3);
        DoesSomething_ExpectAndReturn(4,5,6);
        DoesSomething_ExpectAndThrow(7,8, STATUS_ERROR_OOPS);

        CallsDoesSomething( );
    }


This test will call CallsDoesSomething, which is the function
we are testing. We are expecting that function to call DoesSomething
three times. The first time, we check to make sure it's called
as DoesSomething(1, 2) and we'll magically return a 3. The second
time we check for DoesSomething(4, 5) and we'll return a 6. The
third time we verify DoesSomething(7, 8) and we'll throw an error
instead of returning anything. If CallsDoesSomething gets
any of this wrong, it fails the test. It will fail if you didn't
call DoesSomething enough, or too much, or with the wrong arguments,
or in the wrong order.

CMock is based on Unity, which it uses for all internal testing.
It uses Ruby to do all the main work (versions 2.0.0 and above).


Installing
==========

The first thing you need to do to install CMock is to get yourself
a copy of Ruby. If you're on linux or osx, you probably already
have it. You can prove it by typing the following:

    ruby --version


If it replied in a way that implies ignorance, then you're going to
need to install it. You can go to [ruby-lang](https://ruby-lang.org)
to get the latest version. You're also going to need to do that if it
replied with a version that is older than 2.0.0. Go ahead. We'll wait.

Once you have Ruby, you have three options:

* Clone the latest [CMock repo on github](https://github.com/ThrowTheSwitch/CMock/)
* Download the latest [CMock zip from github](https://github.com/ThrowTheSwitch/CMock/)
* Install Ceedling (which has it built in!) through your commandline using `gem install ceedling`.


Generated Mock Module Summary
=============================

In addition to the mocks themselves, CMock will generate the
following functions for use in your tests. The expect functions
are always generated. The other functions are only generated
if those plugins are enabled:


Expect:
-------

Your basic staple Expects which will be used for most of your day
to day CMock work. By calling this, you are telling CMock that you
expect that function to be called during your test. It also specifies
which arguments you expect it to be called with, and what return
value you want returned when that happens. You can call this function
multiple times back to back in order to queue up multiple calls.

* `void func(void)` => `void func_Expect(void)`
* `void func(params)` => `void func_Expect(expected_params)`
* `retval func(void)` => `void func_ExpectAndReturn(retval_to_return)`
* `retval func(params)` => `void func_ExpectAndReturn(expected_params, retval_to_return)`


ExpectAnyArgs:
--------------

This behaves just like the Expects calls, except that it doesn't really
care what the arguments are that the mock gets called with. It still counts
the number of times the mock is called and it still handles return values
if there are some.

* `void func(void)` => `void func_ExpectAnyArgs(void)`
* `void func(params)` => `void func_ExpectAnyArgs(void)`
* `retval func(void)` => `void func_ExpectAnyArgsAndReturn(retval_to_return)`
* `retval func(params)` => `void func_ExpectAnyArgsAndReturn(retval_to_return)`


Array:
------

An ExpectWithArray is another variant of Expect. Like expect, it cares about
the number of times a mock is called, the arguments it is called with, and the
values it is to return. This variant has another feature, though. For anything
that resembles a pointer or array, it breaks the argument into TWO arguments.
The first is the original pointer. The second specify the number of elements
it is to verify of that array. If you specify 1, it'll check one object. If 2,
it'll assume your pointer is pointing at the first of two elements in an array.
If you specify zero elements, it will check just the pointer if
`:smart` mode is configured or fail if `:compare_data` is set.

* `void func(void)` => (nothing. In fact, an additional function is only generated if the params list contains pointers)
* `void func(ptr * param, other)` => `void func_ExpectWithArray(ptr* param, int param_depth, other)`
* `retval func(void)` => (nothing. In fact, an additional function is only generated if the params list contains pointers)
* `retval func(other, ptr* param)` => `void func_ExpectWithArrayAndReturn(other, ptr* param, int param_depth, retval_to_return)`


Ignore:
-------

Maybe you don't care about the number of times a particular function is called or
the actual arguments it is called with. In that case, you want to use Ignore. Ignore
only needs to be called once per test. It will then ignore any further calls to that
particular mock. The IgnoreAndReturn works similarly, except that it has the added
benefit of knowing what to return when that call happens. If the mock is called more
times than IgnoreAndReturn was called, it will keep returning the last value without
complaint. If it's called less times, it will also ignore that. You SAID you didn't
care how many times it was called, right?

* `void func(void)` => `void func_Ignore(void)`
* `void func(params)` => `void func_Ignore(void)`
* `retval func(void)` => `void func_IgnoreAndReturn(retval_to_return)`
* `retval func(params)` => `void func_IgnoreAndReturn(retval_to_return)`


Ignore Arg:
------------

Maybe you overall want to use Expect and its similar variations, but you don't care
what is passed to a particular argument. This is particularly useful when that argument
is a pointer to a value that is supposed to be filled in by the function. You don't want
to use ExpectAnyArgs, because you still care about the other arguments. Instead, before
any of your Expect calls are made, you can call this function. It tells CMock to ignore
a particular argument for the rest of this test, for this mock function.

* `void func(params)` => `void func_IgnoreArg_paramName(void)`


ReturnThruPtr:
--------------

Another option which operates on a particular argument of a function is the ReturnThruPtr
plugin. For every argument that resembles a pointer or reference, CMock generates an
instance of this function. Just as the AndReturn functions support injecting one or more
return values into a queue, this function lets you specify one or more return values which
are queued up and copied into the space being pointed at each time the mock is called.

* `void func(param1)` => `void func_ReturnThruPtr_paramName(val_to_return)`
* => `void func_ReturnArrayThruPtr_paramName(cal_to_return, len)`
* => `void func_ReturnMemThruPtr_paramName(val_to_return, size)`


Callback:
---------

If all those other options don't work, and you really need to do something custom, you
still have a choice. As soon as you stub a callback in a test, it will call the callback
whenever the mock is encountered and return the retval returned from the callback (if any)
instead of performing the usual expect checks. It can be configured to check the arguments
first (like expects) or just jump directly to the callback.

* `void func(void)` => `void func_StubWithCallback(CMOCK_func_CALLBACK callback)`
where `CMOCK_func_CALLBACK` looks like: `void func(int NumCalls)`
* `void func(params)` => `void func_StubWithCallback(CMOCK_func_CALLBACK callback)`
where `CMOCK_func_CALLBACK` looks like: `void func(params, int NumCalls)`
* `retval func(void)` => `void func_StubWithCallback(CMOCK_func_CALLBACK callback)`
where `CMOCK_func_CALLBACK` looks like: `retval func(int NumCalls)`
* `retval func(params)` => `void func_StubWithCallback(CMOCK_func_CALLBACK callback)`
where `CMOCK_func_CALLBACK` looks like: `retval func(params, int NumCalls)`


Cexception:
-----------

Finally, if you are using Cexception for error handling, you can use this to throw errors
from inside mocks. Like Expects, it remembers which call was supposed to throw the error,
and it still checks parameters first.

* `void func(void)` => `void func_ExpectAndThrow(value_to_throw)`
* `void func(params)` => `void func_ExpectAndThrow(expected_params, value_to_throw)`
* `retval func(void)` => `void func_ExpectAndThrow(value_to_throw)`
* `retval func(params)` => `void func_ExpectAndThrow(expected_params, value_to_throw)`



Running CMock
=============

CMock is a Ruby script and class. You can therefore use it directly
from the command line, or include it in your own scripts or rakefiles.


Mocking from the Command Line
-----------------------------

After unpacking CMock, you will find cmock.rb in the 'lib' directory.
This is the file that you want to run. It takes a list of header files
to be mocked, as well as an optional yaml file for a more detailed
configuration (see config options below).

For example, this will create three mocks using the configuration
specified in MyConfig.yml:

    ruby cmock.rb -oMyConfig.yml super.h duper.h awesome.h

And this will create two mocks using the default configuration:

    ruby cmock.rb ../mocking/stuff/is/fun.h ../try/it/yourself.h


Mocking From Scripts or Rake
----------------------------

CMock can be used directly from your own scripts or from a rakefile.
Start by including cmock.rb, then create an instance of CMock.
When you create your instance, you may initialize it in one of
three ways.

You may specify nothing, allowing it to run with default settings:

    require 'cmock.rb'
    cmock = CMock.new

You may specify a YAML file containing the configuration options
you desire:

    cmock = CMock.new('../MyConfig.yml')

You may specify the options explicitly:

    cmock = Cmock.new(:plugins => [:cexception, :ignore], :mock_path => 'my/mocks/')


Config Options:
---------------

The following configuration options can be specified in the
yaml file or directly when instantiating.

Passed as Ruby, they look like this:

        { :attributes => [“__funky”, “__intrinsic”], :when_ptr => :compare }

Defined in the yaml file, they look more like this:

        :cmock:
          :attributes:
            - __funky
            - __intrinsic
          :when_ptr: :compare

In all cases, you can just include the things that you want to override
from the defaults. We've tried to specify what the defaults are below.

* `:attributes`:
  These are attributes that CMock should ignore for you for testing
  purposes. Custom compiler extensions and externs are handy things to
  put here. If your compiler is choking on some extended syntax, this
  is often a good place to look.

  * defaults: ['__ramfunc', '__irq', '__fiq', 'register', 'extern']
  * **note:** this option will reinsert these attributes onto the mock's calls.
    If that isn't what you are looking for, check out :strippables.

* `:c_calling_conventions`:
  Similarly, CMock may need to understand which C calling conventions
  might show up in your codebase. If it encounters something it doesn't
  recognize, it's not going to mock it. We have the most common covered,
  but there are many compilers out there, and therefore many other options.

  * defaults: ['__stdcall', '__cdecl', '__fastcall']
  * **note:** this option will reinsert these attributes onto the mock's calls.
    If that isn't what you are looking for, check out :strippables.

* `:callback_after_arg_check`:
  Tell `:callback` plugin to do the normal argument checking **before** it
  calls the callback function by setting this to true. When false, the
  callback function is called **instead** of the argument verification.

  * default: false

* `:callback_include_count`:
  Tell `:callback` plugin to include an extra parameter to specify the
  number of times the callback has been called. If set to false, the
  callback has the same interface as the mocked function. This can be
  handy when you're wanting to use callback as a stub.

  * default: true

* `:cexception_include`:
  Tell `:cexception` plugin where to find CException.h... You only need to
  define this if it's not in your build path already... which it usually
  will be for the purpose of your builds.

  * default: *nil*

* `:enforce_strict_ordering`:
  CMock always enforces the order that you call a particular function,
  so if you expect GrabNabber(int size) to be called three times, it
  will verify that the sizes are in the order you specified. You might
  *also* want to make sure that all different functions are called in a
  particular order. If so, set this to true.

  * default: false

* `:framework`:
  Currently the only option is `:unity.` Eventually if we support other
  unity test frameworks (or if you write one for us), they'll get added
  here.

  : default: :unity

* `:includes`:
  An array of additional include files which should be added to the
  mocks. Useful for global types and definitions used in your project.
  There are more specific versions if you care WHERE in the mock files
  the includes get placed. You can define any or all of these options.

  * `:includes`
  * `:includes_h_pre_orig_header`
  * `:includes_h_post_orig_header`
  * `:includes_c_pre_header`
  * `:includes_c_post_header`
  * default: nil #for all 5 options

* `:memcmp_if_unknown`:
  C developers create a lot of types, either through typedef or preprocessor
  macros. CMock isn't going to automatically know what you were thinking all
  the time (though it tries its best). If it comes across a type it doesn't
  recognize, you have a choice on how you want it to handle it. It can either
  perform a raw memory comparison and report any differences, or it can fail
  with a meaningful message. Either way, this feature will only happen after
  all other mechanisms have failed (The thing encountered isn't a standard
  type. It isn't in the :treat_as list. It isn't in a custom unity_helper).

  * default: true

* `:mock_path`:
  The directory where you would like the mock files generated to be
  placed.

  * default: mocks

* `:mock_prefix`:
  The prefix to prepend to your mock files. For example, if it's “Mock”, a file
  “USART.h” will get a mock called “MockUSART.c”. This CAN be used with a suffix
  at the same time.

  * default: Mock

* `:mock_suffix`:
  The suffix to append to your mock files. For example, it it's "_Mock", a file
  "USART.h" will get a mock called "USART_Mock.h". This CAN be used with a prefix
  at the same time.

  * default: ""

* `:plugins`:
  An array of which plugins to enable. ':expect' is always active. Also
  available currently:

  * `:ignore`
  * `:ignore_arg`
  * `:expect_any_args`
  * `:array`
  * `:cexception`
  * `:callback`
  * `:return_thru_ptr`

* `:strippables`:
  An array containing a list of items to remove from the header
  before deciding what should be mocked. This can be something simple
  like a compiler extension CMock wouldn't recognize, or could be a
  regex to reject certain function name patterns. This is a great way to
  get rid of compiler extensions when your test compiler doesn't support
  them. For example, use `:strippables: ['(?:functionName\s*\(+.*?\)+)']`
  to prevent a function `functionName` from being mocked. By default, it
  is ignoring all gcc attribute extensions.

  * default: ['(?:__attribute__\s*\(+.*?\)+)']

* `:subdir`:
  This is a relative subdirectory for your mocks.  Set this to e.g. "sys" in
  order to create a mock for `sys/types.h` in `(:mock_path)/sys/`.

  * default: ""

* `:treat_as`:
  The `:treat_as` list is a shortcut for when you have created typedefs
  of standard types. Why create a custom unity helper for UINT16 when
  the unity function TEST_ASSERT_EQUAL_HEX16 will work just perfectly?
  Just add 'UINT16' => 'HEX16' to your list (actually, don't. We already
  did that one for you). Maybe you have a type that is a pointer to an
  array of unsigned characters? No problem, just add 'UINT8_T*' =>
  'HEX8*'

  * NOTE: unlike the other options, your specifications MERGE with the
    default list. Therefore, if you want to override something, you must
    reassign it to something else (or to *nil* if you don't want it)

  * default:
    * 'int': 'INT'
    * 'char': 'INT8'
    * 'short': 'INT16'
    * 'long': 'INT'
    * 'int8': 'INT8'
    * 'int16': 'INT16'
    * 'int32': 'INT'
    * 'int8_t': 'INT8'
    * 'int16_t': 'INT16'
    * 'int32_t': 'INT'
    * 'INT8_T': 'INT8'
    * 'INT16_T': 'INT16'
    * 'INT32_T': 'INT'
    * 'bool': 'INT'
    * 'bool_t': 'INT'
    * 'BOOL': 'INT'
    * 'BOOL_T': 'INT'
    * 'unsigned int': 'HEX32'
    * 'unsigned long': 'HEX32'
    * 'uint32': 'HEX32'
    * 'uint32_t': 'HEX32'
    * 'UINT32': 'HEX32'
    * 'UINT32_T': 'HEX32'
    * 'void*': 'HEX8_ARRAY'
    * 'unsigned short': 'HEX16'
    * 'uint16': 'HEX16'
    * 'uint16_t': 'HEX16'
    * 'UINT16': 'HEX16'
    * 'UINT16_T': 'HEX16'
    * 'unsigned char': 'HEX8'
    * 'uint8': 'HEX8'
    * 'uint8_t': 'HEX8'
    * 'UINT8': 'HEX8'
    * 'UINT8_T': 'HEX8'
    * 'char*': 'STRING'
    * 'pCHAR': 'STRING'
    * 'cstring': 'STRING'
    * 'CSTRING': 'STRING'
    * 'float': 'FLOAT'
    * 'double': 'FLOAT'

* `:treat_as_void`:
  We've seen "fun" legacy systems typedef 'void' with a custom type,
  like MY_VOID. Add any instances of those to this list to help CMock
  understand how to deal with your code.

  * default: []

* `:treat_externs`:
  This specifies how you want CMock to handle functions that have been
  marked as extern in the header file. Should it mock them?

  * `:include` will mock externed functions
  * `:exclude` will ignore externed functions (default).

* `:unity_helper_path`:
  If you have created a header with your own extensions to unity to
  handle your own types, you can set this argument to that path. CMock
  will then automagically pull in your helpers and use them. The only
  trick is that you make sure you follow the naming convention:
  `UNITY_TEST_ASSERT_EQUAL_YourType`. If it finds macros of the right
  shape that match that pattern, it'll use them.

  * default: []

* `:verbosity`:
  How loud should CMock be?

  * 0 for errors only
  * 1 for errors and warnings
  * 2 for normal (default)
  * 3 for verbose

* `:weak`:
  When set this to some value, the generated mocks are defined as weak
  symbols using the configured format. This allows them to be overridden
  in particular tests.

  * Set to '__attribute ((weak))' for weak mocks when using GCC.
  * Set to any non-empty string for weak mocks when using IAR.
  * default: ""

* `:when_no_prototypes`:
  When you give CMock a header file and ask it to create a mock out of
  it, it usually contains function prototypes (otherwise what was the
  point?). You can control what happens when this isn't true. You can
  set this to `:warn,` `:ignore,` or `:error`

  * default: :warn

* `:when_ptr`:
  You can customize how CMock deals with pointers (c strings result in
  string comparisons... we're talking about **other** pointers here). Your
  options are `:compare_ptr` to just verify the pointers are the same,
  `:compare_data` or `:smart` to verify that the data is the same.
  `:compare_data` and `:smart` behaviors will change slightly based on
  if you have the array plugin enabled. By default, they compare a
  single element of what is being pointed to. So if you have a pointer
  to a struct called ORGAN_T, it will compare one ORGAN_T (whatever that
  is).

  * default: :smart

* `:fail_on_unexpected_calls`:
  By default, CMock will fail a test if a mock is called without _Expect and _Ignore
  called first. While this forces test writers to be more explicit in their expectations,
  it can clutter tests with _Expect or _Ignore calls for functions which are not the focus
  of the test. While this is a good indicator that this module should be refactored, some
  users are not fans of the additional noise.

  Therefore, :fail_on_unexpected_calls can be set to false to force all mocks to start with
  the assumption that they are operating as _Ignore unless otherwise specified.

  * default: true
  * **note:**
    If this option is disabled, the mocked functions will return
    a default value (0) when called (and only if they have to return something of course).


Compiled Options:
-----------------

A number of #defines also exist for customizing the cmock experience.
Feel free to pass these into your compiler or whatever is most
convenient. CMock will otherwise do its best to guess what you want
based on other settings, particularly Unity's settings.

* `CMOCK_MEM_STATIC` or `CMOCK_MEM_DYNAMIC`
  Define one of these to determine if you want to dynamically add
  memory during tests as required from the heap. If static, you
  can control the total footprint of Cmock. If dynamic, you will
  need to make sure you make some heap space available for Cmock.

* `CMOCK_MEM_SIZE`
  In static mode this is the total amount of memory you are allocating
  to Cmock. In Dynamic mode this is the size of each chunk allocated
  at once (larger numbers grab more memory but require less mallocs).

* `CMOCK_MEM_ALIGN`
  The way to align your data to. Not everything is as flexible as
  a PC, as most embedded designers know. This defaults to 2, meaning
  align to the closest 2^2 -> 4 bytes (32 bits). You can turn off alignment
  by setting 0, force alignment to the closest uint16 with 1 or even
  to the closest uint64 with 3.

* `CMOCK_MEM_PTR_AS_INT`
  This is used internally to hold pointers... it needs to be big
  enough. On most processors a pointer is the same as an unsigned
  long... but maybe that's not true for yours?

* `CMOCK_MEM_INDEX_TYPE`
  This needs to be something big enough to point anywhere in Cmock's
  memory space... usually it's an unsigned int.

Examples
========

You can look in the [examples directory](/examples/) for a couple of examples on how
you might tool CMock into your build process. You may also want to consider
using [Ceedling](https://throwtheswitch.org/Ceedling). Please note that
these examples are meant to show how the build process works. They have
failing tests ON PURPOSE to show what that would look like. Don't be alarmed. ;)

