# This script should be run from the root of a submodule repository.
# It will check that the pre-commit config file matches the one in the framework repository (master branch).
# TODO: if a branch with the same name as the current branch exists in the framework repository, use that instead of master.
# It will also check other files related to the pre-commit such as .clang-format and .cmake-format.yaml.

import os


def check_files(base_directory, file_name) -> bool:
    # Check that the pre-commit config file is the same as the one in the framework repository.
    # If not, print a warning and exit with a non-zero exit code.
    # base_directory: the directory where the file is located.
    # file_names: a list of file names to check.
    # Returns: True if the files are the same, False otherwise.
    # Note: this function will exit the script if the files are not the same.

    branch_name = "master"
    github_prefix = (
        f"https://raw.githubusercontent.com/rest-for-physics/framework/{branch_name}/"
    )

    with open(os.path.join(base_directory, file_name), "r") as f:
        local_file = f.read()
    # download file with curl (this way we avoid adding a python dependency, requests, since curl is installed)
    reference_file = os.popen(f"curl -s {github_prefix}{file_name}").read()
    if local_file != reference_file:
        print(
            f"WARNING: The file {file_name} is different from the one in the framework repository (branch={branch_name})."
        )
        return False

    return True


if __name__ == "__main__":
    # check pre-commit config file
    files = [".pre-commit-config.yaml", ".clang-format", ".cmake-format.yaml"]

    status = True  # OK
    for file in files:
        status = check_files(base_directory=".", file_name=file) and status

    if not status:
        exit(1)  # ERROR
    else:
        exit(0)  # OK
