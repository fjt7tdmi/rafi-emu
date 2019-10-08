# Copyright 2019 Akifumi Fujita
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

.option norvc
.global _start

.text
_start:
    li      t0, 0x1000
    addi    a1, t0, 0x20
    li      t0, 0x1000
    slli    t0, t0, 0x13
    jr      t0
.data
.incbin "dtb/qemu.dtb"
