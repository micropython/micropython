# Copyright (c) Quectel Wireless Solution, Co., Ltd.All Rights Reserved.
#  
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#  
#     http://www.apache.org/licenses/LICENSE-2.0
#  
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


from utils import crc32

READ_BLOCK_SIZE = 4096

def _check(fp):
	fp.seek(0)
	fr = fp.read(READ_BLOCK_SIZE)
	while fr:
		yield fr
		fr = fp.read(READ_BLOCK_SIZE)
	else:
		fp.seek(0)

def calc(file_name):
    csumFunc = crc32()
    csum = 0xffffffff
    flag = 0
    try:
        with open(file_name, 'rb') as fp:
            for fr in _check(fp):
                csum = csumFunc.update(csum, fr)
                flag = 1
            fp.close()
    except Exception as e:
        pass
    finally:
        if not flag:
            return None
        return hex(csum)

