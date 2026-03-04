#!/usr/bin/env python3

# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2025 MicroPython Contributors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""
Unit tests for boardgen.py

This test suite validates the core functionality of the boardgen.py tool,
including pin parsing, sorting, and code generation.
"""

import io
import os
import sys
import tempfile
import unittest
from unittest.mock import mock_open, patch

# Add the tools directory to the Python path for import
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import boardgen


class TestPin(boardgen.Pin):
    """Test implementation of Pin class with required methods"""

    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)
        self._af_table = {}

    def add_af(self, af_idx, af_name, af):
        """Store alternate function data"""
        if af_idx not in self._af_table:
            self._af_table[af_idx] = []
        self._af_table[af_idx].append((af_name, af))

    def index(self):
        """Extract numeric index from pin name (e.g., PA5 -> 5)"""
        name = self._cpu_pin_name
        if len(name) >= 2 and name[0] == 'P' and name[2:].isdigit():
            return int(name[2:])
        return None

    def definition(self):
        """Return pin definition for C code generation"""
        return "{{ .base = {{ &machine_pin_type }}, .port = GPIO{}, .pin = {} }}".format(
            self._cpu_pin_name[1], self.index() or 0
        )

    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        """Validate CPU pin names follow Pxy format"""
        if not cpu_pin_name.strip():
            raise boardgen.PinGeneratorError("Missing cpu pin name")
        if not (
            len(cpu_pin_name) >= 2
            and cpu_pin_name.startswith('P')
            and len(cpu_pin_name) >= 3
            and cpu_pin_name[1].isalpha()
        ):
            raise boardgen.PinGeneratorError(f"Invalid cpu pin name: {cpu_pin_name}")


class TestPinClass(unittest.TestCase):
    """Test the Pin class functionality"""

    def test_pin_creation(self):
        """Test basic pin creation"""
        pin = TestPin("PA5")
        self.assertEqual(pin._cpu_pin_name, "PA5")
        self.assertEqual(pin.name(), "PA5")
        self.assertFalse(pin._available)
        self.assertFalse(pin._hidden)
        self.assertEqual(len(pin._board_pin_names), 0)

    def test_pin_validation(self):
        """Test pin name validation"""
        # Valid pin names
        TestPin.validate_cpu_pin_name("PA0")
        TestPin.validate_cpu_pin_name("PB15")
        TestPin.validate_cpu_pin_name("PC13")

        # Invalid pin names
        with self.assertRaises(boardgen.PinGeneratorError):
            TestPin.validate_cpu_pin_name("")
        with self.assertRaises(boardgen.PinGeneratorError):
            TestPin.validate_cpu_pin_name("A5")
        with self.assertRaises(boardgen.PinGeneratorError):
            TestPin.validate_cpu_pin_name("P5")

    def test_board_pin_names(self):
        """Test adding board pin names"""
        pin = TestPin("PA5")
        pin.add_board_pin_name("X1", hidden=False)
        pin.add_board_pin_name("LED1", hidden=True)

        self.assertEqual(len(pin._board_pin_names), 2)
        self.assertIn(("X1", False), pin._board_pin_names)
        self.assertIn(("LED1", True), pin._board_pin_names)

        # Test that board_pin_names() returns sorted list
        sorted_names = pin.board_pin_names()
        self.assertEqual(sorted_names, [("LED1", True), ("X1", False)])

    def test_alternate_functions(self):
        """Test adding alternate functions"""
        pin = TestPin("PA5")
        pin.add_af(0, "SYS", "")
        pin.add_af(1, "TIM2", "TIM2_CH1")
        pin.add_af(1, "TIM2", "TIM2_ETR")

        self.assertEqual(len(pin._af_table), 2)
        self.assertEqual(pin._af_table[0], [("SYS", "")])
        self.assertEqual(pin._af_table[1], [("TIM2", "TIM2_CH1"), ("TIM2", "TIM2_ETR")])

    def test_pin_index(self):
        """Test pin index extraction"""
        self.assertEqual(TestPin("PA0").index(), 0)
        self.assertEqual(TestPin("PA5").index(), 5)
        self.assertEqual(TestPin("PB15").index(), 15)
        self.assertIsNone(TestPin("INVALID").index())

    def test_pin_definition(self):
        """Test pin definition generation"""
        pin = TestPin("PA5")
        definition = pin.definition()
        self.assertIn("GPIOA", definition)
        self.assertIn("5", definition)

    def test_board_pin_names_sorting(self):
        """Test that board_pin_names() returns consistently sorted names"""
        pin = TestPin("PA5")

        # Add board names in non-alphabetical order
        pin.add_board_pin_name("Z_LAST", hidden=False)
        pin.add_board_pin_name("A_FIRST", hidden=True)
        pin.add_board_pin_name("M_MIDDLE", hidden=False)

        # Get sorted board pin names
        sorted_names = pin.board_pin_names()
        expected = [("A_FIRST", True), ("M_MIDDLE", False), ("Z_LAST", False)]
        self.assertEqual(sorted_names, expected)

        # Verify it's always sorted regardless of addition order
        pin2 = TestPin("PB0")
        pin2.add_board_pin_name("B_PIN", hidden=False)
        pin2.add_board_pin_name("A_PIN", hidden=True)
        pin2.add_board_pin_name("C_PIN", hidden=False)

        sorted_names2 = pin2.board_pin_names()
        expected2 = [("A_PIN", True), ("B_PIN", False), ("C_PIN", False)]
        self.assertEqual(sorted_names2, expected2)


class TestPinGenerator(unittest.TestCase):
    """Test the PinGenerator class functionality"""

    def setUp(self):
        """Set up test fixtures"""
        self.generator = boardgen.PinGenerator(TestPin)

    def test_generator_creation(self):
        """Test generator creation"""
        self.assertEqual(len(self.generator._pins), 0)
        self.assertEqual(self.generator._pin_type, TestPin)
        self.assertFalse(self.generator._enable_af)

    def test_add_cpu_pin(self):
        """Test adding CPU pins"""
        pin = self.generator.add_cpu_pin("PA5")
        self.assertEqual(len(self.generator._pins), 1)
        self.assertEqual(pin._cpu_pin_name, "PA5")
        self.assertTrue(pin._available)
        self.assertEqual(pin._generator, self.generator)

    def test_find_pin_by_cpu_pin_name(self):
        """Test finding pins by CPU name"""
        pin1 = self.generator.add_cpu_pin("PA5")

        # Find existing pin
        found = self.generator.find_pin_by_cpu_pin_name("PA5", create=False)
        self.assertEqual(found, pin1)

        # Create new pin if not found
        pin2 = self.generator.find_pin_by_cpu_pin_name("PB0", create=True)
        self.assertEqual(pin2._cpu_pin_name, "PB0")
        self.assertFalse(pin2._available)  # Created with available=False

        # Error if not found and create=False
        with self.assertRaises(boardgen.PinGeneratorError):
            self.generator.find_pin_by_cpu_pin_name("PC0", create=False)

    def test_available_pins_insertion_order(self):
        """Test that available_pins returns pins in insertion order"""
        # Add pins in specific order
        self.generator.add_cpu_pin("PC0")
        self.generator.add_cpu_pin("PA5")
        self.generator.add_cpu_pin("PB10")
        self.generator.add_cpu_pin("PA10")
        self.generator.add_cpu_pin("PB2")

        # Get available pins
        pins = list(self.generator.available_pins())
        pin_names = [pin.name() for pin in pins]

        # Should be in insertion order
        expected_order = ["PC0", "PA5", "PB10", "PA10", "PB2"]
        self.assertEqual(pin_names, expected_order)

    def test_available_pins_exclude_hidden(self):
        """Test filtering hidden pins"""
        pin1 = self.generator.add_cpu_pin("PA5")
        pin2 = self.generator.add_cpu_pin("PB0")
        pin2._hidden = True

        # Include hidden pins
        all_pins = list(self.generator.available_pins(exclude_hidden=False))
        self.assertEqual(len(all_pins), 2)

        # Exclude hidden pins
        visible_pins = list(self.generator.available_pins(exclude_hidden=True))
        self.assertEqual(len(visible_pins), 1)
        self.assertEqual(visible_pins[0], pin1)

    def test_parse_board_csv_basic(self):
        """Test parsing basic board CSV data"""
        csv_data = """# Comment line
X1,PA0
X2,PA1
,PB0
-X3,PA2
X4,-PB1
"""
        with patch('builtins.open', mock_open(read_data=csv_data)):
            self.generator.parse_board_csv("test.csv")

        # Check pins were created (PA0, PA1, PB0, PA2, PB1)
        self.assertEqual(len(self.generator._pins), 5)

        # Check PA0 (board name X1)
        pa0 = self.generator.find_pin_by_cpu_pin_name("PA0", create=False)
        self.assertTrue(pa0._available)
        self.assertFalse(pa0._hidden)
        self.assertIn(("X1", False), pa0._board_pin_names)

        # Check PB0 (no board name)
        pb0 = self.generator.find_pin_by_cpu_pin_name("PB0", create=False)
        self.assertTrue(pb0._available)
        self.assertEqual(len(pb0._board_pin_names), 0)

        # Check PA2 (hidden board name)
        pa2 = self.generator.find_pin_by_cpu_pin_name("PA2", create=False)
        self.assertTrue(pa2._available)
        self.assertIn(("X3", True), pa2._board_pin_names)

        # Check PB1 (hidden CPU)
        pb1 = self.generator.find_pin_by_cpu_pin_name("PB1", create=False)
        self.assertTrue(pb1._available)
        self.assertTrue(pb1._hidden)
        self.assertIn(("X4", False), pb1._board_pin_names)

    def test_parse_board_csv_errors(self):
        """Test error handling in board CSV parsing"""
        # Test invalid row length
        with patch('builtins.open', mock_open(read_data="X1,PA0,EXTRA")):
            with self.assertRaises(boardgen.PinGeneratorError) as cm:
                self.generator.parse_board_csv("test.csv")
            self.assertIn("Expecting two entries", str(cm.exception))

        # Test invalid CPU pin name
        with patch('builtins.open', mock_open(read_data="X1,INVALID")):
            with self.assertRaises(boardgen.PinGeneratorError) as cm:
                self.generator.parse_board_csv("test.csv")
            self.assertIn("Invalid cpu pin name", str(cm.exception))

    def test_parse_af_csv_basic(self):
        """Test parsing alternate function CSV data"""
        csv_data = """Port,Pin,AF0,AF1,AF2
PortA,PA0,,TIM2_CH1,TIM5_CH1
PortA,PA1,,TIM2_CH2,TIM5_CH2
PortB,PB0,,TIM1_CH2N,TIM3_CH3
"""
        with patch('builtins.open', mock_open(read_data=csv_data)):
            self.generator.parse_af_csv("test_af.csv", header_rows=1, pin_col=1, af_col=2)

        # Check that pins were created
        pa0 = self.generator.find_pin_by_cpu_pin_name("PA0", create=False)

        # Check AF data was added (AF indices start from AF0 column, but empty AF0 means index 1 is first)
        self.assertIn(1, pa0._af_table)
        self.assertIn(2, pa0._af_table)
        self.assertEqual(pa0._af_table[1], [("AF1", "TIM2_CH1")])
        self.assertEqual(pa0._af_table[2], [("AF2", "TIM5_CH1")])

    def test_code_generation_output(self):
        """Test C code generation"""
        # Set up some pins
        pin1 = self.generator.add_cpu_pin("PA0")
        pin1.add_board_pin_name("X1", hidden=False)
        pin2 = self.generator.add_cpu_pin("PA1")
        pin2.add_board_pin_name("X2", hidden=True)  # Hidden board pin
        pin3 = self.generator.add_cpu_pin("PB0")
        pin3._hidden = True  # Hidden CPU pin

        # Test board locals dict generation
        output = io.StringIO()
        self.generator.print_board_locals_dict(output)
        board_output = output.getvalue()

        # Should include X1 but not X2 (hidden)
        self.assertIn("MP_QSTR_X1", board_output)
        self.assertNotIn("MP_QSTR_X2", board_output)
        self.assertIn("pin_PA0", board_output)

        # Test CPU locals dict generation
        output = io.StringIO()
        self.generator.print_cpu_locals_dict(output)
        cpu_output = output.getvalue()

        # Should include PA0 and PA1 but not PB0 (hidden)
        self.assertIn("MP_QSTR_PA0", cpu_output)
        self.assertIn("MP_QSTR_PA1", cpu_output)
        self.assertNotIn("MP_QSTR_PB0", cpu_output)


class TestComplexScenarios(unittest.TestCase):
    """Test complex scenarios and edge cases"""

    def setUp(self):
        """Set up test fixtures"""
        self.generator = boardgen.PinGenerator(TestPin)

    def test_real_world_pin_data(self):
        """Test with realistic pin data similar to PYBD_SF6"""
        pins_data = [
            ("W3", "PE3"),
            ("W5", "PA11"),
            ("W6", "PA5"),
            ("W7", "PA4"),
            ("", "PB0"),  # No board name
            ("-W8", "PB11"),  # Hidden board name
            ("W9", "-PA12"),  # Hidden CPU
        ]

        csv_content = "\n".join(f"{board},{cpu}" for board, cpu in pins_data)

        with patch('builtins.open', mock_open(read_data=csv_content)):
            self.generator.parse_board_csv("test.csv")

        # Verify all pins are present in CSV parsing order
        available = list(self.generator.available_pins())
        names = [pin.name() for pin in available]

        # Should be in the order they appear in CSV (which is the order they were processed)
        expected_order = [cpu.lstrip('-') for board, cpu in pins_data]
        self.assertEqual(names, expected_order)

    def test_pin_insertion_order_with_numbers(self):
        """Test that pins are returned in insertion order"""
        pin_names = ["PA2", "PA10", "PA1", "PA11", "PB0", "PA0"]

        for name in pin_names:
            self.generator.add_cpu_pin(name)

        available = list(self.generator.available_pins())
        sorted_names = [pin.name() for pin in available]

        # Should be in insertion order
        expected = ["PA2", "PA10", "PA1", "PA11", "PB0", "PA0"]
        self.assertEqual(sorted_names, expected)

    def test_empty_csv_handling(self):
        """Test handling of empty or comment-only CSV files"""
        csv_data = """# This is a comment
# Another comment

# Empty line above, this line is also a comment
"""
        with patch('builtins.open', mock_open(read_data=csv_data)):
            self.generator.parse_board_csv("test.csv")

        # Should have no pins
        self.assertEqual(len(list(self.generator.available_pins())), 0)


def create_temp_csv_files():
    """Create temporary CSV files for integration testing"""
    pins_csv = tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False)
    pins_csv.write("""X1,PA0
X2,PA1
X3,PB0
""")
    pins_csv.close()

    af_csv = tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False)
    af_csv.write("""Port,Pin,AF0,AF1,AF2
PortA,PA0,,TIM2_CH1,TIM5_CH1
PortA,PA1,,TIM2_CH2,TIM5_CH2
PortB,PB0,,TIM1_CH2N,TIM3_CH3
""")
    af_csv.close()

    return pins_csv.name, af_csv.name


class TestIntegration(unittest.TestCase):
    """Integration tests using temporary files"""

    def setUp(self):
        """Set up temporary files"""
        self.pins_file, self.af_file = create_temp_csv_files()
        self.generator = boardgen.PinGenerator(TestPin, enable_af=True)

    def tearDown(self):
        """Clean up temporary files"""
        os.unlink(self.pins_file)
        os.unlink(self.af_file)

    def test_full_workflow(self):
        """Test the complete pin generation workflow"""
        # Parse board pins
        self.generator.parse_board_csv(self.pins_file)

        # Parse alternate functions
        self.generator.parse_af_csv(self.af_file, header_rows=1, pin_col=1, af_col=2)

        # Verify pins were created and sorted
        available = list(self.generator.available_pins())
        self.assertEqual(len(available), 3)

        names = [pin.name() for pin in available]
        self.assertEqual(names, ["PA0", "PA1", "PB0"])  # Alphabetically sorted

        # Verify AF data was loaded
        pa0 = self.generator.find_pin_by_cpu_pin_name("PA0", create=False)
        self.assertIn(1, pa0._af_table)
        self.assertEqual(pa0._af_table[1], [("AF1", "TIM2_CH1")])

        # Test code generation
        output = io.StringIO()
        self.generator.print_board_locals_dict(output)
        self.generator.print_cpu_locals_dict(output)

        generated_code = output.getvalue()
        self.assertIn("MP_QSTR_X1", generated_code)
        self.assertIn("MP_QSTR_PA0", generated_code)


if __name__ == '__main__':
    # Run the test suite
    unittest.main(verbosity=2)
