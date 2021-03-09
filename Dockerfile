ARG ARCH
ARG BOSSA_VERSION=1.9.1

FROM ${ARCH}/ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive

ARG ARCH
COPY ./assets/qemu/${ARCH}/ /usr/bin/

RUN apt update

RUN apt install -y --no-install-recommends \
	libwxgtk3.0-gtk3-dev \
	libreadline-dev \
	build-essential \
	git \
	ca-certificates

ARG BOSSA_VERSION
RUN git clone -b ${BOSSA_VERSION} https://github.com/shumatech/BOSSA && \
	git -C ./BOSSA switch -c release

WORKDIR ./BOSSA

CMD make

