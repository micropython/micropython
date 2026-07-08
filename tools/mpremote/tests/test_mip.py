from mpremote.mip import _normalise_target


def test_normalise_target_accepts_remote_prefix():
    assert _normalise_target(":") == "."
    assert _normalise_target(":.") == "."
    assert _normalise_target(":/lib") == "/lib"
    assert _normalise_target("/lib") == "/lib"
