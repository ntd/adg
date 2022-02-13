# Image pushed to registry.gitlab.com/libadg/adg:stretch
FROM debian:stretch

USER root

# Install any needed dependency. A recent meson is required,
# so use `pip3` to install it instead of `apt-get`
RUN apt-get update -qq && \
    apt-get install -qq build-essential \
                        gobject-introspection \
                        gtk-doc-tools \
                        libcairo2-dev \
                        libgirepository1.0-dev \
                        libgtk2.0-dev \
                        libgtk-3-dev \
                        libpango1.0-dev \
                        python3-pip && \
    pip3 install meson ninja
