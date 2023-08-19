#include <asynccurl/executor.h>
asynccurl::Executor::Executor() : multi_handle_(curl_multi_init()) {

}
