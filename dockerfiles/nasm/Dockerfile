FROM alpine:3.19.1
MAINTAINER rakiyoshi

ARG UID
ARG GID

RUN apk --no-cache add \
    nasm

RUN echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
