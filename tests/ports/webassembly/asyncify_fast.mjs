// Validates that a suspend (time.sleep -> emscripten_sleep) works through every
// indirect dispatch path. This is the guard for the asyncify-fast variant's
// curated ASYNCIFY_ADD list (ASYNCIFY_IGNORE_INDIRECT trims instrumentation, so a
// dispatch site missing from the list would corrupt the unwind/rewind when a
// suspend happens beneath it). Forces a suspend through operators, comparison,
// subscript, call, property, iter/next, context managers, deep recursion,
// generators and an except handler, and checks the results survive. Runs on any
// yield-capable build (it passes trivially on the whole-program asyncify build;
// CI runs it against asyncify-fast where it actually exercises the ADD list).

import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const module = await import(pathToFileURL(resolve(process.argv[2])));
if (!globalThis.__MICROPYTHON_ASYNC__) {
    console.log("SKIP");
    process.exit(0);
}

let lastOut = "";
const mp = await module.loadMicroPython({
    heapsize: 8 * 1024 * 1024,
    stdout: (s) => {
        lastOut = s;
    },
});

// Each case runs in its own exec (avoids piling many suspends into one async
// call) and asserts internally; a wrong rewind raises and fails the case.
const check = async (name, src) => {
    try {
        await mp.runPythonAsync(src);
    } catch (e) {
        throw new Error(
            `${name}: ${(e.message || String(e)).trim().split("\n").pop()}`,
        );
    }
};

const P = "import time\nS = 0.001\n";
await check(
    "operator __add__",
    `${P}class C:\n  def __init__(s,v): s.v=v\n  def __add__(s,o): time.sleep(S); return C(s.v+o.v)\nassert (C(10)+C(20)).v==30`,
);
await check(
    "comparison __lt__",
    `${P}class C:\n  def __init__(s,v): s.v=v\n  def __lt__(s,o): time.sleep(S); return s.v<o.v\nassert (C(1)<C(2)) is True`,
);
await check(
    "subscript __getitem__",
    `${P}class C:\n  def __getitem__(s,i): time.sleep(S); return i*2\nassert C()[5]==10`,
);
await check(
    "__call__",
    `${P}class C:\n  def __call__(s,x): time.sleep(S); return x*3\nassert C()(4)==12`,
);
await check(
    "attribute (property)",
    `${P}class C:\n  def __init__(s,v): s.v=v\n  @property\n  def d(s): time.sleep(S); return s.v*2\nassert C(21).d==42`,
);
await check(
    "iter/next protocol",
    `${P}class It:\n  n=0\n  def __iter__(s): time.sleep(S); return s\n  def __next__(s):\n    if s.n>=4: raise StopIteration\n    time.sleep(S); s.n+=1; return s.n*s.n\nassert list(It())==[1,4,9,16]`,
);
await check(
    "context manager",
    `${P}class Cm:\n  def __enter__(s): time.sleep(S); return 7\n  def __exit__(s,*a): time.sleep(S); return False\nwith Cm() as v: assert v==7`,
);
await check(
    "deep recursion",
    `${P}def d(n):\n  if n==0: time.sleep(S); return 0\n  return d(n-1)+n\nassert d(20)==210`,
);
await check(
    "generator",
    `${P}def g():\n  for i in range(5): time.sleep(S); yield i*i\nassert list(g())==[0,1,4,9,16]`,
);
await check(
    "except handler",
    `${P}def f():\n  try:\n    raise ValueError(1)\n  except ValueError:\n    time.sleep(S)\n    return 99\nassert f()==99`,
);
await check(
    "map over suspending callable",
    `${P}def slow(x): time.sleep(S); return x*x\nassert list(map(slow,[1,2,3]))==[1,4,9]`,
);
// Real file I/O across suspends. f.flush()/f.close() dispatch through
// stream_flush -> the stream protocol's indirect ioctl (vfs_posix_file_ioctl,
// MP_STREAM_FLUSH); f.write() through mp_stream_rw -> stream_p->write. Missing
// from the ADD list, a suspend around a flush corrupts the rewind (traps
// "unreachable", silently - only ~1 iteration runs first). This is the shape
// that broke badgeware State.save on its JS-backed (suspending) filesystem.
await check(
    "file flush then suspend",
    `${P}f = open("/f1.txt", "w")\nf.write("x")\nf.flush()\nf.close()\ntime.sleep(S)\ntime.sleep(S)\nassert True`,
);
await check(
    "file flush interleaved with suspends",
    `${P}n = 0\nfor i in range(8):\n  f = open("/f2.txt", "w")\n  f.write(str(i))\n  f.flush()\n  f.close()\n  time.sleep(S)\n  n += 1\nassert n == 8, n`,
);
await check(
    "file flush in nested-except + recursion (state-save shape)",
    `${P}import os\ndef save(app):\n  try:\n    with open("/st/%s.json" % app, "w") as fp:\n      fp.write("{}"); fp.flush()\n  except OSError:\n    try:\n      os.stat("/st")\n    except OSError:\n      os.mkdir("/st"); save(app)\nsave("counter")\ntime.sleep(S)\ntime.sleep(S)\nassert True`,
);

// A checksum computed across interleaved suspends must complete and be non-empty
// (a corrupt rewind would abort or produce nothing).
lastOut = "";
await mp.runPythonAsync(
    `${P}t=0\nz=0.0\nfor i in range(20000):\n  z=(z*0.99+i*7)%1000003\n  t=(t+int(z))&0xffffffff\n  if i%4000==0: time.sleep(S)\nprint(t)`,
);
if (!lastOut.trim()) {
    throw new Error("checksum not computed across suspends");
}

console.log("asyncify-fast ok");
