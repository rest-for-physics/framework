# Introduction

This section will contain everything necessary to build the docker containers used in the CI/CD pipeline.

The base image for all other images is an ubuntu:18.04 image containing an installation of root and basic dependencies. All other images (e.g. REST image) will be built on top of this base image.

This image's `Dockerfile` is under the *ROOT* directory. This image can be build with any version of ROOT via environment variable passing, to build this image run the following command (if no version is specified it defaults to ROOT version `6.16.00`):

```
ROOT_VERSION=6.16.00
docker build -t root_v${ROOT_VERSION} --build-arg ROOT_VERSION=${ROOT_VERSION} --build-arg MAKE_N_JOBS=3 .
```

Where the `-t` option is used to give the image a name (`root_v6.16.00` in this case) and `--build-arg ROOT_VERSION` is to assign a value to the `ROOT_VERSION` variable inside the `Dockerfile`. In this example we used an environment variable to keep the naming of the image consistent with the installed ROOT version. Remember you need to be on the directory of the `Dockerfile` (or use the `-f PATH_TO_DOCKERFILE` option) and have superuser privileges. 

# Container Registry

In the near future we will use a feature called the GitLab container registry to upload our images built using the dockerfiles contained here. It is important to use this registry and not a public one (for example https://hub.docker.com/) because our images may contain sensitive information.

For example the ROOT image could be public since it does not contain any sensitive information but in order to build the REST image one needs to pass a deployment key in order for the container to clone the REST repository. This is considered sensitive information therefor this image has to be kept private.

We would pass the deployment key (and any other sensitive information) via `--build-arg` flag when building locally and then upload the image to the GitLab registry. it is important to mention that someone with access to the image still could recover this information unless we do a multi-stage build (and the sensitive information is not on the last stage). 