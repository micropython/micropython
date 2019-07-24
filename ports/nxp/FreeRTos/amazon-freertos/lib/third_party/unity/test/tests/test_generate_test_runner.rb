# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

require '../auto/generate_test_runner.rb'

$generate_test_runner_tests = 0
$generate_test_runner_failures = 0

OUT_FILE  = 'build/testsample_'

RUNNER_TESTS = [
  { :name => 'DefaultsThroughOptions',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => nil, #defaults
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'DefaultsThroughCommandLine',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :cmdline => "", #defaults
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'DefaultsThroughYAMLFile',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :cmdline => "", #defaults
    :yaml => {}, #defaults
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ShorterFilterOfJustTest',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "test",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ShorterFilterOfJustShould',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "should",
    },
    :expected => {
      :to_pass => [ 'should_RunTestsStartingWithShouldByDefault' ],
      :to_fail => [  ],
      :to_ignore => [  ],
    }
  },

  { :name => 'ShorterFilterOfJustSpec',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "spec",
    },
    :expected => {
      :to_pass => [ 'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [  ],
      :to_ignore => [  ],
    }
  },

  { :name => 'InjectIncludes',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => {
      :includes => ['Defs.h'],
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ParameterizedThroughOptions',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "paratest",
      :use_param_tests => true,
    },
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                    'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ParameterizedThroughCommandLine',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :cmdline => " --test_prefix=\"paratest\" --use_param_tests=1",
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                    'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ParameterizedThroughCommandLineAndYaml',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :cmdline => "--use_param_tests=1",
    :yaml => {
      :test_prefix => "paratest"
    },
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                    'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'CException',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'USE_CEXCEPTION'],
    :options => {
      :test_prefix => "extest",
      :plugins => [ :cexception ],
    },
    :expected => {
      :to_pass => [ 'extest_ShouldHandleCExceptionInTest' ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'CustomSetupAndTeardownThroughOptions',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "custtest|test",
      :setup_name => "custom_setup",
      :teardown_name => "custom_teardown",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'custtest_ThisTestPassesWhenCustomSetupRan',
                    'custtest_ThisTestPassesWhenCustomTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomSetupAndTeardownThroughCommandLine',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :cmdline => " --test_prefix=\"custtest|test\" --setup_name=\"custom_setup\" --teardown_name=\"custom_teardown\"",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'custtest_ThisTestPassesWhenCustomSetupRan',
                    'custtest_ThisTestPassesWhenCustomTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomSetupAndTeardownThroughYaml',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :cmdline => " --test_prefix=\"custtest|test\"",
    :yaml => {
      :setup_name => "custom_setup",
      :teardown_name => "custom_teardown",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'custtest_ThisTestPassesWhenCustomSetupRan',
                    'custtest_ThisTestPassesWhenCustomTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomMain',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', "USE_ANOTHER_MAIN"],
    :options => {
      :main_name => "custom_main",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomSuiteSetupAndTeardown',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :includes => ['Defs.h'],
    :options => {
      :test_prefix    => "suitetest|test",
      :suite_setup    => "  CounterSuiteSetup = 1;",
      :suite_teardown => "  return num_failures;",
    },
    :expected =>  {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'suitetest_ThisTestPassesWhenCustomSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'MainExternDeclaration',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST'],
    :includes => ['Defs.h'],
    :options => {
      :main_export_decl => "EXTERN_DECL",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },


  #### WITH MOCKS ##########################################

  { :name => 'DefaultsThroughOptions',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => nil, #defaults
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'DefaultsThroughCommandLine',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :cmdline => "", #defaults
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'DefaultsThroughYAMLFile',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :cmdline => "", #defaults
    :yaml => {}, #defaults
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ShorterFilterOfJustTest',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "test",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ShorterFilterOfJustShould',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "should",
    },
    :expected => {
      :to_pass => [ 'should_RunTestsStartingWithShouldByDefault' ],
      :to_fail => [  ],
      :to_ignore => [  ],
    }
  },

  { :name => 'ShorterFilterOfJustSpec',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "spec",
    },
    :expected => {
      :to_pass => [ 'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [  ],
      :to_ignore => [  ],
    }
  },

  { :name => 'InjectIncludes',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => {
      :includes => ['Defs.h'],
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ParameterizedThroughOptions',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "paratest",
      :use_param_tests => true,
    },
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                    'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ParameterizedThroughCommandLine',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :cmdline => " --test_prefix=\"paratest\" --use_param_tests=1",
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                    'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ParameterizedThroughCommandLineAndYaml',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :cmdline => "--use_param_tests=1",
    :yaml => {
      :test_prefix => "paratest"
    },
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                    'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'CException',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST', 'USE_CEXCEPTION'],
    :options => {
      :test_prefix => "extest",
      :plugins => [ :cexception ],
    },
    :expected => {
      :to_pass => [ 'extest_ShouldHandleCExceptionInTest' ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'CustomSetupAndTeardownThroughOptions',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :options => {
      :test_prefix => "custtest|test",
      :setup_name => "custom_setup",
      :teardown_name => "custom_teardown",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'custtest_ThisTestPassesWhenCustomSetupRan',
                    'custtest_ThisTestPassesWhenCustomTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomSetupAndTeardownThroughCommandLine',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :cmdline => " --test_prefix=\"custtest|test\" --setup_name=\"custom_setup\" --teardown_name=\"custom_teardown\"",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'custtest_ThisTestPassesWhenCustomSetupRan',
                    'custtest_ThisTestPassesWhenCustomTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomSetupAndTeardownThroughYaml',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :cmdline => " --test_prefix=\"custtest|test\"",
    :yaml => {
      :setup_name => "custom_setup",
      :teardown_name => "custom_teardown",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'custtest_ThisTestPassesWhenCustomSetupRan',
                    'custtest_ThisTestPassesWhenCustomTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomMain',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST', "USE_ANOTHER_MAIN"],
    :options => {
      :main_name => "custom_main",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'CustomSuiteSetupAndTeardown',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :includes => ['Defs.h'],
    :options => {
      :test_prefix    => "suitetest|test",
      :suite_setup    => "  CounterSuiteSetup = 1;",
      :suite_teardown => "  return num_failures;",
    },
    :expected =>  {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'suitetest_ThisTestPassesWhenCustomSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'MainExternDeclaration',
    :testfile => 'testdata/testRunnerGeneratorWithMocks.c',
    :testdefines => ['TEST'],
    :includes => ['Defs.h'],
    :options => {
      :main_export_decl => "EXTERN_DECL",
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'test_ShouldCallMockInitAndVerifyFunctionsForEachTest',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },



  #### WITH ARGS ##########################################

  { :name => 'ArgsThroughOptions',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsThroughCommandLine',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :cmdline => "--cmdline_args=1",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsThroughYAMLFile',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :cmdline => "",
    :yaml => {
      :cmdline_args => true,
    },
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterJustTest',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n test_",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterJustShould',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n should_",
    :expected => {
      :to_pass => [ 'should_RunTestsStartingWithShouldByDefault' ],
      :to_fail => [  ],
      :to_ignore => [  ],
    }
  },

  { :name => 'ArgsNameFilterTestAndShould',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n should_,test_",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterWithWildcardOnFile',
    :testfile => 'testdata/testRunnerGeneratorSmall.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGeneratorSma*",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterWithWildcardAsName',
    :testfile => 'testdata/testRunnerGeneratorSmall.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGeneratorSmall:*",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterWithWildcardOnName',
    :testfile => 'testdata/testRunnerGeneratorSmall.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGeneratorSmall:test_*",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterWithWildcardAndShortName',
    :testfile => 'testdata/testRunnerGeneratorSmall.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGeneratorSmall:te*",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsNameFilterWithWildcardOnBoth',
    :testfile => 'testdata/testRunnerGeneratorSmall.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGeneratorSm*:*",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsExcludeFilterJustTest',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-x test_",
    :expected => {
      :to_pass => [ 'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                    'should_RunTestsStartingWithShouldByDefault',
                  ],
      :to_fail => [  ],
      :to_ignore => [  ],
    }
  },

  { :name => 'ArgsIncludeAndExcludeFilter',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
      :includes => ['Defs.h'],
    },
    :cmdline_args => "-n test_ -x Ignored",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [  ],
    }
  },

  { :name => 'ArgsIncludeSingleTest',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n ThisTestAlwaysPasses",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeSingleTestInSpecificFile',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGenerator:ThisTestAlwaysPasses",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeTestFileWithExtension',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGenerator.c:ThisTestAlwaysPasses",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeDoubleQuotes',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n \"testRunnerGenerator:ThisTestAlwaysPasses,test_ThisTestAlwaysFails\"",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeSingleQuotes',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n 'testRunnerGenerator:ThisTestAlwaysPasses,test_ThisTestAlwaysFails'",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeAValidTestForADifferentFile',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n AnotherFile:ThisTestDoesNotExist",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeNoTests',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n ThisTestDoesNotExist",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsExcludeAllTests',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-x _",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeFullFile',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n testRunnerGenerator",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses',
                    'spec_ThisTestPassesWhenNormalSetupRan',
                    'spec_ThisTestPassesWhenNormalTeardownRan',
                    'test_NotBeConfusedByLongComplicatedStrings',
                    'test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings',
                    'test_StillNotBeConfusedByLongComplicatedStrings',
                    'should_RunTestsStartingWithShouldByDefault',
                    'spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan',
                  ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ 'test_ThisTestAlwaysIgnored' ],
    }
  },

  { :name => 'ArgsIncludeWithAlternateFlag',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-f=\"testRunnerGenerator:ThisTestAlwaysPasses,test_ThisTestAlwaysFails\"",
    :expected => {
      :to_pass => [ 'test_ThisTestAlwaysPasses' ],
      :to_fail => [ 'test_ThisTestAlwaysFails' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsIncludeWithParameterized',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :cmdline => "--use_param_tests=1",
    :yaml => {
      :cmdline_args => true,
      :test_prefix => "paratest"
    },
    :cmdline_args => "-n ShouldHandleParameterizedTests",
    :expected => {
      :to_pass => [ 'paratest_ShouldHandleParameterizedTests\(25\)',
                    'paratest_ShouldHandleParameterizedTests\(125\)',
                    'paratest_ShouldHandleParameterizedTests\(5\)',
                    'paratest_ShouldHandleParameterizedTests2\(7\)',
                  ],
      :to_fail => [ 'paratest_ShouldHandleParameterizedTestsThatFail\(17\)' ],
      :to_ignore => [ ],
    }
  },

  { :name => 'ArgsList',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-l",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
      :text => [  "testRunnerGenerator",
                  "test_ThisTestAlwaysPasses",
                  "test_ThisTestAlwaysFails",
                  "test_ThisTestAlwaysIgnored",
                  "spec_ThisTestPassesWhenNormalSuiteSetupAndTeardownRan",
                  "spec_ThisTestPassesWhenNormalSetupRan",
                  "spec_ThisTestPassesWhenNormalTeardownRan",
                  "test_NotBeConfusedByLongComplicatedStrings",
                  "test_NotDisappearJustBecauseTheTestBeforeAndAfterHaveCrazyStrings",
                  "test_StillNotBeConfusedByLongComplicatedStrings",
                  "should_RunTestsStartingWithShouldByDefault"
               ]
    }
  },

  { :name => 'ArgsListParameterized',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :test_prefix => "paratest",
      :use_param_tests => true,
      :cmdline_args => true,
    },
    :cmdline_args => "-l",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
      :text => [  "testRunnerGenerator",
                  'paratest_ShouldHandleParameterizedTests\(25\)',
                  'paratest_ShouldHandleParameterizedTests\(125\)',
                  'paratest_ShouldHandleParameterizedTests\(5\)',
                  'paratest_ShouldHandleParameterizedTests2\(7\)',
                  'paratest_ShouldHandleNonParameterizedTestsWhenParameterizationValid\(RUN_TEST_NO_ARGS\)',
                  'paratest_ShouldHandleParameterizedTestsThatFail\(17\)'
               ],
    }
  },

  { :name => 'ArgsIncompleteIncludeFlags',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-n",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
      :text => [ "ERROR: No Test String to Include Matches For" ],
    }
  },

  { :name => 'ArgsIncompleteExcludeFlags',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-x",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
      :text => [ "ERROR: No Test String to Exclude Matches For" ],
    }
  },

  { :name => 'ArgsIllegalFlags',
    :testfile => 'testdata/testRunnerGenerator.c',
    :testdefines => ['TEST', 'UNITY_USE_COMMAND_LINE_ARGS'],
    :options => {
      :cmdline_args => true,
    },
    :cmdline_args => "-z",
    :expected => {
      :to_pass => [ ],
      :to_fail => [ ],
      :to_ignore => [ ],
      :text => [ "ERROR: Unknown Option z" ],
    }
  },
]

def runner_test(test, runner, expected, test_defines, cmdline_args)
  # Tack on TEST define for compiling unit tests
  load_configuration($cfg_file)

  #compile objects
  obj_list = [
    compile(runner, test_defines),
    compile(test, test_defines),
    compile('../src/unity.c', test_defines),
  ]

  # Link the test executable
  test_base = File.basename(test, C_EXTENSION)
  link_it(test_base, obj_list)

  # Execute unit test and generate results file
  simulator = build_simulator_fields
  cmdline_args ||= ""
  executable = $cfg['linker']['bin_files']['destination'] + test_base + $cfg['linker']['bin_files']['extension'] + " #{cmdline_args}"
  cmd_str = if simulator.nil?
              executable
            else
              "#{simulator[:command]} #{simulator[:pre_support]} #{executable} #{simulator[:post_support]}"
            end
  output = execute(cmd_str, true)

  #compare to the expected pass/fail
  allgood = expected[:to_pass].inject(true)      {|s,v| s && verify_match(/#{v}:PASS/,   output) }
  allgood = expected[:to_fail].inject(allgood)   {|s,v| s && verify_match(/#{v}:FAIL/,   output) }
  allgood = expected[:to_ignore].inject(allgood) {|s,v| s && verify_match(/#{v}:IGNORE/, output) }

  #verify there weren't more pass/fail/etc than expected
  allgood &&= verify_number( expected[:to_pass],   /(:PASS)/,   output)
  allgood &&= verify_number( expected[:to_fail],   /(:FAIL)/,   output)
  allgood &&= verify_number( expected[:to_ignore], /(:IGNORE)/, output)

  #if we care about any other text, check that too
  if (expected[:text])
    allgood = expected[:text].inject(allgood) {|s,v| s && verify_match(/#{v}/, output) }
    allgood &&= verify_number( expected[:text], /.+/, output )
  end

  report output if (!allgood && !$verbose) #report failures if not already reporting everything
  return allgood
end

def verify_match(expression, output)
  if (expression =~ output)
    return true
  else
    report "  FAIL: No Match For /#{expression.to_s}/"
    return false
  end
end

def verify_number(expected, expression, output)
  exp = expected.length
  act = output.scan(expression).length
  if (exp == act)
    return true
  else
    report "  FAIL: Expected #{exp} Matches For /#{expression.to_s}/. Was #{act}"
    return false
  end
end

RUNNER_TESTS.each do |testset|
  basename = File.basename(testset[:testfile], C_EXTENSION)
  testset_name = "Runner_#{basename}_#{testset[:name]}"
  should testset_name do
    runner_name = OUT_FILE + testset[:name] + '_runner.c'

    #create a yaml file first if required
    yaml_option = ""
    if (testset[:yaml])
      File.open("build/runner_options.yml",'w') {|f| f << { :unity => testset[:yaml] }.to_yaml }
      yaml_option = "build/runner_options.yml"
    end

    #run script via command line or through hash function call, as requested
    if (testset[:cmdline])
      cmdstr = "ruby ../auto/generate_test_runner.rb #{yaml_option} #{testset[:cmdline]} \"#{testset[:testfile]}\" \"#{runner_name}\""
      `#{cmdstr}`
    else
      UnityTestRunnerGenerator.new(testset[:options]).run(testset[:testfile], runner_name)
    end

    #test the script against the specified test file and check results
    if (runner_test(testset[:testfile], runner_name, testset[:expected], testset[:testdefines], testset[:cmdline_args]))
      report "#{testset_name}:PASS"
    else
      report "#{testset_name}:FAIL"
      $generate_test_runner_failures += 1
    end
    $generate_test_runner_tests += 1
  end
end

raise "There were #{$generate_test_runner_failures.to_s} failures while testing generate_test_runner.rb" if ($generate_test_runner_failures > 0)
