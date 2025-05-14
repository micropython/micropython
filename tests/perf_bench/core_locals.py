# This tests the performance of an instance class locals dict (importantly, that has all keys as qstrs)

# These are all shorter than 10 characters, so will be interned by the parser.
ZEN = [
    "the",
    "zen",
    "of",
    "python",
    "beautiful",
    "is",
    "better",
    "than",
    "ugly",
    "explicit",
    "is",
    "better",
    "than",
    "implicit",
    "simple",
    "is",
    "better",
    "than",
    "complex",
    "complex",
    "is",
    "better",
    "than",
    "complicate",
    "flat",
    "is",
    "better",
    "than",
    "nested",
    "sparse",
    "is",
    "better",
    "than",
    "dense",
    "readabilit",
    "counts",
    "special",
    "cases",
    "arent",
    "special",
    "enough",
    "to",
    "break",
    "the",
    "rules",
    "although",
    "practicali",
    "beats",
    "purity",
    "errors",
    "should",
    "never",
    "pass",
    "silently",
    "unless",
    "explicitly",
    "silenced",
    "in",
    "the",
    "face",
    "of",
    "ambiguity",
    "refuse",
    "the",
    "temptation",
    "to",
    "guess",
    "there",
    "should",
    "be",
    "one",
    "and",
    "preferably",
    "only",
    "one",
    "obvious",
    "way",
    "to",
    "do",
    "it",
    "although",
    "that",
    "way",
    "may",
    "not",
    "be",
    "obvious",
    "at",
    "first",
    "unless",
    "youre",
    "dutch",
    "now",
    "is",
    "better",
    "than",
    "never",
    "although",
    "never",
    "is",
    "often",
    "better",
    "than",
    "right",
    "now",
    "if",
    "the",
    "implementa",
    "is",
    "hard",
    "to",
    "explain",
    "its",
    "a",
    "bad",
    "idea",
    "if",
    "the",
    "implementa",
    "is",
    "easy",
    "to",
    "explain",
    "it",
    "may",
    "be",
    "a",
    "good",
    "idea",
    "namespaces",
    "are",
    "one",
    "honking",
    "great",
    "idea",
    "",
    "lets",
    "do",
    "more",
    "of",
    "those",
]


class A:
    pass


def test(niter):
    for _ in range(niter):
        a = A()
        for _ in range(niter):
            for word in ZEN:
                setattr(a, word, getattr(a, word, 0) + 1)

    return (
        getattr(a, "python"),
        getattr(a, "is"),
        getattr(a, "than"),
    )


###########################################################################
# Benchmark interface

bm_params = {
    (32, 10): (2,),
    (50, 10): (4,),
    (100, 10): (8,),
    (500, 10): (40,),
    (1000, 10): (80,),
    (5000, 10): (400,),
}


def bm_setup(params):
    (niter,) = params
    state = None

    def run():
        nonlocal state
        state = test(niter)

    def result():
        return niter, state

    return run, result
