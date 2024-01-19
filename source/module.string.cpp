#include "pch.h"
#include "cvAutoTrack.h"

struct cvat_string
{
    std::string context;
};

cvat_string_ptr alloc_string()
{
    return new cvat_string();
}

int get_string_length(cvat_string_ptr str)
{
    return str->context.length();
}

int get_string_context(cvat_string_ptr str, char* buffer, int buffer_size)
{
    if (buffer_size < str->context.length())
        return -1;
    std::copy(str->context.begin(), (str->context.size() > buffer_size) ? (str->context.begin() + buffer_size) : (str->context.end()), buffer);
    return str->context.length();
}

void free_string(cvat_string_ptr str)
{
    delete str;
}
