# Basic docker image for GitLab code integration.
#
# To create the image:
#
#    docker image build -t registry.gitlab.com/libadg/adg:stretch .
#
# To interactively test the result:
#
#    docker run -it registry.gitlab.com/libadg/adg:stretch /bin/bash
#
# To push the image to the GitLab registry:
#
#    docker login registry.gitlab.com --username ... --password ...
#    docker push registry.gitlab.com/libadg/adg:stretch
#    docker logout registry.gitlab.com

FROM debian:stretch

USER root

# Install any needed dependency. A recent meson is required,
# so use `pip3` to install it instead of `apt-get`
RUN apt-get update -qq && \
    apt-get install -qq --no-install-suggests \
        build-essential \
        gcovr \
        git \
        gobject-introspection \
        gtk-doc-tools \
        libcairo2-dev \
        libgirepository1.0-dev \
        libgtk2.0-dev \
        libgtk-3-dev \
        libpango1.0-dev \
        python3-pip && \
    pip3 install meson ninja
