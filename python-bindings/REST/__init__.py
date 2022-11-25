
import os
from pathlib import Path

# Check PyROOT is accessible
try:
    import ROOT
except ImportError:
    rootsys = os.getenv("ROOTSYS")
    if rootsys is None:
        raise ImportError(
            "ROOT installation not detected. Have you correctly sourced REST / ROOT?"
        )
    else:
        raise ImportError(
            f"""ROOT installation found at "{rootsys}" but ROOT Python bindings not found, perhaps ROOT was installed without Python or you are using a different Python interpreter than the one known to ROOT"""
        )

# Check REST is accessible
restpath = os.getenv("REST_PATH")
if restpath is None:
    raise ImportError(
        "REST-for-Physics installation not detected. Have you correctly sourced REST?"
    )
rest_library_path = Path(restpath) / "lib"

# Get REST libraries to load into ROOT
excluded = ["libRestRestG4"]
library_extensions = [".so", ".dll", ".dylib", ".sl", ".dl"]
libraries = [
    str(rest_library_path / file)
    for file in os.listdir(rest_library_path)
    if Path(file).suffix in library_extensions
       and Path(file).stem not in excluded
       and Path(file).stem.startswith("libRest")
]

for library in libraries:
    ROOT.gSystem.Load(library)
