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

#include <rafi/trace.h>

#include <boost/algorithm/string.hpp>

#include "TraceUtil.h"

namespace rafi {

std::unique_ptr<trace::ITraceReader> MakeTraceReader(const std::string& path)
{
    if (boost::algorithm::ends_with(path, ".tbin") || boost::algorithm::ends_with(path, ".bin"))
    {
        return std::make_unique<trace::TraceBinaryReader>(path.c_str());
    }
    else if (boost::algorithm::ends_with(path, ".tidx") || boost::algorithm::ends_with(path, ".idx"))
    {
        return std::make_unique<trace::TraceIndexReader>(path.c_str());
    }
    else if (boost::algorithm::ends_with(path, ".gdb.log"))
    {
        return std::make_unique<trace::GdbTraceReader>(path.c_str());
    }
    else
    {
        return std::make_unique<trace::TraceTextReader>(path.c_str());
    }
}

std::unique_ptr<trace::ITracePrinter> MakeTracePrinter(PrinterType printerType)
{
    switch (printerType)
    {
        case PrinterType::Text:
            return std::make_unique<trace::TraceTextPrinter>();
        case PrinterType::Json:
            return std::make_unique<trace::TraceJsonPrinter>();
        case PrinterType::Pc:
            return std::make_unique<trace::TracePcPrinter>();
        default:
            RAFI_NOT_IMPLEMENTED();
    }
}

}
