/*
 * Copyright 2018 Akifumi Fujita
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <rafi/common.h>

#include "trace/BinaryCycleLogger.h"
#include "trace/CycleTypes.h"
#include "trace/Exception.h"
#include "trace/GdbTraceReader.h"
#include "trace/ICycle.h"
#include "trace/ITracePrinter.h"
#include "trace/ITraceReader.h"
#include "trace/ITraceWriter.h"
#include "trace/TraceBinaryMemoryReader.h"
#include "trace/TraceBinaryMemoryWriter.h"
#include "trace/TraceBinaryReader.h"
#include "trace/TraceBinaryWriter.h"
#include "trace/TraceIndexReader.h"
#include "trace/TraceIndexWriter.h"
#include "trace/TraceTextReader.h"
#include "trace/TracePcPrinter.h"
#include "trace/TraceJsonPrinter.h"
#include "trace/TraceTextPrinter.h"
