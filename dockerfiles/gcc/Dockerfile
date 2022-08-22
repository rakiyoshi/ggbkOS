FROM gcc:10.4.0
MAINTAINER rakiyoshi

ARG UID
ARG GID

RUN apt update && \
    apt install -y gcc-multilib
RUN echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
