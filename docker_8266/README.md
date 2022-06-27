Use a Docker image with a pre-built toolchain (recommended). To use this, install Docker, then prepend docker run --rm -v $HOME:$HOME -u $UID -w $PWD larsks/esp-open-sdk to the start of the mpy-cross and firmware make commands below. This will run the commands using the toolchain inside the container but using the files on your local filesystem.

wsl1 使用windows-docker：
win-docker软件里开启Expose daemon on tcp://localhost:2375 without TLS选项
export DOCKER_HOST=tcp://127.0.0.1:2375

```
docker run --rm -v $HOME:$HOME -u $UID -w $PWD larsks/esp-open-sdk make

改用docker-compose映射之后执行，cd xxx && make 详见docker-compose.yml

用法：
#修改commond 之后运行
docker-compose up
```


