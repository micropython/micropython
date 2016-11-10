
Follow the instructions for getting started with Emscripten [here](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).

Then you can run

```bash
source ./emsdk_env.sh
cd {micropython directory}/emscripten
make EMSCRIPTEN=1 -j
node build/firmware.js
```

