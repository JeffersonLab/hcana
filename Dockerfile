FROM centos:centos7

ARG APP_VERSION
ARG REPO_NAME

RUN yum update -q -y

RUN yum -y install epel-release &&\
    yum -y install git && \
    yum -y groupinstall 'Development Tools'&& \
    yum -y install gcc-c++ && \
    yum -y install make && \
    yum install -y root && \
    localedef -i en_US -f UTF-8 en_US.UTF-8

ADD https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.tar.gz .
RUN tar -xvf cmake-3.22.2-linux-x86_64.tar.gz && rm cmake-3.22.2-linux-x86_64.tar.gz
RUN mv cmake-3.22.2-linux-x86_64 /usr/local/cmake
ENV PATH="/usr/local/cmake/bin:$PATH"
ADD https://github.com/JeffersonLab/hcana/archive/refs/tags/${APP_VERSION}.tar.gz .
RUN tar -xvf ${APP_VERSION}.tar.gz && rm ${APP_VERSION}.tar.gz
WORKDIR "/${REPO_NAME}-${APP_VERSION}"
ADD podd podd/
SHELL ["/bin/bash", "-c"]
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local/hcana -B build  -S /${REPO_NAME}-${APP_VERSION}
RUN cmake --build build -j8
RUN cmake --install build
ENV PATH="/usr/local/hcana/bin:$PATH"
ENV LD_LIBRARY_PATH="/usr/local/hcana/lib64:$LD_LIBRARY_PATH"
ENV ANALYZER=/${REPO_NAME}-${APP_VERSION}/podd
ENV HCANALYZER=/${REPO_NAME}-${APP_VERSION}
WORKDIR /${REPO_NAME}-${APP_VERSION}
RUN mkdir myworkdir
WORKDIR /${REPO_NAME}-${APP_VERSION}/myworkdir

