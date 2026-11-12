"""Recover signals while a loaded unrelated dictionary has removed TObject."""

import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile

restroot, writer, reader, fixture, dictionary, macro = sys.argv[1:]
with tempfile.TemporaryDirectory(prefix="rest-recovery-track-") as directory:
    directory = Path(directory)
    source, recovered = directory / "legacy.root", directory / "recovered.root"
    subprocess.run([writer, source, "--keep-signal-streamer-info"], check=True)
    subprocess.run([fixture, "append", source], check=True)
    original = hashlib.sha256(source.read_bytes()).digest()
    environment = os.environ.copy()
    # Load schema 3 in the recovery process; the writer/verifier use schema 2.
    # This ROOT-only dictionary is also safe in the plain-ROOT signal extractor.
    environment["LD_PRELOAD"] = ":".join(
        filter(None, (dictionary, environment.get("LD_PRELOAD")))
    )
    environment["REST_LEGACY_RECOVERY_MACRO"] = macro
    try:
        subprocess.run(
            [restroot, "--recover-legacy-signals", source, "--output", recovered],
            env=environment,
            check=True,
        )
    finally:
        assert (
            hashlib.sha256(source.read_bytes()).digest() == original
        ), "original was modified"
    subprocess.run([fixture, "verify", source, recovered], check=True)
    subprocess.run([reader, recovered], check=True)
