FROM alpine:3.13.4
MAINTAINER rakiyoshi

ARG UID
ARG GID

RUN apk --no-cache add \
    mtools

RUN echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
