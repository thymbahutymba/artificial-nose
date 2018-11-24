FROM thymbahutymba/allegro4

COPY . /artificial-nose
WORKDIR /artificial-nose

RUN make