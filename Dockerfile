FROM almalinux:9

ARG APP_VERSION
ARG REPO_NAME

ADD https://pki.jlab.org/JLabCA.crt /etc/pki/ca-trust/source/anchors/JLabCA.crt
RUN update-ca-trust

RUN dnf update -q -y

RUN dnf -y install 'dnf-command(config-manager)'

RUN dnf -y install epel-release 
RUN dnf config-manager --set-enabled crb
RUN dnf -y install git && \
    dnf -y groupinstall 'Development Tools' && \
    dnf -y install gcc-c++ cmake make \
        root root-mathcore root-montecarlo-eg \
        root-mathmore root-gui root-hist root-physics root-genvector && \
    dnf clean all

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

