# This script should be run from the root of a submodule repository.
# It will check that the pre-commit config file matches the one in the framework repository (master branch).
# It will also check other files related to the pre-commit such as .clang-format and .cmake-format.yaml.

import os


def check_files(base_directory, file_name, branch) -> bool:
    # Check that the pre-commit config file is the same as the one in the framework repository.
    # If not, print a warning and exit with a non-zero exit code.
    # base_directory: the directory where the file is located.
    # file_names: a list of file names to check.
    # Returns: True if the files are the same, False otherwise.
    # Note: this function will exit the script if the files are not the same.

    github_prefix = (
        f"https://raw.githubusercontent.com/rest-for-physics/framework/{branch}/"
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

    # if the cwd is a git repository, get the branch name
    branch_name = "master"
    if os.path.exists(".git"):
        this_branch_name = os.popen("git rev-parse --abbrev-ref HEAD").read().strip()
        # Check this branch name also exists in framework
        if (
            len(
                os.popen(
                    f"git ls-remote --heads https://github.com/rest-for-physics/framework.git {this_branch_name}"
                )
                .read()
                .strip()
            )
            != 0
        ):
            branch_name = this_branch_name
        else:
            print(
                f"WARNING: The branch '{this_branch_name}' does not exist in the framework repository."
            )

    print(f"Comparing files with branch={branch_name} in the framework repository.")

    status = True  # OK
    for file in files:
        status = (
            check_files(base_directory=".", file_name=file, branch=branch_name)
            and status
        )

    if not status:
        exit(1)  # ERROR
    else:
        exit(0)  # OK
