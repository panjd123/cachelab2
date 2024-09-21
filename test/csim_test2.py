# This file is provided for the automation tool.
from csim_test import test_csim as _test_csim


def test_csim2():
    result = _test_csim()
    assert result == (20, 100.0), "You should pass all the cases in csim_test.py"
