#pragma once

#include <base/make.h>
#include <base/pipeline.h>
#include <base/plugins/base_plugin.h>
#include <base/options/options.h>
#include <base/options/input.h>
#include <base/options/event.h>

#include <plugins/inputs/tcp.h>
#include <plugins/inputs/kafka.h>
#include <plugins/inputs/cin.h>

#include <plugins/outputs/kafka.h>
#include <plugins/outputs/tcp.h>
#include <plugins/outputs/http.h>
#include <plugins/outputs/cout.h>

#include <plugins/serializers/compressed.h>
#include <plugins/serializers/line.h>
#include <plugins/serializers/influx.h>
#include <plugins/serializers/string.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

#include <plugins/deserializers/compressed.h>
#include <plugins/deserializers/line.h>
#include <plugins/deserializers/string.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

#include <plugins/lambda.h>
#include <plugins/collate.h>
#include <plugins/copy.h>
#include <plugins/stats.h>
#include <plugins/stream_sort.h>
#include <plugins/regex.h>
#include <plugins/filter.h>
#include <plugins/time_sort.h>
#include <plugins/elapsed.h>

#include <extras/validators.h>
#include <extras/get_time.h>
