TVM is short for TASChain Virtual Machine.

TASChain's Smart Contract language is Python, which can execute in the TVM.

One module of TVM is the python interpreter, which is forked from [Micropython](https://github.com/micropython/micropython).


## Build

```shell
cd examples/embedding
make -f Makefile.upylib
//output libtvm.a
```



## Test

```shell
cd examples/embedding
//code test-tvm.c
make -f Makefile
//output test-tvm
./test-tvm
```



## License

```
Copyright (C) 2018 TASChain

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
