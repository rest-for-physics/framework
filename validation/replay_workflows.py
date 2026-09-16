#!/usr/bin/env python3
"""Replay repository validation jobs locally, retaining scripts, logs and results.

Checkout/cache/upload actions are infrastructure, replaced by isolated source
snapshots and the local install. Each actual run step is retained. Framework
build/CTest are run separately; library-only build actions are translated to
isolated CMake builds with their workflow flags. Run inside the CI container.
"""
import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

import yaml

parser = argparse.ArgumentParser()
parser.add_argument(
    "repository",
    choices=[
        "framework",
        "axion",
        "detector",
        "raw",
        "geant4",
        "connectors",
        "track",
        "wimp",
        "legacy",
        "restG4",
    ],
)
parser.add_argument("--reference", action="store_true")
parser.add_argument("--skip-build", action="store_true")
parser.add_argument("--jobs", nargs="*")
args = parser.parse_args()
root = Path("/work")
name = args.repository
source = (
    root
    if name == "framework"
    else root / "source" / ("packages" if name == "restG4" else "libraries") / name
)
suffix = name + ("-reference" if args.reference else "")
artifacts = root / "validation" / "workflows" / suffix
artifacts.mkdir(parents=True, exist_ok=True)
workspace = root / "validation" / "workspaces" / suffix
workspace.mkdir(parents=True, exist_ok=True)
alias = (
    "framework"
    if name == "framework"
    else ("restG4" if name == "restG4" else name + "lib")
)
checkout = workspace / alias
if not checkout.exists():
    if name == "framework":
        checkout.mkdir()
        for component in ["pipeline", "projects/basic-examples", "scripts"]:
            shutil.copytree(
                source / component,
                checkout / component,
                ignore=shutil.ignore_patterns(".git"),
            )
    else:
        shutil.copytree(
            source,
            checkout,
            ignore=shutil.ignore_patterns(".git", "build*", "install*"),
        )

workflow = yaml.safe_load((source / ".github/workflows/validation.yml").read_text())
install = (
    root / ("install-reference" if args.reference else "install-ci")
    if name == "framework"
    else root / "validation/install" / suffix
)
variables = dict(workflow.get("env", {}))
variables.update(
    REST_PATH=str(install),
    REST_INSTALL_PATH=str(install),
    CMAKE_BUILD_TYPE="Release",
    BRANCH_NAME="experiment/pr567-reimplementation",
)
results = (
    json.loads((artifacts / "results.json").read_text())
    if (artifacts / "results.json").exists()
    else []
)
# Earlier replay scripts mixed restG4's modern/reference jobs in one install.
# Keep each result file scoped to the container/dependency set it describes.
results = [
    result for result in results if ("reference" in result["step"]) == args.reference
]


def expand(text):
    return re.sub(
        r"\$\{\{\s*env\.([A-Z0-9_]+)\s*\}\}",
        lambda match: str(
            variables.get(match.group(1), os.environ.get(match.group(1), ""))
        ),
        text,
    )


def execute(label, body):
    slug = re.sub(r"[^a-zA-Z0-9_-]+", "-", label)
    script = artifacts / (slug + ".sh")
    script.write_text("#!/bin/bash\nset -eo pipefail\n" + body + "\n")
    start = time.monotonic()
    with (artifacts / (slug + ".log")).open("w") as output:
        try:
            result = subprocess.run(
                ["bash", str(script)],
                cwd=workspace,
                stdout=output,
                stderr=subprocess.STDOUT,
                timeout=3600,
                env=dict(
                    os.environ,
                    USER="runner",
                    XRD_CONNECTIONWINDOW="1",
                    XRD_CONNECTIONRETRY="1",
                    XRD_REQUESTTIMEOUT="2",
                ),
            )
            code = result.returncode
        except subprocess.TimeoutExpired:
            code = 124
    results[:] = [result for result in results if result["step"] != label]
    results.append(
        dict(step=label, code=code, seconds=round(time.monotonic() - start, 2))
    )
    (artifacts / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    print(suffix, label, code, flush=True)
    return code == 0


for job_name, job in workflow["jobs"].items():
    if "uses" in job:
        continue  # Reusable library workflows are replayed as separate invocations.
    reference_job = "reference" in job_name
    if reference_job != args.reference:
        continue
    if args.jobs and job_name not in args.jobs:
        continue
    for index, step in enumerate(job.get("steps", [])):
        label = job_name + "-" + str(index) + "-" + step.get("name", "run")
        if "framework/.github/actions/build" in step.get("uses", ""):
            if name == "framework" or args.skip_build:
                continue
            flags = expand(step["with"]["cmake-flags"])
            body = (
                f"cmake -S /work -B /work/validation/build/{suffix} -DREST_ALL_LIBS=OFF "
                f'-DCMAKE_PREFIX_PATH="$GARFIELD_INSTALL" {flags}\n'
                f"cmake --build /work/validation/build/{suffix} -j4 --target install"
            )
            if not execute(label, body):
                break
            continue
        if "run" not in step:
            continue
        body = expand(step["run"])
        if "checkoutRemoteBranch.sh" in body:
            continue  # Source identities were pinned before this replay.
        if name == "framework" and job_name == "build-test":
            continue  # The complete CTest run has its own independent log.
        # Git operations were already performed in the isolated host worktree.
        body = "\n".join(
            line
            for line in body.splitlines()
            if "git submodule update" not in line and line.strip() != "printenv"
        )
        # Generated directories may already exist after the separately run CTest.
        body = re.sub(r"(?m)^(\s*)mkdir (?!-)", r"\1mkdir -p ", body)
        # Bound unavailable external services and retain network failures as failures.
        body = re.sub(r"(?m)^(\s*)wget ", r"\1wget --timeout=30 --tries=2 ", body)
        if not execute(label, body):
            break  # Match Actions: later steps of the failed job do not execute.

(artifacts / "results.json").write_text(json.dumps(results, indent=2) + "\n")
