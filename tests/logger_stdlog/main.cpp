#include "global/global.include.h"
#include "global/record/record.stdlog.h"

int main()
{
    auto logger = std::make_shared<tianli::global::record::std_logger>();
    logger->log({ 1, "test", "test" });
    logger->perl("test");
    logger->perl_end("test");
    logger->perl("test");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    logger->perl_end("test");
    logger->perl();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    logger->perl_end();
}