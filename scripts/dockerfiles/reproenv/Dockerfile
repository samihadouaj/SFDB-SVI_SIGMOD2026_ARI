#
# dockerized environment to reproduce the experiments from the SIGMOD paper
#
# To build the image, run `docker build -t starfishdb_repro_env_docker_img .`
#
FROM centos:7.9.2009
#
ENV TZ="America/New_York"
#
ADD . /gammapdb_arrow
#
# Use vault.centos.org instead of mirrorlist.centos.org, which doesn't exist
# anymore (see https://serverfault.com/a/1161847).
RUN	set -x && \
	sed -i 's/mirror.centos.org/vault.centos.org/g' /etc/yum.repos.d/*.repo && \
	sed -i 's/^#.*baseurl=http/baseurl=http/g' /etc/yum.repos.d/*.repo && \
	sed -i 's/^mirrorlist=http/#mirrorlist=http/g' /etc/yum.repos.d/*.repo
#
RUN yum update -y
RUN yum install -y epel-release
RUN yum install -y centos-release-scl
RUN	set -x && \
	sed -i 's/mirror.centos.org/vault.centos.org/g' /etc/yum.repos.d/*.repo && \
	sed -i 's/^#.*baseurl=http/baseurl=http/g' /etc/yum.repos.d/*.repo && \
	sed -i 's/^mirrorlist=http/#mirrorlist=http/g' /etc/yum.repos.d/*.repo
#
RUN yum groupinstall -y 'Development Tools'
RUN yum install -y cmake3 ninja-build which
RUN yum install -y screen
RUN yum install -y devtoolset-11
RUN yum install -y devtoolset-7
#RUN yum install -y llvm-devtoolset-11
RUN yum install -y git python3 sudo cmake java-11-openjdk ant nano snappy gsl gsl-devel bzip2 wget bc
RUN pip3 --no-input install mpmath
RUN pip3 --no-input install numpy
RUN pip3 --no-input install pandas
RUN pip3 --no-input install hmmlearn
#
#
# customize the shell prompt
#
RUN echo "" >> ~/.bashrc
RUN echo "parse_git_branch()" >> ~/.bashrc
RUN echo "{" >> ~/.bashrc
RUN echo " git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/(\1)/'" >> ~/.bashrc
RUN echo "}" >> ~/.bashrc
RUN echo "" >> ~/.bashrc
RUN echo "PS1=\"starfishDB-reproenv \[\e[32m\]\w\[\e[91m\] \\\$(parse_git_branch)\[\e[00m\] $ \"" >> ~/.bashrc
RUN echo "" >> ~/.bashrc
#
# by default, enable the modern tool-chain
#
RUN echo "source opt/rh/devtoolset-11/enable" >> ~/.bashrc
RUN echo "" >> ~/.bashrc
# 
#
# change the default directory
#
RUN echo "cd /gammapdb_arrow" >> ~/.bashrc
RUN echo "" >> ~/.bashrc
#
# EOF
#
