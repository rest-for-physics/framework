#!/usr/bin/env bash
# Run after installation, as in the framework validation workflow.
set -eo pipefail
setup=${1:?Pass the installed thisREST.sh}
scratch=$(mktemp -d)
trap 'rm -rf -- "$scratch"' EXIT
mkdir -p "$scratch/home" "$scratch/private state"

# Dependency setup scripts may reference unset variables; do not enable nounset.
env -u REST_HOME HOME="$scratch/home" bash -eo pipefail -c '
    source "$1" >/dev/null
    test "$REST_HOME" = "$HOME"
' bash "$setup"

env REST_HOME="" HOME="$scratch/home" bash -eo pipefail -c '
    source "$1" >/dev/null
    test "$REST_HOME" = "$HOME"
' bash "$setup"

env REST_HOME="$scratch/private state" HOME="$scratch/home" bash -eo pipefail -c '
    expected_rest_home=$REST_HOME
    expected_home=$HOME
    source "$1" >/dev/null
    test "$REST_HOME" = "$expected_rest_home"
    test "$HOME" = "$expected_home"
    source "$1" >/dev/null
    test "$REST_HOME" = "$expected_rest_home"
    test "$HOME" = "$expected_home"
' bash "$setup"
echo "REST_HOME defaults, explicit override and repeated setup passed"
