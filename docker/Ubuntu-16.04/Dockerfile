FROM ubuntu:16.04

RUN apt update && apt upgrade -y

RUN apt install build-essential cmake qt5-default -y

RUN apt install qttools5-dev-tools -y

RUN apt install snapcraft -y

RUN apt install -y locales locales-all

ENV LC_ALL en_US.UTF-8

ENV LANG en_US.UTF-8

ENV LANGUAGE en_US.UTF-8

RUN apt install -y wget

RUN apt install fuse -y

RUN groupadd fuse

RUN usermod -a -G fuse root

