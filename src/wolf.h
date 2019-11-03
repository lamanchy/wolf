//
// Created by lomic on 10/30/2019.
//

#ifndef WOLF_SRC_WOLF_H_
#define WOLF_SRC_WOLF_H_

#include <base/pipeline.h>
#include <base/options/options.h>
#include <base/options/command.h>
#include <base/plugins/plugin.h>
#include <base/options/event.h>
#include <plugins/tcp_in.h>
#include <serializers/line.h>
#include <plugins/string_to_json.h>
#include <plugins/kafka_out.h>
#include <plugins/ysoft/add_local_info.h>
#include <plugins/json_to_string.h>
#include <plugins/ysoft/normalize_nlog_logs.h>
#include <plugins/ysoft/normalize_serilog_logs.h>
#include <plugins/ysoft/normalize_log4j2_logs.h>
#include <plugins/tcp_out.h>
#include <plugins/lambda.h>
#include <serializers/compressed.h>
#include <plugins/collate.h>
#include <extras/validators.h>
#include <plugins/kafka_in.h>
#include <plugins/copy.h>
#include <extras/get_time.h>
#include <plugins/stats.h>
#include <serializers/plain.h>
#include <plugins/http_out.h>
#include <plugins/cin.h>
#include <plugins/cout.h>
#include <plugins/stream_sort.h>
#include <plugins/regex.h>
#include <plugins/ysoft/get_elapsed_preevents.h>
#include <plugins/ysoft/count_logs.h>
#include <plugins/json_to_influx.h>
#include <plugins/filter.h>
#include <serializers/deserialize.h>
#include <plugins/time_sort.h>
#include <plugins/elapsed.h>

#endif //WOLF_SRC_WOLF_H_
