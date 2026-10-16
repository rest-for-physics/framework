"""Run the real restRoot CLI; no simulation or external input files needed."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

REST_ROOT = str(Path(sys.argv.pop(1)).resolve())


class ExpressionArguments(unittest.TestCase):
    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory(prefix="restroot-expression-")
        self.addCleanup(self.scratch.cleanup)
        self.directory = Path(self.scratch.name)
        self.env = dict(os.environ, REST_HOME=str(self.directory / "home"))

    def run_root(self, *arguments):
        return subprocess.run(
            [REST_ROOT, "-b", "-q", *arguments],
            cwd=self.directory,
            env=self.env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=60,
        )

    def test_long_expression_is_executed_without_filesystem_probe(self):
        expression = 'std::cout << "REST_LONG_EXPRESSION_OK" << std::endl;'
        expression += " /*" + "x" * 1024 + "*/"
        result = self.run_root("-e", expression)
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("REST_LONG_EXPRESSION_OK", result.stdout)
        self.assertNotIn("not compatible", result.stdout)

    def test_multiple_expressions_are_passed_to_root_in_order(self):
        result = self.run_root(
            "-e",
            "int restExpressionValue = 41;",
            "-e",
            'if (restExpressionValue != 41) gSystem->Exit(19); std::cout << "REST_ORDER_OK" << std::endl;',
        )
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("REST_ORDER_OK", result.stdout)
        self.assertNotIn("not compatible", result.stdout)

    def test_expression_that_looks_like_help_is_not_a_rest_option(self):
        result = self.run_root("-e", "--help")
        self.assertNotIn("restRoot basic options description", result.stdout)

    def test_missing_expression_reports_a_usage_error(self):
        result = self.run_root("-e")
        self.assertEqual(result.returncode, 1, result.stdout)
        self.assertIn("-e requires a C++ expression", result.stdout)

    def test_macro_arguments_still_execute(self):
        (self.directory / "expression_macro.C").write_text(
            'void expression_macro() { std::cout << "REST_MACRO_OK" << std::endl; }\n'
        )
        result = self.run_root("expression_macro.C")
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("REST_MACRO_OK", result.stdout)


if __name__ == "__main__":
    unittest.main()
