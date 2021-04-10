FROM alpine:3.13.4
MAINTAINER rakiyoshi

RUN apk --no-cache add \
    mtools

RUN export UID=1000 GID=1000 &&\
    echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
