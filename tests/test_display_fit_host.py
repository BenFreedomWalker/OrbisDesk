#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later
"""Host checks for the PS4 sink's output/safe-area geometry.

    python3 tests/test_display_fit_host.py

This tests only integer geometry; it does not exercise VideoOut, GPU presentation or console firmware.
"""
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def main():
    compiler = os.environ.get("CC") or shutil.which("cc") or shutil.which("gcc") or shutil.which("clang")
    if not compiler:
        raise SystemExit("SKIP: no host C compiler")
    with tempfile.TemporaryDirectory() as work:
        binary = pathlib.Path(work) / "display_fit_host_check"
        subprocess.run([
            compiler, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
            "-I" + str(ROOT / "platform/ps4"),
            str(ROOT / "tests/display_fit_host_check.c"), "-lm", "-o", str(binary)
        ], check=True)
        return subprocess.run([str(binary)]).returncode


if __name__ == "__main__":
    sys.exit(main())
