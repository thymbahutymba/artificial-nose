FROM thymbahutymba/allegro4

COPY . /artificial-nose
WORKDIR /artificial-nose

RUN make
CMD echo "-1" > /proc/sys/kernel/sched_rt_runtime_us && ./main