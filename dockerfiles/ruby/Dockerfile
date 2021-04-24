FROM ruby:3.0.1

RUN export UID=1000 GID=1000 &&\
    echo "user:x:${UID}:${GID}:user,,,::/bin/sh" >> /etc/passwd && \
    echo "user:x:${UID}:" >> /etc/group
USER user
