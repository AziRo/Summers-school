FROM ubuntu:xenial
MAINTAINER Pavel Fedotov
RUN apt-get update -y
RUN apt-get install -y gcc nano git make
RUN git clone https://github.com/AziRo/Summers-school.git
RUN cd Summers-school/task1/2/ && make
CMD cp Summers-school/task1/2/a.out /root/tmp
