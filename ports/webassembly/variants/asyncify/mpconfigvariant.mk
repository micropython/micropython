# Asyncify instruments the whole program so any function can suspend via
# emscripten_sleep. The cooperative yield (main.c mp_js_yield) can fire at
# arbitrary Python recursion depth, so give the unwind buffer generous headroom.
JSFLAGS += -s ASYNCIFY
JSFLAGS += -s ASYNCIFY_STACK_SIZE=131072

# Drive the entry points via ccall({async: true}) (see api.js invoke());
# async_asyncify.js announces the backend to api.js.
SRC_JS += async_asyncify.js
