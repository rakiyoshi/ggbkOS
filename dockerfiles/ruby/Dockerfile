FROM ruby:3.1.2

ARG UID
ARG GID

RUN echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
