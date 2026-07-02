FROM ruby:3.4.10

ARG UID
ARG GID

RUN echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
